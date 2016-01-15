
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/wifi/CWifiScanner.h"
#include "elastos/droid/wifi/CWifiScannerHotlistSettings.h"
#include "elastos/droid/wifi/CWifiScannerWifiChangeSettings.h"
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Core::AutoLock;
using Elastos::Core::IThread;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Wifi {

//===============================================================
//                 CWifiScanner::ServiceHandler
//===============================================================

CWifiScanner::ServiceHandler::ServiceHandler(
    /* [in] */ ILooper* looper)
    : Handler(looper)
{
}

//@Override
ECode CWifiScanner::ServiceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
                Int32 arg1;
                msg->GetArg1(&arg1);
                if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
                    sAsyncChannel->SendMessage(IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION);
                }
                else {
                    Logger::E(TAG, "Failed to set up channel connection");
                    // This will cause all further async API calls on the WifiManager
                    // to fail and throw an exception
                    sAsyncChannel = NULL;
                }
                return CWifiScanner::sConnected->CountDown();
            }

        case IAsyncChannel::CMD_CHANNEL_FULLY_CONNECTED:
            return NOERROR;

        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
                Logger::E(TAG, "Channel connection lost");
                // This will cause all further async API calls on the WifiManager
                // to fail and throw an exception
                sAsyncChannel = NULL;
                AutoPtr<ILooper> looper;
                GetLooper((ILooper**)&looper);
                return looper->Quit();
            }
    }

    Int32 arg2;
    msg->GetArg2(&arg2);
    AutoPtr<IObject> listener = CWifiScanner::GetListener(arg2);

    if (listener == NULL) {
        if (DBG) Logger::D(TAG, "invalid listener key = %d", arg2);
        return NOERROR;
    }
    else {
        if (DBG) Logger::D(TAG, "listener key = %d", arg2);
    }

    switch (what) {
            /* ActionListeners grouped together */
        case CMD_OP_SUCCEEDED :
            IWifiScannerActionListener::Probe(listener)->OnSuccess();
            break;
        case CMD_OP_FAILED : {
                AutoPtr<IWifiScannerOperationResult> result;
                msg->GetObj((IInterface**)&result);
                Int32 reason;
                result->GetReason(&reason);
                String description;
                result->GetDescription(&description);
                IWifiScannerActionListener::Probe(listener)->OnFailure(reason, description);
                RemoveListener(arg2);
            }
            break;
        case CMD_SCAN_RESULT : {
                AutoPtr<IWifiScannerParcelableScanResults> result;
                msg->GetObj((IInterface**)&result);
                AutoPtr< ArrayOf<IScanResult*> > results;
                result->GetResults((ArrayOf<IScanResult*>**)&results);
                IWifiScannerScanListener::Probe(listener)->OnResults(results);
                return NOERROR;
            }
        case CMD_FULL_SCAN_RESULT : {
                AutoPtr<IScanResult> result;
                msg->GetObj((IInterface**)&result);
                IWifiScannerScanListener::Probe(listener)->OnFullResult(result);
                return NOERROR;
            }
        case CMD_PERIOD_CHANGED: {
                Int32 arg1;
                msg->GetArg1(&arg1);
                IWifiScannerScanListener::Probe(listener)->OnPeriodChanged(arg1);
                return NOERROR;
            }
        case CMD_AP_FOUND: {
                AutoPtr<IWifiScannerParcelableScanResults> result;
                msg->GetObj((IInterface**)&result);
                AutoPtr< ArrayOf<IScanResult*> > results;
                result->GetResults((ArrayOf<IScanResult*>**)&results);
                IWifiScannerBssidListener::Probe(listener)->OnFound(results);
                return NOERROR;
            }
        case CMD_WIFI_CHANGE_DETECTED: {
                AutoPtr<IWifiScannerParcelableScanResults> result;
                msg->GetObj((IInterface**)&result);
                AutoPtr< ArrayOf<IScanResult*> > results;
                result->GetResults((ArrayOf<IScanResult*>**)&results);
                IWifiScannerWifiChangeListener::Probe(listener)->OnChanging(results);
               return NOERROR;
           }
        case CMD_WIFI_CHANGES_STABILIZED: {
                AutoPtr<IWifiScannerParcelableScanResults> result;
                msg->GetObj((IInterface**)&result);
                AutoPtr< ArrayOf<IScanResult*> > results;
                result->GetResults((ArrayOf<IScanResult*>**)&results);
                IWifiScannerWifiChangeListener::Probe(listener)->OnQuiescence(results);
                return NOERROR;
            }
        default:
            if (DBG) Logger::D(TAG, "Ignoring message %d", what);
            return NOERROR;
    }

    return NOERROR;
}


