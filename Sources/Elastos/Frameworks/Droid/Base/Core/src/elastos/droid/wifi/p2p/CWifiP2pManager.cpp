#include "elastos/droid/wifi/p2p/CWifiP2pManager.h"
#include "elastos/droid/wifi/p2p/CWifiP2pManagerChannel.h"
#include "elastos/droid/wifi/p2p/CWifiP2pDevice.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/internal/utility/AsyncChannel.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Wifi::P2p::CWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::CWifiP2pManagerChannel;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

const String CWifiP2pManager::TAG("WifiP2pManager");
const Boolean CWifiP2pManager::DBG = TRUE;
const Int32 CWifiP2pManager::BASE = IProtocol::BASE_WIFI_P2P_MANAGER;

CAR_INTERFACE_IMPL(CWifiP2pManager, Object, IWifiP2pManager);

CAR_OBJECT_IMPL(CWifiP2pManager);

ECode CWifiP2pManager::constructor(
    /* [in] */ IIWifiP2pManager* service)
{
    mService = service;
    return NOERROR;
}

ECode CWifiP2pManager::Initialize(
    /* [in] */ IContext* srcContext,
    /* [in] */ ILooper* srcLooper,
    /* [in] */ IWifiP2pManagerChannelListener* listener,
    /* [out] */ IWifiP2pManagerChannel** channel)
{
    VALIDATE_NOT_NULL(channel);
    AutoPtr<IMessenger> messenger;
    GetMessenger((IMessenger**)&messenger);
    AutoPtr<IWifiP2pManagerChannel> temp = InitalizeChannel(srcContext, srcLooper, listener, messenger);
    *channel = temp;
    REFCOUNT_ADD(*channel);
    return NOERROR;
}

ECode CWifiP2pManager::InitializeInternal(
    /* [in] */ IContext* srcContext,
    /* [in] */ ILooper* srcLooper,
    /* [in] */ IWifiP2pManagerChannelListener* listener,
    /* [out] */ IWifiP2pManagerChannel** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMessenger> messenger;
    GetP2pStateMachineMessenger((IMessenger**)&messenger);
    AutoPtr<IWifiP2pManagerChannel> temp = InitalizeChannel(srcContext, srcLooper, listener, messenger);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiP2pManager::DiscoverPeers(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "DiscoverPeers");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel;// TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(DISCOVER_PEERS, 0, val);
    return NOERROR;
}

ECode CWifiP2pManager::StopPeerDiscovery(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "StopPeerDiscovery");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel;// TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(STOP_DISCOVERY, 0, val);
    return NOERROR;
}

ECode CWifiP2pManager::Connect(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pConfig* config,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG)  {
        String info;
        assert(0);
        // TODO
        // if (config) config->ToString(&info);
        Slogger::E(TAG, "Connect %s", info.string());
    }

    FAIL_RETURN(CheckChannel(c));
    FAIL_RETURN(CheckP2pConfig(config));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(CONNECT, 0, val,
        config ? IInterface::Probe(config) : NULL);

    return NOERROR;
}

ECode CWifiP2pManager::CancelConnect(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "CancelConnect");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(CANCEL_CONNECT, 0, val);
    return NOERROR;
}

ECode CWifiP2pManager::CreateGroup(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "CreateGroup");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(CREATE_GROUP, IWifiP2pGroup::PERSISTENT_NET_ID, val);
    return NOERROR;
}

ECode CWifiP2pManager::RemoveGroup(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "RemoveGroup");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    return asyncChannel->SendMessage(REMOVE_GROUP, 0, val);
}

ECode CWifiP2pManager::Listen(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ Boolean enable,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    FAIL_RETURN(CheckChannel(c));
    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel = wpmc->mAsyncChannel;
    Int32 value;
    ((CWifiP2pManagerChannel*)c)->PutListener(listener, &value);
    return asyncChannel->SendMessage(enable ? START_LISTEN : STOP_LISTEN,
            0, value);
}

