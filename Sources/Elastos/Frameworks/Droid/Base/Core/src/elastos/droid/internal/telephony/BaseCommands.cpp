
#include "elastos/droid/internal/telephony/BaseCommands.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/AsyncResult.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL_2(BaseCommands, Object, IBaseCommands, ICommandsInterface)
BaseCommands::BaseCommands(
    /* [in] */ IContext* context)
    : mState(RADIO_UNAVAILABLE)
    , mPreferredNetworkType(0)
    , mCdmaSubscription(0)
    , mPhoneType(0)
    , mRilVersion(-1)
{
    mContext = context;  // May be NULL (if so we won't log statistics)
    mRadioStateChangedRegistrants = new RegistrantList();
    mOnRegistrants = new RegistrantList();
    mAvailRegistrants = new RegistrantList();
    mOffOrNotAvailRegistrants = new RegistrantList();
    mNotAvailRegistrants = new RegistrantList();
    mCallStateRegistrants = new RegistrantList();
    mVoiceNetworkStateRegistrants = new RegistrantList();
    mDataNetworkStateRegistrants = new RegistrantList();
    mVoiceRadioTechChangedRegistrants = new RegistrantList();
    mImsNetworkStateChangedRegistrants = new RegistrantList();
    mIccStatusChangedRegistrants = new RegistrantList();
    mVoicePrivacyOnRegistrants = new RegistrantList();
    mVoicePrivacyOffRegistrants = new RegistrantList();
    mOtaProvisionRegistrants = new RegistrantList();
    mCallWaitingInfoRegistrants = new RegistrantList();
    mDisplayInfoRegistrants = new RegistrantList();
    mSignalInfoRegistrants = new RegistrantList();
    mNumberInfoRegistrants = new RegistrantList();
    mRedirNumInfoRegistrants = new RegistrantList();
    mLineControlInfoRegistrants = new RegistrantList();
    mT53ClirInfoRegistrants = new RegistrantList();
    mT53AudCntrlInfoRegistrants = new RegistrantList();
    mRingbackToneRegistrants = new RegistrantList();
    mResendIncallMuteRegistrants = new RegistrantList();
    mCdmaSubscriptionChangedRegistrants = new RegistrantList();
    mCdmaPrlChangedRegistrants = new RegistrantList();
    mExitEmergencyCallbackModeRegistrants = new RegistrantList();
    mRilConnectedRegistrants = new RegistrantList();
    mIccRefreshRegistrants = new RegistrantList();
    mRilCellInfoListRegistrants = new RegistrantList();
    mSubscriptionStatusRegistrants = new RegistrantList();
    mSrvccStateRegistrants = new RegistrantList();
    mHardwareConfigChangeRegistrants = new RegistrantList();
    mWwanIwlanCoexistenceRegistrants = new RegistrantList();
    mSimRefreshRegistrants = new RegistrantList();
    mModemCapRegistrants = new RegistrantList();
}

BaseCommands::~BaseCommands()
{}

//***** CommandsInterface implementation

ECode BaseCommands::GetRadioState(
    /* [out] */ ICommandsInterfaceRadioState* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode BaseCommands::RegisterForRadioStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);

    {
        AutoLock lock(mStateMonitor);
        mRadioStateChangedRegistrants->Add(r);
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode BaseCommands::UnregisterForRadioStateChanged(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mStateMonitor);
        mRadioStateChangedRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode BaseCommands::RegisterForImsNetworkStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mImsNetworkStateChangedRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForImsNetworkStateChanged(
    /* [in] */ IHandler* h)
{
    mImsNetworkStateChangedRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForOn(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);

    {
        AutoLock lock(mStateMonitor);
        mOnRegistrants->Add(r);

        if (mState == RADIO_ON) {
            AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
            r->NotifyRegistrant(ar);
        }
    }
    return NOERROR;
}

ECode BaseCommands::UnregisterForOn(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mStateMonitor);
        mOnRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode BaseCommands::RegisterForAvailable(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);

    {
        AutoLock lock(mStateMonitor);
        mAvailRegistrants->Add(r);

        if (IsAvailable(mState)) {
            AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
            r->NotifyRegistrant(ar);
        }
    }
    return NOERROR;
}

ECode BaseCommands::UnregisterForAvailable(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mStateMonitor);
        mAvailRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode BaseCommands::RegisterForNotAvailable(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);

    {
        AutoLock lock(mStateMonitor);
        mNotAvailRegistrants->Add(r);

        if (!IsAvailable(mState)) {
            AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
            r->NotifyRegistrant(ar);
        }
    }
    return NOERROR;
}

