// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_NET_NETWORKCHANGENOTIFIER_H_
#define _ELASTOS_DROID_WEBKIT_NET_NETWORKCHANGENOTIFIER_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/net/NetworkChangeNotifierAutoDetect.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Webkit::Net::NetworkChangeNotifierAutoDetect;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
        /* [in] */ Int64 nativeChangeNotifier);

    /**
      * Removes a native-side observer.
      */
    // @CalledByNative
    virtual CARAPI RemoveNativeObserver(
        /* [in] */ Int64 nativeChangeNotifier);

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
        /* [in] */ Int64 nativeChangeNotifier);

    static CARAPI_(void) RemoveNativeObserver(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 nativeChangeNotifier);

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
        /* [in] */ Int64 nativePtr,
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
    /*const*/ AutoPtr< List<Int64> > mNativeChangeNotifiers;
    /*const*/ AutoPtr<IList> mConnectionTypeObservers;
    Int32 mCurrentConnectionType;
    static AutoPtr<NetworkChangeNotifier> sInstance;
};

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_NET_NETWORKCHANGENOTIFIER_H_