//===============================================================
//                        CWifiScanner
//===============================================================

static AutoPtr<ISparseArray> sListenerMap_Init()
{
    AutoPtr<ISparseArray> array;
    assert(0);
    // TODO
    // CSparseArray::New((ISparseArray**)&array);
    return array;
}

const String CWifiScanner::TAG("WifiScanner");
const Boolean CWifiScanner::DBG = TRUE;

/* commands for Wifi Service */
const Int32 CWifiScanner::BASE = -1;// TODO IProtocol::BASE_WIFI_SCANNER;

const Int32 CWifiScanner::INVALID_KEY = 0;
Int32 CWifiScanner::sListenerKey = 1;

AutoPtr<ISparseArray> CWifiScanner::sListenerMap = sListenerMap_Init();
Object CWifiScanner::sListenerMapLock;

AutoPtr<IAsyncChannel> CWifiScanner::sAsyncChannel;
AutoPtr<ICountDownLatch> CWifiScanner::sConnected;

Object CWifiScanner::sThreadRefLock;
Int32 CWifiScanner::sThreadRefCount;
AutoPtr<IHandlerThread> CWifiScanner::sHandlerThread;

CAR_INTERFACE_IMPL(CWifiScanner, Object, IWifiScanner)

CAR_OBJECT_IMPL(CWifiScanner)

ECode CWifiScanner::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIWifiScanner* service)
{
    mContext = context;
    mService = service;
    return Init();
}

/**
 * gives you all the possible channels; channel is specified as an
 * integer with frequency in MHz i.e. channel 1 is 2412
 * @hide
 */
ECode CWifiScanner::GetAvailableChannels(
    /* [in] */ Int32 band,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        AutoPtr<IBundle> bundle;
        mService->GetAvailableChannels(band, (IBundle**)&bundle);
        return bundle->GetIntegerArrayList(GET_AVAILABLE_CHANNELS_EXTRA, (IArrayList**)result);
    // } catch (RemoteException e) {
    //     return null;
    // }
}

/** start wifi scan in background
 * @param settings specifies various parameters for the scan; for more information look at
 * {@link ScanSettings}
 * @param listener specifies the object to report events to. This object is also treated as a
 *                 key for this scan, and must also be specified to cancel the scan. Multiple
 *                 scans should also not share this object.
 */
ECode CWifiScanner::StartBackgroundScan(
    /* [in] */ IScanSettings* settings,
    /* [in] */ IWifiScannerScanListener* listener)
{
    ValidateChannel();
    return sAsyncChannel->SendMessage(CMD_START_BACKGROUND_SCAN, 0, PutListener(IObject::Probe(listener)), settings);
}

/**
 * stop an ongoing wifi scan
 * @param listener specifies which scan to cancel; must be same object as passed in {@link
 *  #startBackgroundScan}
 */
ECode CWifiScanner::StopBackgroundScan(
    /* [in] */ IWifiScannerScanListener* listener)
{
    ValidateChannel();
    return sAsyncChannel->SendMessage(CMD_STOP_BACKGROUND_SCAN, 0, RemoveListener(IObject::Probe(listener)));
}

/**
 * retrieves currently available scan results
 */
