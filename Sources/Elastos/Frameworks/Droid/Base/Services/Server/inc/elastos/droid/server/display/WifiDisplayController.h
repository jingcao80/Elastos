
#ifndef __ELASTOS_DROID_SERVER_DISPLAY_WIFIDISPLAYCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_WIFIDISPLAYCONTROLLER_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IRemoteDisplay;
using Elastos::Droid::Media::IRemoteDisplayListener;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerChannel;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerActionListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerPeerListListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerGroupInfoListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::IWifiP2pDeviceList;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Display::IWifiDisplay;
using Elastos::Droid::Hardware::Display::IWifiDisplaySessionInfo;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::View::ISurface;

using Elastos::Utility::Etl::List;
using Elastos::Net::IInet4Address;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * Manages all of the various asynchronous interactions with the {@link WifiP2pManager}
 * on behalf of {@link WifiDisplayAdapter}.
 * <p>
 * This code is isolated from {@link WifiDisplayAdapter} so that we can avoid
 * accidentally introducing any deadlocks due to the display manager calling
 * outside of itself while holding its lock.  It's also way easier to write this
 * asynchronous code if we can assume that it is single-threaded.
 * </p><p>
 * The controller must be instantiated on the handler thread.
 * </p>
 */
class WifiDisplayController : public Object
{
private:
    class WifiP2pReceiver : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WifiDisplayController::BroadcastReceiver")

        WifiP2pReceiver(
            /* [in] */ WifiDisplayController* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiDisplayController* mHost;
    };

    class MyContentObserver : public ContentObserver
    {
    public:
        TO_STRING_IMPL("WifiDisplayController::MyContentObserver")

        MyContentObserver(
            /* [in] */ WifiDisplayController* host);

        CARAPI constructor(
            /* [in] */ IHandler* handler);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        WifiDisplayController* mHost;
    };

    class WFDInfoActionListener
        : public Object
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        WFDInfoActionListener(
            /* [in] */ WifiDisplayController* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        WifiDisplayController* mHost;
    };

    class WFDInfoActionListener2
        : public Object
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        WFDInfoActionListener2(
            /* [in] */ WifiDisplayController* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        WifiDisplayController* mHost;
    };

    class DiscoverPeersActionListener
        : public Object
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        DiscoverPeersActionListener(
            /* [in] */ WifiDisplayController* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        WifiDisplayController* mHost;
    };

    class StopPeerDiscoveryActionListener
        : public Object
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        StopPeerDiscoveryActionListener(
            /* [in] */ WifiDisplayController* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        WifiDisplayController* mHost;
    };

    class RequestPeersPeerListListener
        : public Object
        , public IWifiP2pManagerPeerListListener
    {
    public:
        CAR_INTERFACE_DECL();

        RequestPeersPeerListListener(
            /* [in] */ WifiDisplayController* host);

        CARAPI OnPeersAvailable(
            /* [in] */ IWifiP2pDeviceList* peers);

    private:
        WifiDisplayController* mHost;
    };

    class MyActionListenerEx2
        : public Object
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyActionListenerEx2(
            /* [in] */ WifiDisplayController* host,
            /* [in] */ IWifiP2pDevice* oldDevice,
            /* [in] */ Boolean isDisconnecting);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        CARAPI_(void) Next();

