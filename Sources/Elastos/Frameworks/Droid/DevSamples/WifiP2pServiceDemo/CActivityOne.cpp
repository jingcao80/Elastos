
#include "CActivityOne.h"
#include "R.h"
#include <R.h>
#include <stdio.h>
#include <utils/SystemClock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <unistd.h>
#include <elastos/utility/logging/Slogger.h>
#include "CWifiBroadcastReceiver.h"
#include "GroupOwnerSocketHandler.h"
#include "ClientSocketHandler.h"

using Elastos::Utility::Logging::Slogger;

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CStringWrapper;
using Elastos::Core::CObjectContainer;
using Elastos::Utility::IObjectStringMap;
using Elastos::Utility::CObjectStringMap;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IRadioGroupOnCheckedChangeListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_ITextView;
using Elastos::Droid::Widget::CSimpleAdapter;
using Elastos::Droid::Widget::ISimpleAdapter;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::CWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerChannelListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerChannelListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerActionListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerPeerListListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerConnectionInfoListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerDnsSdServiceResponseListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerDnsSdTxtRecordListener;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pDnsSdServiceInfo;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pDnsSdServiceInfoHelper;
using Elastos::Droid::Wifi::P2p::Nsd::CWifiP2pDnsSdServiceInfoHelper;
using Elastos::Droid::Wifi::P2p::Nsd::IWifiP2pDnsSdServiceRequestHelper;
using Elastos::Droid::Wifi::P2p::Nsd::CWifiP2pDnsSdServiceRequestHelper;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pServiceDemo {

const String CActivityOne::TAG("WifiP2pServiceDemo");

const Int32 CActivityOne::SERVER_PORT = 4545;
const Int32 CActivityOne::MESSAGE_READ;
const Int32 CActivityOne::MY_HANDLE;

const String CActivityOne::TXTRECORD_PROP_AVAILABLE("available");
const String CActivityOne::SERVICE_INSTANCE("_wifidemotest");
const String CActivityOne::SERVICE_REG_TYPE("_presence._tcp");


//==========================================================================================
// CActivityOne::MyListener
//==========================================================================================
CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host) :
    mHost(host)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    else if (riid == EIID_IAdapterViewOnItemClickListener) {
        return (IAdapterViewOnItemClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }
    else if (riid == EIID_IRadioGroupOnCheckedChangeListener) {
        return (IRadioGroupOnCheckedChangeListener*)this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode CActivityOne::MyListener::OnCheckedChanged(
    /* [in] */  IRadioGroup* group,
    /* [in] */  Int32 checkedId)
{
    return NOERROR;
}

ECode CActivityOne::MyListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    ITextView* tv = ITextView::Probe(view);
    assert(tv != NULL);

    AutoPtr<ICharSequence> cs;
    tv->GetText((ICharSequence**)&cs);
    String deviceInfo;
    cs->ToString(&deviceInfo);

    AutoPtr<ArrayOf<String> > infos;
    StringUtils::Split(deviceInfo, String("="), (ArrayOf<String>**)&infos);

    mHost->ConnectToService((*infos)[0], (*infos)[1], (*infos)[2]);
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
        /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);

    if (id == R::id::SendMsgButton) {
        mHost->OnSendMsgButtonClick();
    }
    else if (id == R::id::DiscoveryButton) {
        mHost->OnDiscoveryButtonClick();
    }
    return NOERROR;
}

//======================================================================
// CActivityOne::RemoveServiceRequestActionListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::RemoveServiceRequestActionListener, IWifiP2pManagerActionListener)

CActivityOne::RemoveServiceRequestActionListener::RemoveServiceRequestActionListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::RemoveServiceRequestActionListener::OnSuccess()
{
    Slogger::D("RemoveServiceRequestActionListener", "RemoveServiceRequest success!");
    String info("RemoveServiceRequest success!");
    mHost->ShowToast(info);
    return NOERROR;
}

ECode CActivityOne::RemoveServiceRequestActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    Slogger::D("RemoveServiceRequestActionListener", "RemoveServiceRequest Failed! reasonCode: %d", reason);

    String info("RemoveServiceRequest Failed! reasonCode : ");
    info.AppendFormat("%d", reason);

    mHost->ShowToast(info);
    return NOERROR;
}

//======================================================================
// CActivityOne::ConnectActionListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::ConnectActionListener, IWifiP2pManagerActionListener)