ECode CWifiP2pManager::SetWifiP2pChannels(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ Int32 lc,
    /* [in] */ Int32 oc,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    FAIL_RETURN(CheckChannel(c));
    AutoPtr<IBundle> p2pChannels;
    CBundle::New((IBundle**)&p2pChannels);
    p2pChannels->PutInt32(String("lc"), lc);
    p2pChannels->PutInt32(String("oc"), oc);
    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel = wpmc->mAsyncChannel;
    Int32 value;
    ((CWifiP2pManagerChannel*)c)->PutListener(listener, &value);
    return asyncChannel->SendMessage(SET_CHANNEL, 0, value, p2pChannels);
}

ECode CWifiP2pManager::StartWps(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWpsInfo* wps,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    FAIL_RETURN(CheckChannel(c));
    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel = wpmc->mAsyncChannel;
    Int32 value;
    ((CWifiP2pManagerChannel*)c)->PutListener(listener, &value);
    return asyncChannel->SendMessage(START_WPS, 0, value, wps);
}

ECode CWifiP2pManager::AddLocalService(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pServiceInfo* servInfo,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "AddLocalService");
    FAIL_RETURN(CheckChannel(c));
    FAIL_RETURN(CheckServiceInfo(servInfo));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(ADD_LOCAL_SERVICE, 0, val,
        servInfo ? IInterface::Probe(servInfo) : NULL);
    return NOERROR;
}

ECode CWifiP2pManager::RemoveLocalService(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pServiceInfo* servInfo,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "RemoveLocalService");
    FAIL_RETURN(CheckChannel(c));
    FAIL_RETURN(CheckServiceInfo(servInfo));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(REMOVE_LOCAL_SERVICE, 0, val,
        servInfo ? IInterface::Probe(servInfo) : NULL);
    return NOERROR;
}

ECode CWifiP2pManager::ClearLocalServices(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "ClearLocalServices");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(CLEAR_LOCAL_SERVICES, 0, val);
    return NOERROR;
}

ECode CWifiP2pManager::SetServiceResponseListener(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerServiceResponseListener* listener)
{
    FAIL_RETURN(CheckChannel(c));
    assert(0);
    // TODO
    // return c->SetServRspListener(listener);
    return E_NOT_IMPLEMENTED;
}

ECode CWifiP2pManager::SetDnsSdResponseListeners(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerDnsSdServiceResponseListener* servListener,
    /* [in] */ IWifiP2pManagerDnsSdTxtRecordListener* txtListener)
{
    FAIL_RETURN(CheckChannel(c));
    assert(0);
    // TODO
    // c->SetDnsSdServRspListener(servListener);
    // c->SetDnsSdTxtListener(txtListener);
    return NOERROR;
}

ECode CWifiP2pManager::SetUpnpServiceResponseListener(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerUpnpServiceResponseListener* listener)
{
    FAIL_RETURN(CheckChannel(c));
    assert(0);
    // TODO
    // return c->SetUpnpServRspListener(listener);
    return E_NOT_IMPLEMENTED;
}

ECode CWifiP2pManager::DiscoverServices(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "DiscoverServices");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(DISCOVER_SERVICES, 0, val);
    return NOERROR;
}

ECode CWifiP2pManager::AddServiceRequest(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pServiceRequest* req,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "AddServiceRequest");
    FAIL_RETURN(CheckChannel(c));
    FAIL_RETURN(CheckServiceRequest(req));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(ADD_SERVICE_REQUEST, 0, val,
        req ? IInterface::Probe(req) : req);
    return NOERROR;
}

ECode CWifiP2pManager::RemoveServiceRequest(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pServiceRequest* req,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "RemoveServiceRequest");
    FAIL_RETURN(CheckChannel(c));
    FAIL_RETURN(CheckServiceRequest(req));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(REMOVE_SERVICE_REQUEST, 0, val,
        req ? IInterface::Probe(req) : req);
    return NOERROR;
}

ECode CWifiP2pManager::ClearServiceRequests(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "ClearServiceRequests");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(CLEAR_SERVICE_REQUESTS, 0, val);
    return NOERROR;
}

ECode CWifiP2pManager::RequestPeers(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerPeerListListener* listener)
{
    if (DBG) Slogger::D(TAG, "RequestPeers");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(REQUEST_PEERS, 0, val);
    return NOERROR;
}

ECode CWifiP2pManager::RequestConnectionInfo(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerConnectionInfoListener* listener)
{
    if (DBG) Slogger::D(TAG, "RequestConnectionInfo");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(REQUEST_CONNECTION_INFO, 0, val);
    return NOERROR;
}

ECode CWifiP2pManager::RequestGroupInfo(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerGroupInfoListener* listener)
{
    if (DBG) Slogger::D(TAG, "RequestGroupInfo");
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(REQUEST_GROUP_INFO, 0, val);
    return NOERROR;
}

ECode CWifiP2pManager::SetDeviceName(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ const String& devName,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "SetDeviceName: [%s]", devName.string());
    FAIL_RETURN(CheckChannel(c));

    AutoPtr<IWifiP2pDevice> d;
    CWifiP2pDevice::New((IWifiP2pDevice**)&d);
    d->SetDeviceName(devName);

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(SET_DEVICE_NAME, 0, val, IInterface::Probe(d));
    return NOERROR;
}

/** @hide */
ECode CWifiP2pManager::SetWFDInfo(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pWfdInfo* wfdInfo,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) {
        String info;
        assert(0);
        // TODO
        // if (wfdInfo) wfdInfo->ToString(&info);
        Slogger::D(TAG, "SetWFDInfo: \n", info.string());
    }
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(SET_WFD_INFO, 0, val,
        wfdInfo ? IInterface::Probe(wfdInfo) : NULL);
    return NOERROR;
}

ECode CWifiP2pManager::SetGroupOwnerPsk(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ const String& psk,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    if (DBG) Slogger::D(TAG, "SetGroupOwnerPsk");
    FAIL_RETURN(CheckChannel(c));

    AutoPtr<IWifiP2pDevice> d;
    CWifiP2pDevice::New((IWifiP2pDevice**)&d);
    d->SetDeviceName(psk);

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    // asyncChannel->SendMessage(SET_GO_PSK, 0, val,
    //     d ? d->Probe(EIID_IInterface) : NULL);
    return NOERROR;
}

ECode CWifiP2pManager::DeletePersistentGroup(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ Int32 netId,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(DELETE_PERSISTENT_GROUP, netId, val);
    return NOERROR;
}

ECode CWifiP2pManager::RequestPersistentGroupInfo(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerPersistentGroupInfoListener* listener)
{
    FAIL_RETURN(CheckChannel(c));

    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel; // TODO = wpmc->mAsyncChannel;

    Int32 val;
    assert(0);
    // TODO
    // c->PutListener(listener ? listener->Probe(EIID_IInterface) : NULL, &val);
    asyncChannel->SendMessage(REQUEST_PERSISTENT_GROUP_INFO, 0, val);
    return NOERROR;
}

ECode CWifiP2pManager::SetMiracastMode(
    /* [in] */ Int32 mode)
{
    // try {
        return mService->SetMiracastMode(mode);
    // } catch(RemoteException e) {
    //    // ignore
    // }
}

ECode CWifiP2pManager::GetMessenger(
    /* [out] */ IMessenger** msg)
{
    VALIDATE_NOT_NULL(msg);
    *msg = NULL;

    // try {
    return mService->GetMessenger(msg);
    // } catch (RemoteException e) {
    //     return null;
    // }
}

ECode CWifiP2pManager::GetP2pStateMachineMessenger(
    /* [out] */ IMessenger** result)
{
    // try {
        return mService->GetP2pStateMachineMessenger(result);
    // } catch (RemoteException e) {
    //     return null;
    // }
}

ECode CWifiP2pManager::GetNfcHandoverRequest(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerHandoverMessageListener* listener)
{
    FAIL_RETURN(CheckChannel(c));
    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel = wpmc->mAsyncChannel;
    Int32 value;
    ((CWifiP2pManagerChannel*)c)->PutListener(listener, &value);
    return asyncChannel->SendMessage(GET_HANDOVER_REQUEST, 0, value);
}

ECode CWifiP2pManager::GetNfcHandoverSelect(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ IWifiP2pManagerHandoverMessageListener* listener)
{
    FAIL_RETURN(CheckChannel(c));
    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel = wpmc->mAsyncChannel;
    Int32 value;
    ((CWifiP2pManagerChannel*)c)->PutListener(listener, &value);
    return asyncChannel->SendMessage(GET_HANDOVER_SELECT, 0, value);
}

ECode CWifiP2pManager::InitiatorReportNfcHandover(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ const String& handoverSelect,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    FAIL_RETURN(CheckChannel(c));
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(EXTRA_HANDOVER_MESSAGE, handoverSelect);
    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel = wpmc->mAsyncChannel;
    Int32 value;
    ((CWifiP2pManagerChannel*)c)->PutListener(listener, &value);
    return asyncChannel->SendMessage(INITIATOR_REPORT_NFC_HANDOVER, 0,
            value, bundle);
}

ECode CWifiP2pManager::ResponderReportNfcHandover(
    /* [in] */ IWifiP2pManagerChannel* c,
    /* [in] */ const String& handoverRequest,
    /* [in] */ IWifiP2pManagerActionListener* listener)
{
    FAIL_RETURN(CheckChannel(c));
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(EXTRA_HANDOVER_MESSAGE, handoverRequest);
    CWifiP2pManagerChannel* wpmc = (CWifiP2pManagerChannel*)c;
    AutoPtr<AsyncChannel> asyncChannel = wpmc->mAsyncChannel;
    Int32 value;
    ((CWifiP2pManagerChannel*)c)->PutListener(listener, &value);
    return asyncChannel->SendMessage(RESPONDER_REPORT_NFC_HANDOVER, 0,
            value, bundle);
}

ECode CWifiP2pManager::CheckChannel(
    /* [in] */ IWifiP2pManagerChannel* c)
{
    if (c == NULL) {
        Slogger::E(TAG, "E_ILLEGAL_ARGUMENT_EXCEPTION: Channel needs to be initialized");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode CWifiP2pManager::CheckServiceInfo(
    /* [in] */ IWifiP2pServiceInfo* info)
{
    if (info == NULL) {
        Slogger::E(TAG, "E_ILLEGAL_ARGUMENT_EXCEPTION: service info is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode CWifiP2pManager::CheckServiceRequest(
    /* [in] */ IWifiP2pServiceRequest* req)
{
    if (req == NULL) {
        Slogger::E(TAG, "E_ILLEGAL_ARGUMENT_EXCEPTION: service request is NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode CWifiP2pManager::CheckP2pConfig(
    /* [in] */ IWifiP2pConfig* c)
{
    if (c == NULL) {
        //throw new IllegalArgumentException("config cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String deviceAddress;
    c->GetDeviceAddress(&deviceAddress);
    if (TextUtils::IsEmpty(deviceAddress)) {
        //throw new IllegalArgumentException("deviceAddress cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

AutoPtr<IWifiP2pManagerChannel> CWifiP2pManager::InitalizeChannel(
    /* [in] */ IContext* srcContext,
    /* [in] */ ILooper* srcLooper,
    /* [in] */ IWifiP2pManagerChannelListener* listener,
    /* [in] */ IMessenger* messenger)
{
    if (messenger == NULL) return NULL;

    AutoPtr<IWifiP2pManagerChannel> c;
    CWifiP2pManagerChannel::New(srcContext, srcLooper, listener, (IWifiP2pManagerChannel**)&c);
    Int32 status;
    ((CWifiP2pManagerChannel*)c.Get())->mAsyncChannel->ConnectSync(srcContext, ((CWifiP2pManagerChannel*)c.Get())->mHandler, messenger, &status);
    if (status == IAsyncChannel::STATUS_SUCCESSFUL) {
        return c;
    }
    else {
        return NULL;
    }
}

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
