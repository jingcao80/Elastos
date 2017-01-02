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

// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_NETWORKCHANGENOTIFIER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_NETWORKCHANGENOTIFIER_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/net/NetworkChangeNotifierAutoDetect.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Webkit::Webview::Chromium::Net::NetworkChangeNotifierAutoDetect;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

/**
  * Triggers updates to the underlying network state in Chrome.
  *
  * By default, connectivity is assumed and changes must pushed from the embedder via the
  * forceConnectivityState function.
  * Embedders may choose to have this class auto-detect changes in network connectivity by invoking
  * the setAutoDetectConnectivityState function.
  *
  * WARNING: This class is not thread-safe.
  */
// @JNINamespace("net")
class NetworkChangeNotifier
    : public Object
{
public:
    /**
      * Alerted when the connection type of the network changes.
      * The alert is fired on the UI thread.
      */
    class ConnectionTypeObserver
        : public Object
    {
    public:
        virtual CARAPI OnConnectionTypeChanged(
            /* [in] */ Int32 connectionType) = 0;
    };

    class InnerNetworkChangeNotifierAutoDetectObserver
        : public NetworkChangeNotifierAutoDetect::Observer
    {
    public:
        InnerNetworkChangeNotifierAutoDetectObserver(
            /* [in] */ NetworkChangeNotifier* owner);

        // @Override
        CARAPI OnConnectionTypeChanged(
            /* [in] */ Int32 newConnectionType);

    private:
        NetworkChangeNotifier* mOwner;
    };

public:
    /**
      * Initializes the singleton once.
      */
    // @CalledByNative
    static CARAPI_(AutoPtr<NetworkChangeNotifier>) Init(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsInitialized();

    static CARAPI ResetInstanceForTests(
        /* [in] */ IContext* context);

    // @CalledByNative
    virtual CARAPI_(Int32) GetCurrentConnectionType();

    /**
      * Adds a native-side observer.
      */
    // @CalledByNative
    virtual CARAPI AddNativeObserver(
        /* [in] */ Handle64 nativeChangeNotifier);

    /**
      * Removes a native-side observer.
      */
    // @CalledByNative
    virtual CARAPI RemoveNativeObserver(
        /* [in] */ Handle64 nativeChangeNotifier);

    /**
      * Returns the singleton instance.
      */
    static CARAPI_(AutoPtr<NetworkChangeNotifier>) GetInstance();

    /**
      * Enables auto detection of the current network state based on notifications from the system.
      * Note that passing true here requires the embedding app have the platform ACCESS_NETWORK_STATE
      * permission.
      *
      * @param shouldAutoDetect true if the NetworkChangeNotifier should listen for system changes in
      *    network connectivity.
      */
    static CARAPI SetAutoDetectConnectivityState(
        /* [in] */ Boolean shouldAutoDetect);

    /**
      * Updates the perceived network state when not auto-detecting changes to connectivity.
      *
      * @param networkAvailable True if the NetworkChangeNotifier should perceive a "connected"
      *    state, false implies "disconnected".
      */
    // @CalledByNative
    static CARAPI_(void) ForceConnectivityState(
        /* [in] */ Boolean networkAvailable);

    /**
      * Alerts all observers of a connection change.
      */
    virtual CARAPI NotifyObserversOfConnectionTypeChange(
        /* [in] */ Int32 newConnectionType);

    /**
      * Adds an observer for any connection type changes.
      */
    static CARAPI AddConnectionTypeObserver(
        /* [in] */ ConnectionTypeObserver* observer);

    /**
      * Removes an observer for any connection type changes.
      */
    static CARAPI RemoveConnectionTypeObserver(
        /* [in] */ ConnectionTypeObserver* observer);

    // For testing only.
    static CARAPI_(AutoPtr<NetworkChangeNotifierAutoDetect>) GetAutoDetectorForTest();

    /**
      * Checks if there currently is connectivity.
      */
    static CARAPI_(Boolean) IsOnline();

    static CARAPI_(void*) ElaNetworkChangeNotifierCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) Init(
        /* [in] */ IInterface* context);

    static CARAPI_(Int32) GetCurrentConnectionType(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) AddNativeObserver(
        /* [in] */ IInterface* obj,
        /* [in] */ Handle64 nativeChangeNotifier);

    static CARAPI_(void) RemoveNativeObserver(
        /* [in] */ IInterface* obj,
        /* [in] */ Handle64 nativeChangeNotifier);

private:
    NetworkChangeNotifier(
        /* [in] */ IContext* context);

    CARAPI DestroyAutoDetector();

    CARAPI SetAutoDetectConnectivityStateInternal(
        /* [in] */ Boolean shouldAutoDetect);

    CARAPI ForceConnectivityStateInternal(
        /* [in] */ Boolean forceOnline);

    CARAPI UpdateCurrentConnectionType(
        /* [in] */ Int32 newConnectionType);

    CARAPI AddConnectionTypeObserverInternal(
        /* [in] */ ConnectionTypeObserver* observer);

    CARAPI RemoveConnectionTypeObserverInternal(
        /* [in] */ ConnectionTypeObserver* observer);

    // @NativeClassQualifiedName("NetworkChangeNotifierDelegateAndroid")
    CARAPI NativeNotifyConnectionTypeChanged(
        /* [in] */ Handle64 nativePtr,
        /* [in] */ Int32 newConnectionType);

public:
    // These constants must always match the ones in network_change_notifier.h.
    static const Int32 CONNECTION_UNKNOWN = 0;
    static const Int32 CONNECTION_ETHERNET = 1;
    static const Int32 CONNECTION_WIFI = 2;
    static const Int32 CONNECTION_2G = 3;
    static const Int32 CONNECTION_3G = 4;
    static const Int32 CONNECTION_4G = 5;
    static const Int32 CONNECTION_NONE = 6;
    static const Int32 CONNECTION_BLUETOOTH = 7;
    AutoPtr<NetworkChangeNotifierAutoDetect> mAutoDetector;

private:
    /*const*/ AutoPtr<IContext> mContext;
    /*const*/ AutoPtr< List<Handle64> > mNativeChangeNotifiers;
    /*const*/ AutoPtr<IList> mConnectionTypeObservers;
    Int32 mCurrentConnectionType;
    static AutoPtr<NetworkChangeNotifier> sInstance;
};

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_NETWORKCHANGENOTIFIER_H_

