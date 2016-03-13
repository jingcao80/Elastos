
#include "elastos/droid/server/connectivity/CTethering.h"
#include "elastos/droid/server/IoThread.h"
#include <elastos/droid/net/NetworkUtils.h>
#include <elastos/droid/R.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Net.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Net::INetwork;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Net::IRouteInfoHelper;
using Elastos::Droid::Net::CRouteInfoHelper;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::NetworkInfoDetailedState_FAILED;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::IInterfaceConfiguration;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Server::IoThread;

using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Utility::Arrays;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInet4AddressHelper;
using Elastos::Net::CInet4AddressHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

//===========================================================================
//  CTethering::StateReceiver
//===========================================================================
ECode CTethering::StateReceiver::OnReceive(
    /* [in] */ IContext* content,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.IsNull()) return NOERROR;

    if (action.Equals(IUsbManager::ACTION_USB_STATE)) {
        Object& lockObj = mHost->mPublicSync;
        AutoLock lock(lockObj);
        Boolean usbConnected;
        intent->GetBooleanExtra(IUsbManager::USB_CONNECTED, FALSE, &usbConnected);
        intent->GetBooleanExtra(IUsbManager::USB_FUNCTION_RNDIS, FALSE, &mHost->mRndisEnabled);
        // start tethering if we have a request pending
        if (usbConnected && mHost->mRndisEnabled && mHost->mUsbTetherRequested) {
            mHost->TetherUsb(TRUE);
        }
        mHost->mUsbTetherRequested = FALSE;
    }
    else if (action.Equals(IConnectivityManager::CONNECTIVITY_ACTION)) {
        AutoPtr<IParcelable> obj;
        intent->GetParcelableExtra(
                IConnectivityManager::EXTRA_NETWORK_INFO, (IParcelable**)&obj);
        AutoPtr<INetworkInfo> networkInfo = INetworkInfo::Probe(obj);
        NetworkInfoDetailedState state;
        if (networkInfo != NULL &&
                (networkInfo->GetDetailedState(&state), state != NetworkInfoDetailedState_FAILED)) {
            if (VDBG) Logger::D(TAG, "Tethering got CONNECTIVITY_ACTION");
            mHost->mTetherMasterSM->SendMessage(TetherMasterSM::CMD_UPSTREAM_CHANGED);
        }
    }
    else if (action.Equals(IIntent::ACTION_CONFIGURATION_CHANGED)) {
        mHost->UpdateConfiguration();
    }
    return NOERROR;
}


//===========================================================================
//  CTethering::TetherInterfaceSM::InitialState
//===========================================================================
ECode CTethering::TetherInterfaceSM::InitialState::Enter()
{
    mHost->SetAvailable(TRUE);
    mHost->SetTethered(FALSE);
    mHost->mHost->SendTetherStateChangedBroadcast();
    return NOERROR;
}

ECode CTethering::TetherInterfaceSM::InitialState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 what;
    message->GetWhat(&what);
    if (DBG) {
        Logger::D(CTethering::TAG, "InitialState.processMessage what=%d", what);
    }
    Boolean retValue = TRUE;
    switch (what) {
        case CMD_TETHER_REQUESTED:
            mHost->SetLastError(IConnectivityManager::TETHER_ERROR_NO_ERROR);
            mHost->mHost->mTetherMasterSM->SendMessage(
                TetherMasterSM::CMD_TETHER_MODE_REQUESTED, TO_IINTERFACE(mHost));
            mHost->TransitionTo(mHost->mStartingState);
            break;
        case CMD_INTERFACE_DOWN:
            mHost->TransitionTo(mHost->mUnavailableState);
            break;
        default:
            retValue = FALSE;
            break;
    }
    *value = retValue;
    return NOERROR;
}


//===========================================================================
//  CTethering::TetherInterfaceSM::StartingState
//===========================================================================
ECode CTethering::TetherInterfaceSM::StartingState::Enter()
{
    mHost->SetAvailable(FALSE);
    if (mHost->mUsb) {
        if (!mHost->mHost->ConfigureUsbIface(TRUE)) {
            mHost->mHost->mTetherMasterSM->SendMessage(
                TetherInterfaceSM::CMD_TETHER_UNREQUESTED, TO_IINTERFACE(mHost));
            mHost->SetLastError(IConnectivityManager::TETHER_ERROR_IFACE_CFG_ERROR);

            mHost->TransitionTo(mHost->mInitialState);
            return NOERROR;
        }
    }
    mHost->mHost->SendTetherStateChangedBroadcast();

    // Skipping StartingState
    mHost->TransitionTo(mHost->mTetheredState);
    return NOERROR;
}

ECode CTethering::TetherInterfaceSM::StartingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 what;
    message->GetWhat(&what);
    if (DBG) {
        Logger::D(CTethering::TAG, "StartingState.processMessage what=%d", what);
    }
    Boolean retValue = TRUE;
    switch (what) {
        // maybe a parent class?
        case CMD_TETHER_UNREQUESTED:
            mHost->mHost->mTetherMasterSM->SendMessage(
                TetherMasterSM::CMD_TETHER_MODE_UNREQUESTED, TO_IINTERFACE(mHost));
            if (mHost->mUsb) {
                if (!mHost->mHost->ConfigureUsbIface(FALSE)) {
                    mHost->SetLastErrorAndTransitionToInitialState(
                            IConnectivityManager::TETHER_ERROR_IFACE_CFG_ERROR);
                    break;
                }
            }
            mHost->TransitionTo(mHost->mInitialState);
            break;
        case CMD_CELL_DUN_ERROR:
        case CMD_IP_FORWARDING_ENABLE_ERROR:
        case CMD_IP_FORWARDING_DISABLE_ERROR:
        case CMD_START_TETHERING_ERROR:
        case CMD_STOP_TETHERING_ERROR:
        case CMD_SET_DNS_FORWARDERS_ERROR:
            mHost->SetLastErrorAndTransitionToInitialState(
                    IConnectivityManager::TETHER_ERROR_MASTER_ERROR);
            break;
        case CMD_INTERFACE_DOWN:
            mHost->mHost->mTetherMasterSM->SendMessage(
                TetherMasterSM::CMD_TETHER_MODE_UNREQUESTED, TO_IINTERFACE(mHost));
            mHost->TransitionTo(mHost->mUnavailableState);
            break;
        default:
            retValue = FALSE;
    }
    *value = retValue;
    return NOERROR;
}


//===========================================================================
//  CTethering::TetherInterfaceSM::TetheredState
//===========================================================================
ECode CTethering::TetherInterfaceSM::TetheredState::Enter()
{
    ECode ec = mHost->mHost->mNMService->TetherInterface(mHost->mIfaceName);
    if(FAILED(ec)){
        Logger::E(CTethering::TAG, "Error Tethering: 0x%x", ec);
        mHost->SetLastError(IConnectivityManager::TETHER_ERROR_TETHER_IFACE_ERROR);
        mHost->TransitionTo(mHost->mInitialState);
        return ec;
    }
    if (DBG) Logger::D(CTethering::TAG, "Tethered %s", mHost->mIfaceName.string());
    mHost->SetAvailable(FALSE);
    mHost->SetTethered(TRUE);
    mHost->mHost->SendTetherStateChangedBroadcast();
    return NOERROR;
}

void CTethering::TetherInterfaceSM::TetheredState::CleanupUpstream()
{
    ECode ec = NOERROR;
    if (!mHost->mMyUpstreamIfaceName.IsNull()) {
        // note that we don't care about errors here.
        // sometimes interfaces are gone before we get
        // to remove their rules, which generates errors.
        // just do the best we can.
        // about to tear down NAT; gather remaining statistics
        ec = mHost->mHost->mStatsService->ForceUpdate();
        if (FAILED(ec)) {
            if (VDBG) Logger::E(CTethering::TAG, "Exception in forceUpdate: %x", ec);
        }
        ec = mHost->mHost->mNMService->DisableNat(mHost->mIfaceName, mHost->mMyUpstreamIfaceName);
        if (FAILED(ec)) {
            if (VDBG) Logger::E(CTethering::TAG, "Exception in disableNat: %x", ec);
        }
        mHost->mMyUpstreamIfaceName = NULL;
    }
}

