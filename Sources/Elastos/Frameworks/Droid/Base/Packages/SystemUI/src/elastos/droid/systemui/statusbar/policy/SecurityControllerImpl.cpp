
#include "elastos/droid/systemui/statusbar/policy/SecurityControllerImpl.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Internal::Net::CVpnConfigHelper;
using Elastos::Droid::Internal::Net::IVpnConfigHelper;
using Elastos::Droid::Net::CNetworkRequestBuilder;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::INetworkRequestBuilder;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String SecurityControllerImpl::TAG("SecurityControllerImpl");
const Boolean SecurityControllerImpl::DEBUG = FALSE;
const Int32 SecurityControllerImpl::NO_NETWORK = -1;

SecurityControllerImpl::NetworkCallback::NetworkCallback(
    /* [in] */ SecurityControllerImpl* host)
    : mHost(host)
{}

ECode SecurityControllerImpl::NetworkCallback::OnAvailable(
    /* [in] */ INetwork* network)
{
    AutoPtr<INetworkCapabilities> networkCapabilities;
    if (mHost->mConnectivityManager != NULL) {
        mHost->mConnectivityManager->GetNetworkCapabilities(network
                , (INetworkCapabilities**)&networkCapabilities);
    }
    Int32 netId = 0;
    network->GetNetId(&netId);
    if (DEBUG) Logger::D(TAG, "onAvailable %d : %p", netId, networkCapabilities.Get());
    Boolean tmp = FALSE;
    if (networkCapabilities->HasTransport(INetworkCapabilities::TRANSPORT_VPN, &tmp), tmp) {
        mHost->SetCurrentNetid(netId);
    }
    return NOERROR;
}

ECode SecurityControllerImpl::NetworkCallback::OnLost(
    /* [in] */ INetwork* network)
{
    Int32 netId = 0;
    network->GetNetId(&netId);
    if (DEBUG) Logger::D(TAG, "onLost %d", netId);
    if (mHost->mCurrentVpnNetworkId == netId) {
        mHost->SetCurrentNetid(NO_NETWORK);
    }
    return NOERROR;
}


static AutoPtr<INetworkRequest> InitREQUEST()
{
    AutoPtr<INetworkRequestBuilder> builder;
    CNetworkRequestBuilder::New((INetworkRequestBuilder**)&builder);
    builder->RemoveCapability(INetworkCapabilities::NET_CAPABILITY_NOT_VPN);
    builder->RemoveCapability(INetworkCapabilities::NET_CAPABILITY_NOT_RESTRICTED);
    builder->RemoveCapability(INetworkCapabilities::NET_CAPABILITY_TRUSTED);

    AutoPtr<INetworkRequest> nr;
    builder->Build((INetworkRequest**)&nr);
    return nr;
}

AutoPtr<INetworkRequest> SecurityControllerImpl::REQUEST = InitREQUEST();

CAR_INTERFACE_IMPL(SecurityControllerImpl, Object, ISecurityController)

SecurityControllerImpl::SecurityControllerImpl(
    /* [in] */ IContext* context)
    : mCurrentVpnNetworkId(NO_NETWORK)
{
    mNetworkCallback = new NetworkCallback(this);

    AutoPtr<IInterface> obj;
    obj = ServiceManager::GetService(IContext::CONNECTIVITY_SERVICE);
    mConnectivityService = IIConnectivityManager::Probe(obj);

    CArrayList::New((IArrayList**)&mCallbacks);

    Logger::I(TAG, " >> TODO: CONNECTIVITY_SERVICE");
    mContext = context;
    obj = NULL;
    // context->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    mDevicePolicyManager = IDevicePolicyManager::Probe(obj);

    obj = NULL;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    // mConnectivityManager = IConnectivityManager::Probe(obj);

    // // TODO: re-register network callback on user change.
    // if (mConnectivityManager != NULL) {
    //     mConnectivityManager->RegisterNetworkCallback(REQUEST, mNetworkCallback);
    // }

    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);
    amHelper->GetCurrentUser(&mCurrentUserId);
}

ECode SecurityControllerImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("SecurityController state:"));
    pw->Print(String("  mCurrentVpnNetworkId="));
    pw->Println(mCurrentVpnNetworkId);
    pw->Print(String("  mVpnConfig="));
    pw->Println(mVpnConfig);
    pw->Print(String("  mVpnName="));
    pw->Println(mVpnName);
    return NOERROR;
}

ECode SecurityControllerImpl::HasDeviceOwner(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    String value;
    if (mDevicePolicyManager != NULL) {
        Logger::D(TAG, "TODO: Not Implement===[DEVICE_POLICY_SERVICE].");
        // mDevicePolicyManager->GetDeviceOwner(&value);
    }
    *result = !TextUtils::IsEmpty(value);
    return NOERROR;
}

