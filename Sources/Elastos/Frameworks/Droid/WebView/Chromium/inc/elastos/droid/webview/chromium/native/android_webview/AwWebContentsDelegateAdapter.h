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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWWEBCONTENTSDELEGATEADAPTER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWWEBCONTENTSDELEGATEADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Webkit.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwWebContentsDelegate.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClient.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Webkit::IValueCallback;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewCore;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Adapts the AwWebContentsDelegate interface to the AwContentsClient interface.
 * This class also serves a secondary function of routing certain callbacks from the content layer
 * to specific listener interfaces.
 */
class AwWebContentsDelegateAdapter : public AwWebContentsDelegate
{
public:
    class GetDisplayNameTask : public AsyncTask
    {
    public:
        GetDisplayNameTask(
            /* [in] */ IContentResolver* contentResolver,
            /* [in] */ Int32 processId,
            /* [in] */ Int32 renderId,
            /* [in] */ Int32 modeFlags,
            /* [in] */ ArrayOf<String>* filePaths);

    public:
        const Int32 mProcessId;
        const Int32 mRenderId;
        const Int32 mModeFlags;
        AutoPtr<ArrayOf<String> > mFilePaths;
        AutoPtr<IContentResolver> mContentResolver;

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [in] */ IInterface** result);

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        /**
         * @return the display name of a path if it is a content URI and is present in the database
         * or an empty string otherwise.
         */
        CARAPI_(String) ResolveFileName(
            /* [in] */ const String& filePath);
    };

    class InnerHandler
        : public Handler
    {
    public:
        InnerHandler(
            /* [in] */ AwWebContentsDelegateAdapter* owner,
            /* [in] */ ILooper* looper,
            /* [in] */ ContentViewCore* contentViewCore);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AwWebContentsDelegateAdapter* mOwner;
        ContentViewCore* mContentViewCore;
    };

    class InnerValueCallback
        : public Object
        , public IValueCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerValueCallback(
            /* [in] */ AwWebContentsDelegateAdapter* owner,
            /* [in] */ Int32 processId,
            /* [in] */ Int32 renderId,
            /* [in] */ Int32 modeFlags,
            /* [in] */ Boolean completed);

        CARAPI OnReceiveValue(
            /* [in] */ IInterface* results);
    private:
        AwWebContentsDelegateAdapter* mOwner;
        Int32 mProcessId;
        Int32 mRenderId;
        Int32 mModeFlags;
        Boolean mCompleted;
    };

public:
    AutoPtr<AwContentsClient> mContentsClient;
    AutoPtr<IView> mContainerView;
    AutoPtr<IContext> mContext;

public:
    AwWebContentsDelegateAdapter(
        /* [in] */ AwContentsClient* contentsClient,
        /* [in] */ IView* containerView,
        /* [in] */ IContext* context);

    CARAPI_(void) SetContainerView(
        /* [in] */ IView* containerView);

    //@Override
    CARAPI OnLoadProgressChanged(
        /* [in] */ Int32 progress);

    //@Override
    CARAPI HandleKeyboardEvent(
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) TakeFocus(
        /* [in] */ Boolean reverse);

    //@Override
    CARAPI_(Boolean) AddMessageToConsole(
        /* [in] */ Int32 level,
        /* [in] */ const String& message,
        /* [in] */ Int32 lineNumber,
        /* [in] */ const String& sourceId);

    //@Override
    CARAPI OnUpdateUrl(
        /* [in] */ const String& url);

    //@Override
    CARAPI OpenNewTab(
        /* [in] */ const String& url,
        /* [in] */ const String& extraHeaders,
        /* [in] */ ArrayOf<Byte>* postData,
        /* [in] */ Int32 disposition,
        /* [in] */ Boolean isRendererInitiated);

    //@Override
    CARAPI CloseContents();

    //@Override
    CARAPI ShowRepostFormWarningDialog(
        /* [in] */ ContentViewCore* contentViewCore);

    //@Override
    CARAPI_(void) RunFileChooser(
        /* [in] */ Int32 processId,
        /* [in] */ Int32 renderId,
        /* [in] */ Int32 modeFlags,
        /* [in] */ const String& acceptTypes,
        /* [in] */ const String& title,
        /* [in] */ const String& defaultFilename,
        /* [in] */ Boolean capture);

    //@Override
    CARAPI_(Boolean) AddNewContents(
        /* [in] */ Boolean isDialog,
        /* [in] */ Boolean isUserGesture);

    //@Override
    CARAPI ActivateContents();

    //@Override
    CARAPI ToggleFullscreenModeForTab(
        /* [in] */ Boolean enterFullscreen);

private:
    static const String TAG;
    static const Int32 MSG_CONTINUE_PENDING_RELOAD;
    static const Int32 MSG_CANCEL_PENDING_RELOAD;


    CARAPI_(Boolean) TryToMoveFocus(
        /* [in] */ Int32 direction);
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWWEBCONTENTSDELEGATEADAPTER_H__