ECode CTethering::TetherInterfaceSM::TetheredState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* value)
{
    Int32 what;
    message->GetWhat(&what);
    if (DBG) {
        Logger::D(CTethering::TAG, "TetheredState.processMessage what=%d", what);
    }
    Boolean retValue = TRUE;
    Boolean error = FALSE;
    ECode ec = NOERROR;
    switch (what) {
        case CMD_TETHER_UNREQUESTED:
        case CMD_INTERFACE_DOWN:
            CleanupUpstream();
            ec = mHost->mHost->mNMService->UntetherInterface(mHost->mIfaceName);
            if (FAILED(ec)) {
                mHost->SetLastErrorAndTransitionToInitialState(
                        IConnectivityManager::TETHER_ERROR_UNTETHER_IFACE_ERROR);
                break;
            }
            mHost->mHost->mTetherMasterSM->SendMessage(
                TetherMasterSM::CMD_TETHER_MODE_UNREQUESTED, TO_IINTERFACE(mHost));
            if (what == CMD_TETHER_UNREQUESTED) {
                if (mHost->mUsb) {
                    if (!mHost->mHost->ConfigureUsbIface(FALSE)) {
                        mHost->SetLastError(
                                IConnectivityManager::TETHER_ERROR_IFACE_CFG_ERROR);
                    }
                }
                mHost->TransitionTo(mHost->mInitialState);
            }
            else if (what == CMD_INTERFACE_DOWN) {
                mHost->TransitionTo(mHost->mUnavailableState);
            }
            if (DBG) Logger::D(CTethering::TAG, "Untethered %s", mHost->mIfaceName.string());
            break;

        case CMD_TETHER_CONNECTION_CHANGED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            String newUpstreamIfaceName;
            if (obj != NULL) ICharSequence::Probe(obj)->ToString(&newUpstreamIfaceName);
            if ((mHost->mMyUpstreamIfaceName.IsNull() && newUpstreamIfaceName.IsNull()) ||
                    (!mHost->mMyUpstreamIfaceName.IsNull() &&
                    mHost->mMyUpstreamIfaceName.Equals(newUpstreamIfaceName))) {
                if (VDBG) Logger::D(CTethering::TAG, "Connection changed noop - dropping");
                break;
            }
            CleanupUpstream();
            if (!newUpstreamIfaceName.IsNull()) {
                ec = mHost->mHost->mNMService->EnableNat(mHost->mIfaceName, newUpstreamIfaceName);
                if (FAILED(ec)) {
                    Logger::E(CTethering::TAG, "Exception enabling Nat: %d", ec);
                    ec = mHost->mHost->mNMService->UntetherInterface(mHost->mIfaceName);
                    if (FAILED(ec)) {}

                    mHost->SetLastError(IConnectivityManager::TETHER_ERROR_ENABLE_NAT_ERROR);
                    mHost->TransitionTo(mHost->mInitialState);
                    *value = TRUE;
                    return ec;
                }
            }
            mHost->mMyUpstreamIfaceName = newUpstreamIfaceName;
            break;
        }
        case CMD_CELL_DUN_ERROR:
        case CMD_IP_FORWARDING_ENABLE_ERROR:
        case CMD_IP_FORWARDING_DISABLE_ERROR:
        case CMD_START_TETHERING_ERROR:
        case CMD_STOP_TETHERING_ERROR:
        case CMD_SET_DNS_FORWARDERS_ERROR:
            error = TRUE;
            // fall through
        case CMD_TETHER_MODE_DEAD:
            CleanupUpstream();
            ec = mHost->mHost->mNMService->UntetherInterface(mHost->mIfaceName);
            if (FAILED(ec)) {
                mHost->SetLastErrorAndTransitionToInitialState(
                        IConnectivityManager::TETHER_ERROR_UNTETHER_IFACE_ERROR);
                break;
            }
            if (error) {
                mHost->SetLastErrorAndTransitionToInitialState(
                        IConnectivityManager::TETHER_ERROR_MASTER_ERROR);
                break;
            }
            if (DBG) {
                Logger::D(CTethering::TAG, "Tether lost upstream connection %s", mHost->mIfaceName.string());
            }
            mHost->mHost->SendTetherStateChangedBroadcast();
            if (mHost->mUsb) {
                if (!mHost->mHost->ConfigureUsbIface(FALSE)) {
                    mHost->SetLastError(IConnectivityManager::TETHER_ERROR_IFACE_CFG_ERROR);
                }
            }
            mHost->TransitionTo(mHost->mInitialState);
            break;
        default:
            retValue = FALSE;
            break;
    }
    *value = retValue;
    return ec;
}


//===========================================================================
//  CTethering::TetherInterfaceSM::UnavailableState
//===========================================================================
ECode CTethering::TetherInterfaceSM::UnavailableState::Enter()
{
    mHost->SetAvailable(FALSE);
    mHost->SetLastError(IConnectivityManager::TETHER_ERROR_NO_ERROR);
    mHost->SetTethered(FALSE);
    mHost->mHost->SendTetherStateChangedBroadcast();
    return NOERROR;
}

ECode CTethering::TetherInterfaceSM::UnavailableState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* value)
{
    Boolean retValue = TRUE;
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_INTERFACE_UP:
            mHost->TransitionTo(mHost->mInitialState);
            break;
        default:
            retValue = FALSE;
            break;
    }
    *value = retValue;
    return NOERROR;
}


//===========================================================================
//  CTethering::TetherInterfaceSM
//===========================================================================
const Int32 CTethering::TetherInterfaceSM::CMD_TETHER_MODE_DEAD;
const Int32 CTethering::TetherInterfaceSM::CMD_TETHER_REQUESTED;
const Int32 CTethering::TetherInterfaceSM::CMD_TETHER_UNREQUESTED;
const Int32 CTethering::TetherInterfaceSM::CMD_INTERFACE_DOWN;
const Int32 CTethering::TetherInterfaceSM::CMD_INTERFACE_UP;
const Int32 CTethering::TetherInterfaceSM::CMD_CELL_DUN_ERROR;
const Int32 CTethering::TetherInterfaceSM::CMD_IP_FORWARDING_ENABLE_ERROR;
const Int32 CTethering::TetherInterfaceSM::CMD_IP_FORWARDING_DISABLE_ERROR;
const Int32 CTethering::TetherInterfaceSM::CMD_START_TETHERING_ERROR;
const Int32 CTethering::TetherInterfaceSM::CMD_STOP_TETHERING_ERROR;
const Int32 CTethering::TetherInterfaceSM::CMD_SET_DNS_FORWARDERS_ERROR;
const Int32 CTethering::TetherInterfaceSM::CMD_TETHER_CONNECTION_CHANGED;

CTethering::TetherInterfaceSM::TetherInterfaceSM(
    /* [in] */ const String& name,
    /* [in] */ ILooper* looper,
    /* [in] */ Boolean usb,
    /* [in] */ CTethering* host)
    : StateMachine(name, looper)
    , mLastError(0)
    , mIfaceName(name)
    , mUsb(usb)
    , mAvailable(FALSE)
    , mTethered(FALSE)
    , mHost(host)
{
    SetLastError(IConnectivityManager::TETHER_ERROR_NO_ERROR);

    mInitialState = new InitialState(this);
    AddState(mInitialState);
    mStartingState = new StartingState(this);
    AddState(mStartingState);
    mTetheredState = new TetheredState(this);
    AddState(mTetheredState);
    mUnavailableState = new UnavailableState(this);
    AddState(mUnavailableState);

    SetInitialState(mInitialState);
}

ECode CTethering::TetherInterfaceSM::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    sb += mIfaceName;
    sb += "-";
    AutoPtr<IState> current = GetCurrentState();
    if (current == mInitialState) sb += "InitialState";
    if (current == mStartingState) sb += "StartingState";
    if (current == mTetheredState) sb += "TetheredState";
    if (current == mUnavailableState) sb += "UnavailableState";
    if (mAvailable) sb += " - Available";
    if (mTethered) sb += " - Tethered";
    sb += " - lastError =";
    sb += mLastError;
    *str = sb.ToString();
    return NOERROR;
}

Int32 CTethering::TetherInterfaceSM::GetLastError()
{
    AutoLock lock(mHost->mPublicSync);
    return mLastError;
}

void CTethering::TetherInterfaceSM::SetLastError(
    /* [in] */ Int32 error)
{
    AutoLock lock(mHost->mPublicSync);

    mLastError = error;

    if (IsErrored()) {
        if (mUsb) {
            // note everything's been unwound by this point so nothing to do on
            // further error..
            mHost->ConfigureUsbIface(FALSE);
        }
    }
}

Boolean CTethering::TetherInterfaceSM::IsAvailable()
{
    AutoLock lock(mHost->mPublicSync);
    return mAvailable;
}

void CTethering::TetherInterfaceSM::SetAvailable(
    /* [in] */ Boolean available)
{
    AutoLock lock(mHost->mPublicSync);
    mAvailable = available;
}

Boolean CTethering::TetherInterfaceSM::IsTethered()
{
    AutoLock lock(mHost->mPublicSync);
    return mTethered;
}

void CTethering::TetherInterfaceSM::SetTethered(
    /* [in] */ Boolean tethered)
{
    AutoLock lock(mHost->mPublicSync);
    mTethered = tethered;
}

Boolean CTethering::TetherInterfaceSM::IsErrored()
{
    AutoLock lock(mHost->mPublicSync);
    return mLastError != IConnectivityManager::TETHER_ERROR_NO_ERROR;
}

void CTethering::TetherInterfaceSM::SetLastErrorAndTransitionToInitialState(
    /* [in] */ Int32 error)
{
    SetLastError(error);
    TransitionTo(mInitialState);
}


//===========================================================================
//  CTethering::TetherMasterSM::TetherMasterUtilState
//===========================================================================
const Boolean CTethering::TetherMasterSM::TetherMasterUtilState::TRY_TO_SETUP_MOBILE_CONNECTION = TRUE;
const Boolean CTethering::TetherMasterSM::TetherMasterUtilState::WAIT_FOR_NETWORK_TO_SETTLE = FALSE;