CActivityOne::ConnectActionListener::ConnectActionListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::ConnectActionListener::OnSuccess()
{
    Slogger::D("ConnectActionListener", "Connect success!");
    String info("Connect success!");
    mHost->ShowToast(info);
    return NOERROR;
}

ECode CActivityOne::ConnectActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    Slogger::D("ConnectActionListener", "Connect Failed! reasonCode: %d", reason);

    String info("Connect Failed! reasonCode : ");
    info.AppendFormat("%d", reason);

    mHost->ShowToast(info);
    return NOERROR;
}

//======================================================================
// CActivityOne::AddLocalServiceActionListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::AddLocalServiceActionListener, IWifiP2pManagerActionListener)

CActivityOne::AddLocalServiceActionListener::AddLocalServiceActionListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::AddLocalServiceActionListener::OnSuccess()
{
    Slogger::D("AddLocalServiceActionListener", "Added Local Service");
    String info("Added Local Servic success!");
    mHost->ShowToast(info);
    return NOERROR;
}

ECode CActivityOne::AddLocalServiceActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    Slogger::D("AddLocalServiceActionListener", "Failed to add a service! reasonCode: %d", reason);

    String info("Failed to add a service! reasonCode : ");
    info.AppendFormat("%d", reason);

    mHost->ShowToast(info);
    return NOERROR;
}

//======================================================================
// CActivityOne::DnsSdServiceResponseListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::DnsSdServiceResponseListener, IWifiP2pManagerDnsSdServiceResponseListener)

CActivityOne::DnsSdServiceResponseListener::DnsSdServiceResponseListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::DnsSdServiceResponseListener::OnDnsSdServiceAvailable(
    /* [in] */  const String& instanceName,
    /* [in] */  const String& registrationType,
    /* [in] */  IWifiP2pDevice* srcDevice)
{
    // A service has been discovered. Is this our app?

    Slogger::D("DnsSdServiceResponseListener", "OnDnsSdServiceAvailable: %s, %s",
        instanceName.string(), registrationType.string());

    if (instanceName.EqualsIgnoreCase(CActivityOne::SERVICE_INSTANCE)) {
        // update the UI and add the item the discovered
        // device.

        AutoPtr<WifiP2pService> service = new WifiP2pService();
        service->mDevice = srcDevice;
        service->mInstanceName = instanceName;
        service->mServiceRegistrationType = registrationType;

        mHost->mServiceList.Clear();
        mHost->mServiceList.PushBack(service);
        mHost->UpdateServiceList();
    }
    return NOERROR;
}

//======================================================================
// CActivityOne::DnsSdTxtRecordListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::DnsSdTxtRecordListener, IWifiP2pManagerDnsSdServiceResponseListener)

CActivityOne::DnsSdTxtRecordListener::DnsSdTxtRecordListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

/**
 * A new TXT record is available. Pick up the advertised
 * buddy name.
 */
ECode CActivityOne::DnsSdTxtRecordListener::OnDnsSdTxtRecordAvailable(
    /* [in] */  const String& instanceName,
    /* [in] */  IObjectStringMap* txtRecordMap,
    /* [in] */  IWifiP2pDevice* srcDevice)
{
    assert(srcDevice != NULL && txtRecordMap != NULL);

    AutoPtr<ICharSequence> seq;
    txtRecordMap->Get(CActivityOne::TXTRECORD_PROP_AVAILABLE, (IInterface**)&seq);
    String recordInfo;
    seq->ToString(&recordInfo);
    String deviceName;
    srcDevice->GetDeviceName(&deviceName);
    Slogger::D("DnsSdTxtRecordListener", "OnDnsSdTxtRecordAvailable: device:%s, instance: %s, record: %s",
        deviceName.string(), instanceName.string(), recordInfo.string());
    return NOERROR;
}

//======================================================================
// CActivityOne::ServiceRequestActionListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::ServiceRequestActionListener, IWifiP2pManagerActionListener)

CActivityOne::ServiceRequestActionListener::ServiceRequestActionListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::ServiceRequestActionListener::OnSuccess()
{
    Slogger::D("ServiceRequestActionListener", "Added service discovery request");
    String info("Added service discovery request!");
    mHost->ShowToast(info);
    return NOERROR;
}

ECode CActivityOne::ServiceRequestActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    Slogger::D("ServiceRequestActionListener", "Failed adding service discovery request: %d", reason);

    String info("Failed adding service discovery request! reasonCode : ");
    info.AppendFormat("%d", reason);

    mHost->ShowToast(info);
    return NOERROR;
}