ECode BaseCommands::UnregisterForNotAvailable(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mStateMonitor);
        mNotAvailRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode BaseCommands::RegisterForOffOrNotAvailable(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);

    {
        AutoLock lock(mStateMonitor);
        mOffOrNotAvailRegistrants->Add(r);

        if (mState == RADIO_OFF || !IsAvailable(mState)) {
            AutoPtr<AsyncResult> ar = new AsyncResult(NULL, NULL, NULL);
            r->NotifyRegistrant(ar);
        }
    }
    return NOERROR;
}

ECode BaseCommands::UnregisterForOffOrNotAvailable(
    /* [in] */ IHandler* h)
{
    {
        AutoLock lock(mStateMonitor);
        mOffOrNotAvailRegistrants->Remove(h);
    }
    return NOERROR;
}

ECode BaseCommands::RegisterForCallStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);

    mCallStateRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForCallStateChanged(
    /* [in] */ IHandler* h)
{
    mCallStateRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForVoiceNetworkStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);

    mVoiceNetworkStateRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForVoiceNetworkStateChanged(
    /* [in] */ IHandler* h)
{
    mVoiceNetworkStateRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForDataNetworkStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);

    mDataNetworkStateRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForDataNetworkStateChanged(
    /* [in] */ IHandler* h)
{
    mDataNetworkStateRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForVoiceRadioTechChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mVoiceRadioTechChangedRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForVoiceRadioTechChanged(
    /* [in] */ IHandler* h)
{
    mVoiceRadioTechChangedRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForIccStatusChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mIccStatusChangedRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForIccStatusChanged(
    /* [in] */ IHandler* h)
{
    mIccStatusChangedRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::SetOnNewGsmSms(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mGsmSmsRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnNewGsmSms(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mGsmSmsRegistrant->GetHandler((IHandler**)&handler);
    if (mGsmSmsRegistrant != NULL && handler.Get() == h) {
        mGsmSmsRegistrant->Clear();
        mGsmSmsRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnNewCdmaSms(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mCdmaSmsRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnNewCdmaSms(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mCdmaSmsRegistrant->GetHandler((IHandler**)&handler);
    if (mCdmaSmsRegistrant != NULL && handler.Get() == h) {
        mCdmaSmsRegistrant->Clear();
        mCdmaSmsRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnNewGsmBroadcastSms(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mGsmBroadcastSmsRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnNewGsmBroadcastSms(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mGsmBroadcastSmsRegistrant->GetHandler((IHandler**)&handler);
    if (mGsmBroadcastSmsRegistrant != NULL && handler.Get() == h) {
        mGsmBroadcastSmsRegistrant->Clear();
        mGsmBroadcastSmsRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnSmsOnSim(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mSmsOnSimRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnSmsOnSim(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mSmsOnSimRegistrant->GetHandler((IHandler**)&handler);
    if (mSmsOnSimRegistrant != NULL && handler.Get() == h) {
        mSmsOnSimRegistrant->Clear();
        mSmsOnSimRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnSmsStatus(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mSmsStatusRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnSmsStatus(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mSmsStatusRegistrant->GetHandler((IHandler**)&handler);
    if (mSmsStatusRegistrant != NULL && handler.Get() == h) {
        mSmsStatusRegistrant->Clear();
        mSmsStatusRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnSignalStrengthUpdate(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mSignalStrengthRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnSignalStrengthUpdate(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mSignalStrengthRegistrant->GetHandler((IHandler**)&handler);
    if (mSignalStrengthRegistrant != NULL && handler.Get() == h) {
        mSignalStrengthRegistrant->Clear();
        mSignalStrengthRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnNITZTime(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mNITZTimeRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnNITZTime(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mNITZTimeRegistrant->GetHandler((IHandler**)&handler);
    if (mNITZTimeRegistrant != NULL && handler.Get() == h) {
        mNITZTimeRegistrant->Clear();
        mNITZTimeRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnUSSD(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mUSSDRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnUSSD(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mUSSDRegistrant->GetHandler((IHandler**)&handler);
    if (mUSSDRegistrant != NULL && handler.Get() == h) {
        mUSSDRegistrant->Clear();
        mUSSDRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mSsnRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mSsnRegistrant->GetHandler((IHandler**)&handler);
    if (mSsnRegistrant != NULL && handler.Get() == h) {
        mSsnRegistrant->Clear();
        mSsnRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnCatSessionEnd(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mCatSessionEndRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnCatSessionEnd(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mCatSessionEndRegistrant->GetHandler((IHandler**)&handler);
    if (mCatSessionEndRegistrant != NULL && handler.Get() == h) {
        mCatSessionEndRegistrant->Clear();
        mCatSessionEndRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnCatProactiveCmd(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mCatProCmdRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnCatProactiveCmd(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mCatProCmdRegistrant->GetHandler((IHandler**)&handler);
    if (mCatProCmdRegistrant != NULL && handler.Get() == h) {
        mCatProCmdRegistrant->Clear();
        mCatProCmdRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnCatEvent(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mCatEventRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnCatEvent(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mCatEventRegistrant->GetHandler((IHandler**)&handler);
    if (mCatEventRegistrant != NULL && handler.Get() == h) {
        mCatEventRegistrant->Clear();
        mCatEventRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnCatCallSetUp(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mCatCallSetUpRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnCatCallSetUp(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mCatCallSetUpRegistrant->GetHandler((IHandler**)&handler);
    if (mCatCallSetUpRegistrant != NULL && handler.Get() == h) {
        mCatCallSetUpRegistrant->Clear();
        mCatCallSetUpRegistrant = NULL;
    }
    return NOERROR;
}

// For Samsung STK
ECode BaseCommands::SetOnCatSendSmsResult(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mCatSendSmsResultRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnCatSendSmsResult(
    /* [in] */ IHandler* h)
{
    mCatSendSmsResultRegistrant->Clear();
    return NOERROR;
}

ECode BaseCommands::SetOnIccSmsFull(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mIccSmsFullRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnIccSmsFull(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mIccSmsFullRegistrant->GetHandler((IHandler**)&handler);
    if (mIccSmsFullRegistrant != NULL && handler.Get() == h) {
        mIccSmsFullRegistrant->Clear();
        mIccSmsFullRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::RegisterForIccRefresh(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mIccRefreshRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::SetOnIccRefresh(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    RegisterForIccRefresh(h, what, obj);
    return NOERROR;
}

ECode BaseCommands::SetEmergencyCallbackMode(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mEmergencyCallbackModeRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnregisterForIccRefresh(
    /* [in] */ IHandler* h)
{
    mIccRefreshRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::UnsetOnIccRefresh(
    /* [in] */ IHandler* h)
{
    UnregisterForIccRefresh(h);
    return NOERROR;
}

ECode BaseCommands::SetOnCallRing(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mRingRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnCallRing(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mRingRegistrant->GetHandler((IHandler**)&handler);
    if (mRingRegistrant != NULL && handler.Get() == h) {
        mRingRegistrant->Clear();
        mRingRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::SetOnSs(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mSsRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnSs(
    /* [in] */ IHandler* h)
{
    mSsRegistrant->Clear();
    return NOERROR;
}

ECode BaseCommands::SetOnCatCcAlphaNotify(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mCatCcAlphaRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnCatCcAlphaNotify(
    /* [in] */ IHandler* h)
{
    mCatCcAlphaRegistrant->Clear();
    return NOERROR;
}

ECode BaseCommands::RegisterForInCallVoicePrivacyOn(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mVoicePrivacyOnRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForInCallVoicePrivacyOn(
    /* [in] */ IHandler* h)
{
    mVoicePrivacyOnRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForInCallVoicePrivacyOff(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mVoicePrivacyOffRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForInCallVoicePrivacyOff(
    /* [in] */ IHandler* h)
{
    mVoicePrivacyOffRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::SetOnRestrictedStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mRestrictedStateRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnRestrictedStateChanged(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mRestrictedStateRegistrant->GetHandler((IHandler**)&handler);
    if (mRestrictedStateRegistrant != NULL && handler.Get() != h) {
        mRestrictedStateRegistrant->Clear();
        mRestrictedStateRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::RegisterForDisplayInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mDisplayInfoRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForDisplayInfo(
    /* [in] */ IHandler* h)
{
    mDisplayInfoRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForCallWaitingInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mCallWaitingInfoRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForCallWaitingInfo(
    /* [in] */ IHandler* h)
{
    mCallWaitingInfoRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForSignalInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mSignalInfoRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::SetOnUnsolOemHookRaw(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    CRegistrant::New(h, what, obj, (IRegistrant**)&mUnsolOemHookRawRegistrant);
    return NOERROR;
}

ECode BaseCommands::UnSetOnUnsolOemHookRaw(
    /* [in] */ IHandler* h)
{
    AutoPtr<IHandler> handler;
    mUnsolOemHookRawRegistrant->GetHandler((IHandler**)&handler);
    if (mUnsolOemHookRawRegistrant != NULL && handler.Get() == h) {
        mUnsolOemHookRawRegistrant->Clear();
        mUnsolOemHookRawRegistrant = NULL;
    }
    return NOERROR;
}

ECode BaseCommands::UnregisterForSignalInfo(
    /* [in] */ IHandler* h)
{
    mSignalInfoRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForCdmaOtaProvision(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mOtaProvisionRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForCdmaOtaProvision(
    /* [in] */ IHandler* h)
{
    mOtaProvisionRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForNumberInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mNumberInfoRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForNumberInfo(
    /* [in] */ IHandler* h)
{
    mNumberInfoRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForRedirectedNumberInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mRedirNumInfoRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForRedirectedNumberInfo(
    /* [in] */ IHandler* h)
{
    mRedirNumInfoRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForLineControlInfo(
    /* [in] */ IHandler* h,
    /* [in] */  Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mLineControlInfoRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForLineControlInfo(
    /* [in] */ IHandler* h)
{
    mLineControlInfoRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterFoT53ClirlInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mT53ClirInfoRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForT53ClirInfo(
    /* [in] */ IHandler* h)
{
    mT53ClirInfoRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForT53AudioControlInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mT53AudCntrlInfoRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForT53AudioControlInfo(
    /* [in] */ IHandler* h)
{
    mT53AudCntrlInfoRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForRingbackTone(
    /* [in] */ IHandler* h,
    /* [in] */  Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mRingbackToneRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForRingbackTone(
    /* [in] */ IHandler* h)
{
    mRingbackToneRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForResendIncallMute(
    /* [in] */ IHandler* h,
    /* [in] */  Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mResendIncallMuteRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForResendIncallMute(
    /* [in] */ IHandler* h)
{
    mResendIncallMuteRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForCdmaSubscriptionChanged(
    /* [in] */ IHandler* h,
    /* [in] */  Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mCdmaSubscriptionChangedRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForCdmaSubscriptionChanged(
    /* [in] */ IHandler* h)
{
    mCdmaSubscriptionChangedRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForCdmaPrlChanged(
    /* [in] */ IHandler* h,
    /* [in] */  Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mCdmaPrlChangedRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForCdmaPrlChanged(
    /* [in] */ IHandler* h)
{
    mCdmaPrlChangedRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForExitEmergencyCallbackMode(
    /* [in] */ IHandler* h,
    /* [in] */  Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mExitEmergencyCallbackModeRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForExitEmergencyCallbackMode(
    /* [in] */ IHandler* h)
{
    mExitEmergencyCallbackModeRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForHardwareConfigChanged(
    /* [in] */ IHandler* h,
    /* [in] */  Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mHardwareConfigChangeRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForHardwareConfigChanged(
    /* [in] */ IHandler* h)
{
    mHardwareConfigChangeRegistrants->Remove(h);
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode BaseCommands::RegisterForRilConnected(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mRilConnectedRegistrants->Add(r);
    if (mRilVersion != -1) {
        AutoPtr<IInteger32> value;
        CInteger32::New(mRilVersion, (IInteger32**)&value);
        AutoPtr<AsyncResult> ar = new AsyncResult(NULL, value, NULL);
        r->NotifyRegistrant(ar);
    }
    return NOERROR;
}

ECode BaseCommands::UnregisterForRilConnected(
    /* [in] */ IHandler* h)
{
    mRilConnectedRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForSubscriptionStatusChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mSubscriptionStatusRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForSubscriptionStatusChanged(
    /* [in] */ IHandler* h)
{
    mSubscriptionStatusRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForWwanIwlanCoexistence(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mWwanIwlanCoexistenceRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode BaseCommands::UnregisterForWwanIwlanCoexistence(
    /* [in] */ IHandler* h)
{
    mWwanIwlanCoexistenceRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForSimRefreshEvent(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mSimRefreshRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode BaseCommands::UnregisterForSimRefreshEvent(
    /* [in] */ IHandler* h)
{
    mSimRefreshRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForModemCapEvent(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mModemCapRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode BaseCommands::UnregisterForModemCapEvent(
    /* [in] */ IHandler* h)
{
    mModemCapRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::GetDataCallProfile(
    /* [in] */ Int32 appType,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

//***** Protected Methods
/**
 * Store new RadioState and send notification based on the changes
 *
 * This function is called only by RIL.java when receiving unsolicited
 * RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED
 *
 * RadioState has 3 values : RADIO_OFF, RADIO_UNAVAILABLE, RADIO_ON.
 *
 * @param newState new RadioState decoded from RIL_UNSOL_RADIO_STATE_CHANGED
 */
void BaseCommands::SetRadioState(
    /* [in] */ ICommandsInterfaceRadioState newState)
{
    ICommandsInterfaceRadioState oldState;

    {
        AutoLock lock(mStateMonitor);
        oldState = mState;
        mState = newState;

        if (oldState == mState) {
            // no state transition
            return;
        }

        mRadioStateChangedRegistrants->NotifyRegistrants();

        if (IsAvailable(mState) && !IsAvailable(oldState)) {
            mAvailRegistrants->NotifyRegistrants();
            OnRadioAvailable();
        }

        if (!IsAvailable(mState) && IsAvailable(oldState)) {
            mNotAvailRegistrants->NotifyRegistrants();
        }

        if (IsOn(mState) && !IsOn(oldState)) {
            mOnRegistrants->NotifyRegistrants();
        }

        if ((!IsOn(mState) || !IsAvailable(mState))
            && !((!IsOn(oldState) || !IsAvailable(oldState)))
        ) {
            mOffOrNotAvailRegistrants->NotifyRegistrants();
        }
    }
}

ECode BaseCommands::SendSMSExpectMore(
    /* [in] */ const String& smscPDU,
    /* [in] */ const String& pdu,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode BaseCommands::OnRadioAvailable()
{
    return NOERROR;
}

ECode BaseCommands::GetModemCapability(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode BaseCommands::UpdateStackBinding(
    /* [in] */ Int32 stackId,
    /* [in] */ Int32 enable,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

/**
 * {@inheritDoc}
 */
ECode BaseCommands::GetLteOnCdmaMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    return CTelephonyManager::GetLteOnCdmaModeStatic(mode);
}

/**
 * {@inheritDoc}
 */
ECode BaseCommands::RegisterForCellInfoList(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mRilCellInfoListRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForCellInfoList(
    /* [in] */ IHandler* h)
{
    mRilCellInfoListRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::RegisterForSrvccStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);

    mSrvccStateRegistrants->Add(r);
    return NOERROR;
}

ECode BaseCommands::UnregisterForSrvccStateChanged(
    /* [in] */ IHandler* h)
{
    mSrvccStateRegistrants->Remove(h);
    return NOERROR;
}

ECode BaseCommands::TestingEmergencyCall()
{
    return NOERROR;
}

ECode BaseCommands::GetRilVersion(
    /* [out] */ Int32* version)
{
    VALIDATE_NOT_NULL(version);
    *version = mRilVersion;
    return NOERROR;
}

ECode BaseCommands::SetUiccSubscription(
    /* [in] */ Int32 slotId,
    /* [in] */ Int32 appIndex,
    /* [in] */ Int32 subId,
    /* [in] */ Int32 subStatus,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode BaseCommands::SetDataProfile(
    /* [in] */ ArrayOf<IDataProfile*>* dps,
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode BaseCommands::SetDataAllowed(
    /* [in] */ Boolean allowed,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode BaseCommands::RequestShutdown(
    /* [in] */ IMessage* result)
{
    return NOERROR;
}

ECode BaseCommands::IccOpenLogicalChannel(
    /* [in] */ const String& AID,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode BaseCommands::IccCloseLogicalChannel(
    /* [in] */ Int32 channel,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode BaseCommands::IccTransmitApduLogicalChannel(
    /* [in] */ Int32 channel,
    /* [in] */ Int32 cla,
    /* [in] */ Int32 instruction,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode BaseCommands::IccTransmitApduBasicChannel(
    /* [in] */ Int32 cla,
    /* [in] */ Int32 instruction,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode BaseCommands::GetAtr(
    /* [in] */ IMessage* response)
{
    return NOERROR;
}

ECode BaseCommands::SetLocalCallHold(
    /* [in] */ Int32 lchStatus)
{
    return NOERROR;
}

/**
 * @hide
 */
ECode BaseCommands::GetLteOnGsmMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    assert(0 && "TODO");
    // return CTelephonyManager::GetLteOnGsmModeStatic(mode);
    return NOERROR;
}

Boolean BaseCommands::IsAvailable(
    /* [in] */ ICommandsInterfaceRadioState state)
{
    return state != RADIO_UNAVAILABLE;
}

Boolean BaseCommands::IsOn(
    /* [in] */ ICommandsInterfaceRadioState state)
{
    return state == RADIO_ON;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
