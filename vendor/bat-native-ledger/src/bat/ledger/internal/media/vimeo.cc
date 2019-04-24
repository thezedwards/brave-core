/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <algorithm>
#include <cmath>
#include <vector>

#include "base/strings/stringprintf.h"
#include "bat/ledger/internal/bat_helper.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/media/vimeo.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

namespace braveledger_media {

MediaVimeo::MediaVimeo(bat_ledger::LedgerImpl* ledger):
  ledger_(ledger) {
}

MediaVimeo::~MediaVimeo() {
}

// static
std::string MediaVimeo::GetLinkType(const std::string& url) {
  const std::string api = "https://fresnel.vimeocdn.com/add/player-stats?";
  std::string type;

  if (!url.empty() && url.find(api) != std::string::npos) {
    type = VIMEO_MEDIA_TYPE;
  }

  return type;
}

// static
std::string MediaVimeo::GetVideoUrl(const std::string& video_id) {
  if (video_id.empty()) {
    return std::string();
  }

  return "https://vimeo.com/" + video_id;
}

// static
std::string MediaVimeo::GetMediaKey(const std::string& video_id,
                                    const std::string& type) {
  if (video_id.empty()) {
    return std::string();
  }

  if (type == "vimeo-vod") {
    return (std::string)VIMEO_MEDIA_TYPE  + "_" + video_id;
  }

  return std::string();
}

// static
std::string MediaVimeo::GetPublisherKey(const std::string& key) {
  if (key.empty()) {
    return std::string();
  }

  return (std::string)VIMEO_MEDIA_TYPE + "#channel:" + key;
}

// static
std::string MediaVimeo::GetIdFromVideoPage(const std::string& data) {
  if (data.empty()) {
    return "";
  }

  return braveledger_media::ExtractData(data,
      "\\/i.vimeocdn.com\\/portrait\\/", "_75x75");
}

// static
std::string MediaVimeo::GenerateFaviconUrl(const std::string& id) {
  if (id.empty()) {
    return std::string();
  }

  return base::StringPrintf("https://i.vimeocdn.com/portrait/%s_300x300.webp",
                            id.c_str());
}

// static
std::string MediaVimeo::GetNameFromVideoPage(const std::string& data) {
  if (data.empty()) {
    return std::string();
  }

  const std::string name =
      braveledger_media::ExtractData(data, ",\"display_name\":\"", "\"");

  std::string publisher_name;
  const std::string publisher_json = "{\"brave_publisher\":\"" +
      name + "\"}";
  // scraped data could come in with JSON code points added.
  // Make to JSON object above so we can decode.
  braveledger_bat_helper::getJSONValue(
      "brave_publisher", publisher_json, &publisher_name);
  return publisher_name;
}

// static
std::string MediaVimeo::GetPublisherUrl(const std::string& data) {
  if (data.empty()) {
    return std::string();
  }

  const std::string wrapper = braveledger_media::ExtractData(data,
      "<span class=\"userlink userlink--md\">", "</span>");

  const std::string name = braveledger_media::ExtractData(wrapper,
      "<a href=\"/", "\">");

  if (name.empty()) {
    return std::string();
  }

  return base::StringPrintf("https://vimeo.com/%s/videos",
                            name.c_str());
}

// static
bool MediaVimeo::AllowedEvent(const std::string& event) {
  if (event.empty()) {
    return false;
  }

  const std::vector<std::string> allowed = {
    "video-start-time",
    "video-minute-watched",
    "video-paused",
    "video-played",
    "video-seek",
    "video-seeked"};

  auto it = std::find(allowed.begin(), allowed.end(), event);
  return it != allowed.end();
}

// static
uint64_t MediaVimeo::GetDuration(
    const ledger::MediaEventInfo& old_event,
    const ledger::MediaEventInfo& new_event) {
  // Remove duplicated events
  if (old_event.event_ == new_event.event_ &&
      old_event.time_ == new_event.time_) {
    return 0u;
  }

  double time = 0.0;
  std::stringstream tempNew(new_event.time_);
  double newTime = 0.0;
  tempNew >> newTime;

  // Video started
  if (new_event.event_ == "video-start-time") {
    time = newTime;
  } else {
    std::stringstream tempOld(old_event.time_);
    double oldTime = 0;
    tempOld >> oldTime;

    if (new_event.event_ == "video-minute-watched" ||
        new_event.event_ == "video-paused") {
      time = newTime - oldTime;
    }
  }

  return static_cast<uint64_t>(std::round(time));
}

void MediaVimeo::FetchDataFromUrl(
    const std::string& url,
    braveledger_media::FetchDataFromUrlCallback callback) {
  ledger_->LoadURL(url,
                   std::vector<std::string>(),
                   std::string(),
                   std::string(),
                   ledger::URL_METHOD::GET,
                   callback);
}

void MediaVimeo::ProcessMedia(const std::map<std::string, std::string>& parts,
                              const ledger::VisitData& visit_data) {
  auto iter = parts.find("video_id");
  std::string media_id;
  if (iter != parts.end()) {
    media_id = iter->second;
  }

  if (media_id.empty()) {
    return;
  }

  std::string type;
  iter = parts.find("type");
  if (iter != parts.end()) {
    type = iter->second;
  }

  const std::string media_key = GetMediaKey(media_id, type);

  ledger::MediaEventInfo event_info;
  iter = parts.find("event");
  if (iter != parts.end()) {
    event_info.event_ = iter->second;
  }

  // We should only record events that are relevant to us
  if (!AllowedEvent(event_info.event_)) {
    return;
  }

  iter = parts.find("time");
  if (iter != parts.end()) {
    event_info.time_ = iter->second;
  }

  ledger_->GetMediaPublisherInfo(media_key,
      std::bind(&MediaVimeo::OnMediaPublisherInfo,
                this,
                media_id,
                media_key,
                event_info,
                visit_data,
                _1,
                _2));
}

void MediaVimeo::ProcessActivityFromUrl(uint64_t window_id,
                                        const ledger::VisitData& visit_data) {

}

void MediaVimeo::OnMediaPublisherInfo(
    const std::string& media_id,
    const std::string& media_key,
    const ledger::MediaEventInfo& event_info,
    const ledger::VisitData& visit_data,
    ledger::Result result,
    std::unique_ptr<ledger::PublisherInfo> publisher_info) {
  if (result != ledger::Result::LEDGER_OK &&
      result != ledger::Result::NOT_FOUND) {
    BLOG(ledger_, ledger::LogLevel::LOG_ERROR)
        << "Failed to get publisher info";
    return;
  }

  if (!publisher_info && !publisher_info.get()) {
    auto callback = std::bind(&MediaVimeo::OnPublisherVideoPage,
                            this,
                            media_key,
                            event_info,
                            visit_data,
                            _1,
                            _2,
                            _3);

    FetchDataFromUrl(GetVideoUrl(media_id), callback);
  } else {
    ledger::VisitData updated_visit_data(visit_data);
    updated_visit_data.name = publisher_info->name;
    updated_visit_data.url = publisher_info->url;
    updated_visit_data.provider = VIMEO_MEDIA_TYPE;
    updated_visit_data.favicon_url = publisher_info->favicon_url;

    ledger::MediaEventInfo old_event;
    std::map<std::string, ledger::MediaEventInfo>::const_iterator iter =
        events.find(media_key);
    if (iter != events.end()) {
      old_event = iter->second;
    }

    uint64_t duration = GetDuration(old_event, event_info);
    events[media_key] = event_info;

    ledger_->SaveMediaVisit(publisher_info->id,
                            updated_visit_data,
                            duration,
                            0);
  }
}

void MediaVimeo::OnPublisherVideoPage(
    const std::string& media_key,
    ledger::MediaEventInfo event_info,
    const ledger::VisitData& visit_data,
    int response_status_code,
    const std::string& response,
    const std::map<std::string, std::string>& headers) {
  ledger_->LogResponse(
      __func__,
      response_status_code,
      "HTML from Vimeo Video page",
      headers);

  const std::string user_id = GetIdFromVideoPage(response);
  const std::string publisher_key = GetPublisherKey(user_id);

  if (publisher_key.empty()) {
    return;
  }

  ledger::VisitData updated_visit_data(visit_data);
  updated_visit_data.favicon_url = GenerateFaviconUrl(user_id);
  updated_visit_data.provider = VIMEO_MEDIA_TYPE;
  updated_visit_data.name = GetNameFromVideoPage(response);
  updated_visit_data.url = GetPublisherUrl(response);

  ledger::MediaEventInfo old_event;
  std::map<std::string, ledger::MediaEventInfo>::const_iterator iter =
      events.find(media_key);
  if (iter != events.end()) {
    old_event = iter->second;
  }

  uint64_t duration = GetDuration(old_event, event_info);
  events[media_key] = event_info;

  ledger_->SaveMediaVisit(publisher_key,
                          updated_visit_data,
                          duration,
                          0);
  ledger_->SetMediaPublisherInfo(media_key, publisher_key);
}

}  // namespace braveledger_media