    private:
        WifiDisplayController* mHost;
        AutoPtr<IWifiP2pDevice> mOldDevice;
        Boolean mIsDisconnecting;
    };

    class ConnectDeviceActionListener
        : public Object
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        ConnectDeviceActionListener(
            /* [in] */ WifiDisplayController* host,
            /* [in] */ IWifiP2pDevice* newDevice);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        WifiDisplayController* mHost;
        AutoPtr<IWifiP2pDevice> mNewDevice;
    };

    class DisconnectingDeviceActionListener
        : public Object
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        DisconnectingDeviceActionListener(
            /* [in] */ WifiDisplayController* host,
            /* [in] */ IWifiP2pDevice* oldDevice);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        CARAPI_(void) Next();

    private:
        WifiDisplayController* mHost;
        AutoPtr<IWifiP2pDevice> mOldDevice;
    };

    class CancelingDeviceActionListener
        : public Object
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        CancelingDeviceActionListener(
            /* [in] */ WifiDisplayController* host,
            /* [in] */ IWifiP2pDevice* oldDevice);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        CARAPI_(void) Next();

    private:
        WifiDisplayController* mHost;
        AutoPtr<IWifiP2pDevice> mOldDevice;
    };

    class MyRemoteDisplayListener
        : public Object
        , public IRemoteDisplayListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyRemoteDisplayListener(
            /* [in] */ WifiDisplayController* host,
            /* [in] */ IWifiP2pDevice* oldDevice);

        CARAPI OnDisplayConnected(
            /* [in] */ ISurface* surface,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 flags,
            /* [in] */ Int32 session);

        CARAPI OnDisplayDisconnected();

        CARAPI OnDisplayError(
            /* [in] */ Int32 error);

    private:
        WifiDisplayController* mHost;
        AutoPtr<IWifiP2pDevice> mOldDevice;
    };

    class ConnectionChangedGroupInfoListener
        : public Object
        , public IWifiP2pManagerGroupInfoListener
    {
    public:
        CAR_INTERFACE_DECL();

        ConnectionChangedGroupInfoListener(
            /* [in] */ WifiDisplayController* host);

        CARAPI OnGroupInfoAvailable(
            /* [in] */ IWifiP2pGroup* info);

    private:
        WifiDisplayController* mHost;
    };

    class AdvertiseDisplayRunnable
        : public Runnable
    {
    public:
        AdvertiseDisplayRunnable(
            /* [in] */ IWifiDisplay* display,
            /* [in] */ ISurface* surface,
            /* [in] */ WifiDisplayController* host)
            : mOldDisplay(display)
            , mOldSurface(surface)
            , mHost(host)
        {}

        CARAPI Run();

    private:
        AutoPtr<IWifiDisplay> mOldDisplay;
        AutoPtr<ISurface> mOldSurface;
        WifiDisplayController* mHost;
    };

    class ReportFeatureStateRunnable
        : public Runnable
    {
    public:
        ReportFeatureStateRunnable(
            /* [in] */ Int32 featureState,
            /* [in] */ WifiDisplayController* host);

        CARAPI Run();

    private:
        Int32 mFeatureState;
        WifiDisplayController* mHost;
    };

    class ScanStartedRunnable
        : public Runnable
    {
    public:
        ScanStartedRunnable(
            /* [in] */ WifiDisplayController* host);

        CARAPI Run();

    private:
        WifiDisplayController* mHost;
    };

    class ScanResultsRunnable
        : public Runnable
    {
    public:
        ScanResultsRunnable(
            /* [in] */ ArrayOf<IWifiDisplay*>* displays,
            /* [in] */ WifiDisplayController* host);

        CARAPI Run();

    private:
        AutoPtr<ArrayOf<IWifiDisplay*> > mDisplays;
        WifiDisplayController* mHost;
    };

    class ScanFinishedRunnable
        : public Runnable
    {
    public:
        ScanFinishedRunnable(
            /* [in] */ WifiDisplayController* host);

        CARAPI Run();

    private:
        WifiDisplayController* mHost;
    };

    class ConnectionFailureRunnable
        : public Runnable
    {
    public:
        ConnectionFailureRunnable(
            /* [in] */ IWifiP2pDevice* oldDevice,
            /* [in] */ WifiDisplayController* host);

        CARAPI Run();

    private:
        AutoPtr<IWifiP2pDevice> mOldDevice;
        WifiDisplayController* mHost;
    };

    class ConnectionTimeoutRunnable
        : public Runnable
    {
    public:
        ConnectionTimeoutRunnable(
            /* [in] */ WifiDisplayController* host);

        CARAPI Run();

    private:
        WifiDisplayController* mHost;
    };

    class DiscoverPeersRunnable
        : public Runnable
    {
    public:
        DiscoverPeersRunnable(
            /* [in] */ WifiDisplayController* host);

        CARAPI Run();

    private:
        WifiDisplayController* mHost;
    };

    class RtspTimeoutRunnable
        : public Runnable
    {
    public:
        RtspTimeoutRunnable(
            /* [in] */ WifiDisplayController* host);

        CARAPI Run();

    private:
        WifiDisplayController* mHost;
    };