//======================================================================
// CActivityOne::DiscoverServicesActionListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::DiscoverServicesActionListener, IWifiP2pManagerActionListener)

CActivityOne::DiscoverServicesActionListener::DiscoverServicesActionListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::DiscoverServicesActionListener::OnSuccess()
{
    Slogger::D("DiscoverServicesActionListener", "Service discovery initiated");
    String info("Service discovery initiated!");
    mHost->ShowToast(info);

    return NOERROR;
}

ECode CActivityOne::DiscoverServicesActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    Slogger::D("DiscoverServicesActionListener", "Service discovery failed: %d", reason);

    String info("Service discovery failed! reasonCode : ");
    info.AppendFormat("%d", reason);

    mHost->ShowToast(info);
    return NOERROR;
}


//======================================================================
// CActivityOne::RemoveGroupActionListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::RemoveGroupActionListener, IWifiP2pManagerActionListener)

CActivityOne::RemoveGroupActionListener::RemoveGroupActionListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::RemoveGroupActionListener::OnSuccess()
{
    Slogger::D("RemoveGroupActionListener", "Disconnect succeed!");
    String info("Disconnect succeed!");
    mHost->ShowToast(info);
    return NOERROR;
}

ECode CActivityOne::RemoveGroupActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    Slogger::D("RemoveGroupActionListener", "Disconnect failed. Reason :%d", reason);

    String info("Disconnect failed. Reason : ");
    info.AppendFormat("%d", reason);

    mHost->ShowToast(info);
    return NOERROR;
}

//======================================================================
// CActivityOne::ConnectionInfoListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::ConnectionInfoListener, IWifiP2pManagerConnectionInfoListener)

CActivityOne::ConnectionInfoListener::ConnectionInfoListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::ConnectionInfoListener::OnConnectionInfoAvailable(
    /* [in] */ IWifiP2pInfo* p2pInfo)
{
    assert(p2pInfo != NULL);
    String info;
    p2pInfo->ToString(&info);
    Slogger::D("ConnectionInfoListener", "OnConnectionInfoAvailable: %s", info.string());

    mHost->OnConnectionInfoAvailable(p2pInfo);
    return NOERROR;
}

//======================================================================
// CActivityOne::WifiP2pManagerChannelListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::WifiP2pManagerChannelListener, IWifiP2pManagerChannelListener)

CActivityOne::WifiP2pManagerChannelListener::WifiP2pManagerChannelListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::WifiP2pManagerChannelListener::OnChannelDisconnected()
{
    return mHost->OnChannelDisconnected();
}

//======================================================================
// CActivityOne::HandlerCallback
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::HandlerCallback, IHandlerCallback)

CActivityOne::HandlerCallback::HandlerCallback(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::HandlerCallback::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case CActivityOne::MESSAGE_READ: {
            ICharSequence* seq = ICharSequence::Probe(obj);
            String record;
            seq->ToString(&record);
            Slogger::D("HandleMessage", "MESSAGE_READ: [%s]", record.string());
            mHost->AppendInfo(record);
            break;
        }
        case CActivityOne::MY_HANDLE: {
            ChatManager* chat = reinterpret_cast<ChatManager*>(obj->Probe(EIID_ChatManager));
            assert(chat != NULL);
            mHost->mChatManager = chat;
        }
    }

    *result = TRUE;
    return NOERROR;
}

//======================================================================
// CActivityOne
//======================================================================
CActivityOne::CActivityOne()
    : mIsWifiP2pEnabled(0)
    , mRetryChannel(0)
{
    AutoPtr<IHandlerCallback> cb = new HandlerCallback(this);
    CHandler::New(cb, (IHandler**)&mHandler);
}

ECode CActivityOne::ShowToast(
    /* [in] */ const String& info,
    /* [in] */ Int32 style)
{
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(info, (ICharSequence**)&cs);
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    helper->MakeText(this, cs, style, (IToast**)&toast);
    toast->Show();

    AppendInfo(info);
    return NOERROR;
}

