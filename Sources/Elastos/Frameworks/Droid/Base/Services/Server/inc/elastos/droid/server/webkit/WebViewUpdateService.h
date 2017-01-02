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

#ifndef __ELASTOS_DROID_SERVER_WEBKIT_WEBVIEW_UPDATE_SERVICE_H__
#define __ELASTOS_DROID_SERVER_WEBKIT_WEBVIEW_UPDATE_SERVICE_H__

#include "elastos/droid/server/SystemService.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Webkit.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Webkit::IIWebViewUpdateService;

using Elastos::Droid::Server::SystemService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Webkit {

/**
 * Private service to wait for the updatable WebView to be ready for use.
 * @hide
 */
class WebViewUpdateService
    : public SystemService
{
public:
    class BinderService
        : public Object
        , public IIWebViewUpdateService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ISystemService* webViewUpdateService);

        /**
         * The shared relro process calls this to notify us that it's done trying to create a relro
         * file. This method gets called even if the relro creation has failed or the process
         * crashed.
         */
        //@Override // Binder call
        CARAPI NotifyRelroCreationCompleted(
            /* [in] */ Boolean is64Bit,
            /* [in] */ Boolean success);

        /**
         * WebViewFactory calls this to block WebView loading until the relro file is created.
         */
        //@Override // Binder call
        CARAPI WaitForRelroCreationCompleted(
            /* [in] */ Boolean is64Bit);

        CARAPI ToString(
            /* [out] */ String* str)
        {
            return Object::ToString(str);
        }

    private:
        WebViewUpdateService* mHost;
    };

private:
    class WebViewUpdatedReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WebViewUpdateService::WebViewUpdatedReceiver")

        WebViewUpdatedReceiver(
            /* [in] */ WebViewUpdateService* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WebViewUpdateService* mHost;
    };

public:
    WebViewUpdateService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnStart();

private:
    void OnWebViewUpdateInstalled();

private:
    static const String TAG;
    static const Int32 WAIT_TIMEOUT_MS; // Same as KEY_DISPATCHING_TIMEOUT.

    Boolean mRelroReady32Bit;
    Boolean mRelroReady64Bit;

    AutoPtr<BroadcastReceiver> mWebViewUpdatedReceiver;
};

}// Webkit
}// Server
}// Droid
}// Elastos

#endif //__ELASTOS_DROID_SERVER_WEBKIT_WEBVIEW_UPDATE_SERVICE_H__
