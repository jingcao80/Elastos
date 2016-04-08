
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include "ChatManager.h"

using Elastos::ElRefBase;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IRadioGroup;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::IWifiP2pDeviceList;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerChannel;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerChannelListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerActionListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerPeerListListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerConnectionInfoListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerDnsSdServiceResponseListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerDnsSdTxtRecordListener;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pDnsSdServiceRequest;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pServiceDemo {


class CActivityOne : public Activity
{
public:
    /**
     * A structure to hold service information.
     */
    class WifiP2pService
        : public ElRefBase {
    public:
        AutoPtr<IWifiP2pDevice> mDevice;
        String mInstanceName;
        String mServiceRegistrationType;
    };

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

    class RemoveServiceRequestActionListener
        : public ElRefBase
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        RemoveServiceRequestActionListener(
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

    class AddLocalServiceActionListener
        : public ElRefBase
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        AddLocalServiceActionListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);
    private:
        CActivityOne* mHost;
    };

    class DnsSdServiceResponseListener
        : public ElRefBase
        , public IWifiP2pManagerDnsSdServiceResponseListener
    {
    public:
        CAR_INTERFACE_DECL()

        DnsSdServiceResponseListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnDnsSdServiceAvailable(
            /* [in] */  const String& instanceName,
            /* [in] */  const String& registrationType,
            /* [in] */  IWifiP2pDevice* srcDevice);
    private:
        CActivityOne* mHost;
    };

    class DnsSdTxtRecordListener
        : public ElRefBase
        , public IWifiP2pManagerDnsSdTxtRecordListener
    {
    public:
        CAR_INTERFACE_DECL()

        DnsSdTxtRecordListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnDnsSdTxtRecordAvailable(
            /* [in] */  const String& instanceName,
            /* [in] */  IObjectStringMap* txtRecordMap,
            /* [in] */  IWifiP2pDevice* srcDevice);
    private:
        CActivityOne* mHost;
    };

    class ServiceRequestActionListener
        : public ElRefBase
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceRequestActionListener(
            /* [in] */ CActivityOne* host);

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);
    private:
        CActivityOne* mHost;
    };

    class DiscoverServicesActionListener
        : public ElRefBase
        , public IWifiP2pManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL()

        DiscoverServicesActionListener(
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

    class HandlerCallback
        : public ElRefBase
        , public IHandlerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        HandlerCallback(
            /* [in] */ CActivityOne* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

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

    CARAPI OnConnectionChanged(
        /* [in] */ INetworkInfo* networkInfo);

    CARAPI OnThisDeviceChanged(
        /* [in] */ IWifiP2pDevice* deiviceInfo);

    CARAPI OnSendMsgButtonClick();

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

    CARAPI UpdateServiceList();

    CARAPI ConnectToService(
        /* [in] */ const String& deviceName,
        /* [in] */ const String& deviceAddress,
        /* [in] */ const String& instanceName);

    CARAPI OnConnectionInfoAvailable(
        /* [in] */ IWifiP2pInfo* p2pInfo);

    CARAPI AppendInfo(
        /* [in] */ const String& info);
private:
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI SetupViews();

    CARAPI SetupWifi();

    CARAPI StartRegistrationAndDiscovery();

    CARAPI DiscoverService();

    CARAPI_(AutoPtr<IHandler>) GetHandler();

public:
    // TXT RECORD properties
    static const String TXTRECORD_PROP_AVAILABLE;// = "available";
    static const String SERVICE_INSTANCE;// = "_wifidemotest";
    static const String SERVICE_REG_TYPE;// = "_presence._tcp";

    static const Int32 SERVER_PORT;

    static const Int32 MESSAGE_READ = 0x400 + 1;
    static const Int32 MY_HANDLE = 0x400 + 2;

    List<AutoPtr<WifiP2pService> > mServiceList;

    AutoPtr<ChatManager> mChatManager;
private:
    static const String TAG;// = "wifidirectdemo";

    AutoPtr<IWifiP2pManager> mP2pManager;
    Boolean mIsWifiP2pEnabled;// = FALSE;
    Boolean mRetryChannel;// = FALSE;

    AutoPtr<IWifiP2pDnsSdServiceRequest> mServiceRequest;

    AutoPtr<IWifiP2pManagerChannelListener> mWifiP2pManagerChannelListener;
    AutoPtr<IIntentFilter> mIntentFilter;// = new IntentFilter();
    AutoPtr<IWifiP2pManagerChannel> mChannel;
    AutoPtr<IBroadcastReceiver> mReceiver;

    AutoPtr<IHandler> mHandler;

    AutoPtr<ITextView> mInfoTextView;
    AutoPtr<IListView> mWifiListView;
};

} // namespace WifiP2pServiceDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
