/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVELEDGER_MEDIA_VIMEO_H_
#define BRAVELEDGER_MEDIA_VIMEO_H_

#include <map>
#include <memory>
#include <string>

#include "base/gtest_prod_util.h"
#include "bat/ledger/ledger.h"
#include "bat/ledger/internal/media/helper.h"

namespace bat_ledger {
class LedgerImpl;
}

namespace braveledger_media {

class MediaVimeo : public ledger::LedgerCallbackHandler {
 public:
  explicit MediaVimeo(bat_ledger::LedgerImpl* ledger);

  ~MediaVimeo() override;

  void ProcessMedia(const std::map<std::string, std::string>& parts,
                    const ledger::VisitData& visit_data);

  static std::string GetLinkType(const std::string& url);

  void ProcessActivityFromUrl(uint64_t window_id,
                              const ledger::VisitData& visit_data);

 private:
  static std::string GetVideoUrl(const std::string& video_id);

  static std::string GetMediaKey(const std::string& video_id,
                                 const std::string& type);

  static std::string GetPublisherKey(const std::string& key);

  static std::string GetIdFromVideoPage(const std::string& data);

  static std::string GenerateFaviconUrl(const std::string& id);

  static std::string GetNameFromVideoPage(const std::string& data);

  static std::string GetPublisherUrl(const std::string& data);

  static bool AllowedEvent(const std::string& event);

  static uint64_t GetDuration(const ledger::MediaEventInfo& old_event,
                              const ledger::MediaEventInfo& new_event);

  void FetchDataFromUrl(
    const std::string& url,
    braveledger_media::FetchDataFromUrlCallback callback);

  void OnMediaPublisherInfo(
    const std::string& media_id,
    const std::string& media_key,
    const ledger::MediaEventInfo& event_info,
    const ledger::VisitData& visit_data,
    ledger::Result result,
    std::unique_ptr<ledger::PublisherInfo> publisher_info);

  void OnPublisherVideoPage(
    const std::string& media_key,
    ledger::MediaEventInfo event_info,
    const ledger::VisitData& visit_data,
    int response_status_code,
    const std::string& response,
    const std::map<std::string, std::string>& headers);

  bat_ledger::LedgerImpl* ledger_;  // NOT OWNED
  std::map<std::string, ledger::MediaEventInfo> events;

  // For testing purposes
  friend class MediaVimeoTest;
  FRIEND_TEST_ALL_PREFIXES(MediaVimeoTest, GetVideoUrl);
  FRIEND_TEST_ALL_PREFIXES(MediaVimeoTest, GetMediaKey);
  FRIEND_TEST_ALL_PREFIXES(MediaVimeoTest, GetPublisherKey);
  FRIEND_TEST_ALL_PREFIXES(MediaVimeoTest, GetIdFromVideoPage);
  FRIEND_TEST_ALL_PREFIXES(MediaVimeoTest, GenerateFaviconUrl);
  FRIEND_TEST_ALL_PREFIXES(MediaVimeoTest, GetNameFromVideoPage);
  FRIEND_TEST_ALL_PREFIXES(MediaVimeoTest, GetPublisherUrl);
  FRIEND_TEST_ALL_PREFIXES(MediaVimeoTest, AllowedEvent);
  FRIEND_TEST_ALL_PREFIXES(MediaVimeoTest, GetDuration);
};

}  // namespace braveledger_media

#endif  // BRAVELEDGER_MEDIA_VIMEO_H_