ECode CTethering::TetherMasterSM::TetherMasterUtilState::ProcessMessage(
    /* [in] */ IMessage* m,
    /* [out]*/ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

String CTethering::TetherMasterSM::TetherMasterUtilState::EnableString(
    /* [in] */ Int32 apnType)
{
    switch (apnType) {
    case IConnectivityManager::TYPE_MOBILE_DUN:
        // TODO:
        // return Phone::FEATURE_ENABLE_DUN_ALWAYS;
        return String("enableDUNAlways");
    case IConnectivityManager::TYPE_MOBILE:
    case IConnectivityManager::TYPE_MOBILE_HIPRI:
        // TOOD:
        // return Phone::FEATURE_ENABLE_HIPRI;
        return String("enableHIPRI");
    }
    return String(NULL);
}

Boolean CTethering::TetherMasterSM::TetherMasterUtilState::TurnOnUpstreamMobileConnection(
    /* [in] */Int32 apnType)
{
    Boolean retValue = TRUE;
    if (apnType == IConnectivityManager::TYPE_NONE) return FALSE;
    if (apnType != mHost->mMobileApnReserved) TurnOffUpstreamMobileConnection();

    String enableString = EnableString(apnType);
    if (enableString.IsNull()) return FALSE;

    Int32 result = IPhoneConstants::APN_REQUEST_FAILED;
    mHost->mHost->GetConnectivityManager()->StartUsingNetworkFeature(
        IConnectivityManager::TYPE_MOBILE, enableString, &result);

    switch (result) {
        case IPhoneConstants::APN_ALREADY_ACTIVE:
        case IPhoneConstants::APN_REQUEST_STARTED: {
            mHost->mMobileApnReserved = apnType;
            AutoPtr<IMessage> m;
            mHost->ObtainMessage(CMD_CELL_CONNECTION_RENEW, (IMessage**)&m);
            m->SetArg1(++mHost->mCurrentConnectionSequence);
            mHost->SendMessageDelayed(m, CELL_CONNECTION_RENEW_MS);
            break;
        }
        case IPhoneConstants::APN_REQUEST_FAILED:
        default:
            retValue = FALSE;
            break;
    }

    return retValue;
}

Boolean CTethering::TetherMasterSM::TetherMasterUtilState::TurnOffUpstreamMobileConnection()
{
    // ignore pending renewal requests
    ++mHost->mCurrentConnectionSequence;
    if (mHost->mMobileApnReserved != IConnectivityManager::TYPE_NONE) {
        Int32 result;
        ECode ec = mHost->mHost->GetConnectivityManager()->StopUsingNetworkFeature(
            IConnectivityManager::TYPE_MOBILE,
            EnableString(mHost->mMobileApnReserved), &result);
        if (FAILED(ec)) {
            return FALSE;
        }
        mHost->mMobileApnReserved = IConnectivityManager::TYPE_NONE;
    }
    return TRUE;
}

Boolean CTethering::TetherMasterSM::TetherMasterUtilState::TurnOnMasterTetherSettings()
{
    ECode ec = mHost->mHost->mNMService->SetIpForwardingEnabled(TRUE);
    if (FAILED(ec)) {
        mHost->TransitionTo(mHost->mSetIpForwardingEnabledErrorState);
        return FALSE;
    }
    ec = mHost->mHost->mNMService->StartTethering(mHost->mHost->mDhcpRange);
    if (FAILED(ec)) {
        if (FAILED(mHost->mHost->mNMService->StopTethering()) ||
            FAILED(mHost->mHost->mNMService->StartTethering(mHost->mHost->mDhcpRange))) {
            mHost->TransitionTo(mHost->mStartTetheringErrorState);
            return FALSE;
        }
    }

    return TRUE;
}

Boolean CTethering::TetherMasterSM::TetherMasterUtilState::TurnOffMasterTetherSettings()
{
    ECode ec = mHost->mHost->mNMService->StopTethering();
    if (FAILED(ec)) {
        mHost->TransitionTo(mHost->mStopTetheringErrorState);
        return FALSE;
    }
    ec = mHost->mHost->mNMService->SetIpForwardingEnabled(FALSE);
    if (FAILED(ec)) {
        mHost->TransitionTo(mHost->mSetIpForwardingDisabledErrorState);
        return FALSE;
    }
    mHost->TransitionTo(mHost->mInitialState);
    return TRUE;
}

void CTethering::TetherMasterSM::TetherMasterUtilState::ChooseUpstreamType(
    /* [in]*/ Boolean tryCell)
{
    ECode ec = NOERROR;
    Int32 upType = IConnectivityManager::TYPE_NONE;
    String iface;

    mHost->mHost->UpdateConfiguration(); // TODO - remove?
    IConnectivityManager* connMgr = mHost->mHost->GetConnectivityManager();

    {
        Object& lockOb = mHost->mHost->mPublicSync;
        AutoLock lock(lockOb);
        if (VDBG) {
            Logger::D(CTethering::TAG, "chooseUpstreamType has upstream iface types:");
            List< AutoPtr<IInteger32> >::Iterator it = mHost->mHost->mUpstreamIfaceTypes.Begin();
            for (; it != mHost->mHost->mUpstreamIfaceTypes.End(); ++it) {
                Int32 netType;
                (*it)->GetValue(&netType);
                Logger::D(CTethering::TAG, " %d", netType);
            }
        }

        Boolean connected;
        List< AutoPtr<IInteger32> >::Iterator it = mHost->mHost->mUpstreamIfaceTypes.Begin();
        for (; it != mHost->mHost->mUpstreamIfaceTypes.End(); ++it) {
            Int32 netType;
            (*it)->GetValue(&netType);
            AutoPtr<INetworkInfo> info;
            connMgr->GetNetworkInfo(netType, (INetworkInfo**)&info);
            if ((info != NULL) && (info->IsConnected(&connected), connected)) {
                upType = netType;
                break;
            }
        }
    }

    if (DBG) {
        Logger::D(CTethering::TAG, "chooseUpstreamType(%d), preferredApn =%d, got type=%d", tryCell,
                mHost->mHost->mPreferredUpstreamMobileApn, upType);
    }

    // if we're on DUN, put our own grab on it
    if (upType == IConnectivityManager::TYPE_MOBILE_DUN ||
            upType == IConnectivityManager::TYPE_MOBILE_HIPRI) {
        TurnOnUpstreamMobileConnection(upType);
    }
    else if (upType != IConnectivityManager::TYPE_NONE) {
        /* If we've found an active upstream connection that's not DUN/HIPRI
         * we should stop any outstanding DUN/HIPRI start requests.
         *
         * If we found NONE we don't want to do this as we want any previous
         * requests to keep trying to bring up something we can use.
         */
        TurnOffUpstreamMobileConnection();
    }

    if (upType == IConnectivityManager::TYPE_NONE) {
        Boolean tryAgainLater = TRUE;
        if ((tryCell == TRY_TO_SETUP_MOBILE_CONNECTION) &&
                TurnOnUpstreamMobileConnection(mHost->mHost->mPreferredUpstreamMobileApn)) {
            // we think mobile should be coming up - don't set a retry
            tryAgainLater = FALSE;
        }
        if (tryAgainLater) {
            mHost->SendMessageDelayed(CMD_RETRY_UPSTREAM, UPSTREAM_SETTLE_TIME_MS);
        }
    }
    else {
        AutoPtr<ILinkProperties> linkProperties;
        connMgr->GetLinkProperties(upType, (ILinkProperties**)&linkProperties);
        if (linkProperties != NULL) {
            // Find the interface with the default IPv4 route. It may be the
            // interface described by linkProperties, or one of the interfaces
            // stacked on top of it.
            Logger::I(TAG, "Finding IPv4 upstream interface on: %s",
                TO_CSTR(linkProperties));
            AutoPtr<IList> allRoutes;
            linkProperties->GetAllRoutes((IList**)&allRoutes);
            AutoPtr<IRouteInfoHelper> riHelper;
            CRouteInfoHelper::AcquireSingleton((IRouteInfoHelper**)&riHelper);
            AutoPtr<IInet4AddressHelper> i4aHelper;
            CInet4AddressHelper::AcquireSingleton((IInet4AddressHelper**)&i4aHelper);
            AutoPtr<IInetAddress> iaddr;
            i4aHelper->GetANY((IInetAddress**)&iaddr);
            AutoPtr<IRouteInfo> ipv4Default;
            riHelper->SelectBestRoute(ICollection::Probe(allRoutes), iaddr, (IRouteInfo**)&ipv4Default);
            if (ipv4Default != NULL) {
                ipv4Default->GetInterface(&iface);
                Logger::I(TAG, "Found interface %s", iface.string());
            }
            else {
                Logger::I(TAG, "No IPv4 upstream interface, giving up.");
            }
        }

        if (!iface.IsNull()) {
            AutoPtr<ArrayOf<String> > dnsServers = mHost->mHost->mDefaultDnsServers;
            AutoPtr<IList> dnses;
            linkProperties->GetDnsServers((IList**)&dnses);
            if (dnses != NULL) {
                // we currently only handle IPv4
                Int32 size;
                dnses->GetSize(&size);
                AutoPtr<IArrayList> v4Dnses;
                CArrayList::New(size, (IArrayList**)&v4Dnses);
                AutoPtr<IIterator> it;
                dnses->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> obj;
                    it->GetNext((IInterface**)&obj);
                    IInet4Address* dnsAddress = IInet4Address::Probe(obj);
                    if (dnsAddress) {
                        v4Dnses->Add(dnsAddress);
                    }
                }

                v4Dnses->GetSize(&size);
                if (size > 0) {
                    dnsServers = NULL;
                    NetworkUtils::MakeStrings(
                        ICollection::Probe(v4Dnses), (ArrayOf<String>**)&dnsServers);
                }
            }

            AutoPtr<INetwork> network;
            ec = connMgr->GetNetworkForType(upType, (INetwork**)&network);
            FAIL_GOTO(ec, _EXIT_)

            if (network == NULL) {
                Logger::E(TAG, "No Network for upstream type %d!", upType);
            }
            if (VDBG) {
                Logger::D(TAG, "Setting DNS forwarders: Network=%s, dnsServers=%s",
                    TO_CSTR(network), Arrays::ToString(dnsServers).string());
            }
            mHost->mHost->mNMService->SetDnsForwarders(network, dnsServers);

        _EXIT_:
            if (FAILED(ec)) {
                Logger::E(TAG, "Setting DNS forwarders failed!");
                mHost->TransitionTo(mHost->mSetDnsForwardersErrorState);
            }
        }
    }

    NotifyTetheredOfNewUpstreamIface(iface);
}