ECode CWifiScanner::GetScanResults(
    /* [out, callee] */ ArrayOf<IScanResult*>** results)
{
    VALIDATE_NOT_NULL(results);
    ValidateChannel();
    AutoPtr<IMessage> reply;
    sAsyncChannel->SendMessageSynchronously(CMD_GET_SCAN_RESULTS, 0, (IMessage**)&reply);
    assert(0);
    // TODO
    // ScanResult[] results = (ScanResult[]) reply.obj;
    return NOERROR;
}

/** configure WifiChange detection
 * @param rssiSampleSize number of samples used for RSSI averaging
 * @param lostApSampleSize number of samples to confirm an access point's loss
 * @param unchangedSampleSize number of samples to confirm there are no changes
 * @param minApsBreachingThreshold minimum number of access points that need to be
 *                                 out of range to detect WifiChange
 * @param periodInMs indicates period of scan to find changes
 * @param bssidInfos access points to watch
 */
ECode CWifiScanner::ConfigureWifiChange(
    /* [in] */ Int32 rssiSampleSize,                          /* sample size for RSSI averaging */
    /* [in] */ Int32 lostApSampleSize,                        /* samples to confirm AP's loss */
    /* [in] */ Int32 unchangedSampleSize,                     /* samples to confirm no change */
    /* [in] */ Int32 minApsBreachingThreshold,                /* change threshold to trigger event */
    /* [in] */ Int32 periodInMs,                              /* period of scan */
    /* [in] */ ArrayOf<IWifiScannerBssidInfo*>* bssidInfos    /* signal thresholds to crosss */
)
{
    ValidateChannel();

    AutoPtr<IWifiScannerWifiChangeSettings> settings;
    CWifiScannerWifiChangeSettings::New((IWifiScannerWifiChangeSettings**)&settings);
    settings->SetRssiSampleSize(rssiSampleSize);
    settings->SetLostApSampleSize(lostApSampleSize);
    settings->SetUnchangedSampleSize(unchangedSampleSize);
    settings->SetMinApsBreachingThreshold(minApsBreachingThreshold);
    settings->SetPeriodInMs(periodInMs);
    settings->SetBssidInfos(bssidInfos);

    return ConfigureWifiChange(settings);
}

/**
 * track changes in wifi environment
 * @param listener object to report events on; this object must be unique and must also be
 *                 provided on {@link #stopTrackingWifiChange}
 */
ECode CWifiScanner::StartTrackingWifiChange(
    /* [in] */ IWifiScannerWifiChangeListener* listener)
{
    ValidateChannel();
    return sAsyncChannel->SendMessage(CMD_START_TRACKING_CHANGE, 0, PutListener(IObject::Probe(listener)));
}

/**
 * stop tracking changes in wifi environment
 * @param listener object that was provided to report events on {@link
 * #stopTrackingWifiChange}
 */
ECode CWifiScanner::StopTrackingWifiChange(
    /* [in] */ IWifiScannerWifiChangeListener* listener)
{
    ValidateChannel();
    return sAsyncChannel->SendMessage(CMD_STOP_TRACKING_CHANGE, 0, RemoveListener(IObject::Probe(listener)));
}

/** @hide */
// @SystemApi
ECode CWifiScanner::ConfigureWifiChange(
    /* [in] */ IWifiScannerWifiChangeSettings* settings)
{
    ValidateChannel();
    return sAsyncChannel->SendMessage(CMD_CONFIGURE_WIFI_CHANGE, 0, 0, settings);
}

/**
 * set interesting access points to find
 * @param bssidInfos access points of interest
 * @param apLostThreshold number of scans needed to indicate that AP is lost
 * @param listener object provided to report events on; this object must be unique and must
 *                 also be provided on {@link #stopTrackingBssids}
 */
ECode CWifiScanner::StartTrackingBssids(
    /* [in] */ ArrayOf<IWifiScannerBssidInfo*>* bssidInfos,
    /* [in] */ Int32 apLostThreshold,
    /* [in] */ IWifiScannerBssidListener* listener)
{
    ValidateChannel();
    AutoPtr<IWifiScannerHotlistSettings> settings;
    CWifiScannerHotlistSettings::New((IWifiScannerHotlistSettings**)&settings);
    settings->SetBssidInfos(bssidInfos);
    return sAsyncChannel->SendMessage(CMD_SET_HOTLIST, 0, PutListener(IObject::Probe(listener)), settings);
}