ECode SecurityControllerImpl::HasProfileOwner(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    String value;
    if (mDevicePolicyManager != NULL) {
        Logger::D(TAG, "TODO: Not Implement===[DEVICE_POLICY_SERVICE].");
        // mDevicePolicyManager->GetProfileOwnerNameAsUser(mCurrentUserId, &value);
    }
    *result = !TextUtils::IsEmpty(value);
    return NOERROR;
}

ECode SecurityControllerImpl::GetDeviceOwnerName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mDevicePolicyManager != NULL) {
        Logger::D(TAG, "TODO: Not Implement===[DEVICE_POLICY_SERVICE].");
        // return mDevicePolicyManager->GetDeviceOwnerName(result);
    }

    *result = String(NULL);
    return NOERROR;
}

ECode SecurityControllerImpl::GetProfileOwnerName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mDevicePolicyManager != NULL) {
        Logger::D(TAG, "TODO: Not Implement===[DEVICE_POLICY_SERVICE].");
        // return mDevicePolicyManager->GetProfileOwnerNameAsUser(mCurrentUserId, result);
    }

    *result = String(NULL);
    return NOERROR;
}

ECode SecurityControllerImpl::IsVpnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCurrentVpnNetworkId != NO_NETWORK;
    return NOERROR;
}

ECode SecurityControllerImpl::IsLegacyVpn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mVpnConfig->GetLegacy(result);
    return NOERROR;
}

ECode SecurityControllerImpl::GetVpnApp(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVpnName;
    return NOERROR;
}

ECode SecurityControllerImpl::GetLegacyVpnName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return mVpnConfig->GetSession(result);
}

ECode SecurityControllerImpl::DisconnectFromVpn()
{
    ECode ec = NOERROR;
    do {
        Boolean tmp = FALSE;
        if (IsLegacyVpn(&tmp), tmp) {
            if (mConnectivityService != NULL) {
                ec = mConnectivityService->PrepareVpn(IVpnConfig::LEGACY_VPN,
                    IVpnConfig::LEGACY_VPN, &tmp);
            }
            if (FAILED(ec)) break;
        }
        else {
            // Prevent this app from initiating VPN connections in the future without user
            // intervention.
            if (mConnectivityService != NULL) {
                ec = mConnectivityService->SetVpnPackageAuthorization(FALSE);
            }
            if (FAILED(ec)) break;

            String user;
            mVpnConfig->GetUser(&user);
            if (mConnectivityService != NULL) {
                ec = mConnectivityService->PrepareVpn(user, IVpnConfig::LEGACY_VPN, &tmp);
            }
        }
    } while (0);

    if (FAILED(ec)) {
        Logger::E(TAG, "Unable to disconnect from VPN");
    }
    return NOERROR;
}

ECode SecurityControllerImpl::RemoveCallback(
    /* [in] */ ISecurityControllerCallback* callback)
{
    if (callback == NULL) return NOERROR;
    if (DEBUG) Logger::D(TAG, "removeCallback %p", callback);
    mCallbacks->Remove(callback);
    return NOERROR;
}

ECode SecurityControllerImpl::AddCallback(
    /* [in] */ ISecurityControllerCallback* callback)
{
    Boolean tmp = FALSE;
    if (callback == NULL || (mCallbacks->Contains(callback, &tmp), tmp)) return NOERROR;
    if (DEBUG) Logger::D(TAG, "addCallback %p", callback);
    mCallbacks->Add(callback);
    return NOERROR;
}

ECode SecurityControllerImpl::OnUserSwitched(
    /* [in] */ Int32 newUserId)
{
    mCurrentUserId = newUserId;
    FireCallbacks();
    return NOERROR;
}

void SecurityControllerImpl::SetCurrentNetid(
    /* [in] */ Int32 netId)
{
    if (netId != mCurrentVpnNetworkId) {
        mCurrentVpnNetworkId = netId;
        UpdateState();
        FireCallbacks();
    }
}

void SecurityControllerImpl::FireCallbacks()
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> callback;
        ator->GetNext((IInterface**)&callback);
        ISecurityControllerCallback::Probe(callback)->OnStateChanged();
    }
}

void SecurityControllerImpl::UpdateState()
{
    ECode ec = NOERROR;
    do {
        if (mConnectivityService != NULL) {
            mVpnConfig = NULL;
            ec = mConnectivityService->GetVpnConfig((IVpnConfig**)&mVpnConfig);
        }
        if (FAILED(ec)) break;

        Boolean legacy = FALSE;
        if (mVpnConfig != NULL && (mVpnConfig->GetLegacy(&legacy), !legacy)) {
            AutoPtr<IVpnConfigHelper> helper;
            CVpnConfigHelper::AcquireSingleton((IVpnConfigHelper**)&helper);
            AutoPtr<ICharSequence> cs;
            String user;
            mVpnConfig->GetUser(&user);
            ec = helper->GetVpnLabel(mContext, user, (ICharSequence**)&cs);
            if (FAILED(ec)) break;
            cs->ToString(&mVpnName);
        }
    } while (/*RemoteException | NameNotFoundException e*/0);

    if (FAILED(ec)) {
        Logger::W(TAG, "Unable to get current VPN");
    }
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