ECode CActivityOne::SetupViews()
{
    AutoPtr<MyListener> l = new MyListener(this);

    AutoPtr<IView> view = FindViewById(R::id::InfoTextView);
    assert(view != NULL);
    mInfoTextView = ITextView::Probe(view);
    view = FindViewById(R::id::WifiListView);
    assert(view != NULL);
    mWifiListView = IListView::Probe(view);

    view = FindViewById(R::id::SendMsgButton);
    assert(view != NULL);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::DiscoveryButton);
    assert(view != NULL);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::WifiListView);
    mWifiListView = IListView::Probe(view);
    assert(mWifiListView != NULL);

    AutoPtr<IColorDrawable> drawable;
    CColorDrawable::New(0xFF0000FF, (IColorDrawable**)&drawable);
    assert(drawable != NULL);
    mWifiListView->SetDivider(drawable);
    mWifiListView->SetDividerHeight(1);
    mWifiListView->SetOnItemClickListener(l);

    return NOERROR;
}

ECode CActivityOne::SetupWifi()
{
    // add necessary intent values to be matched.
    CIntentFilter::New((IIntentFilter**)&mIntentFilter);
    mIntentFilter->AddAction(IWifiP2pManager::WIFI_P2P_STATE_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiP2pManager::WIFI_P2P_PEERS_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiP2pManager::WIFI_P2P_CONNECTION_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiP2pManager::WIFI_P2P_THIS_DEVICE_CHANGED_ACTION);

    AutoPtr<ILooper> looper;
    GetMainLooper((ILooper**)&looper);
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::WIFI_P2P_SERVICE, (IInterface**)&obj);
    mP2pManager = IWifiP2pManager::Probe(obj);
    assert(mP2pManager != NULL);

    mWifiP2pManagerChannelListener = new WifiP2pManagerChannelListener(this);
    mP2pManager->Initialize(
        this, looper,
        mWifiP2pManagerChannelListener,
        (IWifiP2pManagerChannel**)&mChannel);

    mReceiver = new CWifiBroadcastReceiver(mP2pManager, mChannel, this);

    StartRegistrationAndDiscovery();

    return NOERROR;
}

/**
 * Registers a local service and then initiates a service discovery
 */
ECode CActivityOne::StartRegistrationAndDiscovery()
{
    AutoPtr<IObjectStringMap> record;
    CObjectStringMap::New((IObjectStringMap**)&record);

    AutoPtr<ICharSequence> seq;
    CStringWrapper::New(String("visible"), (ICharSequence**)&seq);
    record->Put(TXTRECORD_PROP_AVAILABLE, seq);

    AutoPtr<IWifiP2pDnsSdServiceInfoHelper> helper;
    CWifiP2pDnsSdServiceInfoHelper::AcquireSingleton((IWifiP2pDnsSdServiceInfoHelper**)&helper);
    AutoPtr<IWifiP2pDnsSdServiceInfo> service;
    helper->NewInstance(SERVICE_INSTANCE, SERVICE_REG_TYPE, record,
        (IWifiP2pDnsSdServiceInfo**)&service);

    AutoPtr<IWifiP2pManagerActionListener> listener = new AddLocalServiceActionListener(this);
    mP2pManager->AddLocalService(mChannel, service, listener);

    DiscoverService();

    return NOERROR;
}

