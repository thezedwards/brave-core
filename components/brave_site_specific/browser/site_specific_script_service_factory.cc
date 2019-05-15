/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_site_specific/browser/site_specific_script_service_factory.h"

#include <memory>
#include <string>

#include "base/memory/singleton.h"
#include "brave/components/brave_site_specific/browser/site_specific_script_service.h"
#include "brave/components/brave_site_specific/browser/site_specific_script_service_impl.h"
#include "chrome/browser/profiles/incognito_helpers.h"
#include "chrome/browser/profiles/profile.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/keyed_service/core/keyed_service.h"

namespace brave_site_specific {

// static
SiteSpecificScriptServiceFactory*
SiteSpecificScriptServiceFactory::GetInstance() {
  return base::Singleton<SiteSpecificScriptServiceFactory>::get();
}

// static
SiteSpecificScriptService*
SiteSpecificScriptServiceFactory::GetForProfile(Profile* profile) {
  if (profile->IsOffTheRecord())
    return NULL;

  return static_cast<SiteSpecificScriptService*>(
      GetInstance()->GetServiceForBrowserContext(profile, true));
}

SiteSpecificScriptServiceFactory::SiteSpecificScriptServiceFactory()
    : BrowserContextKeyedServiceFactory(
        "SiteSpecificScriptService",
        BrowserContextDependencyManager::GetInstance()) {
}

SiteSpecificScriptServiceFactory::
~SiteSpecificScriptServiceFactory() = default;

KeyedService* SiteSpecificScriptServiceFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) const {
  std::unique_ptr<SiteSpecificScriptServiceImpl>
    site_specific_script_service(
      new SiteSpecificScriptServiceImpl(
        Profile::FromBrowserContext(context)));
  return site_specific_script_service.release();
}

content::BrowserContext*
SiteSpecificScriptServiceFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return chrome::GetBrowserContextRedirectedInIncognito(context);
}

bool SiteSpecificScriptServiceFactory::ServiceIsNULLWhileTesting() const {
  return false;
}

}  // namespace brave_site_specific
