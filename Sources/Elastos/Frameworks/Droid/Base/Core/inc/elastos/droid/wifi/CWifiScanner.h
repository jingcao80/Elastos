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

#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISCANNER_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISCANNER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Wifi_CWifiScanner.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::Concurrent::ICountDownLatch;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiScanner)
    , public Object
    , public IWifiScanner
{
private:
    class ServiceHandler : public Handler
    {
    public:
        ServiceHandler(
            /* [in] */ ILooper* looper);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIWifiScanner* service);

    /**
     * gives you all the possible channels; channel is specified as an
     * integer with frequency in MHz i.e. channel 1 is 2412
     * @hide
     */
    CARAPI GetAvailableChannels(
        /* [in] */ Int32 band,
        /* [out] */ IList** result);

    /** start wifi scan in background
     * @param settings specifies various parameters for the scan; for more information look at
     * {@link ScanSettings}
     * @param listener specifies the object to report events to. This object is also treated as a
     *                 key for this scan, and must also be specified to cancel the scan. Multiple
     *                 scans should also not share this object.
     */
    CARAPI StartBackgroundScan(
        /* [in] */ IScanSettings* settings,
        /* [in] */ IWifiScannerScanListener* listener);

    /**
     * stop an ongoing wifi scan
     * @param listener specifies which scan to cancel; must be same object as passed in {@link
     *  #startBackgroundScan}
     */
    CARAPI StopBackgroundScan(
        /* [in] */ IWifiScannerScanListener* listener);

    /**
     * retrieves currently available scan results
     */
    CARAPI GetScanResults(
        /* [out, callee] */ ArrayOf<IScanResult*>** result);

    /** configure WifiChange detection
     * @param rssiSampleSize number of samples used for RSSI averaging
     * @param lostApSampleSize number of samples to confirm an access point's loss
     * @param unchangedSampleSize number of samples to confirm there are no changes
     * @param minApsBreachingThreshold minimum number of access points that need to be
     *                                 out of range to detect WifiChange
     * @param periodInMs indicates period of scan to find changes
     * @param bssidInfos access points to watch
     */
    CARAPI ConfigureWifiChange(
        /* [in] */ Int32 rssiSampleSize,                          /* sample size for RSSI averaging */
        /* [in] */ Int32 lostApSampleSize,                        /* samples to confirm AP's loss */
        /* [in] */ Int32 unchangedSampleSize,                     /* samples to confirm no change */
        /* [in] */ Int32 minApsBreachingThreshold,                /* change threshold to trigger event */
        /* [in] */ Int32 periodInMs,                              /* period of scan */
        /* [in] */ ArrayOf<IWifiScannerBssidInfo*>* bssidInfos    /* signal thresholds to crosss */
    );

    /**
     * track changes in wifi environment
     * @param listener object to report events on; this object must be unique and must also be
     *                 provided on {@link #stopTrackingWifiChange}
     */
    CARAPI StartTrackingWifiChange(
        /* [in] */ IWifiScannerWifiChangeListener* listener);

    /**
     * stop tracking changes in wifi environment
     * @param listener object that was provided to report events on {@link
     * #stopTrackingWifiChange}
     */
    CARAPI StopTrackingWifiChange(
        /* [in] */ IWifiScannerWifiChangeListener* listener);

    /** @hide */
    // @SystemApi
    CARAPI ConfigureWifiChange(
        /* [in] */ IWifiScannerWifiChangeSettings* settings);

    /**
     * set interesting access points to find
     * @param bssidInfos access points of interest
     * @param apLostThreshold number of scans needed to indicate that AP is lost
     * @param listener object provided to report events on; this object must be unique and must
     *                 also be provided on {@link #stopTrackingBssids}
     */
    CARAPI StartTrackingBssids(
        /* [in] */ ArrayOf<IWifiScannerBssidInfo*>* bssidInfos,
        /* [in] */ Int32 apLostThreshold,
        /* [in] */ IWifiScannerBssidListener* listener);

    /**
     * remove tracking of interesting access points
     * @param listener same object provided in {@link #startTrackingBssids}
     */
    CARAPI StopTrackingBssids(
        /* [in] */ IWifiScannerBssidListener* listener);

private:
    CARAPI Init();

    CARAPI ValidateChannel();

    static CARAPI_(Int32) PutListener(
        /* [in] */ IObject* listener);

    static CARAPI_(AutoPtr<IObject>) GetListener(
        /* [in] */ Int32 key);

    static CARAPI_(Int32) GetListenerKey(
        /* [in] */ IObject* listener);

    static CARAPI_(AutoPtr<IObject>) RemoveListener(
        /* [in] */ Int32 key);

    static CARAPI_(Int32) RemoveListener(
        /* [in] */ IObject* listener);

private:
    /* private members and methods */

    static const String TAG;
    static const Boolean DBG;

    /* commands for Wifi Service */
    static const Int32 BASE;

    AutoPtr<IContext> mContext;
    AutoPtr<IIWifiScanner> mService;

    static const Int32 INVALID_KEY;
    static Int32 sListenerKey;

    static AutoPtr<ISparseArray> sListenerMap;
    static Object sListenerMapLock;

    static AutoPtr<IAsyncChannel> sAsyncChannel;
    static AutoPtr<ICountDownLatch> sConnected;

    static Object sThreadRefLock;
    static Int32 sThreadRefCount;
    static AutoPtr<IHandlerThread> sHandlerThread;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFISCANNER_H__