ECode CActivityOne::DiscoverService()
{
    /*
     * Register listeners for DNS-SD services. These are callbacks invoked
     * by the system when a service is actually discovered.
     */
    AutoPtr<IWifiP2pManagerDnsSdServiceResponseListener> respListener =
        new DnsSdServiceResponseListener(this);
    AutoPtr<IWifiP2pManagerDnsSdTxtRecordListener> txtRecordListener =
        new DnsSdTxtRecordListener(this);
    mP2pManager->SetDnsSdResponseListeners(mChannel, respListener, txtRecordListener);

    // After attaching listeners, create a service request and initiate
    // discovery.
    AutoPtr<IWifiP2pDnsSdServiceRequestHelper> helper;
    CWifiP2pDnsSdServiceRequestHelper::AcquireSingleton((IWifiP2pDnsSdServiceRequestHelper**)&helper);
    helper->NewInstance((IWifiP2pDnsSdServiceRequest**)&mServiceRequest);
    AutoPtr<IWifiP2pManagerActionListener> serviceRequestListener = new ServiceRequestActionListener(this);
    mP2pManager->AddServiceRequest(mChannel, mServiceRequest, serviceRequestListener);

    AutoPtr<IWifiP2pManagerActionListener> discoverServiceListener = new DiscoverServicesActionListener(this);
    mP2pManager->DiscoverServices(mChannel, discoverServiceListener);

    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Slogger::D(TAG, " >> OnCreate");
    Activity::OnCreate(savedInstanceState);

    SetContentView(R::layout::main);

    SetupViews();

    SetupWifi();

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Activity::OnStart();
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    Slogger::D(TAG, " >> OnResume");
    Activity::OnResume();

    AutoPtr<IIntent> intent;
    RegisterReceiver(mReceiver, mIntentFilter, (IIntent**)&intent);
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    Slogger::D(TAG, " >> OnPause");
    Activity::OnPause();

    UnregisterReceiver(mReceiver);
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    Slogger::D(TAG, " >> OnStop");

    if (mP2pManager != NULL && mChannel != NULL) {
        AutoPtr<IWifiP2pManagerActionListener> listener = new RemoveGroupActionListener(this);
        mP2pManager->RemoveGroup(mChannel, listener);
    }

    Activity::OnStop();
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    Slogger::D(TAG, " >> OnDestroy");
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

void CActivityOne::SetIsWifiP2pEnabled(
    /* [in] */ Boolean isWifiP2pEnabled)
{
    Slogger::D("CActivityOne", "SetIsWifiP2pEnabled %s", isWifiP2pEnabled ? "TRUE" : "FALSE");
    mIsWifiP2pEnabled = isWifiP2pEnabled;

    if (!mIsWifiP2pEnabled) {
        ResetData();
    }
}

void CActivityOne::ResetData()
{
}

ECode CActivityOne::OnChannelDisconnected()
{
    Slogger::D("CActivityOne", "OnChannelDisconnected.");

    // we will try once more
    if (mP2pManager != NULL && !mRetryChannel) {
        String info("Channel lost. Trying again");
        ShowToast(info, IToast::LENGTH_LONG);

        ResetData();
        mRetryChannel = TRUE;
        mChannel = NULL;

        AutoPtr<ILooper> looper;
        GetMainLooper((ILooper**)&looper);
        mP2pManager->Initialize(
            this, looper,
            mWifiP2pManagerChannelListener,
            (IWifiP2pManagerChannel**)&mChannel);
    }
    else {
        String info("Severe! Channel is probably lost premanently. Try Disable/Re-Enable P2P.");
        ShowToast(info, IToast::LENGTH_LONG);
    }
    return NOERROR;
}

ECode CActivityOne::OnConnectionChanged(
    /* [in] */ INetworkInfo* networkInfo)
{
    Boolean isConnected;
    networkInfo->IsConnected(&isConnected);

    Slogger::D(TAG, "OnConnectionChanged isConnected %s", isConnected ? "TRUE" : "FALSE");

    if (isConnected) {
        // we are connected with the other device, request connection
        // info to find group owner IP
        AutoPtr<IWifiP2pManagerConnectionInfoListener> listener = new ConnectionInfoListener(this);
        mP2pManager->RequestConnectionInfo(mChannel, listener);
    }
    else {
        // It's a disconnect
        ResetData();
    }
    return NOERROR;
}

ECode CActivityOne::OnThisDeviceChanged(
    /* [in] */ IWifiP2pDevice* deviceInfo)
{
    String info;
    if (deviceInfo != NULL) {
        deviceInfo->ToString(&info);
    }
    Slogger::D(TAG, "OnThisDeviceChanged: %s", info.string());
    return NOERROR;
}

ECode CActivityOne::OnSendMsgButtonClick()
{
    if (mChatManager == NULL) {
        Slogger::E(TAG, "ChatManager not ready!");
        return NOERROR;
    }

    static Int32 msgIndex = 0;
    static List<String> msgs;
    if (msgs.IsEmpty()) {
        msgs.PushBack(String("Hello World!"));
        msgs.PushBack(String("Hello Wifi!"));
        msgs.PushBack(String("Hello Elastos!"));
        msgs.PushBack(String("Hello Activiy!"));
    }

    Int32 index = (msgIndex++)/msgs.GetSize();
    String msg = msgs[index];
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(msg.GetByteLength());
    buffer->Copy((Byte*)msg.string(), msg.GetByteLength());

    mChatManager->Write(buffer);

    StringBuilder sb("Me: ");
    sb += msg;

    AppendInfo(sb.ToString());
    return NOERROR;
}

AutoPtr<IHandler> CActivityOne::GetHandler()
{
    return mHandler;
}

ECode CActivityOne::OnConnectionInfoAvailable(
    /* [in] */ IWifiP2pInfo* p2pInfo)
{
    AutoPtr<IThread> handlerThread;
    /*
     * The group owner accepts connections using a server socket and then spawns a
     * client socket for every client. This is handled by {@code
     * GroupOwnerSocketHandler}
     */

    assert(p2pInfo != NULL);
    Boolean isGroupOwner;
    p2pInfo->IsGroupOwner(&isGroupOwner);

    AutoPtr<IHandler> handler = GetHandler();
    if (isGroupOwner) {
        Slogger::D(TAG, "Connected as group owner");
        AppendInfo(String("Connected as group owner."));
        // try {
            handlerThread = new GroupOwnerSocketHandler(handler);
            handlerThread->Start();
        // } catch (IOException e) {
        //     Slogger::D(TAG,
        //             "Failed to create a server thread - " + e.getMessage());
        //     return;
        // }
    }
    else {
        AutoPtr<IInetAddress> address;
        p2pInfo->GetGroupOwnerAddress((IInetAddress**)&address);

        AppendInfo(String("Connected as peer."));
        Slogger::D(TAG, "Connected as peer");
        handlerThread = new ClientSocketHandler(handler, address);
        handlerThread->Start();
    }

    // chatFragment = new WiFiChatFragment();
    // getFragmentManager().beginTransaction()
    //         .replace(R.id.container_root, chatFragment).commit();
    // statusTxtView.setVisibility(View.GONE);
    return NOERROR;
}

ECode CActivityOne::ConnectToService(
    /* [in] */ const String& deviceName,
    /* [in] */ const String& deviceAddress,
    /* [in] */ const String& instanceName)
{
    AutoPtr<IWifiP2pConfig> config;
    CWifiP2pConfig::New((IWifiP2pConfig**)&config);
    config->SetDeviceAddress(deviceAddress);

    AutoPtr<IWpsInfo> wps;
    config->GetWps((IWpsInfo**)&wps);
    wps->SetSetup(IWpsInfo::PBC);

    if (mServiceRequest != NULL) {
        AutoPtr<IWifiP2pManagerActionListener> listener = new RemoveServiceRequestActionListener(this);
        mP2pManager->RemoveServiceRequest(mChannel, mServiceRequest, listener);
    }

    Slogger::D(TAG, "Connect to Service %s : %s ...", deviceName.string(), deviceAddress.string());
    AutoPtr<IWifiP2pManagerActionListener> listener = new ConnectActionListener(this);
    mP2pManager->Connect(mChannel, config, listener);
    return NOERROR;
}

ECode CActivityOne::OnDiscoveryButtonClick()
{
    if (mP2pManager != NULL && mChannel != NULL) {
        AutoPtr<IWifiP2pManagerActionListener> discoverServiceListener = new DiscoverServicesActionListener(this);
        mP2pManager->DiscoverServices(mChannel, discoverServiceListener);
    }

    return NOERROR;
}

ECode CActivityOne::UpdateServiceList()
{
    if (mServiceList.IsEmpty()) {
        Slogger::D(TAG, "DiscoverService list not ready.");
        mWifiListView->SetAdapter(NULL);
        return NOERROR;
    }

    Slogger::D(TAG, "UpdateServiceList: size :%d ...", mServiceList.GetSize());

    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);

    String name, address;
    List<AutoPtr<WifiP2pService> >::Iterator it = mServiceList.Begin();
    for (; it != mServiceList.End(); ++it) {
        WifiP2pService* service = *it;
        service->mDevice->GetDeviceName(&name);
        service->mDevice->GetDeviceAddress(&address);
        StringBuilder sb(name);
        sb += "=";
        sb += address;
        sb += "=";
        sb += service->mInstanceName;

        AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        dataList->Add(seq->Probe(EIID_IInterface));
    }

    AutoPtr<IArrayAdapter> adapter;
    CArrayAdapter::New(this, R::layout::list_item, dataList, (IArrayAdapter**)&adapter);
    assert(adapter != NULL);
    mWifiListView->SetAdapter(adapter.Get());
    return NOERROR;
}

ECode CActivityOne::AppendInfo(
    /* [in] */ const String& info)
{
    AutoPtr<ICharSequence> curInfo;
    mInfoTextView->GetText((ICharSequence**)&curInfo);

    StringBuilder sb("");
    if (curInfo != NULL) {
        String temp;
        curInfo->ToString(&temp);
        sb += temp;
        sb += "\n";
    }

    sb += info;

    curInfo = sb.ToCharSequence();
    mInfoTextView->SetText(curInfo);
    return NOERROR;
}

} // namespace WifiP2pServiceDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
