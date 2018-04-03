/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "android_webview/browser/aw_download_manager_delegate.h"

#include "base/files/file_path.h"
#include "content/public/browser/download_danger_type.h"
#include "content/public/browser/download_item.h"

#include "base/bind.h"
#include "content/public/browser/browser_context.h"

namespace android_webview {

AwDownloadManagerDelegate::~AwDownloadManagerDelegate() {
}

AwDownloadManagerDelegate::AwDownloadManagerDelegate()
    : next_download_id_(content::DownloadItem::kInvalidId) {
}

void OnSavePackageDownloadCreated(content::DownloadItem* download) {
  LOG(WARNING) << "SP>>> DownloadCreated Callback";
  AwContents* contents = AwContents::FromWebContents(download->GetWebContents());
  contents->OnSavePageStart(download->GetId(), download->GetFullPath().value());
  LOG(WARNING) << "MM>>> DownloadObserver Construction";
  download->AddObserver(new AwDownloadManagerDelegate::DownloadObserver());
}

AwDownloadManagerDelegate::DownloadObserver::~DownloadObserver() {
  LOG(WARNING) << "MM>>> DownloadObserver Destruction";
}

void AwDownloadManagerDelegate::DownloadObserver::OnDownloadUpdated(content::DownloadItem* download) {
  LOG(WARNING) << "SP>>> DownloadItem OnDownloadUpdated";
  AwContents* contents = AwContents::FromWebContents(download->GetWebContents());
  if (!contents || contents->IsDestroy()) {
    LOG(WARNING) << "SP>>> AwContents was Destroy";
    download->RemoveObserver(this);
    delete this;
    return;
  }
  if (download->IsDone()) {
    contents->OnSaveFinish(download->GetId(), download->GetState());
    download->RemoveObserver(this);
    delete this;
    return;
  }
  contents->OnSaveProgressChange(download->GetId(), download->PercentComplete());
}

void AwDownloadManagerDelegate::DownloadObserver::OnDownloadRemoved(content::DownloadItem* download) {
  LOG(WARNING) << "SP>>> DownloadItem OnDownloadRemoved";
}

void AwDownloadManagerDelegate::DownloadObserver::OnDownloadDestroyed(content::DownloadItem* donwload) {
  LOG(WARNING) << "SP>>> DownloadItem OnDownloadDestroyed";
}

AwDownloadManagerDelegate::SavePageReceiver::~SavePageReceiver() {
  LOG(WARNING) << "MM>>> SavePageReceiver Destruction";
}

void AwDownloadManagerDelegate::SavePageReceiver::SetSaveDir(const std::string& path) {
  LOG(WARNING) << "SP>>> path:" << path;
  callback_.Run(base::FilePath(path + file_),
                     content::SAVE_PAGE_TYPE_AS_COMPLETE_HTML,
                     base::Bind(&OnSavePackageDownloadCreated));
  delete this;
}

AwDownloadManagerDelegate::SavePageReceiver::SavePageReceiver(const content::SavePackagePathPickedCallback& callback,
                                                 const std::string file)
    : callback_(callback),
      file_(file) {
}


void AwDownloadManagerDelegate::ChooseSavePath(
    content::WebContents* web_contents,
    const base::FilePath& suggested_path,
    const base::FilePath::StringType& default_extension,
    bool can_save_as_complete,
    const content::SavePackagePathPickedCallback& callback) {
  AwContents* aw_contents = AwContents::FromWebContents(web_contents);
  LOG(WARNING) << "MM>>> SavePageReceiver Construction";
  aw_contents->SetReceiver(new AwDownloadManagerDelegate::SavePageReceiver(callback, suggested_path.value()));
  aw_contents->GetSaveDir(can_save_as_complete);
}

void AwDownloadManagerDelegate::GetSaveDir(
    content::BrowserContext* browser_context,
    base::FilePath* website_save_dir,
    base::FilePath* download_save_dir,
    bool* skip_dir_check) {
  // not implements
}

bool AwDownloadManagerDelegate::DetermineDownloadTarget(
    content::DownloadItem* item,
    const content::DownloadTargetCallback& callback) {
  // Note this cancel is independent of the URLRequest cancel in
  // AwResourceDispatcherHostDelegate::DownloadStarting. The request
  // could have already finished by the time DownloadStarting is called.
  callback.Run(base::FilePath() /* Empty file path for cancel */,
               content::DownloadItem::TARGET_DISPOSITION_OVERWRITE,
               content::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS,
               base::FilePath());
  return true;
}

bool AwDownloadManagerDelegate::ShouldCompleteDownload(
    content::DownloadItem* item,
    const base::Closure& complete_callback) {
  NOTREACHED();
  return true;
}

bool AwDownloadManagerDelegate::ShouldOpenDownload(
    content::DownloadItem* item,
    const content::DownloadOpenDelayedCallback& callback) {
  NOTREACHED();
  return true;
}

void AwDownloadManagerDelegate::GetNextId(
    const content::DownloadIdCallback& callback) {
  //static uint32 next_id = content::DownloadItem::kInvalidId + 1;
  //callback.Run(next_id++);
  callback.Run(next_download_id_++);
}

}  // namespace android_webview
