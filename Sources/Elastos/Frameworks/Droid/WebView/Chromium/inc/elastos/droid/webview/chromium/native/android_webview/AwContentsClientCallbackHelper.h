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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTSCLIENTCALLBACKHELPER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTSCLIENTCALLBACKHELPER_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClient.h"

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::Concurrent::ICallable;

// import com.google.common.annotations.VisibleForTesting;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * This class is responsible for calling certain client callbacks on the UI thread.
 *
 * Most callbacks do no go through here, but get forwarded to AwContentsClient directly. The
 * messages processed here may originate from the IO or UI thread.
 */
class AwContentsClientCallbackHelper
{
public:
    // TODO(boliu): Consider removing DownloadInfo and LoginRequestInfo by using native
    // MessageLoop to post directly to AwContents.

    class DownloadInfo
        : public Object
    {
    public:
        DownloadInfo(
            /* [in] */ const String& url,
            /* [in] */ const String& userAgent,
            /* [in] */ const String& contentDisposition,
            /* [in] */ const String& mimeType,
            /* [in] */ Int64 contentLength);

    public:
        const String mUrl;
        const String mUserAgent;
        const String mContentDisposition;
        const String mMimeType;
        const Int64 mContentLength;
    };

    class LoginRequestInfo
        : public Object
    {
    public:
        LoginRequestInfo(
            /* [in] */ const String& realm,
            /* [in] */ const String& account,
            /* [in] */ const String& args);

    public:
        const String mRealm;
        const String mAccount;
        const String mArgs;
    };

    class OnReceivedErrorInfo
        : public Object
    {
    public:
        OnReceivedErrorInfo(
            /* [in] */ Int32 errorCode,
            /* [in] */ const String& description,
            /* [in] */ const String& failingUrl);

    public:
        const Int32 mErrorCode;
        const String mDescription;
        const String mFailingUrl;
    };

    class MyHandler : public Handler
    {
    public:
        //@Override
        CARAPI HandleMessage(

            /* [in] */ IMessage* msg);

        MyHandler(
            /* [in] */ AwContentsClientCallbackHelper* owner,
            /* [in] */ ILooper* looper);
    private:
        AwContentsClientCallbackHelper* mOwner;
    };

public:
    AwContentsClientCallbackHelper(
        /* [in] */ ILooper* looper,
        /* [in] */ AwContentsClient* contentsClient);

    virtual CARAPI_(void) PostOnLoadResource(
        /* [in] */ const String& url);

    virtual CARAPI_(void) PostOnPageStarted(
        /* [in] */ const String& url);

    virtual CARAPI_(void) PostOnDownloadStart(
        /* [in] */ const String& url,
        /* [in] */ const String& userAgent,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& mimeType,
        /* [in] */ Int64 contentLength);

    virtual CARAPI_(void) PostOnReceivedLoginRequest(
        /* [in] */ const String& realm,
        /* [in] */ const String& account,
        /* [in] */ const String& args);

    virtual CARAPI_(void) PostOnReceivedError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& description,
        /* [in] */ const String& failingUrl);

    virtual CARAPI_(void) PostOnNewPicture(
        /* [in] */ ICallable* pictureProvider);

    virtual CARAPI_(void) PostOnScaleChangedScaled(
        /* [in] */ Float oldScale,
        /* [in] */ Float newScale);

private:
    AwContentsClientCallbackHelper(const AwContentsClientCallbackHelper&);
    AwContentsClientCallbackHelper &operator= (const AwContentsClientCallbackHelper&);
    Int32 FloatToIntBits(
        /* [in] */ Float value);
    static const Int32 MSG_ON_LOAD_RESOURCE = 1;
    static const Int32 MSG_ON_PAGE_STARTED = 2;
    static const Int32 MSG_ON_DOWNLOAD_START = 3;
    static const Int32 MSG_ON_RECEIVED_LOGIN_REQUEST = 4;
    static const Int32 MSG_ON_RECEIVED_ERROR = 5;
    static const Int32 MSG_ON_NEW_PICTURE = 6;
    static const Int32 MSG_ON_SCALE_CHANGED_SCALED = 7;

    // Minimum period allowed between consecutive onNewPicture calls, to rate-limit the callbacks.
    static const Int64 ON_NEW_PICTURE_MIN_PERIOD_MILLIS = 500;
    // Timestamp of the most recent onNewPicture callback.
    Int64 mLastPictureTime;
    // True when a onNewPicture callback is currenly in flight.
    Boolean mHasPendingOnNewPicture;

    AutoPtr<AwContentsClient> mContentsClient;

    AutoPtr<IHandler> mHandler;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTSCLIENTCALLBACKHELPER_H__
