
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

using Elastos::Utility::Logging::Slogger;

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CStringWrapper;
using Elastos::Core::CObjectContainer;
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

using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;

using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Droid::Wifi::CWpsInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pWfdInfo;
using Elastos::Droid::Wifi::P2p::CWifiP2pWfdInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::CWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerChannelListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerGroupInfoListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerChannelListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerActionListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerPeerListListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerConnectionInfoListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pDemo {

String CActivityOne::TAG("WifiP2pDemo");

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
    Slogger::D("CActivityOne::ListView", "OnItemClick position %d", position);
    ITextView* tv = ITextView::Probe(view);
    assert(tv != NULL);

    AutoPtr<ICharSequence> cs;
    tv->GetText((ICharSequence**)&cs);
    String deviceInfo;
    cs->ToString(&deviceInfo);

    AutoPtr<ArrayOf<String> > infos;
    StringUtils::Split(deviceInfo, String("="), (ArrayOf<String>**)&infos);

    mHost->ConnectToPeer((*infos)[0], (*infos)[1]);
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
        /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);

    if (id == R::id::SettingButton) {
        mHost->OnUpdateSettingButtonClick();
    }
    else if (id == R::id::DiscoveryButton) {
        mHost->OnDiscoveryButtonClick();
    }
    else if (id == R::id::DisconnectButton) {
        mHost->Disconnect();
    }
    return NOERROR;
}

//======================================================================
// CActivityOne::DiscoverPeerActionListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::DiscoverPeerActionListener, IWifiP2pManagerActionListener)

CActivityOne::DiscoverPeerActionListener::DiscoverPeerActionListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::DiscoverPeerActionListener::OnSuccess()
{
    Slogger::D("DiscoverPeerActionListener", "Discovery Initiated!");

    String info("Discovery Initiated");
    mHost->ShowToast(info);

    return NOERROR;
}

ECode CActivityOne::DiscoverPeerActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    Slogger::D("DiscoverPeerActionListener", "Discovery Failed! reasonCode: %d", reason);

    String info("Discovery Failed : ");
    info.AppendFormat("%d", reason);

    mHost->ShowToast(info);

    return NOERROR;
}

//======================================================================
// CActivityOne::RequestPeerListListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::RequestPeerListListener, IWifiP2pManagerPeerListListener)

CActivityOne::RequestPeerListListener::RequestPeerListListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::RequestPeerListListener::OnPeersAvailable(
    /* [in] */ IWifiP2pDeviceList* peers)
{
    assert(peers != NULL);
    String info;
    peers->ToString(&info);
    Slogger::D("CActivityOne::RequestPeerListListener", "OnPeersAvailable: [%s]", info.string());
    mHost->mP2pDeviceList = peers;

    mHost->UpdateWifiList();
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
// CActivityOne::CreateGroupActionListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::CreateGroupActionListener, IWifiP2pManagerActionListener)

CActivityOne::CreateGroupActionListener::CreateGroupActionListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::CreateGroupActionListener::OnSuccess()
{
    Slogger::D("CreateGroupActionListener", "Create group success!");
    String info("Create group success!");
    mHost->ShowToast(info);
    return NOERROR;
}

ECode CActivityOne::CreateGroupActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    Slogger::D("CreateGroupActionListener", "Create group Failed! reasonCode: %d", reason);

    String info("Create group Failed! reasonCode : ");
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
    Slogger::D("RemoveGroupActionListener", "Remove group success!");
    String info("Remove group success!");
    mHost->ShowToast(info);
    return NOERROR;
}

ECode CActivityOne::RemoveGroupActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    Slogger::D("RemoveGroupActionListener", "Remove group Failed! reasonCode: %d", reason);

    String info("Remove group Failed! reasonCode : ");
    info.AppendFormat("%d", reason);

    mHost->ShowToast(info);
    return NOERROR;
}

//======================================================================
// CActivityOne::CancelConnectActionListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::CancelConnectActionListener, IWifiP2pManagerActionListener)

CActivityOne::CancelConnectActionListener::CancelConnectActionListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::CancelConnectActionListener::OnSuccess()
{
    Slogger::D("CancelConnectActionListener", "Cancel connect success!");
    String info("Cancel connect success!");
    mHost->ShowToast(info);
    return NOERROR;
}

ECode CActivityOne::CancelConnectActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    Slogger::D("CancelConnectActionListener", "Cancel connect Failed! reasonCode: %d", reason);

    String info("Cancel connect Failed! reasonCode : ");
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
    /* [in] */ IWifiP2pInfo* peers)
{
    assert(peers != NULL);
    String info;
    peers->ToString(&info);
    Slogger::D("ConnectionInfoListener", "OnConnectionInfoAvailable: %s", info.string());
    mHost->OnConnectionInfoAvailable(peers);
    return NOERROR;
}

//======================================================================
// CActivityOne::ConnectionInfoListener
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
// CActivityOne::GroupInfoListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::GroupInfoListener, IWifiP2pManagerGroupInfoListener)

CActivityOne::GroupInfoListener::GroupInfoListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::GroupInfoListener::OnGroupInfoAvailable(
    /* [in] */ IWifiP2pGroup* peers)
{
    mHost->OnGroupInfoAvailable(peers);
    return NOERROR;
}

//======================================================================
// CActivityOne::DisplayListener
//======================================================================
CAR_INTERFACE_IMPL(CActivityOne::DisplayListener, IDisplayListener)

CActivityOne::DisplayListener::DisplayListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::DisplayListener::OnDisplayAdded(
    /* [in] */ Int32 displayId)
{
    Slogger::D(CActivityOne::TAG, " >> OnDisplayAdded %d", displayId);
    return NOERROR;
}

ECode CActivityOne::DisplayListener::OnDisplayChanged(
    /* [in] */ Int32 displayId)
{
    Slogger::D(CActivityOne::TAG, " >> OnDisplayChanged %d", displayId);
    return NOERROR;
}

ECode CActivityOne::DisplayListener::OnDisplayRemoved(
    /* [in] */ Int32 displayId)
{
    Slogger::D(CActivityOne::TAG, " >> OnDisplayRemoved %d", displayId);
    return NOERROR;
}

//======================================================================
// CActivityOne
//======================================================================
CActivityOne::CActivityOne()
    : mIsWifiP2pEnabled(0)
    , mRetryChannel(0)
{}

ECode CActivityOne::ShowToast(
    /* [in] */ const String& info,
    /* [in] */ Int32 style)
{
    // AutoPtr<ICharSequence> cs;
    // CStringWrapper::New(info, (ICharSequence**)&cs);
    // AutoPtr<IToastHelper> helper;
    // CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    // AutoPtr<IToast> toast;
    // helper->MakeText((IContext*)this->Probe(EIID_IContext), cs, style, (IToast**)&toast);
    // toast->Show();

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

    view = FindViewById(R::id::SettingButton);
    assert(view != NULL);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::DiscoveryButton);
    assert(view != NULL);
    view->SetOnClickListener(l.Get());

    view = FindViewById(R::id::DisconnectButton);
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
        (IContext*)this->Probe(EIID_IContext), looper,
        mWifiP2pManagerChannelListener,
        (IWifiP2pManagerChannel**)&mChannel);

    mReceiver = new CWifiBroadcastReceiver(mP2pManager, mChannel, this);

    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Slogger::D(TAG, " >> OnCreate");
    Activity::OnCreate(savedInstanceState);

    SetContentView(R::layout::main);

    SetupViews();

    if (mDisplayManager == NULL) {
        GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&mDisplayManager);

        AutoPtr<IDisplayListener> listener = new DisplayListener(this);
        mDisplayManager->RegisterDisplayListener(listener, NULL);
    }

    SetupWifi();

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
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

    Disconnect();
    UnregisterReceiver(mReceiver);
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    Slogger::D(TAG, " >> OnStop");
    Disconnect();
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
    mP2pDeviceList = NULL;
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
            (IContext*)this->Probe(EIID_IContext), looper,
            mWifiP2pManagerChannelListener,
            (IWifiP2pManagerChannel**)&mChannel);
    }
    else {
        String info("Severe! Channel is probably lost premanently. Try Disable/Re-Enable P2P.");
        ShowToast(info, IToast::LENGTH_LONG);
    }
    return NOERROR;
}

ECode CActivityOne::OnPeersChanged()
{
    // request available peers from the wifi p2p manager. This is an
    // asynchronous call and the calling activity is notified with a
    // callback on PeerListListener.onPeersAvailable()

    Slogger::D(TAG, "P2P peers changed, RequestPeers ...");
    AutoPtr<IWifiP2pManagerPeerListListener> listener = new RequestPeerListListener(this);
    mP2pManager->RequestPeers(mChannel, listener);

    return NOERROR;
}

ECode CActivityOne::OnConnectionChanged(
    /* [in] */ INetworkInfo* networkInfo)
{
    Boolean isConnected;
    networkInfo->IsConnected(&isConnected);

    Slogger::D(TAG, "OnConnectionChanged isConnected %s", isConnected ? "TRUE" : "FALSE");
    AppendInfo(String(isConnected ? "OnConnectionChanged: Connected!" : "OnConnectionChanged: Unconnect!"));

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

        AutoPtr<IWifiP2pWfdInfo> wfdInfo;
        deviceInfo->GetWfdInfo((IWifiP2pWfdInfo**)&wfdInfo);
        Int32 deviceType;
        wfdInfo->GetDeviceType(&deviceType);

        Boolean isCoupledSinkSupportedAtSource;
        wfdInfo->IsCoupledSinkSupportedAtSource(&isCoupledSinkSupportedAtSource);

        Boolean isCoupledSinkSupportedAtSink;
        wfdInfo->IsCoupledSinkSupportedAtSink(&isCoupledSinkSupportedAtSink);

        Boolean isSessionAvailable;
        wfdInfo->IsSessionAvailable(&isSessionAvailable);

        Int32 controlPort;
        wfdInfo->GetControlPort(&controlPort);

        Slogger::D(TAG, "OnThisDeviceChanged: deviceType: %d, atSource: %d, atSink: %d, session: %d, controlPort: %d",
            deviceType, isCoupledSinkSupportedAtSource, isCoupledSinkSupportedAtSink, isSessionAvailable, controlPort);
    }
    Slogger::D(TAG, "OnThisDeviceChanged: %s", info.string());

    return NOERROR;
}

ECode CActivityOne::OnConnectionInfoAvailable(
    /* [in] */ IWifiP2pInfo* info)
{
    Boolean isGroupOwner;
    info->IsGroupOwner(&isGroupOwner);
    if (isGroupOwner) {
        ShowToast(String("create Group success!"));
    }

    AutoPtr<IInetAddress> groupOwnerAddress;
    info->GetGroupOwnerAddress((IInetAddress**)&groupOwnerAddress);
    String address;
    if (groupOwnerAddress)
        groupOwnerAddress->ToString(&address);

    Slogger::D(TAG, "OnConnectionInfoAvailable: isGroupOwner: %d, groupOwnerAddress: %s",
        isGroupOwner, address.string());
    return NOERROR;
}

ECode CActivityOne::OnGroupInfoAvailable(
    /* [in] */ IWifiP2pGroup* group)
{
    String groupInfo;
    group->ToString(&groupInfo);
    Slogger::D(TAG, "OnGroupInfoAvailable: %s", groupInfo.string());
    return NOERROR;
}

ECode CActivityOne::OnUpdateSettingButtonClick()
{
    if (mP2pManager != NULL) {
        AutoPtr<IWifiP2pWfdInfo> wfdInfo;
        CWifiP2pWfdInfo::New(337, 0, 300, (IWifiP2pWfdInfo**)&wfdInfo);

        mP2pManager->SetWFDInfo(mChannel, wfdInfo, NULL);
    }
    return NOERROR;
}

ECode CActivityOne::CreateGroup()
{
    if (mChannel != NULL) {
        AutoPtr<IWifiP2pManagerActionListener> listener = new CreateGroupActionListener(this);
        mP2pManager->CreateGroup(mChannel, listener);
    }
    return NOERROR;
}

ECode CActivityOne::Disconnect()
{
    if (mP2pManager != NULL) {
        AutoPtr<IWifiP2pManagerActionListener> cancelConnectlistener = new CancelConnectActionListener(this);
        mP2pManager->CancelConnect(mChannel, cancelConnectlistener);

        AutoPtr<IWifiP2pManagerActionListener> removeGrouplistener = new RemoveGroupActionListener(this);
        mP2pManager->RemoveGroup(mChannel, removeGrouplistener);
    }

    return NOERROR;
}

ECode CActivityOne::ConnectToPeer(
    /* [in] */ const String& deviceName,
    /* [in] */ const String& deviceAddress)
{
    AutoPtr<IWifiP2pConfig> config;
    CWifiP2pConfig::New((IWifiP2pConfig**)&config);
    config->SetDeviceAddress(deviceAddress);

    AutoPtr<IWpsInfo> wpsInfo;
    config->GetWps((IWpsInfo**)&wpsInfo);
    if (wpsInfo == NULL) {
        CWpsInfo::New((IWpsInfo**)&wpsInfo);
    }
    wpsInfo->SetSetup(IWpsInfo::DISPLAY); // PBC, DISPLAY, KEYPAD, LABEL

    config->SetWps(wpsInfo);
    // config->SetGroupOwnerIntent(0);// 0 ~ 15

    Slogger::D(TAG, "Connect to peer %s : %s ...", deviceName.string(), deviceAddress.string());
    AutoPtr<IWifiP2pManagerActionListener> listener = new ConnectActionListener(this);
    mP2pManager->Connect(mChannel, config, listener);

    if (mDisplayManager) {
        // mDisplayManager->ConnectWifiDisplay(deviceAddress);
    }

    return NOERROR;
}

ECode CActivityOne::OnDiscoveryButtonClick()
{
    Slogger::D(TAG, "Discovery peers ...");
    AutoPtr<IWifiP2pManagerActionListener> listener = new DiscoverPeerActionListener(this);
    mP2pManager->DiscoverPeers(mChannel, listener);
    return NOERROR;
}

ECode CActivityOne::UpdateWifiList()
{
    if (mP2pDeviceList == NULL) {
        Slogger::D(TAG, "UpdateWifiList device list not ready.");
        mWifiListView->SetAdapter(NULL);
        return NOERROR;
    }

    AutoPtr<ArrayOf<IWifiP2pDevice*> > devices;
    mP2pDeviceList->GetDeviceList((ArrayOf<IWifiP2pDevice*>**)&devices);
    if (devices == NULL || devices->GetLength() == 0) {
        Slogger::D(TAG, "UpdateWifiList no device discoveried.");
        mWifiListView->SetAdapter(NULL);
        return NOERROR;
    }

    Slogger::D(TAG, "UpdateWifiList: size :%d ...", devices->GetLength());

    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);

    String name, address;
    for (Int32 i = 0; i < devices->GetLength(); ++i) {
        IWifiP2pDevice* device = (*devices)[i];
        device->GetDeviceName(&name);
        device->GetDeviceAddress(&address);
        StringBuilder sb(name);
        sb += "=";
        sb += address;

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

} // namespace WifiP2pDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
