
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_MODEMSTACKCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_MODEMSTACKCONTROLLER_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/RegistrantList.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/*
 * ModemStackController: Utility to provide the Stack capabilites and binding/unbinding
 * of Stacks based on client preference.
 * Includes:
 *    CurrentStackIds
 *    PreferredStackIds
 *    ModemCapabilities
 * Provides:
 *    Crossmapping (i.e. Binding/unbinding of stacks)
 *    RAT Capabilities of Stack for a sub.
 *    Current stack for a sub
 * Notifies:
 *    Stack Ready after update binding is completed to registrants.
 */

class ModemStackController
    : public Handler
{
public:
    //Utility class holding modem capabilities information per stack.
    class ModemCapabilityInfo: public Object
    {
    public:
        ModemCapabilityInfo(
            /* [in] */ Int32 stackId,
            /* [in] */ Int32 supportedRatBitMask,
            /* [in] */ Int32 voiceCap,
            /* [in] */ Int32 dataCap);

        CARAPI_(Int32) GetSupportedRatBitMask();

        CARAPI_(Int32) GetStackId();

        CARAPI_(Int32) GetMaxDataCap();

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        Int32 mStackId;
        //bit vector mask of all suported RATs
        Int32 mSupportedRatBitMask;
        Int32 mVoiceDataCap;
        Int32 mMaxDataCap;
    };

private:
    class Receiver: public BroadcastReceiver
    {
    public:
        Receiver(
            /* [in] */ ModemStackController* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        ModemStackController* mHost;
    };

public:
    //***** Class Methods
    static CARAPI_(AutoPtr<ModemStackController>) Make(
        /* [in] */ IContext* context,
        /* [in] */ IUiccController* uiccMgr,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    static CARAPI_(AutoPtr<ModemStackController>) GetInstance();

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI_(Boolean) IsStackReady();

    CARAPI_(Int32) GetMaxDataAllowed();

    CARAPI_(Int32) GetCurrentStackIdForPhoneId(
        /* [in] */ Int32 phoneId);

    CARAPI_(Int32) GetPrimarySub();

    CARAPI_(AutoPtr<ModemCapabilityInfo>) GetModemRatCapsForPhoneId(
        /* [in] */ Int32 phoneId);

    CARAPI_(Int32) UpdateStackBinding(
        /* [in] */ ArrayOf<Int32>* prefStackIds,
        /* [in] */ Boolean isBootUp,
        /* [in] */ IMessage* msg);

    CARAPI_(void) RegisterForStackReady(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI_(void) RegisterForModemRatCapsAvailable(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI_(void) RegisterForModemDataCapsUpdate(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

private:
    //***** Constructor
    ModemStackController(
        /* [in] */ IContext* context,
        /* [in] */ IUiccController* uiccManager,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    CARAPI_(void) ProcessRadioAvailable(
        /* [in] */ AsyncResult* ar,
        /* [in] */ Int32 phoneId);

    CARAPI_(void) OnGetModemCapabilityDone(
        /* [in] */ AsyncResult* ar,
        /* [in] */ ArrayOf<Byte>* result,
        /* [in] */ Int32 phoneId);

    CARAPI_(void) OnUnsolModemCapabilityChanged(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) OnSubDeactivated(
        /* [in] */ AsyncResult* ar,
        /* [in] */ Int32 phoneId);

    CARAPI_(void) BindStackOnSub(
        /* [in] */ Int32 phoneId);

    CARAPI_(void) UnbindStackOnSub(
        /* [in] */ Int32 phoneId);

    CARAPI_(void) OnUnbindComplete(
        /* [in] */ AsyncResult* ar,
        /* [in] */ Int32 phoneId);

    CARAPI_(void) OnBindComplete(
        /* [in] */ AsyncResult* ar,
        /* [in] */ Int32 phoneId);

    CARAPI_(void) OnSetPrefNwModeDone(
        /* [in] */ AsyncResult* ar,
        /* [in] */ Int32 phoneId);

    CARAPI_(void) UpdateNetworkSelectionMode();

    CARAPI_(void) TriggerUnBindingOnAllSubs();

    CARAPI_(void) TriggerBindingOnAllSubs();

    CARAPI_(void) TriggerDeactivationOnAllSubs();

    CARAPI_(void) SetPrefNwTypeOnAllSubs();

    CARAPI_(Boolean) AreAllSubsinSameState(
        /* [in] */ Int32 state);

    CARAPI_(void) ResetSubStates();

    CARAPI_(Boolean) IsAnyCmdFailed();

    CARAPI_(void) UpdateModemCapInfo(
        /* [in] */ Int32 phoneId,
        /* [in] */ Int32 stackId,
        /* [in] */ Int32 supportedRatBitMask,
        /* [in] */ Int32 voiceDataCap,
        /* [in] */ Int32 maxDataCap);

    CARAPI_(void) ParseGetModemCapabilityResponse(
        /* [in] */ ArrayOf<Byte>* result,
        /* [in] */ Int32 phoneId);

    CARAPI_(void) SyncPreferredNwModeFromDB();

    CARAPI_(Boolean) IsAnyCallsInProgress();

    CARAPI_(void) DeactivateAllSubscriptions();

    CARAPI_(void) NotifyStackReady(
        /* [in] */ Boolean isCrossMapDone);

    CARAPI_(void) NotifyModemRatCapabilitiesAvailable();

    CARAPI_(void) NotifyModemDataCapabilitiesAvailable();

    CARAPI_(void) RecoverToPrevState();

    CARAPI_(void) SendResponseToTarget(
        /* [in] */ IMessage* response,
        /* [in] */ Int32 responseCode);

    CARAPI_(void) Logd(
        /* [in] */ const String& string);

    CARAPI_(void) Loge(
        /* [in] */ const String& string);

private:
    static const String TAG;

    //***** Events
    static const Int32 CMD_DEACTIVATE_ALL_SUBS;
    static const Int32 EVENT_GET_MODEM_CAPS_DONE;
    static const Int32 CMD_TRIGGER_UNBIND;
    static const Int32 EVENT_UNBIND_DONE;
    static const Int32 CMD_TRIGGER_BIND;
    static const Int32 EVENT_BIND_DONE;
    static const Int32 EVENT_SET_PREF_MODE_DONE;
    static const Int32 EVENT_SUB_DEACTIVATED;
    static const Int32 EVENT_RADIO_AVAILABLE;
    static const Int32 EVENT_MODEM_CAPABILITY_CHANGED;

    //*****States
    static const Int32 STATE_UNKNOWN;
    static const Int32 STATE_GOT_MODEM_CAPS;
    static const Int32 STATE_SUB_DEACT;
    static const Int32 STATE_UNBIND;
    static const Int32 STATE_BIND;
    static const Int32 STATE_SUB_ACT;
    static const Int32 STATE_SET_PREF_MODE;

    //*****Constants
    static const Int32 BIND_TO_STACK;
    static const Int32 UNBIND_TO_STACK;
    static const Int32 GET_MODEM_CAPS_BUFFER_LEN;
    static const Int32 SUCCESS;
    static const Int32 FAILURE;
    static const Int32 PRIMARY_STACK_ID;
    static const Int32 DEFAULT_MAX_DATA_ALLOWED;

    //***** Class Variables
    static AutoPtr<ModemStackController> sModemStackController;
    AutoPtr<ArrayOf<ICommandsInterface*> > mCi;
    AutoPtr<IContext> mContext;
    Int32 mNumPhones;
    Int32 mActiveSubCount;
    Int32 mDeactivedSubCount;
    AutoPtr<ArrayOf<Int32> > mPreferredStackId;
    AutoPtr<ArrayOf<Int32> > mCurrentStackId;
    AutoPtr<ArrayOf<Int32> > mPrefNwMode;
    AutoPtr<ArrayOf<Int32> > mSubState;
    Boolean mIsStackReady;
    Boolean mIsRecoveryInProgress;
    Boolean mIsPhoneInEcbmMode;
    Boolean mModemRatCapabilitiesAvailable;
    Boolean mDeactivationInProgress;
    AutoPtr<ArrayOf<Boolean> > mCmdFailed;
    AutoPtr<RegistrantList> mStackReadyRegistrants;
    AutoPtr<RegistrantList> mModemRatCapsAvailableRegistrants;
    AutoPtr<RegistrantList> mModemDataCapsAvailableRegistrants;
    AutoPtr<IMessage> mUpdateStackMsg;
    HashMap<Int32, Int32> mSubcriptionStatus; // = new HashMap<Integer, Integer>();

    //Modem capabilities as per StackId
    AutoPtr<ArrayOf<ModemCapabilityInfo*> > mModemCapInfo;

    AutoPtr<IBroadcastReceiver> mReceiver;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Internal::Telephony::ModemStackController::ModemCapabilityInfo, IInterface)

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_MODEMSTACKCONTROLLER_H__
