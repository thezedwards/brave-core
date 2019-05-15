/* Copyright 2016 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_SITE_SPECIFIC_BROWSER_SITE_SPECIFIC_SCRIPT_SERVICE_IMPL_H_
#define BRAVE_COMPONENTS_BRAVE_SITE_SPECIFIC_BROWSER_SITE_SPECIFIC_SCRIPT_SERVICE_IMPL_H_

#include <string>
#include <vector>

#include "base/macros.h"
#include "brave/components/brave_site_specific/browser/site_specific_script_service.h"
#include "url/gurl.h"

class Profile;

namespace brave_site_specific {

class SiteSpecificScriptServiceImpl
    : public SiteSpecificScriptService {
 public:
  explicit SiteSpecificScriptServiceImpl(Profile *profile);
  ~SiteSpecificScriptServiceImpl() override;

  // SiteSpecificScriptService overrides
  bool ScriptsFor(const GURL& primary_url, std::vector<std::string>* scripts) override;

 private:
  Profile* profile_;  // NOT OWNED

  DISALLOW_COPY_AND_ASSIGN(SiteSpecificScriptServiceImpl);
};

}  // namespace brave_site_specific

#endif  // BRAVE_COMPONENTS_BRAVE_SITE_SPECIFIC_BROWSER_SITE_SPECIFIC_SCRIPT_SERVICE_IMPL_H_
