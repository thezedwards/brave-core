/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_site_specific/browser/site_specific_script_service_impl.h"

#include "brave/browser/brave_browser_process_impl.h"
#include "brave/components/brave_rewards/common/pref_names.h"
#include "brave/components/brave_site_specific/browser/site_specific_script_config_service.h"
#include "components/prefs/pref_service.h"
#include "chrome/browser/profiles/profile.h"

namespace brave_site_specific {

SiteSpecificScriptServiceImpl::SiteSpecificScriptServiceImpl(Profile* profile) :
    profile_(profile) {
  DCHECK(!profile_->IsOffTheRecord());
}

SiteSpecificScriptServiceImpl::~SiteSpecificScriptServiceImpl() = default;

bool SiteSpecificScriptServiceImpl::ScriptsFor(
    const GURL& primary_url,
    std::vector<std::string>* scripts) {
  auto rewards_enabled = profile_->GetPrefs()->GetBoolean(
    brave_rewards::prefs::kBraveRewardsEnabled);
  bool any = false;
  std::vector<std::unique_ptr<SiteSpecificScriptRule>>* rules =
    g_brave_browser_process->site_specific_script_config_service()->rules();
  scripts->clear();
  for (const auto& rule : *rules) {
    if (rule->Matches(primary_url, rewards_enabled)) {
      rule->Populate(scripts);
      any = true;
    }
  }
  return any;
}

}  // namespace brave_site_specific
