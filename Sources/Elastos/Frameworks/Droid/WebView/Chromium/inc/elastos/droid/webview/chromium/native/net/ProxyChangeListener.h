// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_PROXYCHANGELISTENER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_PROXYCHANGELISTENER_H_

#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

/**
  * This class partners with native ProxyConfigServiceAndroid to listen for
  * proxy change notifications from Android.
  */
// @JNINamespace("net")
class ProxyChangeListener
    : public Object
{
public:
    class Delegate
        : public Object
    {
    public:
        virtual CARAPI ProxySettingsChanged() = 0;
    };

private:
    class ProxyConfig
        : public Object
    {
    public:
        ProxyConfig(
            /* [in] */ const String& host,
            /* [in] */ Int32 port);

    public:
        const String mHost;
        const Int32 mPort;
    };

    class ProxyReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("ProxyChangeListener::ProxyReceiver")

        ProxyReceiver(
            /* [in] */ ProxyChangeListener* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        // Extract a ProxyConfig object from the supplied Intent's extra data
        // bundle. The android.net.ProxyProperties class is not exported from
        // tne Android SDK, so we have to use reflection to get at it and invoke
        // methods on it. If we fail, return an empty proxy config (meaning
        // 'direct').
        // TODO(sgurun): once android.net.ProxyInfo is public, rewrite this.
        CARAPI_(AutoPtr<ProxyChangeListener::ProxyConfig>) ExtractNewProxy(
            /* [in] */ IIntent* intent);

    private:
        ProxyChangeListener* mOwner;
    };

public:
    static CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    virtual CARAPI SetDelegateForTesting(
        /* [in] */ Delegate* delegate);

    // @CalledByNative
    static CARAPI_(AutoPtr<ProxyChangeListener>) Create(
        /* [in] */ IContext* context);

    // @CalledByNative
    static CARAPI_(String) GetProperty(
        /* [in] */ const String& property);

    // @CalledByNative
    virtual CARAPI Start(
        /* [in] */ Handle64 nativePtr);

    // @CalledByNative
    virtual CARAPI Stop();

    static CARAPI_(void*) ElaProxyChangeListenerCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ IInterface* context);

    static CARAPI_(void) Start(
        /* [in] */ IInterface* obj,
        /* [in] */ Handle64 nativePtr);

    static CARAPI_(void) Stop(
        /* [in] */ IInterface* obj);

private:
    ProxyChangeListener(
        /* [in] */ IContext* context);

    CARAPI ProxySettingsChanged(
        /* [in] */ ProxyChangeListener::ProxyConfig* cfg);

    CARAPI RegisterReceiver();

    CARAPI UnregisterReceiver();

    /**
      * See net/proxy/proxy_config_service_android.cc
      */
    // @NativeClassQualifiedName("ProxyConfigServiceAndroid::JNIDelegate")
    CARAPI NativeProxySettingsChangedTo(
        /* [in] */ Handle64 nativePtr,
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    // @NativeClassQualifiedName("ProxyConfigServiceAndroid::JNIDelegate")
    CARAPI NativeProxySettingsChanged(
        /* [in] */ Handle64 nativePtr);

private:
    static const String TAG;
    static Boolean sEnabled;
    Handle64 mNativePtr;
    AutoPtr<IContext> mContext;
    AutoPtr<ProxyReceiver> mProxyReceiver;
    AutoPtr<Delegate> mDelegate;
};

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_PROXYCHANGELISTENER_H_

