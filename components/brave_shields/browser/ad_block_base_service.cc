/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_shields/browser/ad_block_base_service.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "base/bind.h"
#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "base/strings/utf_string_conversions.h"
#include "brave/browser/net/url_context.h"
#include "brave/common/pref_names.h"
#include "brave/components/brave_component_updater/browser/dat_file_util.h"
#include "brave/components/brave_shields/common/brave_shield_constants.h"
#include "brave/vendor/ad-block/ad_block_client.h"
#include "components/prefs/pref_service.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "url/origin.h"

using brave_component_updater::BraveComponent;
using namespace net::registry_controlled_domains;  // NOLINT

namespace {

FilterOption ResourceTypeToFilterOption(content::ResourceType resource_type) {
  FilterOption filter_option = FONoFilterOption;
  switch (resource_type) {
    // top level page
    case content::ResourceType::kMainFrame:
      filter_option = FODocument;
      break;
    // frame or iframe
    case content::ResourceType::kSubFrame:
      filter_option = FOSubdocument;
      break;
    // a CSS stylesheet
    case content::ResourceType::kStylesheet:
      filter_option = FOStylesheet;
      break;
    // an external script
    case content::ResourceType::kScript:
      filter_option = FOScript;
      break;
    // an image (jpg/gif/png/etc)
    case content::ResourceType::kFavicon:
    case content::ResourceType::kImage:
      filter_option = FOImage;
      break;
    // a font
    case content::ResourceType::kFontResource:
      filter_option = FOFont;
      break;
    // an "other" subresource.
    case content::ResourceType::kSubResource:
      filter_option = FOOther;
      break;
    // an object (or embed) tag for a plugin.
    case content::ResourceType::kObject:
      filter_option = FOObject;
      break;
    // a media resource.
    case content::ResourceType::kMedia:
      filter_option = FOMedia;
      break;
    // a XMLHttpRequest
    case content::ResourceType::kXhr:
      filter_option = FOXmlHttpRequest;
      break;
    // a ping request for <a ping>/sendBeacon.
    case content::ResourceType::kPing:
      filter_option = FOPing;
      break;
    // the main resource of a dedicated worker.
    case content::ResourceType::kWorker:
    // the main resource of a shared worker.
    case content::ResourceType::kSharedWorker:
    // an explicitly requested prefetch
    case content::ResourceType::kPrefetch:
    // the main resource of a service worker.
    case content::ResourceType::kServiceWorker:
    // a report of Content Security Policy violations.
    case content::ResourceType::kCspReport:
    // a resource that a plugin requested.
    case content::ResourceType::kPluginResource:
    // a service worker navigation preload request.
    case content::ResourceType::kNavigationPreload:
    // an invalid type (see brave/browser/net/url_context.h)
    case brave::BraveRequestInfo::kInvalidResourceType:
    default:
      break;
  }
  return filter_option;
}

}  // namespace

namespace brave_shields {

AdBlockBaseService::AdBlockBaseService(BraveComponent::Delegate* delegate)
    : BaseBraveShieldsService(delegate),
      ad_block_client_(new AdBlockClient()),
      weak_factory_(this) {
  DETACH_FROM_SEQUENCE(sequence_checker_);
}

AdBlockBaseService::~AdBlockBaseService() {
  Cleanup();
}

void AdBlockBaseService::Cleanup() {
  GetTaskRunner()->DeleteSoon(FROM_HERE, ad_block_client_.release());
}

bool AdBlockBaseService::ShouldStartRequest(const GURL& url,
    content::ResourceType resource_type, const std::string& tab_host,
    bool* did_match_exception, bool* cancel_request_explicitly) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  FilterOption current_option = ResourceTypeToFilterOption(resource_type);

  // Determine third-party here so the library doesn't need to figure it out.
  // CreateFromNormalizedTuple is needed because SameDomainOrHost needs
  // a URL or origin and not a string to a host name.
  if (SameDomainOrHost(url, url::Origin::CreateFromNormalizedTuple(
        "https", tab_host.c_str(), 80), INCLUDE_PRIVATE_REGISTRIES)) {
    current_option = static_cast<FilterOption>(
        current_option | FONotThirdParty);
  } else {
    current_option = static_cast<FilterOption>(current_option | FOThirdParty);
  }

  Filter* matching_filter = nullptr;
  Filter* matching_exception_filter = nullptr;
  if (ad_block_client_->matches(url.spec().c_str(),
        current_option, tab_host.c_str(), &matching_filter,
        &matching_exception_filter)) {
    if (matching_filter && cancel_request_explicitly &&
        (matching_filter->filterOption & FOExplicitCancel)) {
      *cancel_request_explicitly = true;
    }
    // We'd only possibly match an exception filter if we're returning true.
    *did_match_exception = false;
    // LOG(ERROR) << "AdBlockBaseService::ShouldStartRequest(), host: "
    //  << tab_host
    //  << ", resource type: " << resource_type
    //  << ", url.spec(): " << url.spec();
    return false;
  }

  if (did_match_exception) {
    *did_match_exception = !!matching_exception_filter;
  }

  return true;
}

void AdBlockBaseService::EnableTag(const std::string& tag, bool enabled) {
  GetTaskRunner()->PostTask(FROM_HERE,
      base::BindOnce(&AdBlockBaseService::EnableTagOnTaskRunner,
                     weak_factory_.GetWeakPtr(),
                     tag,
                     enabled));
}

void AdBlockBaseService::EnableTagOnTaskRunner(
    const std::string& tag, bool enabled) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (enabled) {
    ad_block_client_->addTag(tag);
  } else {
    ad_block_client_->removeTag(tag);
  }
}

void AdBlockBaseService::GetDATFileData(const base::FilePath& dat_file_path) {
  GetTaskRunner()->PostTask(
      FROM_HERE,
      base::BindOnce(&AdBlockBaseService::GetDATFileDataOnTaskRunner,
                     weak_factory_.GetWeakPtr(),
                     dat_file_path));
}

void AdBlockBaseService::GetDATFileDataOnTaskRunner(
    const base::FilePath& dat_file_path) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  brave_component_updater::DATFileDataBuffer buffer;
  brave_component_updater::GetDATFileData(dat_file_path, &buffer);
  if (buffer.empty()) {
    LOG(ERROR) << "Could not obtain ad block data";
    return;
  }

  auto new_ad_block_client = std::make_unique<AdBlockClient>();
  if (!new_ad_block_client->deserialize(
      reinterpret_cast<char*>(&buffer.front()))) {
    LOG(ERROR) << "Failed to deserialize ad block data";
    return;
  }

  ad_block_client_ = std::move(new_ad_block_client);
}

bool AdBlockBaseService::Init() {
  return true;
}

AdBlockClient* AdBlockBaseService::GetAdBlockClientForTest() {
  return ad_block_client_.get();
}

///////////////////////////////////////////////////////////////////////////////

}  // namespace brave_shields