void CTethering::TetherMasterSM::TetherMasterUtilState::NotifyTetheredOfNewUpstreamIface(
    /* [in]*/ const String& ifaceName)
{
    if (DBG) Logger::D(CTethering::TAG, "notifying tethered with iface =%s", ifaceName.string());
    mHost->mUpstreamIfaceName = ifaceName;
    List< AutoPtr<TetherInterfaceSM> >::Iterator it;
    for (it = mHost->mNotifyList.Begin(); it != mHost->mNotifyList.End(); ++it) {
        AutoPtr<TetherInterfaceSM> sm = *it;
        AutoPtr<ICharSequence> cs;
        CString::New(ifaceName, (ICharSequence**)&cs);
        sm->SendMessage(TetherInterfaceSM::CMD_TETHER_CONNECTION_CHANGED, cs);
    }
}


//===========================================================================
//  CTethering::TetherMasterSM::InitialState
//===========================================================================
ECode CTethering::TetherMasterSM::InitialState::Enter()
{
    return NOERROR;
}

ECode CTethering::TetherMasterSM::InitialState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 what;
    message->GetWhat(&what);
    if (DBG) Logger::D(CTethering::TAG, "MasterInitialState.processMessage what=%d", what);
    Boolean retValue = TRUE;
    switch (what) {
        case CMD_TETHER_MODE_REQUESTED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            TetherInterfaceSM* who = (TetherInterfaceSM*)IObject::Probe(obj);
            if (VDBG) Logger::D(CTethering::TAG, "Tether Mode requested by %p", who);
            mHost->mNotifyList.PushBack(who);
            mHost->TransitionTo(mHost->mTetherModeAliveState);
            break;
        }
        case CMD_TETHER_MODE_UNREQUESTED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            TetherInterfaceSM* who = (TetherInterfaceSM*)IObject::Probe(obj);
            if (VDBG) Logger::D(CTethering::TAG, "Tether Mode unrequested by %p", who);
            mHost->mNotifyList.Remove(who);
            break;
        }
        default:
            retValue = FALSE;
            break;
    }
    *value = retValue;
    return NOERROR;
}


//===========================================================================
//  CTethering::TetherMasterSM::TetherModeAliveState
//===========================================================================
ECode CTethering::TetherMasterSM::TetherModeAliveState::Enter()
{
    TurnOnMasterTetherSettings(); // may transition us out

    mTryCell = !WAIT_FOR_NETWORK_TO_SETTLE; // better try something first pass
                                            // or crazy tests cases will fail
    ChooseUpstreamType(mTryCell);
    mTryCell = !mTryCell;
    return NOERROR;
}

ECode CTethering::TetherMasterSM::TetherModeAliveState::Exit()
{
    TurnOffUpstreamMobileConnection();
    NotifyTetheredOfNewUpstreamIface(String(NULL));
    return NOERROR;
}

ECode CTethering::TetherMasterSM::TetherModeAliveState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 what;
    message->GetWhat(&what);
    if (DBG) Logger::D(CTethering::TAG, "TetherModeAliveState.processMessage what=%d", what);
    Boolean retValue = TRUE;
    switch (what) {
        case CMD_TETHER_MODE_REQUESTED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            TetherInterfaceSM* who = (TetherInterfaceSM*)IObject::Probe(obj);
            if (VDBG) Logger::D(CTethering::TAG, "Tether Mode requested by %s", TO_CSTR(who));
            mHost->mNotifyList.PushBack(who);
            AutoPtr<ICharSequence> cs;
            CString::New(mHost->mUpstreamIfaceName, (ICharSequence**)&cs);
            who->SendMessage(TetherInterfaceSM::CMD_TETHER_CONNECTION_CHANGED, cs);
            break;
        }
        case CMD_TETHER_MODE_UNREQUESTED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<TetherInterfaceSM> who = (TetherInterfaceSM*)IObject::Probe(obj);
            if (VDBG) Logger::D(CTethering::TAG, "Tether Mode unrequested by %s", TO_CSTR(who));
            List< AutoPtr<TetherInterfaceSM> >::Iterator it = Find(
                    mHost->mNotifyList.Begin(), mHost->mNotifyList.End(), who);
            if (it != mHost->mNotifyList.End()) {
                if (DBG) Logger::D(CTethering::TAG, "TetherModeAlive removing notifyee %s", TO_CSTR(who));
                mHost->mNotifyList.Erase(it);
                if (mHost->mNotifyList.IsEmpty()) {
                    TurnOffMasterTetherSettings(); // transitions appropriately
                }
                else {
                    if (DBG) {
                        Logger::D(CTethering::TAG, "TetherModeAlive still has %d live requests:",
                                mHost->mNotifyList.GetSize());
                        // for (Object o : mNotifyList) Log.d(TAG, "  " + o);
                    }
                }
            }
            else {
                Logger::E(CTethering::TAG, "TetherModeAliveState UNREQUESTED has unknown who: %s", TO_CSTR(who));
            }
            break;
        }
        case CMD_UPSTREAM_CHANGED:
            // need to try DUN immediately if Wifi goes down
            mTryCell = !WAIT_FOR_NETWORK_TO_SETTLE;
            ChooseUpstreamType(mTryCell);
            mTryCell = !mTryCell;
            break;
        case CMD_CELL_CONNECTION_RENEW: {
            // make sure we're still using a requested connection - may have found
            // wifi or something since then.
            Int32 arg1;
            message->GetArg1(&arg1);
            if (mHost->mCurrentConnectionSequence == arg1) {
                if (VDBG) {
                    Logger::D(CTethering::TAG, "renewing mobile connection - requeuing for another %sms",
                            CELL_CONNECTION_RENEW_MS);
                }
                TurnOnUpstreamMobileConnection(mHost->mMobileApnReserved);
            }
            break;
        }
        case CMD_RETRY_UPSTREAM:
            ChooseUpstreamType(mTryCell);
            mTryCell = !mTryCell;
            break;
        default:
            retValue = FALSE;
            break;
    }
    *value = retValue;
    return NOERROR;
}


//===========================================================================
//  CTethering::TetherMasterSM::ErrorState
//===========================================================================
ECode CTethering::TetherMasterSM::ErrorState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    Boolean retValue = TRUE;
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case CMD_TETHER_MODE_REQUESTED: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            TetherInterfaceSM* who = (TetherInterfaceSM*)IObject::Probe(obj);
            who->SendMessage(mErrorNotification);
            break;
        }
        default:
           retValue = FALSE;
    }
    *value = retValue;
    return NOERROR;
}

void CTethering::TetherMasterSM::ErrorState::Notify(
    /* [in] */Int32 msgType)
{
    mErrorNotification = msgType;
    List< AutoPtr<TetherInterfaceSM> >::Iterator it;
    for (it = mHost->mNotifyList.Begin(); it != mHost->mNotifyList.End(); ++it) {
        AutoPtr<TetherInterfaceSM> sm = *it;
        sm->SendMessage(msgType);
    }
}

//===========================================================================
//  CTethering::TetherMasterSM::SetIpForwardingEnabledErrorState
//===========================================================================
ECode CTethering::TetherMasterSM::SetIpForwardingEnabledErrorState::Enter()
{
    Logger::E(CTethering::TAG, "Error in setIpForwardingEnabled");
    Notify(TetherInterfaceSM::CMD_IP_FORWARDING_ENABLE_ERROR);
    return NOERROR;
}

