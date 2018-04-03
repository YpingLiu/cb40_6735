/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ANDROID_WEBVIEW_BROWSER_AW_DOWNLOAD_MANAGER_DELEGATE_H_
#define ANDROID_WEBVIEW_BROWSER_AW_DOWNLOAD_MANAGER_DELEGATE_H_

#include "content/public/browser/download_manager_delegate.h"

#include "android_webview/native/aw_contents.h"
#include "content/public/browser/download_item.h"

namespace android_webview {

// Android WebView does not use Chromium downloads, so implement methods here to
// unconditionally cancel the download.
class AwDownloadManagerDelegate : public content::DownloadManagerDelegate {
 public:

  class SavePageReceiver : public AwContents::Receiver {
   public:
    virtual ~SavePageReceiver();
    SavePageReceiver(const content::SavePackagePathPickedCallback& callback, const std::string file);
    virtual void SetSaveDir(const std::string& path) OVERRIDE;

    content::SavePackagePathPickedCallback callback_;
    std::string file_;
  };

  class DownloadObserver : public content::DownloadItem::Observer {
   public:
    virtual ~DownloadObserver();
    virtual void OnDownloadUpdated(content::DownloadItem* download) OVERRIDE;
    //virtual void OnDownloadOpened(DownloadItem* download) OVERRIDE;
    virtual void OnDownloadRemoved(content::DownloadItem* download) OVERRIDE;
    virtual void OnDownloadDestroyed(content::DownloadItem* download) OVERRIDE;
  };

  AwDownloadManagerDelegate();
  virtual ~AwDownloadManagerDelegate();

  virtual void ChooseSavePath(
      content::WebContents* web_contents,
      const base::FilePath& suggested_path,
      const base::FilePath::StringType& default_extension,
      bool can_save_as_complete,
      const content::SavePackagePathPickedCallback& callback) OVERRIDE;

  virtual void GetSaveDir(content::BrowserContext* browser_context,
                          base::FilePath* website_save_dir,
                          base::FilePath* download_save_dir,
                          bool* skip_dir_check) OVERRIDE;

  // content::DownloadManagerDelegate implementation.
  virtual bool DetermineDownloadTarget(
      content::DownloadItem* item,
      const content::DownloadTargetCallback& callback) OVERRIDE;
  virtual bool ShouldCompleteDownload(
      content::DownloadItem* item,
      const base::Closure& complete_callback) OVERRIDE;
  virtual bool ShouldOpenDownload(
      content::DownloadItem* item,
      const content::DownloadOpenDelayedCallback& callback) OVERRIDE;
  virtual void GetNextId(const content::DownloadIdCallback& callback) OVERRIDE;

  uint32 next_download_id_;

};

}  // namespace android_webview

#endif  // ANDROID_WEBVIEW_BROWSER_AW_DOWNLOAD_MANAGER_DELEGATE_H_
