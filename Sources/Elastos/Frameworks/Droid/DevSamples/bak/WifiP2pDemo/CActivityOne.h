
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"

using Elastos::ElRefBase;
using Elastos::Net::IInetAddress;
using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IRadioGroup;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Hardware::Display::IDisplayManager;

using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::IWifiP2pDeviceList;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerChannel;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerChannelListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerActionListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerPeerListListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerConnectionInfoListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerGroupInfoListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pDemo {


class CActivityOne : public Activity
{
public:
    class MyListener
    : public IAdapterViewOnItemClickListener
    , public IViewOnClickListener
    , public IRadioGroupOnCheckedChangeListener
    , public ElRefBase
    {
    public:
        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnCheckedChanged(
            /* [in] */  IRadioGroup* group,
            /* [in] */  Int32 checkedId);

    private:
        CActivityOne* mHost;
    };

    class DiscoverPeerActionListener
        : public ElRefBase
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        DiscoverPeerActionListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);
    private:
        CActivityOne* mHost;
    };

    class ConnectActionListener
        : public ElRefBase
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        ConnectActionListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);
    private:
        CActivityOne* mHost;
    };

    class CreateGroupActionListener
        : public ElRefBase
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        CreateGroupActionListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);
    private:
        CActivityOne* mHost;
    };

    class CancelConnectActionListener
        : public ElRefBase
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        CancelConnectActionListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);
    private:
        CActivityOne* mHost;
    };

    class RemoveGroupActionListener
        : public ElRefBase
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        RemoveGroupActionListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);
    private:
        CActivityOne* mHost;
    };

    class RequestPeerListListener
        : public ElRefBase
        , public IWifiP2pManagerPeerListListener
    {
    public:
        CAR_INTERFACE_DECL()

        RequestPeerListListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnPeersAvailable(
            /* [in] */ IWifiP2pDeviceList* peers);
    private:
        CActivityOne* mHost;
    };

    class ConnectionInfoListener
        : public ElRefBase
        , public IWifiP2pManagerConnectionInfoListener
    {
    public:
        CAR_INTERFACE_DECL()

        ConnectionInfoListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnConnectionInfoAvailable(
            /* [in] */ IWifiP2pInfo* peers);
    private:
        CActivityOne* mHost;
    };

    class WifiP2pManagerChannelListener
        : public ElRefBase
        , public IWifiP2pManagerChannelListener
    {
    public:
        CAR_INTERFACE_DECL()

        WifiP2pManagerChannelListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnChannelDisconnected();
    private:
        CActivityOne* mHost;
    };

    class GroupInfoListener
        : public ElRefBase
        , public IWifiP2pManagerGroupInfoListener
    {
    public:
        CAR_INTERFACE_DECL()

        GroupInfoListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnGroupInfoAvailable(
            /* [in] */ IWifiP2pGroup* peers);
    private:
        CActivityOne* mHost;
    };

    class DisplayListener
        : public ElRefBase
        , public IDisplayListener
    {
    public:
        CAR_INTERFACE_DECL()

        DisplayListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnDisplayAdded(
            /* [in] */ Int32 displayId);

        CARAPI OnDisplayChanged(
            /* [in] */ Int32 displayId);

        CARAPI OnDisplayRemoved(
            /* [in] */ Int32 displayId);
    private:
        CActivityOne* mHost;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

public:
    CActivityOne();

    CARAPI OnPeersChanged();

    CARAPI OnConnectionChanged(
        /* [in] */ INetworkInfo* networkInfo);

    CARAPI OnThisDeviceChanged(
        /* [in] */ IWifiP2pDevice* deiviceInfo);

    CARAPI OnConnectionInfoAvailable(
        /* [in] */ IWifiP2pInfo* peers);

    CARAPI OnGroupInfoAvailable(
        /* [in] */ IWifiP2pGroup* peers);

    CARAPI OnUpdateSettingButtonClick();

    CARAPI OnDiscoveryButtonClick();

    /**
     * @param isWifiP2pEnabled the isWifiP2pEnabled to set
     */
    void SetIsWifiP2pEnabled(
        /* [in] */ Boolean isWifiP2pEnabled);

    /**
     * Remove all peers and clear all fields. This is called on
     * BroadcastReceiver receiving a state change event.
     */
    void ResetData();

    CARAPI OnChannelDisconnected();

    CARAPI ShowToast(
        /* [in] */ const String& info,
        /* [in] */ Int32 style = IToast::LENGTH_SHORT);

    CARAPI UpdateWifiList();

    CARAPI ConnectToPeer(
        /* [in] */ const String& deviceName,
        /* [in] */ const String& deviceAddress);

    CARAPI AppendInfo(
        /* [in] */ const String& info);

    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI SetupViews();

    CARAPI SetupWifi();

    CARAPI CreateGroup();

    CARAPI Disconnect();

public:
    AutoPtr<IWifiP2pDeviceList> mP2pDeviceList;
    AutoPtr<IDisplayManager> mDisplayManager;
private:
    static String TAG;// = "wifidirectdemo";

    AutoPtr<IWifiP2pManager> mP2pManager;
    Boolean mIsWifiP2pEnabled;// = FALSE;
    Boolean mRetryChannel;// = FALSE;

    AutoPtr<IWifiP2pManagerChannelListener> mWifiP2pManagerChannelListener;
    AutoPtr<IIntentFilter> mIntentFilter;// = new IntentFilter();
    AutoPtr<IWifiP2pManagerChannel> mChannel;
    AutoPtr<IBroadcastReceiver> mReceiver;

    AutoPtr<ITextView> mInfoTextView;
    AutoPtr<IListView> mWifiListView;
};

} // namespace WifiP2pDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
