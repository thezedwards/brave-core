/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_SITE_SPECIFIC_BROWSER_SITE_SPECIFIC_SCRIPT_SERVICE_H_
#define BRAVE_COMPONENTS_BRAVE_SITE_SPECIFIC_BROWSER_SITE_SPECIFIC_SCRIPT_SERVICE_H_

#include <string>

#include "base/macros.h"
#include "build/build_config.h"
#include "components/keyed_service/core/keyed_service.h"
#include "components/sessions/core/session_id.h"
#include "url/gurl.h"

namespace brave_site_specific {

class SiteSpecificScriptService : public KeyedService {
 public:
  SiteSpecificScriptService() = default;

  virtual bool ScriptsFor(const GURL& primary_url, std::vector<std::string>* scripts) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(SiteSpecificScriptService);
};

}  // namespace brave_site_specific

#endif  // BRAVE_COMPONENTS_BRAVE_SITE_SPECIFIC_BROWSER_SITE_SPECIFIC_SCRIPT_SERVICE_H_