//===========================================================================
//  CTethering::TetherMasterSM::SetIpForwardingDisabledErrorState
//===========================================================================
ECode CTethering::TetherMasterSM::SetIpForwardingDisabledErrorState::Enter()
{
    Logger::E(CTethering::TAG, "Error in setIpForwardingDisabled");
    Notify(TetherInterfaceSM::CMD_IP_FORWARDING_DISABLE_ERROR);
    return NOERROR;
}

//===========================================================================
//  CTethering::TetherMasterSM::StartTetheringErrorState
//===========================================================================
ECode CTethering::TetherMasterSM::StartTetheringErrorState::Enter()
{
    Logger::E(CTethering::TAG, "Error in startTethering");
    Notify(TetherInterfaceSM::CMD_START_TETHERING_ERROR);
    ECode ec = mHost->mHost->mNMService->SetIpForwardingEnabled(FALSE);
    if (FAILED(ec)) {}
    return NOERROR;
}

//===========================================================================
//  CTethering::TetherMasterSM::StopTetheringErrorState
//===========================================================================
ECode CTethering::TetherMasterSM::StopTetheringErrorState::Enter()
{
    Logger::E(CTethering::TAG, "Error in stopTethering");
    Notify(TetherInterfaceSM::CMD_STOP_TETHERING_ERROR);
    ECode ec = mHost->mHost->mNMService->SetIpForwardingEnabled(FALSE);
    if (FAILED(ec)) {}
    return NOERROR;
}

//===========================================================================
//  CTethering::TetherMasterSM::SetDnsForwardersErrorState
//===========================================================================
ECode CTethering::TetherMasterSM::SetDnsForwardersErrorState::Enter()
{
    Logger::E(CTethering::TAG, "Error in setDnsForwarders");
    Notify(TetherInterfaceSM::CMD_SET_DNS_FORWARDERS_ERROR);
    ECode ec = mHost->mHost->mNMService->StopTethering();
    if (FAILED(ec)) {}
    ec = mHost->mHost->mNMService->SetIpForwardingEnabled(FALSE);
    if (FAILED(ec)) {}
    return NOERROR;
}

//===========================================================================
//  CTethering::TetherMasterSM
//===========================================================================
const Int32 CTethering::TetherMasterSM::CMD_TETHER_MODE_REQUESTED;
const Int32 CTethering::TetherMasterSM::CMD_TETHER_MODE_UNREQUESTED;
const Int32 CTethering::TetherMasterSM::CMD_UPSTREAM_CHANGED;
const Int32 CTethering::TetherMasterSM::CMD_CELL_CONNECTION_RENEW;
const Int32 CTethering::TetherMasterSM::CMD_RETRY_UPSTREAM;
const Int32 CTethering::TetherMasterSM::UPSTREAM_SETTLE_TIME_MS;
const Int32 CTethering::TetherMasterSM::CELL_CONNECTION_RENEW_MS;

CTethering::TetherMasterSM::TetherMasterSM(
    /* [in] */ const String& name,
    /* [in] */ ILooper* looper,
    /* [in] */ CTethering* host)
    : StateMachine(name, looper)
    , mSequenceNumber(0)
    , mCurrentConnectionSequence(0)
    , mMobileApnReserved(0)
    , mHost(host)
{
    //Add states
    mInitialState = new InitialState(this);
    AddState(mInitialState);
    mTetherModeAliveState = new TetherModeAliveState(this);
    AddState(mTetherModeAliveState);

    mSetIpForwardingEnabledErrorState = new SetIpForwardingEnabledErrorState(this);
    AddState(mSetIpForwardingEnabledErrorState);
    mSetIpForwardingDisabledErrorState = new SetIpForwardingDisabledErrorState(this);
    AddState(mSetIpForwardingDisabledErrorState);
    mStartTetheringErrorState = new StartTetheringErrorState(this);
    AddState(mStartTetheringErrorState);
    mStopTetheringErrorState = new StopTetheringErrorState(this);
    AddState(mStopTetheringErrorState);
    mSetDnsForwardersErrorState = new SetDnsForwardersErrorState(this);
    AddState(mSetDnsForwardersErrorState);

    SetInitialState(mInitialState);
}

CTethering::TetherMasterSM::~TetherMasterSM()
{
    mNotifyList.Clear();
}

//===========================================================================
//  CTethering
//===========================================================================
static AutoPtr<IInteger32> InitType(
    /* [in] */ Int32 type)
{
    AutoPtr<IInteger32> itype;
    CInteger32::New(type, (IInteger32**)&itype);
    return itype;
}

static AutoPtr< ArrayOf<String> > InitDhcpDefaultRange()
{
    AutoPtr< ArrayOf<String> > range = ArrayOf<String>::Alloc(16);
    (*range)[0] = "192.168.42.2";
    (*range)[1] = "192.168.42.254";
    (*range)[2] = "192.168.43.2";
    (*range)[3] = "192.168.43.254";
    (*range)[4] = "192.168.44.2";
    (*range)[5] = "192.168.44.254";
    (*range)[6] = "192.168.45.2";
    (*range)[7] = "192.168.45.254";
    (*range)[8] = "192.168.46.2";
    (*range)[9] = "192.168.46.254";
    (*range)[10] = "192.168.47.2";
    (*range)[11] = "192.168.47.254";
    (*range)[12] = "192.168.48.2";
    (*range)[13] = "192.168.48.254";
    (*range)[14] = "192.168.49.2";
    (*range)[15] = "192.168.49.254";
    return range;
}

const String CTethering::TAG("Tethering");
const Boolean CTethering::DBG = FALSE;
const Boolean CTethering::VDBG = FALSE;

const AutoPtr<IInteger32> CTethering::MOBILE_TYPE = InitType(IConnectivityManager::TYPE_MOBILE);
const AutoPtr<IInteger32> CTethering::HIPRI_TYPE = InitType(IConnectivityManager::TYPE_MOBILE_HIPRI);
const AutoPtr<IInteger32> CTethering::DUN_TYPE = InitType(IConnectivityManager::TYPE_MOBILE_DUN);

const String CTethering::USB_NEAR_IFACE_ADDR("192.168.42.129");
const Int32 CTethering::USB_PREFIX_LENGTH = 24;

AutoPtr< ArrayOf<String> > CTethering::DHCP_DEFAULT_RANGE = InitDhcpDefaultRange();

const String CTethering::DNS_DEFAULT_SERVER1("8.8.8.8");
const String CTethering::DNS_DEFAULT_SERVER2("8.8.4.4");

CAR_OBJECT_IMPL(CTethering)

CTethering::CTethering()
    : mWifiTethered(FALSE)
    , mPreferredUpstreamMobileApn(0)
    , mIfaces(20)
    , mRndisEnabled(FALSE)
    , mUsbTetherRequested(FALSE)
{}

CTethering::~CTethering()
{
    mUpstreamIfaceTypes.Clear();
    mIfaces.Clear();
}

ECode CTethering::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IINetworkManagementService* nmService,
    /* [in] */ IINetworkStatsService* statsService,
    /* [in] */ ILooper* looper)
{
    mContext = context;
    mNMService = nmService;
    mStatsService = statsService;
    mLooper = looper;

    // make our own thread so we don't anr the system
    AutoPtr<IoThread> thread = IoThread::Get();
    thread->GetLooper((ILooper**)&mLooper);
    mTetherMasterSM = new TetherMasterSM(String("TetherMaster"), mLooper, this);
    mTetherMasterSM->Start();

    mStateReceiver = new StateReceiver(this);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IUsbManager::ACTION_USB_STATE);
    filter->AddAction(IConnectivityManager::CONNECTIVITY_ACTION);
    AutoPtr<IIntent> result;
    mContext->RegisterReceiver(mStateReceiver, filter, (IIntent**)&result);

    filter = NULL;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_MEDIA_SHARED);
    filter->AddAction(IIntent::ACTION_MEDIA_UNSHARED);
    filter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);
    filter->AddDataScheme(String("file"));
    result = NULL;
    mContext->RegisterReceiver(mStateReceiver, filter, (IIntent**)&result);

    AutoPtr<IResources> resource;
    context->GetResources((IResources**)&resource);
    resource->GetStringArray(
            R::array::config_tether_dhcp_range, (ArrayOf<String>**)&mDhcpRange);
    if ((mDhcpRange->GetLength() == 0) || (mDhcpRange->GetLength() % 2 == 1)) {
        mDhcpRange = DHCP_DEFAULT_RANGE;
    }

    // load device config info
    UpdateConfiguration();

    // TODO - remove and rely on real notifications of the current iface
    mDefaultDnsServers = ArrayOf<String>::Alloc(2);
    (*mDefaultDnsServers)[0] = DNS_DEFAULT_SERVER1;
    (*mDefaultDnsServers)[1] = DNS_DEFAULT_SERVER2;
    return NOERROR;
}

AutoPtr<IConnectivityManager> CTethering::GetConnectivityManager()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    return IConnectivityManager::Probe(obj);
}

