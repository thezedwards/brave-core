/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_GREASELION_BROWSER_GREASELION_TAB_HELPER_H_
#define BRAVE_COMPONENTS_GREASELION_BROWSER_GREASELION_TAB_HELPER_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "base/macros.h"
#include "base/synchronization/lock.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"

namespace content {
class WebContents;
}

namespace greaselion {

class GreaselionTabHelper : public content::WebContentsObserver,
    public content::WebContentsUserData<GreaselionTabHelper> {
 public:
  explicit GreaselionTabHelper(content::WebContents*);
  ~GreaselionTabHelper() override;

 protected:
  // content::WebContentsObserver overrides.
  void DocumentLoadedInFrame(content::RenderFrameHost* render_frame_host)
      override;

 private:
  friend class content::WebContentsUserData<GreaselionTabHelper>;

  WEB_CONTENTS_USER_DATA_KEY_DECL();
  DISALLOW_COPY_AND_ASSIGN(GreaselionTabHelper);
};

}  // namespace greaselion

#endif  // BRAVE_COMPONENTS_GREASELION_BROWSER_GREASELION_TAB_HELPER_H_
