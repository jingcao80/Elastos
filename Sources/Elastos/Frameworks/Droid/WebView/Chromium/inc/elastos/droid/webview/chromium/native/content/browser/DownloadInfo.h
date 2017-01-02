//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DOWNLOADINFO_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DOWNLOADINFO_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Class representing the state of a single download.
  */
class DownloadInfo : public Object
{
public:
    class Builder : public Object
    {
        friend class DownloadInfo;
    public:
        virtual CARAPI_(AutoPtr<Builder>) SetUrl(
            /* [in] */ const String& url);

        virtual CARAPI_(AutoPtr<Builder>) SetUserAgent(
            /* [in] */ const String& userAgent);

        virtual CARAPI_(AutoPtr<Builder>) SetMimeType(
            /* [in] */ const String& mimeType);

        virtual CARAPI_(AutoPtr<Builder>) SetCookie(
            /* [in] */ const String& cookie);

        virtual CARAPI_(AutoPtr<Builder>) SetFileName(
            /* [in] */ const String& fileName);

        virtual CARAPI_(AutoPtr<Builder>) SetDescription(
            /* [in] */ const String& description);

        virtual CARAPI_(AutoPtr<Builder>) SetFilePath(
            /* [in] */ const String& filePath);

        virtual CARAPI_(AutoPtr<Builder>) SetReferer(
            /* [in] */ const String& referer);

        virtual CARAPI_(AutoPtr<Builder>) SetContentLength(
            /* [in] */ Int64 contentLength);

        virtual CARAPI_(AutoPtr<Builder>) SetIsGETRequest(
            /* [in] */ Boolean isGETRequest);

        virtual CARAPI_(AutoPtr<Builder>) SetHasDownloadId(
            /* [in] */ Boolean hasDownloadId);

        virtual CARAPI_(AutoPtr<Builder>) SetDownloadId(
            /* [in] */ Int32 downloadId);

        virtual CARAPI_(AutoPtr<Builder>) SetIsSuccessful(
            /* [in] */ Boolean isSuccessful);

        virtual CARAPI_(AutoPtr<Builder>) SetContentDisposition(
            /* [in] */ const String& contentDisposition);

        virtual CARAPI_(AutoPtr<Builder>) SetPercentCompleted(
            /* [in] */ Int32 percentCompleted);

        virtual CARAPI_(AutoPtr<Builder>) SetTimeRemainingInMillis(
            /* [in] */ Int64 timeRemainingInMillis);

        virtual CARAPI_(AutoPtr<DownloadInfo>) Build();

        /**
          * Create a builder from the DownloadInfo object.
          * @param downloadInfo DownloadInfo object from which builder fields are populated.
          * @return A builder initialized with fields from downloadInfo object.
          */
        static CARAPI_(AutoPtr<Builder>) FromDownloadInfo(
            /* [in] */ /*const*/ DownloadInfo* downloadInfo);

    private:
        String mUrl;
        String mUserAgent;
        String mMimeType;
        String mCookie;
        String mFileName;
        String mDescription;
        String mFilePath;
        String mReferer;
        Int64 mContentLength;
        Boolean mIsGETRequest;
        Boolean mHasDownloadId;
        Int32 mDownloadId;
        Boolean mIsSuccessful;
        String mContentDisposition;
        Int32 mPercentCompleted;
        Int64 mTimeRemainingInMillis;
    };

public:
    virtual CARAPI_(String) GetUrl();

    virtual CARAPI_(String) GetUserAgent();

    virtual CARAPI_(String) GetMimeType();

    virtual CARAPI_(String) GetCookie();

    virtual CARAPI_(String) GetFileName();

    virtual CARAPI_(String) GetDescription();

    virtual CARAPI_(String) GetFilePath();

    virtual CARAPI_(String) GetReferer();

    virtual CARAPI_(Int64) GetContentLength();

    virtual CARAPI_(Boolean) IsGETRequest();

    virtual CARAPI_(Boolean) HasDownloadId();

    virtual CARAPI_(Int32) GetDownloadId();

    virtual CARAPI_(Boolean) IsSuccessful();

    virtual CARAPI_(String) GetContentDisposition();

    /**
      * @return percent completed as an integer, -1 if there is no download progress.
      */
    virtual CARAPI_(Int32) GetPercentCompleted();

    virtual CARAPI_(Int64) GetTimeRemainingInMillis();

private:
    DownloadInfo(
        /* [in] */ Builder* builder);

private:
    const String mUrl;
    const String mUserAgent;
    const String mMimeType;
    const String mCookie;
    const String mFileName;
    const String mDescription;
    const String mFilePath;
    const String mReferer;
    const Int64 mContentLength;
    const Boolean mHasDownloadId;
    const Int32 mDownloadId;
    const String mContentDisposition;
    const Boolean mIsGETRequest;
    const Boolean mIsSuccessful;
    const Int32 mPercentCompleted;
    const Int64 mTimeRemainingInMillis;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DOWNLOADINFO_H_