void CTethering::UpdateConfiguration()
{
    AutoPtr<IResources> resource;
    mContext->GetResources((IResources**)&resource);
    AutoPtr< ArrayOf<String> > tetherableUsbRegexs;
    resource->GetStringArray(
            R::array::config_tether_usb_regexs, (ArrayOf<String>**)&tetherableUsbRegexs);
    AutoPtr< ArrayOf<String> > tetherableWifiRegexs;
    resource->GetStringArray(
            R::array::config_tether_wifi_regexs, (ArrayOf<String>**)&tetherableWifiRegexs);
    AutoPtr< ArrayOf<String> > tetherableBluetoothRegexs;
    resource->GetStringArray(
            R::array::config_tether_bluetooth_regexs, (ArrayOf<String>**)&tetherableBluetoothRegexs);

    AutoPtr< ArrayOf<Int32> > ifaceTypes;
    resource->GetInt32Array(
            R::array::config_tether_upstream_types, (ArrayOf<Int32>**)&ifaceTypes);
    for (Int32 i = 0; i != ifaceTypes->GetLength(); i++) {
        AutoPtr<IInteger32> type;
        CInteger32::New((*ifaceTypes)[i], (IInteger32**)&type);
        mUpstreamIfaceTypes.PushBack(type);
    }

    {
        AutoLock lock(mPublicSync);
        mTetherableUsbRegexs = tetherableUsbRegexs;
        mTetherableWifiRegexs = tetherableWifiRegexs;
        mTetherableBluetoothRegexs = tetherableBluetoothRegexs;
    }

    // check if the upstream type list needs to be modified due to secure-settings
    CheckDunRequired();
}

ECode CTethering::InterfaceStatusChanged(
    /* [in] */const String& iface,
    /* [in] */ Boolean up)
{
    if (VDBG) Logger::D(TAG, "interfaceStatusChanged %s, %d", iface.string(), up);
    Boolean found = FALSE;
    Boolean usb = FALSE;
    {
        AutoLock lock(mPublicSync);
        if (IsWifi(iface)) {
            found = TRUE;
        }
        else if (IsUsb(iface)) {
            found = TRUE;
            usb = TRUE;
        }
        else if (IsBluetooth(iface)) {
            found = TRUE;
        }
        if (found == FALSE) return NOERROR;

        AutoPtr<TetherInterfaceSM> sm;
        HashMap<String, AutoPtr<TetherInterfaceSM> >::Iterator it = mIfaces.Find(iface);
        if (it != mIfaces.End()) {
            sm = it->mSecond;
        }
        if (up) {
            if (sm == NULL) {
                sm = new TetherInterfaceSM(iface, mLooper, usb, this);
                mIfaces[iface] = sm;
                sm->Start();
            }
        }
        else {
            if (IsUsb(iface)) {
                // ignore usb0 down after enabling RNDIS
                // we will handle disconnect in interfaceRemoved instead
                if (VDBG) Logger::D(TAG, "ignore interface down for %s", iface.string());
            }
            else if (sm != NULL) {
                sm->SendMessage(TetherInterfaceSM::CMD_INTERFACE_DOWN);
                mIfaces.Erase(it);
            }
        }
    }
    return NOERROR;
}

ECode CTethering::InterfaceLinkStateChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    if (VDBG) Logger::D(TAG, "interfaceLinkStateChanged %s, %d", iface.string(), up);
    return InterfaceStatusChanged(iface, up);
}

