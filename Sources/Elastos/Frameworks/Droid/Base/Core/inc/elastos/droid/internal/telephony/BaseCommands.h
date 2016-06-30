
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_BASECOMMANDS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_BASECOMMANDS_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/os/Registrant.h"
#include "elastos/droid/os/RegistrantList.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::DataConnection::IDataProfile;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Registrant;
using Elastos::Droid::Os::RegistrantList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
class BaseCommands
    : public Object
    , public IBaseCommands
    , public ICommandsInterface
{
public:
    CAR_INTERFACE_DECL()

    BaseCommands();

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual ~BaseCommands();

    //***** CommandsInterface implementation

    // @Override
    CARAPI GetRadioState(
        /* [out] */ ICommandsInterfaceRadioState* state);

    // @Override
    CARAPI RegisterForRadioStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForRadioStateChanged(
        /* [in] */ IHandler* h);

    virtual CARAPI RegisterForImsNetworkStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForImsNetworkStateChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForOn(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForOn(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForAvailable(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForAvailable(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForNotAvailable(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForNotAvailable(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForOffOrNotAvailable(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForOffOrNotAvailable(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForCallStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForCallStateChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForVoiceNetworkStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForVoiceNetworkStateChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForDataNetworkStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForDataNetworkStateChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForVoiceRadioTechChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForVoiceRadioTechChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForIccStatusChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForIccStatusChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnNewGsmSms(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnNewGsmSms(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnNewCdmaSms(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnNewCdmaSms(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnNewGsmBroadcastSms(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnNewGsmBroadcastSms(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnSmsOnSim(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnSmsOnSim(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnSmsStatus(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnSmsStatus(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnSignalStrengthUpdate(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnSignalStrengthUpdate(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnNITZTime(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnNITZTime(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnUSSD(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnUSSD(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnSuppServiceNotification(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnSuppServiceNotification(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnCatSessionEnd(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnCatSessionEnd(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnCatProactiveCmd(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnCatProactiveCmd(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnCatEvent(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnCatEvent(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnCatCallSetUp(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnCatCallSetUp(
        /* [in] */ IHandler* h);

    // For Samsung STK
    virtual CARAPI SetOnCatSendSmsResult(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnSetOnCatSendSmsResult(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnIccSmsFull(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnIccSmsFull(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForIccRefresh(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI SetOnIccRefresh(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI SetEmergencyCallbackMode(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForIccRefresh(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI UnsetOnIccRefresh(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnCallRing(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnCallRing(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnSs(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnSs(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnCatCcAlphaNotify(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnCatCcAlphaNotify(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForInCallVoicePrivacyOn(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForInCallVoicePrivacyOn(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForInCallVoicePrivacyOff(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForInCallVoicePrivacyOff(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI SetOnRestrictedStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnSetOnRestrictedStateChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForDisplayInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForDisplayInfo(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForCallWaitingInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForCallWaitingInfo(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForSignalInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI SetOnUnsolOemHookRaw(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnSetOnUnsolOemHookRaw(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI UnregisterForSignalInfo(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForCdmaOtaProvision(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForCdmaOtaProvision(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForNumberInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForNumberInfo(
        /* [in] */ IHandler* h);

     // @Override
    CARAPI RegisterForRedirectedNumberInfo(
        /* [in] */ IHandler*,
        /* [in] */ Int32    what,
        /* [in] */ IInterface*);

    // @Override
    CARAPI UnregisterForRedirectedNumberInfo(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForLineControlInfo(
        /* [in] */ IHandler* h,
        /* [in] */  Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForLineControlInfo(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterFoT53ClirlInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForT53ClirInfo(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForT53AudioControlInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForT53AudioControlInfo(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForRingbackTone(
        /* [in] */ IHandler* h,
        /* [in] */  Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForRingbackTone(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForResendIncallMute(
        /* [in] */ IHandler* h,
        /* [in] */  Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForResendIncallMute(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForCdmaSubscriptionChanged(
        /* [in] */ IHandler* h,
        /* [in] */  Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForCdmaSubscriptionChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForCdmaPrlChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForCdmaPrlChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForExitEmergencyCallbackMode(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForExitEmergencyCallbackMode(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForHardwareConfigChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForHardwareConfigChanged(
        /* [in] */ IHandler* h);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI RegisterForRilConnected(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForRilConnected(
        /* [in] */ IHandler* h);

    virtual CARAPI RegisterForSubscriptionStatusChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForSubscriptionStatusChanged(
        /* [in] */ IHandler* h);

    virtual CARAPI RegisterForWwanIwlanCoexistence(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForWwanIwlanCoexistence(
        /* [in] */ IHandler* h);

    virtual CARAPI RegisterForSimRefreshEvent(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForSimRefreshEvent(
        /* [in] */ IHandler* h);

    virtual CARAPI RegisterForModemCapEvent(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForModemCapEvent(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI GetDataCallProfile(
        /* [in] */ Int32 appType,
        /* [in] */ IMessage* result);

    virtual CARAPI SendSMSExpectMore(
        /* [in] */ const String& smscPDU,
        /* [in] */ const String& pdu,
        /* [in] */ IMessage* result);

    virtual CARAPI OnRadioAvailable();

    virtual CARAPI GetModemCapability(
        /* [in] */ IMessage* response);

    virtual CARAPI UpdateStackBinding(
        /* [in] */ Int32 stackId,
        /* [in] */ Int32 enable,
        /* [in] */ IMessage* response);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI GetLteOnCdmaMode(
        /* [out] */ Int32* mode);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI RegisterForCellInfoList(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForCellInfoList(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForSrvccStateChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForSrvccStateChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI TestingEmergencyCall();

    // @Override
    CARAPI GetRilVersion(
        /* [out] */ Int32* version);

    virtual CARAPI SetUiccSubscription(
        /* [in] */ Int32 slotId,
        /* [in] */ Int32 appIndex,
        /* [in] */ Int32 subId,
        /* [in] */ Int32 subStatus,
        /* [in] */ IMessage* response);

    virtual CARAPI SetDataProfile(
        /* [in] */ ArrayOf<IDataProfile*>* dps,
        /* [in] */ IMessage* result);

    virtual CARAPI SetDataAllowed(
        /* [in] */ Boolean allowed,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI RequestShutdown(
        /* [in] */ IMessage* result);

    // @Override
    CARAPI IccOpenLogicalChannel(
        /* [in] */ const String& AID,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI IccCloseLogicalChannel(
        /* [in] */ Int32 channel,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI IccTransmitApduLogicalChannel(
        /* [in] */ Int32 channel,
        /* [in] */ Int32 cla,
        /* [in] */ Int32 instruction,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI IccTransmitApduBasicChannel(
        /* [in] */ Int32 cla,
        /* [in] */ Int32 instruction,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [in] */ IMessage* response);

    // @Override
    CARAPI GetAtr(
        /* [in] */ IMessage* response);

    CARAPI SetLocalCallHold(
        /* [in] */ Int32 lchStatus);

    /**
     * @hide
     */
    // @Override
    CARAPI GetLteOnGsmMode(
        /* [out] */ Int32* mode);

protected:
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
    virtual CARAPI_(void) SetRadioState(
        /* [in] */ ICommandsInterfaceRadioState newState);

private:
    static CARAPI_(Boolean) IsAvailable(
        /* [in] */ ICommandsInterfaceRadioState state);

    static CARAPI_(Boolean) IsOn(
        /* [in] */ ICommandsInterfaceRadioState state);

protected:
    //***** Instance Variables
    AutoPtr<IContext> mContext;
    ICommandsInterfaceRadioState mState;
    Object mStateMonitor;

    AutoPtr<RegistrantList> mRadioStateChangedRegistrants;
    AutoPtr<RegistrantList> mOnRegistrants;
    AutoPtr<RegistrantList> mAvailRegistrants;
    AutoPtr<RegistrantList> mOffOrNotAvailRegistrants;
    AutoPtr<RegistrantList> mNotAvailRegistrants;
    AutoPtr<RegistrantList> mCallStateRegistrants;
    AutoPtr<RegistrantList> mVoiceNetworkStateRegistrants;
    AutoPtr<RegistrantList> mDataNetworkStateRegistrants;
    AutoPtr<RegistrantList> mVoiceRadioTechChangedRegistrants;
    AutoPtr<RegistrantList> mImsNetworkStateChangedRegistrants;
    AutoPtr<RegistrantList> mIccStatusChangedRegistrants;
    AutoPtr<RegistrantList> mVoicePrivacyOnRegistrants;
    AutoPtr<RegistrantList> mVoicePrivacyOffRegistrants;
    AutoPtr<Registrant> mUnsolOemHookRawRegistrant;
    AutoPtr<RegistrantList> mOtaProvisionRegistrants;
    AutoPtr<RegistrantList> mCallWaitingInfoRegistrants;
    AutoPtr<RegistrantList> mDisplayInfoRegistrants;
    AutoPtr<RegistrantList> mSignalInfoRegistrants;
    AutoPtr<RegistrantList> mNumberInfoRegistrants;
    AutoPtr<RegistrantList> mRedirNumInfoRegistrants;
    AutoPtr<RegistrantList> mLineControlInfoRegistrants;
    AutoPtr<RegistrantList> mT53ClirInfoRegistrants;
    AutoPtr<RegistrantList> mT53AudCntrlInfoRegistrants;
    AutoPtr<RegistrantList> mRingbackToneRegistrants;
    AutoPtr<RegistrantList> mResendIncallMuteRegistrants;
    AutoPtr<RegistrantList> mCdmaSubscriptionChangedRegistrants;
    AutoPtr<RegistrantList> mCdmaPrlChangedRegistrants;
    AutoPtr<RegistrantList> mExitEmergencyCallbackModeRegistrants;
    AutoPtr<RegistrantList> mRilConnectedRegistrants;
    AutoPtr<RegistrantList> mIccRefreshRegistrants;
    AutoPtr<RegistrantList> mRilCellInfoListRegistrants;
    AutoPtr<RegistrantList> mSubscriptionStatusRegistrants;
    AutoPtr<RegistrantList> mSrvccStateRegistrants;
    AutoPtr<RegistrantList> mHardwareConfigChangeRegistrants;
    AutoPtr<RegistrantList> mWwanIwlanCoexistenceRegistrants;
    AutoPtr<RegistrantList> mSimRefreshRegistrants;
    AutoPtr<RegistrantList> mModemCapRegistrants;

    AutoPtr<Registrant> mGsmSmsRegistrant;
    AutoPtr<Registrant> mCdmaSmsRegistrant;
    AutoPtr<Registrant> mNITZTimeRegistrant;
    AutoPtr<Registrant> mSignalStrengthRegistrant;
    AutoPtr<Registrant> mUSSDRegistrant;
    AutoPtr<Registrant> mSmsOnSimRegistrant;
    AutoPtr<Registrant> mSmsStatusRegistrant;
    AutoPtr<Registrant> mSsnRegistrant;
    AutoPtr<Registrant> mCatSessionEndRegistrant;
    AutoPtr<Registrant> mCatProCmdRegistrant;
    AutoPtr<Registrant> mCatEventRegistrant;
    AutoPtr<Registrant> mCatCallSetUpRegistrant;
    AutoPtr<Registrant> mCatSendSmsResultRegistrant;
    AutoPtr<Registrant> mIccSmsFullRegistrant;
    AutoPtr<Registrant> mEmergencyCallbackModeRegistrant;
    AutoPtr<Registrant> mRingRegistrant;
    AutoPtr<Registrant> mRestrictedStateRegistrant;
    AutoPtr<Registrant> mGsmBroadcastSmsRegistrant;
    AutoPtr<Registrant> mCatCcAlphaRegistrant;
    AutoPtr<Registrant> mSsRegistrant;

    // Preferred network type received from PhoneFactory.
    // This is used when establishing a connection to the
    // vendor ril so it starts up in the correct mode.
    Int32 mPreferredNetworkType;
    // CDMA subscription received from PhoneFactory
    Int32 mCdmaSubscription;
    // Type of Phone, GSM or CDMA. Set by CDMAPhone or GSMPhone.
    Int32 mPhoneType;
    // RIL Version
    Int32 mRilVersion;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_BASECOMMANDS_H__
