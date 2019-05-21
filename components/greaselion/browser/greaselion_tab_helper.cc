/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/greaselion/browser/greaselion_tab_helper.h"

#include "base/strings/utf_string_conversions.h"
#include "brave/browser/brave_browser_process_impl.h"
#include "brave/common/brave_isolated_worlds.h"
#include "brave/components/greaselion/browser/greaselion_download_service.h"
#include "brave/components/greaselion/browser/greaselion_service_factory.h"
#include "brave/components/greaselion/browser/greaselion_service.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_user_data.h"

using greaselion::GreaselionService;
using greaselion::GreaselionServiceFactory;

namespace greaselion {

GreaselionTabHelper::~GreaselionTabHelper() {
}

GreaselionTabHelper::GreaselionTabHelper(
    content::WebContents* web_contents)
    : WebContentsObserver(web_contents) {
}

void GreaselionTabHelper::DocumentLoadedInFrame(
  content::RenderFrameHost* render_frame_host) {
  const GURL& url = render_frame_host->GetLastCommittedURL();
  std::vector<std::string> scripts;
  GreaselionService* greaselion_service =
    GreaselionServiceFactory::GetForBrowserContext(
      web_contents()->GetBrowserContext());
  if (!greaselion_service->ScriptsFor(url, &scripts))
    return;

  for (auto script : scripts) {
    render_frame_host->ExecuteJavaScriptInIsolatedWorld(
      base::UTF8ToUTF16(script),
      base::DoNothing(),
      ISOLATED_WORLD_ID_GREASELION);
  }
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(GreaselionTabHelper)

}  // namespace brave_shields