Boolean CTethering::IsUsb(
    /* [in] */ const String& iface)
{
    AutoLock lock(mPublicSync);

    for (Int32 i = 0; i < mTetherableUsbRegexs->GetLength(); i++){
        String regex = (*mTetherableUsbRegexs)[i];
        AutoPtr<IPatternHelper> helper;
        CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
        Boolean match = FALSE;
        helper->Matches(regex, iface, &match);
        if(match) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean CTethering::IsWifi(
    /* [in] */ const String& iface)
{
    AutoLock lock(mPublicSync);

    for (Int32 i = 0; i < mTetherableWifiRegexs->GetLength(); i++){
        String regex = (*mTetherableWifiRegexs)[i];
        AutoPtr<IPatternHelper> helper;
        CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
        Boolean match = FALSE;
        helper->Matches(regex, iface, &match);
        if(match) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean CTethering::IsBluetooth(
    /* [in] */ const String& iface)
{
    AutoLock lock(mPublicSync);

    for (Int32 i = 0; i < mTetherableBluetoothRegexs->GetLength(); i++){
        String regex = (*mTetherableBluetoothRegexs)[i];
        AutoPtr<IPatternHelper> helper;
        CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
        Boolean match = FALSE;
        helper->Matches(regex, iface, &match);
        if(match) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode CTethering::InterfaceAdded(
    /* [in] */ const String& iface)
{
    if (VDBG) Logger::D(TAG, "interfaceAdded %s", iface.string());
    Boolean found = FALSE;
    Boolean usb = FALSE;
    {
        AutoLock lock(mPublicSync);

        if (IsWifi(iface)) {
            found = TRUE;
        }
        if (IsUsb(iface)) {
            found = TRUE;
            usb = TRUE;
        }
        if (IsBluetooth(iface)) {
            found = TRUE;
        }

        if (found == FALSE) {
            if (VDBG) Logger::D(TAG, "%s is not a tetherable iface, ignoring", iface.string());
            return NOERROR;
        }

        AutoPtr<TetherInterfaceSM> sm;
        HashMap<String, AutoPtr<TetherInterfaceSM> >::Iterator it = mIfaces.Find(iface);
        if (it != mIfaces.End()) {
            sm = it->mSecond;
        }
        if (sm != NULL) {
            if (VDBG) Logger::D(TAG, "active iface (%s) reported as added, ignoring", iface.string());
            return NOERROR;
        }
        sm = new TetherInterfaceSM(iface, mLooper, usb, this);
        mIfaces[iface] = sm;
        sm->Start();
    }
    return NOERROR;
}

ECode CTethering::InterfaceRemoved(
    /* [in] */ const String& iface)
{
    if (VDBG) Logger::D(TAG, "interfaceRemoved %s", iface.string());
    {
        AutoLock lock(mPublicSync);

        AutoPtr<TetherInterfaceSM> sm;
        HashMap<String, AutoPtr<TetherInterfaceSM> >::Iterator it = mIfaces.Find(iface);
        if (it != mIfaces.End()) {
            sm = it->mSecond;
        }
        if (sm == NULL) {
            if (VDBG) {
                Logger::E(TAG, "attempting to remove unknown iface (%s), ignoring", iface.string());
            }
            return NOERROR;
        }
        sm->SendMessage(TetherInterfaceSM::CMD_INTERFACE_DOWN);
        mIfaces.Erase(it);
    }
    return NOERROR;
}

ECode CTethering::Tether(
    /* [in] */ const String& iface,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    if (DBG) Logger::D(TAG, "Tethering %s", iface.string());
    AutoPtr<TetherInterfaceSM> sm;
    {
        AutoLock lock(mPublicSync);
        HashMap<String, AutoPtr<TetherInterfaceSM> >::Iterator it = mIfaces.Find(iface);
        if (it != mIfaces.End()) {
            sm = it->mSecond;
        }
    }
    if (sm == NULL) {
        Logger::E(TAG, "Tried to Tether an unknown iface :%s, ignoring", iface.string());
        *value = IConnectivityManager::TETHER_ERROR_UNKNOWN_IFACE;
        return NOERROR;
    }
    if (!sm->IsAvailable() && !sm->IsErrored()) {
        Logger::E(TAG, "Tried to Tether an unavailable iface :%s, ignoring", iface.string());
        *value = IConnectivityManager::TETHER_ERROR_UNAVAIL_IFACE;
        return NOERROR;
    }
    sm->SendMessage(TetherInterfaceSM::CMD_TETHER_REQUESTED);
    *value = IConnectivityManager::TETHER_ERROR_NO_ERROR;
    return NOERROR;
}

ECode CTethering::Untether(
    /* [in] */ const String& iface,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    if (DBG) Logger::D(TAG, "Untethering %s", iface.string());
    AutoPtr<TetherInterfaceSM> sm;
    {
        AutoLock lock(mPublicSync);
        HashMap<String, AutoPtr<TetherInterfaceSM> >::Iterator it = mIfaces.Find(iface);
        if (it != mIfaces.End()) {
            sm = it->mSecond;
        }
    }
    if (sm == NULL) {
        Logger::E(TAG, "Tried to Untether an unknown iface :%s, ignoring", iface.string());
        *value = IConnectivityManager::TETHER_ERROR_UNKNOWN_IFACE;
        return NOERROR;
    }
    if (sm->IsErrored()) {
        Logger::E(TAG, "Tried to Untethered an errored iface :%s, ignoring", iface.string());
        *value = IConnectivityManager::TETHER_ERROR_UNAVAIL_IFACE;
        return NOERROR;
    }
    sm->SendMessage(TetherInterfaceSM::CMD_TETHER_REQUESTED);
    *value = IConnectivityManager::TETHER_ERROR_NO_ERROR;
    return NOERROR;

}

ECode CTethering::GetLastTetherError(
    /* [in] */ const String& iface,
    /* [in] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<TetherInterfaceSM> sm;
    {
        AutoLock lock(mPublicSync);
        HashMap<String, AutoPtr<TetherInterfaceSM> >::Iterator it = mIfaces.Find(iface);
        if (it != mIfaces.End()) {
            sm = it->mSecond;
        }
        if(sm == NULL){
            Logger::E(TAG, "Tried to getLastTetherError on an unknown iface :%s", iface.string());
            *value = IConnectivityManager::TETHER_ERROR_UNKNOWN_IFACE;
            return NOERROR;
        }
        *value = sm->GetLastError();
        return NOERROR;
    }
}

// TODO - move all private methods used only by the state machine into the state machine
// to clarify what needs synchronized protection.
void CTethering::SendTetherStateChangedBroadcast()
{
    AutoPtr<IConnectivityManager> connMgr = GetConnectivityManager();
    Boolean result = FALSE;
    ECode ec = connMgr->IsTetheringSupported(&result);
    if (FAILED(ec) || !result) return;

    AutoPtr<IArrayList> availableList, activeList, erroredList;
    CArrayList::New((IArrayList**)&availableList);
    CArrayList::New((IArrayList**)&activeList);
    CArrayList::New((IArrayList**)&erroredList);

    Boolean wifiTethered = FALSE;
    Boolean usbTethered = FALSE;
    Boolean bluetoothTethered = FALSE;

    {
        AutoLock lock(mPublicSync);

        HashMap<String, AutoPtr<TetherInterfaceSM> >::Iterator it;
        for (it = mIfaces.Begin(); it != mIfaces.End(); ++it) {
            String iface = it->mFirst;
            AutoPtr<TetherInterfaceSM> sm = it->mSecond;
            if (sm != NULL) {
                if (sm->IsErrored()) {
                    AutoPtr<ICharSequence> cs = CoreUtils::Convert(iface);
                    erroredList->Add(cs.Get());
                }
                else if (sm->IsAvailable()) {
                    AutoPtr<ICharSequence> cs = CoreUtils::Convert(iface);
                    availableList->Add(cs.Get());
                }
                else if (sm->IsTethered()) {
                    if (IsUsb(iface)) {
                        usbTethered = TRUE;
                    }
                    else if (IsWifi(iface)) {
                        wifiTethered = TRUE;
                    }
                    else if (IsBluetooth(iface)) {
                        bluetoothTethered = TRUE;
                    }
                    AutoPtr<ICharSequence> cs = CoreUtils::Convert(iface);
                    activeList->Add(cs.Get());
                }
            }
        }
    }
    AutoPtr<IIntent> broadcast;
    CIntent::New(IConnectivityManager::ACTION_TETHER_STATE_CHANGED, (IIntent**)&broadcast);
    broadcast->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING |
            IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    broadcast->PutStringArrayListExtra(IConnectivityManager::EXTRA_AVAILABLE_TETHER,
            availableList);
    broadcast->PutStringArrayListExtra(IConnectivityManager::EXTRA_ACTIVE_TETHER, activeList);
    broadcast->PutStringArrayListExtra(IConnectivityManager::EXTRA_ERRORED_TETHER,
            erroredList);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);
    mContext->SendStickyBroadcastAsUser(broadcast, ALL);
    /*if (DBG) {
        Log.d(TAG, "sendTetherStateChangedBroadcast " + availableList.size() + ", " +
                activeList.size() + ", " + erroredList.size());
    }*/

    if (usbTethered) {
        if (wifiTethered || bluetoothTethered) {
            ShowTetheredNotification(R::drawable::stat_sys_tether_general);
        }
        else {
            ShowTetheredNotification(R::drawable::stat_sys_tether_usb);
        }
    }
    else if (wifiTethered) {
        if (bluetoothTethered) {
            ShowTetheredNotification(R::drawable::stat_sys_tether_general);
        }
        else {
            ShowTetheredNotification(R::drawable::stat_sys_tether_wifi);
        }
    }
    else if (bluetoothTethered) {
        ShowTetheredNotification(R::drawable::stat_sys_tether_bluetooth);
    }
    else {
        ClearTetheredNotification();
    }
}

void CTethering::ShowTetheredNotification(
    /* [in] */ Int32 icon)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    AutoPtr<INotificationManager> notificationManager = INotificationManager::Probe(obj);
    if (notificationManager == NULL){
        return;
    }

    AutoPtr<IUserHandleHelper> uhhelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhhelper);
    AutoPtr<IUserHandle> ALL;
    uhhelper->GetALL((IUserHandle**)&ALL);
    if (mTetheredNotification != NULL) {
        Int32 _icon;
        mTetheredNotification->GetIcon(&_icon);
        if (_icon == icon) {
            return;
        }
        notificationManager->CancelAsUser(String(NULL), _icon, ALL);
    }

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent **)&intent);
    intent->SetClassName(String("com.android.settings"), String("com.android.settings.TetherSettings"));
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NO_HISTORY);

    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pi;
    AutoPtr<IUserHandle> CURRENT;
    uhhelper->GetCURRENT((IUserHandle**)&CURRENT);
    helper->GetActivityAsUser(mContext, 0, intent, 0, NULL, CURRENT, (IPendingIntent**)&pi);

    AutoPtr<IResourcesHelper> resHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHelper);
    AutoPtr<IResources> r;
    resHelper->GetSystem((IResources**)&r);

    AutoPtr<ICharSequence> title;
    r->GetText(R::string::tethered_notification_title, (ICharSequence**)&title);
    AutoPtr<ICharSequence> message;
    r->GetText(R::string::tethered_notification_message, (ICharSequence**)&message);

    if (mTetheredNotification == NULL) {
        CNotification::New((INotification**)&mTetheredNotification);
        mTetheredNotification->SetWhen(0);
    }
    mTetheredNotification->SetIcon(icon);
    Int32 defaultValue;
    mTetheredNotification->GetDefaults(&defaultValue);
    mTetheredNotification->SetDefaults(defaultValue & ~INotification::DEFAULT_SOUND);
    mTetheredNotification->SetFlags(INotification::FLAG_ONGOING_EVENT);
    mTetheredNotification->SetTickerText(title);
    mTetheredNotification->SetVisibility(INotification::VISIBILITY_PUBLIC);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(R::color::system_notification_accent_color, &color);
    mTetheredNotification->SetColor(color);
    mTetheredNotification->SetLatestEventInfo(mContext, title, message, pi);
    mTetheredNotification->SetCategory(INotification::CATEGORY_STATUS);

    notificationManager->NotifyAsUser(String(NULL), icon, mTetheredNotification, ALL);
}

void CTethering::ClearTetheredNotification()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&obj);
    AutoPtr<INotificationManager> notificationManager = INotificationManager::Probe(obj);
    if (notificationManager != NULL && mTetheredNotification != NULL) {
        Int32 icon;
        mTetheredNotification->GetIcon(&icon);
        AutoPtr<IUserHandleHelper> uhhelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhhelper);
        AutoPtr<IUserHandle> ALL;
        uhhelper->GetALL((IUserHandle**)&ALL);
        notificationManager->CancelAsUser(String(NULL), icon, ALL);
        mTetheredNotification = NULL;
   }
}

void CTethering::TetherUsb(
    /* [in] */ Boolean enable)
{
    if (VDBG) Logger::D(TAG, "tetherUsb %d", enable);

    AutoPtr< ArrayOf<String> > ifaces;
    ECode ec = mNMService->ListInterfaces((ArrayOf<String> **)&ifaces);
    if (FAILED(ec)) {
        Logger::E(TAG, "Error listing Interfaces%x", ec);
        return;
    }
    for (Int32 i = 0; i < ifaces->GetLength(); i++){
        String iface = (*ifaces)[i];
        if (IsUsb(iface)) {
            Int32 result;
            if (enable) {
                Tether(iface, &result);
            }
            else {
                Untether(iface, &result);
            }
            if (result == IConnectivityManager::TETHER_ERROR_NO_ERROR) {
                return;
            }
        }
    }
    Logger::E(TAG, "unable start or stop USB tethering");
}

