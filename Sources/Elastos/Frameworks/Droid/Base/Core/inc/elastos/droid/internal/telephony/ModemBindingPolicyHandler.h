
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_MODEMBINDINGPOLICYHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_MODEMBINDINGPOLICYHANDLER_H__

#include "elastos/droid/internal/telephony/ModemStackController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/*
 * ModemBindingPolicyHandler: Class used to determine stack binding policy based on
 * Network mode selected by the user.
 *
 * Before sending NwMode to RIL, checks if the NwMode is supported on the Stack and
 * if possible initiate a cross mapping sequence.
 * Includes:
 *    Current preferred NwModes
 *    Logic for determining NwMode supported on particular Stack
 *    Logic for determining stack binding policy based on Network mode
 *    selected by user.
 *    Send request for Stack binding update based on above policy.
 */

class ModemBindingPolicyHandler
    : public Handler
    , public IModemBindingPolicyHandler
{
public:
    TO_STRING_IMPL("ModemBindingPolicyHandler")

    CAR_INTERFACE_DECL()

    //***** Class Methods
    static CARAPI_(AutoPtr<ModemBindingPolicyHandler>) Make(
        /* [in] */ IContext* context,
        /* [in] */ IUiccController* uiccMgr,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    static CARAPI_(AutoPtr<ModemBindingPolicyHandler>) GetInstance();

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /*
    * updatePrefNwTypeIfRequired: Method used to set pref network type if required.
    *
    * Description: If Network mode for a subid in simInfo table is valid and and is not
    * equal to value in DB, then update the DB value and send request to RIL.
    */
    CARAPI UpdatePrefNwTypeIfRequired();

    CARAPI SetPreferredNetworkType(
        /* [in] */ Int32 networkType,
        /* [in] */ Int32 phoneId,
        /* [in] */ IMessage* response);

private:
    //***** Constructor
    ModemBindingPolicyHandler(
        /* [in] */ IContext* context,
        /* [in] */ IUiccController* uiccManager,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    CARAPI_(void) HandleSetPreferredNetwork(
        /* [in] */ IMessage* msg);

    CARAPI_(void) HandleUpdateBindingDone(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) HandleModemRatCapsAvailable();

    CARAPI_(void) SyncCurrentStackInfo();

    /*
    * updateStackBindingIfRequired: Method used to update stack binding if required.
    * @param: Boolean override - if TRUE send update stack binding request always
    * @return: Int32 SUCCESS or FAILURE. i.e. Request acepted or not.
    *   SUCCESS, if update stack binding request is sent and it returns SUCCESS
    *   FAILURE, if update stack binding request is not sent or
    *       if update stack binding request is sent and it returns FAILURE.
    */
    CARAPI_(Int32) UpdateStackBindingIfRequired(
        /* [in] */ Boolean isBootUp);

    /*
    * updatePreferredStackIds: Method used to get isCrossBindingRequired and set the Preferred
    * StackIds as per Network Mode for phoneId.
    * @return: Boolean TRUE/FALSE based on crossMapping is required.
    *
    * Description: used to determine the preferred stackIds based on Network Mode selected
    * and return if CrossBindingRequired.
    * Logic:
    * 1. For each phoneId, check if current stack supports the present Network Mode.
    * 2. If supported, continue to next phoneId. If stacks on all phoneId support their respective
    *     Network Mode then crossmapping is not required so preferredStackIds will be same as
    *     CurrentStackIds
    * 3. If stack of any of the phoneId is not supporting their Network Mode, then check if stack of
    *     other phoneIds supports present NetworkMode.
    *    a. If none of the stacks support the present Network Mode, cross mapping is not required
    *       and preferredStackIds will be same as CurrentStackIds.
    *    b. If some other stack supports, check if network mode on other phoneId is supported by
    *       stack after cross mapping.
    *           i. if supported cross mapping is required and set preferredStackIds accordingly.
    *           ii. if not supported cross mapping is not required and preferredStackIds will be
    *              same as CurrentStackIds.
    */
    CARAPI_(void) UpdatePreferredStackIds();

    CARAPI_(Boolean) IsNwModeSupportedOnStack(
        /* [in] */ Int32 nwMode,
        /* [in] */ Int32 stackId);

    CARAPI_(void) SyncPreferredNwModeFromDB();

    CARAPI_(void) SendResponseToTarget(
        /* [in] */ IMessage* response,
        /* [in] */ Int32 responseCode);

    CARAPI_(Int32) GetNumOfRatSupportedForNwMode(
        /* [in] */ Int32 nwMode,
        /* [in] */ ModemStackController::ModemCapabilityInfo* modemCaps);

    //This method will return no. of 1's in binary representation of Int32.
    CARAPI_(Int32) GetNumRatSupportedInMask(
        /* [in] */ Int32 mask);

    CARAPI_(void) Logd(
        /* [in] */ const String& string);

    CARAPI_(void) Loge(
        /* [in] */ const String& string);

private:
    static const String TAG;

    //*****Network Mode bit vector mask
    //Basic Network Modes
    static const Int32 NETWORK_MASK_GSM_ONLY;
    static const Int32 NETWORK_MASK_WCDMA_ONLY;
    static const Int32 NETWORK_MASK_CDMA_NO_EVDO;
    static const Int32 NETWORK_MASK_EVDO_NO_CDMA;
    static const Int32 NETWORK_MASK_LTE_ONLY;
    static const Int32 NETWORK_MASK_TD_SCDMA_ONLY;

    //Complex modes, i.e. Combinations of above basic Network Modes
    static const Int32 NETWORK_MASK_GSM_UMTS;
    static const Int32 NETWORK_MASK_CDMA;
    static const Int32 NETWORK_MASK_WCDMA_PREF;
    static const Int32 NETWORK_MASK_GLOBAL;
    static const Int32 NETWORK_MASK_LTE_CDMA_EVDO;
    static const Int32 NETWORK_MASK_LTE_GSM_WCDMA;
    static const Int32 NETWORK_MASK_LTE_CMDA_EVDO_GSM_WCDMA;
    static const Int32 NETWORK_MASK_LTE_WCDMA;
    static const Int32 NETWORK_MASK_TD_SCDMA_WCDMA;
    static const Int32 NETWORK_MASK_TD_SCDMA_LTE;
    static const Int32 NETWORK_MASK_TD_SCDMA_GSM;
    static const Int32 NETWORK_MASK_TD_SCDMA_GSM_LTE;
    static const Int32 NETWORK_MASK_TD_SCDMA_GSM_WCDMA;
    static const Int32 NETWORK_MASK_TD_SCDMA_WCDMA_LTE;
    static const Int32 NETWORK_MASK_TD_SCDMA_GSM_WCDMA_LTE;
    static const Int32 NETWORK_MASK_TD_SCDMA_CDMA_EVDO_GSM_WCDMA;
    static const Int32 NETWORK_MASK_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA;

    //***** Events
    static const Int32 EVENT_MODEM_RAT_CAPS_AVAILABLE;
    static const Int32 EVENT_UPDATE_BINDING_DONE;
    static const Int32 EVENT_SET_NW_MODE_DONE;

    //*****Constants
    static const Int32 SUCCESS;
    static const Int32 FAILURE;

    //***** Class Variables
    static AutoPtr<ModemBindingPolicyHandler> sModemBindingPolicyHandler;
    static AutoPtr<ModemStackController> mModemStackController;
    AutoPtr<ArrayOf<ICommandsInterface*> > mCi;
    AutoPtr<IContext> mContext;
    Int32 mNumOfSetPrefNwModeSuccess;
    Int32 mNumPhones;
    Boolean mModemRatCapabilitiesAvailable;
    Boolean mIsSetPrefNwModeInProgress;
    //Boolean mIsBindingInProgress = FALSE;
    AutoPtr<ArrayOf<Int32> > mPreferredStackId;
    AutoPtr<ArrayOf<Int32> > mCurrentStackId;
    AutoPtr<ArrayOf<Int32> > mPrefNwMode;
    AutoPtr<ArrayOf<Int32> > mNwModeinSubIdTable;
    HashMap<Int32, AutoPtr<IMessage> > mStoredResponse; // = new HashMap<Integer, Message>();

    //Modem capabilities as per StackId
    AutoPtr<ArrayOf<ModemStackController::ModemCapabilityInfo*> > mModemCapInfo;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_MODEMBINDINGPOLICYHANDLER_H__