/**
 * remove tracking of interesting access points
 * @param listener same object provided in {@link #startTrackingBssids}
 */
ECode CWifiScanner::StopTrackingBssids(
    /* [in] */ IWifiScannerBssidListener* listener)
{
    ValidateChannel();
    return sAsyncChannel->SendMessage(CMD_RESET_HOTLIST, 0, RemoveListener(IObject::Probe(listener)));
}

ECode CWifiScanner::Init()
{
    AutoLock lock(sThreadRefLock);

    if (++sThreadRefCount == 1) {
        AutoPtr<IMessenger> messenger;
        // try {
            mService->GetMessenger((IMessenger**)&messenger);
        // } catch (RemoteException e) {
        //     /* do nothing */
        // } catch (SecurityException e) {
        //     /* do nothing */
        // }

        if (messenger == NULL) {
            sAsyncChannel = NULL;
            return NOERROR;
        }

        CHandlerThread::New(String("WifiScanner"), (IHandlerThread**)&sHandlerThread);
        CAsyncChannel::New((IAsyncChannel**)&sAsyncChannel);
        assert(0);
        // TODO
        // CCountDownLatch::New(1, (ICountDownLatch**)&sConnected);

        IThread::Probe(sHandlerThread)->Start();
        AutoPtr<ILooper> looper;
        sHandlerThread->GetLooper((ILooper**)&looper);
        AutoPtr<IHandler> handler;
        assert(0);
        // TODO
        // CServiceHandler::New(looper, (IHandler**)&handler);
        sAsyncChannel->Connect(mContext, handler, messenger);
        // try {
            return sConnected->Await();
        // } catch (InterruptedException e) {
        //     Log.e(TAG, "interrupted wait at init");
        // }
    }

    return NOERROR;
}

ECode CWifiScanner::ValidateChannel()
{
    if (sAsyncChannel == NULL) {
        // throw new IllegalStateException(
        //         "No permission to access and change wifi or a bad initialization");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

Int32 CWifiScanner::PutListener(
    /* [in] */ IObject* listener)
{
    if (listener == NULL) {
        return INVALID_KEY;
    }

    Int32 key;

    {
        AutoLock lock(sListenerMapLock);
        do {
            key = sListenerKey++;
        } while (key == INVALID_KEY);
        sListenerMap->Put(key, listener);
    }
    return key;
}

AutoPtr<IObject> CWifiScanner::GetListener(
    /* [in] */ Int32 key)
{
    if (key == INVALID_KEY) {
        return NULL;
    }

    {
        AutoLock lock(sListenerMapLock);
        AutoPtr<IObject> listener;
        sListenerMap->Get(key, (IInterface**)&listener);
        return listener;
    }
}

Int32 CWifiScanner::GetListenerKey(
    /* [in] */ IObject* listener)
{
    if (listener == NULL) {
        return INVALID_KEY;
    }

    {
        AutoLock lock(sListenerMapLock);
        Int32 index;
        sListenerMap->IndexOfValue(listener, &index);
        if (index == -1) {
            return INVALID_KEY;
        }
        else {
            Int32 key;
            sListenerMap->KeyAt(index, &key);
            return key;
        }
    }
}

AutoPtr<IObject> CWifiScanner::RemoveListener(
    /* [in] */ Int32 key)
{
    if (key == INVALID_KEY) {
        return NULL;
    }

    {
        AutoLock lock(sListenerMapLock);
        AutoPtr<IObject> listener;
        sListenerMap->Get(key, (IInterface**)&listener);
        sListenerMap->Remove(key);
        return listener;
    }
}

Int32 CWifiScanner::RemoveListener(
    /* [in] */ IObject* listener)
{
    Int32 key = GetListenerKey(listener);
    if (key == INVALID_KEY) {
        return key;
    }

    {
        AutoLock lock(sListenerMapLock);
        sListenerMap->Remove(key);
        return key;
    }
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