public:
    WifiDisplayController(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IWifiDisplayControllerListener* listener);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw);

    CARAPI_(void) RequestStartScan();

    CARAPI_(void) RequestStopScan();

    CARAPI_(void) RequestConnect(
        /* [in] */ const String& address);

    CARAPI_(void) RequestPause();

    CARAPI_(void) RequestResume();

    CARAPI_(void) RequestDisconnect();

private:

    CARAPI_(void) UpdateSettings();

    CARAPI_(void) UpdateWfdEnableState();

    CARAPI_(void) ReportFeatureState();

    CARAPI_(Int32) ComputeFeatureState();

    CARAPI_(void) UpdateScanState();

    CARAPI_(void) TryDiscoverPeers();

    CARAPI_(void) StopPeerDiscovery();

    CARAPI_(void) RequestPeers();

    CARAPI_(void) HandleScanStarted();

    CARAPI_(void) HandleScanResults();

    CARAPI_(void) HandleScanFinished();

    CARAPI_(void) UpdateDesiredDevice(
        /* [in] */ IWifiP2pDevice* device);

    CARAPI_(void) Connect(
        /* [in] */ IWifiP2pDevice* device);

    CARAPI_(void) Disconnect();

    CARAPI_(void) RetryConnection();

    /**
     * This function is called repeatedly after each asynchronous operation
     * until all preconditions for the connection have been satisfied and the
     * connection is established (or not).
     */
    CARAPI_(void) UpdateConnection();

    CARAPI_(AutoPtr<IWifiDisplaySessionInfo>) GetSessionInfo(
        /* [in] */ IWifiP2pGroup* info,
        /* [in] */ Int32 session);

    CARAPI_(void) HandleStateChanged(
        /* [in] */ Boolean enabled);

    CARAPI_(void) HandlePeersChanged();

    CARAPI_(void) HandleConnectionChanged(
        /* [in] */ INetworkInfo* networkInfo);

    CARAPI_(void) HandleConnectionFailure(
        /* [in] */ Boolean timeoutOccurred);

    CARAPI_(void) AdvertiseDisplay(
        /* [in] */ IWifiDisplay* display,
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 flags);

    CARAPI_(void) UnadvertiseDisplay();

    CARAPI_(void) ReadvertiseDisplay(
        /* [in] */ IWifiDisplay* display);
private:
    static CARAPI_(AutoPtr<IInet4Address>) GetInterfaceAddress(
        /* [in] */ IWifiP2pGroup* info);

    static CARAPI_(Int32) GetPortNumber(
        /* [in] */ IWifiP2pDevice* device);

    static CARAPI_(Boolean) IsWifiDisplay(
        /* [in] */ IWifiP2pDevice* device);

    static CARAPI_(Boolean) IsPrimarySinkDeviceType(
        /* [in] */ Int32 deviceType);

    static CARAPI_(String) DescribeWifiP2pDevice(
        /* [in] */ IWifiP2pDevice* device);

    static CARAPI_(String) DescribeWifiP2pGroup(
        /* [in] */ IWifiP2pGroup* group);

    static CARAPI_(AutoPtr<IWifiDisplay>) CreateWifiDisplay(
        /* [in] */ IWifiP2pDevice* device);