Boolean CTethering::ConfigureUsbIface(
    /* [in] */ Boolean enabled)
{
    if (VDBG) Logger::D(TAG, "configureUsbIface(%d)", enabled);

    // toggle the USB interfaces
    AutoPtr<ArrayOf<String> > ifaces;
    ECode ec = mNMService->ListInterfaces((ArrayOf<String> **)&ifaces);
    if (FAILED(ec)) {
        Logger::E(TAG, "Error listing Interfaces%x", ec);
        return FALSE;
    }
    for (Int32 i = 0; i < ifaces->GetLength(); i++){
        String iface = (*ifaces)[i];
        if (IsUsb(iface)) {
            AutoPtr<IInterfaceConfiguration> ifcg;
            ec = mNMService->GetInterfaceConfig(iface, (IInterfaceConfiguration**)&ifcg);
            if (FAILED(ec)) {
                Logger::E(TAG, "Error configuring Interfaces%x", ec);
                return FALSE;
            }
            if (ifcg != NULL) {
                AutoPtr<IInetAddress> addr;
                NetworkUtils::NumericToInetAddress(USB_NEAR_IFACE_ADDR, (IInetAddress**)&addr);

                AutoPtr<ILinkAddress> linkAddr;
                CLinkAddress::New(addr, USB_PREFIX_LENGTH, (ILinkAddress **)&linkAddr);
                ifcg->SetLinkAddress(linkAddr);
                if (enabled) {
                    ec = ifcg->SetInterfaceUp();
                }
                else {
                    ec = ifcg->SetInterfaceDown();
                }
                if (FAILED(ec)) {
                    Logger::E(TAG, "Error configuring Interfaces%x", ec);
                    return FALSE;
                }
                ifcg->ClearFlag(String("running"));
                ec = mNMService->SetInterfaceConfig(iface, ifcg);
                if (FAILED(ec)) {
                    Logger::E(TAG, "Error configuring Interfaces%x", ec);
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

ECode CTethering::GetTetherableUsbRegexs(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTetherableUsbRegexs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTethering::GetTetherableWifiRegexs(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTetherableWifiRegexs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTethering::GetTetherableBluetoothRegexs(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTetherableBluetoothRegexs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTethering::SetUsbTethering(
    /* [in] */ Boolean enable,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Logger::D(TAG, "setUsbTethering(%d)", enable);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USB_SERVICE, (IInterface**)&obj);
    AutoPtr<IUsbManager> usbManager = IUsbManager::Probe(obj);

    {
        AutoLock lock(mPublicSync);

        if (enable){
            if (mRndisEnabled) {
                TetherUsb(TRUE);
            }
            else {
                mUsbTetherRequested = TRUE;
                usbManager->SetCurrentFunction(IUsbManager::USB_FUNCTION_RNDIS, FALSE);
            }
        }
        else{
            TetherUsb(FALSE);
            if (mRndisEnabled) {
                usbManager->SetCurrentFunction(String(NULL), FALSE);
            }
            mUsbTetherRequested = FALSE;
        }
    }
    *result = IConnectivityManager::TETHER_ERROR_NO_ERROR;
    return NOERROR;
}

AutoPtr< ArrayOf<Int32> > CTethering::GetUpstreamIfaceTypes()
{
    AutoPtr< ArrayOf<Int32> > values;
    {
        AutoLock lock(mPublicSync);
        UpdateConfiguration();  // TODO - remove?
        values = ArrayOf<Int32>::Alloc(mUpstreamIfaceTypes.GetSize());
        List< AutoPtr<IInteger32> >::Iterator it = mUpstreamIfaceTypes.Begin();
        Int32 i = 0;
        for (; it != mUpstreamIfaceTypes.End(); ++it, ++i) {
            Int32 value;
            (*it)->GetValue(&value);
            (*values)[i] = value;
        }
    }
    return values;
}

void CTethering::CheckDunRequired()
{
    AutoPtr<IContentResolver> contentresolver;
    mContext->GetContentResolver((IContentResolver**)&contentresolver);
    Int32 secureSetting = 0;
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    global->GetInt32(contentresolver, ISettingsGlobal::TETHER_DUN_REQUIRED,
         2, &secureSetting);
    {
        AutoLock lock(mPublicSync);
        // 2 = not set, 0 = DUN not required, 1 = DUN required
        if (secureSetting != 2) {
            Int32 requiredApn = (secureSetting == 1 ?
                    IConnectivityManager::TYPE_MOBILE_DUN :
                    IConnectivityManager::TYPE_MOBILE_HIPRI);
            if (requiredApn == IConnectivityManager::TYPE_MOBILE_DUN) {
                while (Find(mUpstreamIfaceTypes.Begin(), mUpstreamIfaceTypes.End(), MOBILE_TYPE)
                        != mUpstreamIfaceTypes.End()) {
                    mUpstreamIfaceTypes.Remove(MOBILE_TYPE);
                }
                while (Find(mUpstreamIfaceTypes.Begin(), mUpstreamIfaceTypes.End(), HIPRI_TYPE)
                        != mUpstreamIfaceTypes.End()) {
                    mUpstreamIfaceTypes.Remove(HIPRI_TYPE);
                }
                if (Find(mUpstreamIfaceTypes.Begin(), mUpstreamIfaceTypes.End(), DUN_TYPE)
                        == mUpstreamIfaceTypes.End()) {
                    mUpstreamIfaceTypes.PushBack(DUN_TYPE);
                }
            }
            else {
                while (Find(mUpstreamIfaceTypes.Begin(), mUpstreamIfaceTypes.End(), DUN_TYPE)
                        != mUpstreamIfaceTypes.End()) {
                    mUpstreamIfaceTypes.Remove(DUN_TYPE);
                }
                if (Find(mUpstreamIfaceTypes.Begin(), mUpstreamIfaceTypes.End(), MOBILE_TYPE)
                        == mUpstreamIfaceTypes.End()) {
                    mUpstreamIfaceTypes.PushBack(MOBILE_TYPE);
                }
                if (Find(mUpstreamIfaceTypes.Begin(), mUpstreamIfaceTypes.End(), HIPRI_TYPE)
                        == mUpstreamIfaceTypes.End()) {
                    mUpstreamIfaceTypes.PushBack(HIPRI_TYPE);
                }
            }
        }
        if (Find(mUpstreamIfaceTypes.Begin(), mUpstreamIfaceTypes.End(), DUN_TYPE)
                != mUpstreamIfaceTypes.End()) {
            mPreferredUpstreamMobileApn = IConnectivityManager::TYPE_MOBILE_DUN;
        }
        else {
            mPreferredUpstreamMobileApn = IConnectivityManager::TYPE_MOBILE_HIPRI;
        }
    }
}

ECode CTethering::GetTetheredIfaces(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    List<String> list;
    {
        AutoLock lock(mPublicSync);
        HashMap<String, AutoPtr<TetherInterfaceSM> >::Iterator it;
        for (it = mIfaces.Begin(); it != mIfaces.End(); ++it) {
            AutoPtr<TetherInterfaceSM> sm = it->mSecond;
            if (sm->IsTethered()) {
                list.PushBack(it->mFirst);
            }
        }
    }
    AutoPtr< ArrayOf<String> > retVal = ArrayOf<String>::Alloc(list.GetSize());
    List<String>::Iterator it;
    Int32 i;
    for (it = list.Begin(), i = 0; it != list.End(); ++it, ++i) {
        (*retVal)[i] = *it;
    }
    *result = retVal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTethering::GetTetherableIfaces(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    List<String> list;
    {
        AutoLock lock(mPublicSync);
        HashMap<String, AutoPtr<TetherInterfaceSM> >::Iterator it;
        for (it = mIfaces.Begin(); it != mIfaces.End(); ++it) {
            AutoPtr<TetherInterfaceSM> sm = it->mSecond;
            if (sm->IsAvailable()) {
                list.PushBack(it->mFirst);
            }
        }
    }
    AutoPtr< ArrayOf<String> > retVal = ArrayOf<String>::Alloc(list.GetSize());
    List<String>::Iterator it;
    Int32 i;
    for (it = list.Begin(), i = 0; it != list.End(); ++it, ++i) {
        (*retVal)[i] = *it;
    }
    *result = retVal;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTethering::GetTetheredDhcpRanges(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDhcpRange;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr< ArrayOf<String> > CTethering::GetErroredIfaces()
{
    List<String> list;
    {
        AutoLock lock(mPublicSync);
        HashMap<String, AutoPtr<TetherInterfaceSM> >::Iterator it;
        for (it = mIfaces.Begin(); it != mIfaces.End(); ++it) {
            AutoPtr<TetherInterfaceSM> sm = it->mSecond;
            if (sm->IsErrored()) {
                list.PushBack(it->mFirst);
            }
        }
    }
    AutoPtr< ArrayOf<String> > retVal = ArrayOf<String>::Alloc(list.GetSize());
    List<String>::Iterator it;
    Int32 i;
    for (it = list.Begin(), i = 0; it != list.End(); ++it, ++i) {
        (*retVal)[i] = *it;
    }
    return retVal;
}

ECode CTethering::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0);
//     Boolean result;
// //  mContext->CheckCallingOrSelfPermission(
// //          (android::Manifest::permission::DUMP) != IPackageManager::PERMISSION_GRANTED, &result);
// //
//     if (result) {
// //        /*pw.println("Permission Denial: can't dump ConnectivityService.Tether " +
// //                "from from pid=" + Binder.getCallingPid() + ", uid=" +
// //                Binder.getCallingUid());*/
//         return NOERROR;
//     }

//     {
//         AutoLock lock(mPublicSync);
// //      //pw.println("mUpstreamIfaceTypes: ");
// //      /*for (Integer netType : mUpstreamIfaceTypes) {
// //          pw.println(" " + netType);
// //      }*/
// //
// //      //pw.println();
// //      //pw.println("Tether state:");
// //      /*for (Object o : mIfaces.values()) {
// //          pw.println(" " + o);
// //      }*/
//     }
// //
// //    //pw.println();
    return NOERROR;
}

ECode CTethering::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace Connectivity
} // namespace Server
} // namespace Droid
} // namespace Elastos
