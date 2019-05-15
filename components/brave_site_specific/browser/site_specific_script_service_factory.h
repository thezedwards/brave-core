/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_SITE_SPECIFIC_BROWSER_SITE_SPECIFIC_SCRIPT_SERVICE_FACTORY_H_
#define BRAVE_COMPONENTS_BRAVE_SITE_SPECIFIC_BROWSER_SITE_SPECIFIC_SCRIPT_SERVICE_FACTORY_H_

#include "base/macros.h"
#include "base/memory/singleton.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"

class Profile;

namespace brave_site_specific {

class SiteSpecificScriptService;

class SiteSpecificScriptServiceFactory
  : public BrowserContextKeyedServiceFactory {
 public:
  static SiteSpecificScriptService* GetForProfile(Profile* profile);
  static SiteSpecificScriptService* GetForBrowserContext(
      content::BrowserContext* context);
  static SiteSpecificScriptServiceFactory* GetInstance();

 private:
  friend struct base::DefaultSingletonTraits<SiteSpecificScriptServiceFactory>;

  SiteSpecificScriptServiceFactory();
  ~SiteSpecificScriptServiceFactory() override;

  // BrowserContextKeyedServiceFactory:
  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* context) const override;
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
  bool ServiceIsNULLWhileTesting() const override;

  DISALLOW_COPY_AND_ASSIGN(SiteSpecificScriptServiceFactory);
};

} // namespace brave_site_specific

#endif // BRAVE_COMPONENTS_BRAVE_SITE_SPECIFIC_BROWSER_SITE_SPECIFIC_SERVICE_FACTORY_H_