private:
    static const String TAG ;
    static const Boolean DEBUG;

    static const Int32 DEFAULT_CONTROL_PORT = 7236;
    static const Int32 MAX_THROUGHPUT = 50;
    static const Int32 CONNECTION_TIMEOUT_SECONDS = 30;
    static const Int32 RTSP_TIMEOUT_SECONDS = 30;
    static const Int32 RTSP_TIMEOUT_SECONDS_CERT_MODE = 120;

    // We repeatedly issue calls to discover peers every so often for a few reasons.
    // 1. The initial request may fail and need to retried.
    // 2. Discovery will self-abort after any group is initiated, which may not necessarily
    //    be what we want to have happen.
    // 3. Discovery will self-timeout after 2 minutes, whereas we want discovery to
    //    be occur for as long as a client is requesting it be.
    // 4. We don't seem to get updated results for displays we've already found until
    //    we ask to discover again, particularly for the isSessionAvailable() property.
    static const Int32 DISCOVER_PEERS_INTERVAL_MILLIS = 10000;

    static const Int32 CONNECT_MAX_RETRIES = 3;
    static const Int32 CONNECT_RETRY_DELAY_MILLIS = 500;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IWifiDisplayControllerListener> mListener;

    AutoPtr<IWifiP2pManager> mWifiP2pManager;
    AutoPtr<IWifiP2pManagerChannel> mWifiP2pChannel;

    Boolean mWifiP2pEnabled;
    Boolean mWfdEnabled;
    Boolean mWfdEnabling;
    AutoPtr<INetworkInfo> mNetworkInfo;

    List<AutoPtr<IWifiP2pDevice> > mAvailableWifiDisplayPeers;

    // True if Wifi display is enabled by the user.
    Boolean mWifiDisplayOnSetting;

    // True if a scan was requested independent of whether one is actually in progress.
    Boolean mScanRequested;

    // True if there is a call to discoverPeers in progress.
    Boolean mDiscoverPeersInProgress;

    // The device to which we want to connect, or NULL if we want to be disconnected.
    AutoPtr<IWifiP2pDevice> mDesiredDevice;

    // The device to which we are currently connecting, or NULL if we have already connected
    // or are not trying to connect.
    AutoPtr<IWifiP2pDevice> mConnectingDevice;

    // The device from which we are currently disconnecting.
    AutoPtr<IWifiP2pDevice> mDisconnectingDevice;

    // The device to which we were previously trying to connect and are now canceling.
    AutoPtr<IWifiP2pDevice> mCancelingDevice;

    // The device to which we are currently connected, which means we have an active P2P group.
    AutoPtr<IWifiP2pDevice> mConnectedDevice;

    // The group info obtained after connecting.
    AutoPtr<IWifiP2pGroup> mConnectedDeviceGroupInfo;

    // Number of connection retries remaining.
    Int32 mConnectionRetriesLeft;

    // The Extended remote display that is listening on the connection.
    // Created after the Wifi P2P network is connected.
    AutoPtr<IInterface> mExtRemoteDisplay;

    // The remote display that is listening on the connection.
    // Created after the Wifi P2P network is connected.
    AutoPtr<IRemoteDisplay> mRemoteDisplay;

    // The remote display interface.
    String mRemoteDisplayInterface;

    // True if RTSP has connected.
    Boolean mRemoteDisplayConnected;

    // The information we have most recently told WifiDisplayAdapter about.
    AutoPtr<IWifiDisplay> mAdvertisedDisplay;
    AutoPtr<ISurface> mAdvertisedDisplaySurface;
    Int32 mAdvertisedDisplayWidth;
    Int32 mAdvertisedDisplayHeight;
    Int32 mAdvertisedDisplayFlags;

    // Certification
    Boolean mWifiDisplayCertMode;
    Int32 mWifiDisplayWpsConfig;// = WpsInfo.INVALID;

    AutoPtr<IWifiP2pDevice> mThisDevice;

    AutoPtr<IRunnable> mDiscoverPeers;
    AutoPtr<IRunnable> mConnectionTimeout;
    AutoPtr<IRunnable> mRtspTimeout;
    AutoPtr<IBroadcastReceiver> mWifiP2pReceiver;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_WIFIDISPLAYCONTROLLER_H__
