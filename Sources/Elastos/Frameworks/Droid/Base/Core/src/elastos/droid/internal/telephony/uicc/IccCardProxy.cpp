/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.uicc;

using static android::Manifest::Permission::IREAD_PHONE_STATE;

using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Internal::Telephony::IIccCardConstants;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IMccTable;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::IccCardConstants::IState;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManager;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::ISubscription;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IPersoSubState;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardStatus::ICardState;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardStatus::IPinState;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IRuimRecords;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_APN_RUIM_OPERATOR_NUMERIC;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_APN_SIM_OPERATOR_NUMERIC;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_ALPHA;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_NUMERIC;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_ICC_OPERATOR_ISO_COUNTRY;
using static com::Android::Internal::Telephony::TelephonyProperties::IPROPERTY_SIM_STATE;


/**
 * @Deprecated use {@link UiccController}.getUiccCard instead.
 *
 * The Phone App assumes that there is only one icc card, and one icc application
 * available at a time. Moreover, it assumes such Object (represented with IccCard)
 * is available all the Time (whether {@link RILConstants#RIL_REQUEST_GET_SIM_STATUS} returned
 * or not, whether card has desired application or not, whether there really is a card in the
 * slot or not).
 *
 * UiccController, however, can handle multiple instances of icc Objects (multiple
 * {@link UiccCardApplication}, multiple {@link IccFileHandler}, multiple {@link IccRecords})
 * created and destroyed dynamically during phone operation.
 *
 * This class implements the IccCard interface that is always Available (right after default
 * phone object is constructed) to expose the Current (based on voice radio technology)
 * application on the uicc card, so that external apps won't break.
 */

public class IccCardProxy extends Handler implements IccCard {
    private static const Boolean DBG = TRUE;
    private static const String LOG_TAG = "IccCardProxy";

    private static const Int32 EVENT_RADIO_OFF_OR_UNAVAILABLE = 1;
    private static const Int32 EVENT_RADIO_ON = 2;
    private static const Int32 EVENT_ICC_CHANGED = 3;
    private static const Int32 EVENT_ICC_ABSENT = 4;
    private static const Int32 EVENT_ICC_LOCKED = 5;
    private static const Int32 EVENT_APP_READY = 6;
    private static const Int32 EVENT_RECORDS_LOADED = 7;
    private static const Int32 EVENT_IMSI_READY = 8;
    private static const Int32 EVENT_PERSO_LOCKED = 9;
    private static const Int32 EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED = 11;

    private static const Int32 EVENT_ICC_RECORD_EVENTS = 500;
    private static const Int32 EVENT_SUBSCRIPTION_ACTIVATED = 501;
    private static const Int32 EVENT_SUBSCRIPTION_DEACTIVATED = 502;
    private static const Int32 EVENT_CARRIER_PRIVILIGES_LOADED = 503;

    // FIXME Rename mCardIndex to mSlotId.
    private Integer mCardIndex = NULL;
    private Subscription mSubscriptionData = NULL;

    private final Object mLock = new Object();
    private Context mContext;
    private CommandsInterface mCi;

    private RegistrantList mAbsentRegistrants = new RegistrantList();
    private RegistrantList mPinLockedRegistrants = new RegistrantList();
    private RegistrantList mPersoLockedRegistrants = new RegistrantList();

    private Int32 mCurrentAppType = UiccController.APP_FAM_3GPP; //default to 3gpp?
    private UiccController mUiccController = NULL;
    private UiccCard mUiccCard = NULL;
    private UiccCardApplication mUiccApplication = NULL;
    private IccRecords mIccRecords = NULL;
    private CdmaSubscriptionSourceManager mCdmaSSM = NULL;
    private Boolean mRadioOn = FALSE;
    private Boolean mQuietMode = FALSE; // when set to TRUE IccCardProxy will not broadcast
                                        // ACTION_SIM_STATE_CHANGED intents
    private Boolean mInitialized = FALSE;
    protected State mExternalState = State.UNKNOWN;
    private Boolean mIsCardStatusAvailable = FALSE;
    private PersoSubState mPersoSubState = PersoSubState.PERSOSUBSTATE_UNKNOWN;

    public IccCardProxy(Context context, CommandsInterface ci) {
        Log("Creating");
        mContext = context;
        mCi = ci;
        mCdmaSSM = CdmaSubscriptionSourceManager->GetInstance(context,
                ci, this, EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
        mUiccController = UiccController->GetInstance();
        mUiccController->RegisterForIccChanged(this, EVENT_ICC_CHANGED, NULL);
        ci->RegisterForOn(this,EVENT_RADIO_ON, NULL);
        ci->RegisterForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_UNAVAILABLE, NULL);
        SetExternalState(State.NOT_READY);
    }

    public IccCardProxy(Context context, CommandsInterface ci, Int32 cardIndex) {
        This(context, ci);

        mCardIndex = cardIndex;

        ResetProperties();
        SetExternalState(State.NOT_READY, FALSE);
    }

    CARAPI Dispose() {
        Synchronized (mLock) {
            Log("Disposing");
            //Cleanup icc references
            mUiccController->UnregisterForIccChanged(this);
            mUiccController = NULL;
            mCi->UnregisterForOn(this);
            mCi->UnregisterForOffOrNotAvailable(this);
            mCdmaSSM->Dispose(this);
        }
    }

    /*
     * The card application that the external world sees will be based on the
     * voice radio technology only!
     */
    CARAPI SetVoiceRadioTech(Int32 radioTech) {
        Synchronized (mLock) {
            If (DBG) {
                Log("Setting radio tech " + ServiceState->RilRadioTechnologyToString(radioTech));
            }
            If (ServiceState->IsGsm(radioTech)) {
                mCurrentAppType = UiccController.APP_FAM_3GPP;
            } else {
                mCurrentAppType = UiccController.APP_FAM_3GPP2;
            }
            UpdateQuietMode();
            UpdateActiveRecord();
        }
    }

    /**
     * This method sets the IccRecord, corresponding to the currently active
     * subscription, as the active record.
     */
    private void UpdateActiveRecord() {
        Log("updateActiveRecord app type = " + mCurrentAppType +
                "mIccRecords = " + mIccRecords);

        If (mIccRecords == NULL) {
            return;
        }

        If (mCurrentAppType == UiccController.APP_FAM_3GPP2) {
            Int32 newSubscriptionSource = mCdmaSSM->GetCdmaSubscriptionSource();
            // Allow RUIM to fetch in CDMA LTE mode if NV LTE mode.
            // Fixes cases where iccid could be unknown on some CDMA NV devices.
            If (newSubscriptionSource == CdmaSubscriptionSourceManager.SUBSCRIPTION_FROM_RUIM
                    || PhoneFactory->GetDefaultPhone()->GetLteOnCdmaMode()
                       == PhoneConstants.LTE_ON_CDMA_TRUE) {
                // Set this as the Active record.
                Log("Setting Ruim Record as active");
                mIccRecords->RecordsRequired();
            }
        } else If (mCurrentAppType == UiccController.APP_FAM_3GPP) {
            Log("Setting SIM Record as active");
            mIccRecords->RecordsRequired();
        }
    }

    /**
     * In case of 3gpp2 we need to find out if subscription used is coming from
     * NV in which case we shouldn't broadcast any sim states changes.
     */
    private void UpdateQuietMode() {
        Synchronized (mLock) {
            Boolean oldQuietMode = mQuietMode;
            Boolean newQuietMode;
            // "config_lte_capable" is set to TRUE when the device is
            // LTE capable
            Boolean isLteCapable = mContext->GetResources()->GetBoolean(
                    R.bool.config_lte_capable);
            Int32 cdmaSource = Phone.CDMA_SUBSCRIPTION_UNKNOWN;
            If (mCurrentAppType == UiccController.APP_FAM_3GPP) {
                newQuietMode = FALSE;
                If (DBG) Log("updateQuietMode: 3GPP subscription -> newQuietMode=" + newQuietMode);
            } else {
                cdmaSource = mCdmaSSM != NULL ?
                        mCdmaSSM->GetCdmaSubscriptionSource() : Phone.CDMA_SUBSCRIPTION_UNKNOWN;

                If (isLteCapable) {
                    // For a LTE capable device, always be out of Quiet Mode
                    newQuietMode = FALSE;
                } else {
                    newQuietMode = (cdmaSource == Phone.CDMA_SUBSCRIPTION_NV)
                            && (mCurrentAppType == UiccController.APP_FAM_3GPP2);
                }
                If (DBG) Log("updateQuietMode: 3GPP2 subscription -> newQuietMode=" + newQuietMode);
            }

            If (mQuietMode == FALSE && newQuietMode == TRUE) {
                // Last thing to do before switching to quiet mode is
                // broadcast ICC_READY
                Log("Switching to QuietMode.");
                SetExternalState(State.READY);
                mQuietMode = newQuietMode;
            } else If (mQuietMode == TRUE && newQuietMode == FALSE) {
                If (DBG) {
                    Log("updateQuietMode: Switching out from QuietMode."
                            + " Force broadcast of current state=" + mExternalState);
                }
                mQuietMode = newQuietMode;
                SetExternalState(mExternalState, TRUE);
            }
            If (DBG) {
                Log("updateQuietMode: QuietMode is " + mQuietMode + " (app_type="
                    + mCurrentAppType + " cdmaSource=" + cdmaSource + ")");
            }
            mInitialized = TRUE;
            //Send EVENT_ICC_CHANGED only if it is already received atleast once from RIL.
            If (mIsCardStatusAvailable) SendMessage(ObtainMessage(EVENT_ICC_CHANGED));
        }
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        Switch (msg.what) {
            case EVENT_RADIO_OFF_OR_UNAVAILABLE:
                mRadioOn = FALSE;
                If (CommandsInterface.RadioState.RADIO_UNAVAILABLE == mCi->GetRadioState()) {
                    SetExternalState(State.NOT_READY);
                }
                break;
            case EVENT_RADIO_ON:
                mRadioOn = TRUE;
                If (!mInitialized) {
                    UpdateQuietMode();
                }
                // When the radio is off, if EVENT_ICC_CHANGED is received, the
                // mExternalState will be updated. After the radio turns on, if
                // EVENT_ICC_CHANGED is not received, the mExternalState will not be
                // be restored. Therefore, updateExternalState when the radio turns on.
                UpdateExternalState();
                break;
            case EVENT_ICC_CHANGED:
                mIsCardStatusAvailable = TRUE;
                If (mInitialized) {
                    UpdateIccAvailability();
                }
                break;
            case EVENT_ICC_ABSENT:
                mAbsentRegistrants->NotifyRegistrants();
                SetExternalState(State.ABSENT);
                break;
            case EVENT_ICC_LOCKED:
                ProcessLockedState();
                break;
            case EVENT_APP_READY:
                SetExternalState(State.READY);
                break;
            case EVENT_RECORDS_LOADED:
                If (mIccRecords != NULL) {
                    String operator = mIccRecords->GetOperatorNumeric();
                    Int32 slotId = mCardIndex;

                    Log("operator = " + operator + " slotId = " + slotId);

                    If (operator != NULL) {
                        Log("update icc_operator_numeric=" + operator);
                        SetSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, slotId, operator);
                        If (mCurrentAppType == UiccController.APP_FAM_3GPP) {
                            SetSystemProperty(PROPERTY_APN_SIM_OPERATOR_NUMERIC,
                                     slotId, operator);
                            Log("update sim_operator_numeric=" + operator);
                        } else If (mCurrentAppType == UiccController.APP_FAM_3GPP2) {
                            SetSystemProperty(PROPERTY_APN_RUIM_OPERATOR_NUMERIC,
                                     slotId, operator);
                            Log("update ruim_operator_numeric=" + operator);
                        }
                        String countryCode = operator->Substring(0,3);
                        If (countryCode != NULL) {
                            SetSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY, slotId,
                                    MccTable->CountryCodeForMcc(Integer->ParseInt(countryCode)));
                        } else {
                            Loge("EVENT_RECORDS_LOADED Country code is NULL");
                        }

                        Int64[] subId = SubscriptionController->GetInstance()->GetSubId(slotId);
                        // Update MCC MNC device configuration information only for default sub.
                        If (subId[0] == SubscriptionController->GetInstance()->GetDefaultSubId()) {
                            Log("update mccmnc=" + operator + " config for default subscription.");
                            MccTable->UpdateMccMncConfiguration(mContext, operator, FALSE);
                        }
                        SubscriptionController->GetInstance()->SetMccMnc(operator, subId[0]);
                    } else {
                        Loge("EVENT_RECORDS_LOADED Operator name is NULL");
                    }
                }
                If (mUiccCard != NULL && !mUiccCard->AreCarrierPriviligeRulesLoaded()) {
                    mUiccCard->RegisterForCarrierPrivilegeRulesLoaded(
                        this, EVENT_CARRIER_PRIVILIGES_LOADED, NULL);
                } else {
                    OnRecordsLoaded();
                }
                break;
            case EVENT_IMSI_READY:
                BroadcastIccStateChangedIntent(IccCardConstants.INTENT_VALUE_ICC_IMSI, NULL);
                break;
            case EVENT_PERSO_LOCKED:
                mPersoSubState = mUiccApplication->GetPersoSubState();
                mPersoLockedRegistrants->NotifyRegistrants((AsyncResult)msg.obj);
                SetExternalState(State.PERSO_LOCKED);
                break;
            case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:
                UpdateQuietMode();
                UpdateActiveRecord();
                break;
            case EVENT_SUBSCRIPTION_ACTIVATED:
                Log("EVENT_SUBSCRIPTION_ACTIVATED");
                OnSubscriptionActivated();
                break;

            case EVENT_SUBSCRIPTION_DEACTIVATED:
                Log("EVENT_SUBSCRIPTION_DEACTIVATED");
                OnSubscriptionDeactivated();
                break;

            case EVENT_ICC_RECORD_EVENTS:
                If ((mCurrentAppType == UiccController.APP_FAM_3GPP) && (mIccRecords != NULL)) {
                    Int32 slotId = mCardIndex;
                    AsyncResult ar = (AsyncResult)msg.obj;
                    Int32 eventCode = (Integer) ar.result;
                    If (eventCode == SIMRecords.EVENT_SPN) {
                        SetSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, slotId,
                                mIccRecords->GetServiceProviderName());
                    }
                }
                break;

            case EVENT_CARRIER_PRIVILIGES_LOADED:
                Log("EVENT_CARRIER_PRIVILEGES_LOADED");
                If (mUiccCard != NULL) {
                    mUiccCard->UnregisterForCarrierPrivilegeRulesLoaded(this);
                }
                OnRecordsLoaded();
                break;

            default:
                Loge("Unhandled message with number: " + msg.what);
                break;
        }
    }

    private void OnSubscriptionActivated() {
        //mSubscriptionData = SubscriptionManager->GetCurrentSubscription(mCardIndex);

        UpdateIccAvailability();
        UpdateStateProperty();
    }

    private void OnSubscriptionDeactivated() {
        ResetProperties();
        mSubscriptionData = NULL;
        UpdateIccAvailability();
        UpdateStateProperty();
    }

    private void OnRecordsLoaded() {
        BroadcastIccStateChangedIntent(IccCardConstants.INTENT_VALUE_ICC_LOADED, NULL);
    }

    private void UpdateIccAvailability() {
        Synchronized (mLock) {
            UiccCard newCard = mUiccController->GetUiccCard(mCardIndex);
            CardState state = CardState.CARDSTATE_ABSENT;
            UiccCardApplication newApp = NULL;
            IccRecords newRecords = NULL;
            If (newCard != NULL) {
                state = newCard->GetCardState();
                newApp = newCard->GetApplication(mCurrentAppType);
                If (newApp != NULL) {
                    newRecords = newApp->GetIccRecords();
                }
            }

            If (mIccRecords != newRecords || mUiccApplication != newApp || mUiccCard != newCard) {
                If (DBG) Log("Icc changed. Reregestering.");
                UnregisterUiccCardEvents();
                mUiccCard = newCard;
                mUiccApplication = newApp;
                mIccRecords = newRecords;
                RegisterUiccCardEvents();
                UpdateActiveRecord();
            }
            UpdateExternalState();
        }
    }

    void ResetProperties() {
        If (mCurrentAppType == UiccController.APP_FAM_3GPP) {
            Log("update icc_operator_numeric=" + "");
            SetSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, mCardIndex, "");
            SetSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY, mCardIndex, "");
            SetSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, mCardIndex, "");
         }
    }

    private void UpdateExternalState() {

        // mUiccCard could be NULL at bootup, before valid card states have
        // been received from UiccController.
        If (mUiccCard == NULL) {
            SetExternalState(State.NOT_READY);
            return;
        }

        If (mUiccCard->GetCardState() == CardState.CARDSTATE_ABSENT) {
            If (mRadioOn) {
                SetExternalState(State.ABSENT);
            } else {
                SetExternalState(State.NOT_READY);
            }
            return;
        }

        If (mUiccCard->GetCardState() == CardState.CARDSTATE_ERROR) {
            SetExternalState(State.CARD_IO_ERROR);
            return;
        }

        If (mUiccApplication == NULL) {
            SetExternalState(State.NOT_READY);
            return;
        }

        Switch (mUiccApplication->GetState()) {
            case APPSTATE_UNKNOWN:
                SetExternalState(State.UNKNOWN);
                break;
            case APPSTATE_PIN:
                SetExternalState(State.PIN_REQUIRED);
                break;
            case APPSTATE_PUK:
                SetExternalState(State.PUK_REQUIRED);
                break;
            case APPSTATE_SUBSCRIPTION_PERSO:
                If (mUiccApplication->IsPersoLocked()) {
                    mPersoSubState = mUiccApplication->GetPersoSubState();
                    SetExternalState(State.PERSO_LOCKED);
                } else {
                    SetExternalState(State.UNKNOWN);
                }
                break;
            case APPSTATE_READY:
                SetExternalState(State.READY);
                break;
        }
    }

    private void RegisterUiccCardEvents() {
        If (mUiccCard != NULL) {
            mUiccCard->RegisterForAbsent(this, EVENT_ICC_ABSENT, NULL);
        }
        If (mUiccApplication != NULL) {
            mUiccApplication->RegisterForReady(this, EVENT_APP_READY, NULL);
            mUiccApplication->RegisterForLocked(this, EVENT_ICC_LOCKED, NULL);
            mUiccApplication->RegisterForPersoLocked(this, EVENT_PERSO_LOCKED, NULL);
        }
        If (mIccRecords != NULL) {
            mIccRecords->RegisterForImsiReady(this, EVENT_IMSI_READY, NULL);
            mIccRecords->RegisterForRecordsLoaded(this, EVENT_RECORDS_LOADED, NULL);
            mIccRecords->RegisterForRecordsEvents(this, EVENT_ICC_RECORD_EVENTS, NULL);
        }
    }

    private void UnregisterUiccCardEvents() {
        If (mUiccCard != NULL) mUiccCard->UnregisterForAbsent(this);
        If (mUiccApplication != NULL) mUiccApplication->UnregisterForReady(this);
        If (mUiccApplication != NULL) mUiccApplication->UnregisterForLocked(this);
        If (mUiccApplication != NULL) mUiccApplication->UnregisterForPersoLocked(this);
        If (mIccRecords != NULL) mIccRecords->UnregisterForImsiReady(this);
        If (mIccRecords != NULL) mIccRecords->UnregisterForRecordsLoaded(this);
        If (mIccRecords != NULL) mIccRecords->UnregisterForRecordsEvents(this);
    }

    private void UpdateStateProperty() {
        SetSystemProperty(PROPERTY_SIM_STATE, mCardIndex,GetState()->ToString());
    }

    private void BroadcastIccStateChangedIntent(String value, String reason) {
        Synchronized (mLock) {
            If (mCardIndex == NULL) {
                Loge("broadcastIccStateChangedIntent: Card Index is not set; Return!!");
                return;
            }

            If (mQuietMode) {
                Log("QuietMode: NOT Broadcasting intent ACTION_SIM_STATE_CHANGED " +  value
                        + " reason " + reason);
                return;
            }

            Intent intent = new Intent(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
            intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
            intent->PutExtra(PhoneConstants.PHONE_NAME_KEY, "Phone");
            intent->PutExtra(IccCardConstants.INTENT_KEY_ICC_STATE, value);
            intent->PutExtra(IccCardConstants.INTENT_KEY_LOCKED_REASON, reason);
            SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, mCardIndex);
            Log("Broadcasting intent ACTION_SIM_STATE_CHANGED " +  value
                + " reason " + reason + " for mCardIndex : " + mCardIndex);
            ActivityManagerNative->BroadcastStickyIntent(intent, READ_PHONE_STATE,
                    UserHandle.USER_ALL);
        }
    }

    private void SetExternalState(State newState, Boolean override) {
        Synchronized (mLock) {
            If (mCardIndex == NULL) {
                Loge("setExternalState: Card Index is not set; Return!!");
                return;
            }

            If (!override && newState == mExternalState) {
                return;
            }
            mExternalState = newState;
            SetSystemProperty(PROPERTY_SIM_STATE, mCardIndex, GetState()->ToString());
            BroadcastIccStateChangedIntent(GetIccStateIntentString(mExternalState),
                    GetIccStateReason(mExternalState));
            // TODO: Need to notify registrants for other states as well.
            If ( State.ABSENT == mExternalState) {
                mAbsentRegistrants->NotifyRegistrants();
            }
        }
    }

    private void ProcessLockedState() {
        Synchronized (mLock) {
            If (mUiccApplication == NULL) {
                //Don't need to do anything if non-existent application is locked
                return;
            }
            PinState pin1State = mUiccApplication->GetPin1State();
            If (pin1State == PinState.PINSTATE_ENABLED_PERM_BLOCKED) {
                SetExternalState(State.PERM_DISABLED);
                return;
            }

            AppState appState = mUiccApplication->GetState();
            Switch (appState) {
                case APPSTATE_PIN:
                    mPinLockedRegistrants->NotifyRegistrants();
                    SetExternalState(State.PIN_REQUIRED);
                    break;
                case APPSTATE_PUK:
                    SetExternalState(State.PUK_REQUIRED);
                    break;
                case APPSTATE_DETECTED:
                case APPSTATE_READY:
                case APPSTATE_SUBSCRIPTION_PERSO:
                case APPSTATE_UNKNOWN:
                    // Neither required
                    break;
            }
        }
    }

    private void SetExternalState(State newState) {
        SetExternalState(newState, FALSE);
    }

    public Boolean GetIccRecordsLoaded() {
        Synchronized (mLock) {
            If (mIccRecords != NULL) {
                return mIccRecords->GetRecordsLoaded();
            }
            return FALSE;
        }
    }

    private String GetIccStateIntentString(State state) {
        Switch (state) {
            case ABSENT: return IccCardConstants.INTENT_VALUE_ICC_ABSENT;
            case PIN_REQUIRED: return IccCardConstants.INTENT_VALUE_ICC_LOCKED;
            case PUK_REQUIRED: return IccCardConstants.INTENT_VALUE_ICC_LOCKED;
            case PERSO_LOCKED: return IccCardConstants.INTENT_VALUE_ICC_LOCKED;
            case READY: return IccCardConstants.INTENT_VALUE_ICC_READY;
            case NOT_READY: return IccCardConstants.INTENT_VALUE_ICC_NOT_READY;
            case PERM_DISABLED: return IccCardConstants.INTENT_VALUE_ICC_LOCKED;
            case CARD_IO_ERROR: return IccCardConstants.INTENT_VALUE_ICC_CARD_IO_ERROR;
            default: return IccCardConstants.INTENT_VALUE_ICC_UNKNOWN;
        }
    }

    /**
     * Locked state have a Reason (PIN, PUK, NETWORK, PERM_DISABLED, CARD_IO_ERROR)
     * @return reason
     */
    private String GetIccStateReason(State state) {
        Switch (state) {
            case PIN_REQUIRED: return IccCardConstants.INTENT_VALUE_LOCKED_ON_PIN;
            case PUK_REQUIRED: return IccCardConstants.INTENT_VALUE_LOCKED_ON_PUK;
            case PERSO_LOCKED: return IccCardConstants.INTENT_VALUE_LOCKED_PERSO;
            case PERM_DISABLED: return IccCardConstants.INTENT_VALUE_ABSENT_ON_PERM_DISABLED;
            case CARD_IO_ERROR: return IccCardConstants.INTENT_VALUE_ICC_CARD_IO_ERROR;
            default: return NULL;
       }
    }

    /* IccCard interface implementation */
    //@Override
    public State GetState() {
        Synchronized (mLock) {
            return mExternalState;
        }
    }

    //@Override
    public IccRecords GetIccRecords() {
        Synchronized (mLock) {
            return mIccRecords;
        }
    }

    //@Override
    public IccFileHandler GetIccFileHandler() {
        Synchronized (mLock) {
            If (mUiccApplication != NULL) {
                return mUiccApplication->GetIccFileHandler();
            }
            return NULL;
        }
    }

    /**
     * Notifies handler of any transition into State.ABSENT
     */
    //@Override
    CARAPI RegisterForAbsent(Handler h, Int32 what, Object obj) {
        Synchronized (mLock) {
            Registrant r = new Registrant (h, what, obj);

            mAbsentRegistrants->Add(r);

            If (GetState() == State.ABSENT) {
                r->NotifyRegistrant();
            }
        }
    }

    //@Override
    CARAPI UnregisterForAbsent(Handler h) {
        Synchronized (mLock) {
            mAbsentRegistrants->Remove(h);
        }
    }

    /**
     * Notifies handler of any transition into State.PERSO_LOCKED
     */
    //@Override
    CARAPI RegisterForPersoLocked(Handler h, Int32 what, Object obj) {
        Synchronized (mLock) {
            Registrant r = new Registrant (h, what, obj);

            mPersoLockedRegistrants->Add(r);

            If (GetState() == State.PERSO_LOCKED) {
                r->NotifyRegistrant(new AsyncResult(NULL, mPersoSubState->Ordinal(), NULL));
            }
        }
    }

    //@Override
    CARAPI UnregisterForPersoLocked(Handler h) {
        Synchronized (mLock) {
            mPersoLockedRegistrants->Remove(h);
        }
    }

    /**
     * Notifies handler of any transition into State->IsPinLocked()
     */
    //@Override
    CARAPI RegisterForLocked(Handler h, Int32 what, Object obj) {
        Synchronized (mLock) {
            Registrant r = new Registrant (h, what, obj);

            mPinLockedRegistrants->Add(r);

            If (GetState()->IsPinLocked()) {
                r->NotifyRegistrant();
            }
        }
    }

    //@Override
    CARAPI UnregisterForLocked(Handler h) {
        Synchronized (mLock) {
            mPinLockedRegistrants->Remove(h);
        }
    }

    //@Override
    CARAPI SupplyPin(String pin, Message onComplete) {
        Synchronized (mLock) {
            If (mUiccApplication != NULL) {
                mUiccApplication->SupplyPin(pin, onComplete);
            } else If (onComplete != NULL) {
                Exception e = new RuntimeException("ICC card is absent.");
                AsyncResult->ForMessage(onComplete).exception = e;
                onComplete->SendToTarget();
                return;
            }
        }
    }

    //@Override
    CARAPI SupplyPuk(String puk, String newPin, Message onComplete) {
        Synchronized (mLock) {
            If (mUiccApplication != NULL) {
                mUiccApplication->SupplyPuk(puk, newPin, onComplete);
            } else If (onComplete != NULL) {
                Exception e = new RuntimeException("ICC card is absent.");
                AsyncResult->ForMessage(onComplete).exception = e;
                onComplete->SendToTarget();
                return;
            }
        }
    }

    //@Override
    CARAPI SupplyPin2(String pin2, Message onComplete) {
        Synchronized (mLock) {
            If (mUiccApplication != NULL) {
                mUiccApplication->SupplyPin2(pin2, onComplete);
            } else If (onComplete != NULL) {
                Exception e = new RuntimeException("ICC card is absent.");
                AsyncResult->ForMessage(onComplete).exception = e;
                onComplete->SendToTarget();
                return;
            }
        }
    }

    //@Override
    CARAPI SupplyPuk2(String puk2, String newPin2, Message onComplete) {
        Synchronized (mLock) {
            If (mUiccApplication != NULL) {
                mUiccApplication->SupplyPuk2(puk2, newPin2, onComplete);
            } else If (onComplete != NULL) {
                Exception e = new RuntimeException("ICC card is absent.");
                AsyncResult->ForMessage(onComplete).exception = e;
                onComplete->SendToTarget();
                return;
            }
        }
    }

    //@Override
    CARAPI SupplyDepersonalization(String pin, String type, Message onComplete) {
        Synchronized (mLock) {
            If (mUiccApplication != NULL) {
                mUiccApplication->SupplyDepersonalization(pin, type, onComplete);
            } else If (onComplete != NULL) {
                Exception e = new RuntimeException("CommandsInterface is not set.");
                AsyncResult->ForMessage(onComplete).exception = e;
                onComplete->SendToTarget();
                return;
            }
        }
    }

    //@Override
    public Boolean GetIccLockEnabled() {
        Synchronized (mLock) {
            /* defaults to FALSE, if ICC is absent/deactivated */
            Boolean retValue = mUiccApplication != NULL ?
                    mUiccApplication->GetIccLockEnabled() : FALSE;
            return retValue;
        }
    }

    //@Override
    public Boolean GetIccFdnEnabled() {
        Synchronized (mLock) {
            Boolean retValue = mUiccApplication != NULL ?
                    mUiccApplication->GetIccFdnEnabled() : FALSE;
            return retValue;
        }
    }

    public Boolean GetIccFdnAvailable() {
        Boolean retValue = mUiccApplication != NULL ? mUiccApplication->GetIccFdnAvailable() : FALSE;
        return retValue;
    }

    public Boolean GetIccPin2Blocked() {
        /* defaults to disabled */
        Boolean retValue = mUiccApplication != NULL ? mUiccApplication->GetIccPin2Blocked() : FALSE;
        return retValue;
    }

    public Boolean GetIccPuk2Blocked() {
        /* defaults to disabled */
        Boolean retValue = mUiccApplication != NULL ? mUiccApplication->GetIccPuk2Blocked() : FALSE;
        return retValue;
    }

    //@Override
    CARAPI SetIccLockEnabled(Boolean enabled, String password, Message onComplete) {
        Synchronized (mLock) {
            If (mUiccApplication != NULL) {
                mUiccApplication->SetIccLockEnabled(enabled, password, onComplete);
            } else If (onComplete != NULL) {
                Exception e = new RuntimeException("ICC card is absent.");
                AsyncResult->ForMessage(onComplete).exception = e;
                onComplete->SendToTarget();
                return;
            }
        }
    }

    //@Override
    CARAPI SetIccFdnEnabled(Boolean enabled, String password, Message onComplete) {
        Synchronized (mLock) {
            If (mUiccApplication != NULL) {
                mUiccApplication->SetIccFdnEnabled(enabled, password, onComplete);
            } else If (onComplete != NULL) {
                Exception e = new RuntimeException("ICC card is absent.");
                AsyncResult->ForMessage(onComplete).exception = e;
                onComplete->SendToTarget();
                return;
            }
        }
    }

    //@Override
    CARAPI ChangeIccLockPassword(String oldPassword, String newPassword, Message onComplete) {
        Synchronized (mLock) {
            If (mUiccApplication != NULL) {
                mUiccApplication->ChangeIccLockPassword(oldPassword, newPassword, onComplete);
            } else If (onComplete != NULL) {
                Exception e = new RuntimeException("ICC card is absent.");
                AsyncResult->ForMessage(onComplete).exception = e;
                onComplete->SendToTarget();
                return;
            }
        }
    }

    //@Override
    CARAPI ChangeIccFdnPassword(String oldPassword, String newPassword, Message onComplete) {
        Synchronized (mLock) {
            If (mUiccApplication != NULL) {
                mUiccApplication->ChangeIccFdnPassword(oldPassword, newPassword, onComplete);
            } else If (onComplete != NULL) {
                Exception e = new RuntimeException("ICC card is absent.");
                AsyncResult->ForMessage(onComplete).exception = e;
                onComplete->SendToTarget();
                return;
            }
        }
    }

    //@Override
    public String GetServiceProviderName() {
        Synchronized (mLock) {
            If (mIccRecords != NULL) {
                return mIccRecords->GetServiceProviderName();
            }
            return NULL;
        }
    }

    //@Override
    public Boolean IsApplicationOnIcc(IccCardApplicationStatus.AppType type) {
        Synchronized (mLock) {
            Boolean retValue = mUiccCard != NULL ? mUiccCard->IsApplicationOnIcc(type) : FALSE;
            return retValue;
        }
    }

    //@Override
    public Boolean HasIccCard() {
        Synchronized (mLock) {
            If (mUiccCard != NULL && mUiccCard->GetCardState() != CardState.CARDSTATE_ABSENT) {
                return TRUE;
            }
            return FALSE;
        }
    }

    private void SetSystemProperty(String property, Int32 slotId, String value) {
        Int64[] subId = SubscriptionController->GetInstance()->GetSubId(slotId);
        TelephonyManager->SetTelephonyProperty(property, subId[0], value);
    }

    private void Log(String s) {
        Rlog->D(LOG_TAG, s);
    }

    private void Loge(String msg) {
        Rlog->E(LOG_TAG, msg);
    }

    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("IccCardProxy: " + this);
        pw->Println(" mContext=" + mContext);
        pw->Println(" mCi=" + mCi);
        pw->Println(" mAbsentRegistrants: size=" + mAbsentRegistrants->Size());
        For (Int32 i = 0; i < mAbsentRegistrants->Size(); i++) {
            pw->Println("  mAbsentRegistrants[" + i + "]="
                    + ((Registrant)mAbsentRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mPinLockedRegistrants: size=" + mPinLockedRegistrants->Size());
        For (Int32 i = 0; i < mPinLockedRegistrants->Size(); i++) {
            pw->Println("  mPinLockedRegistrants[" + i + "]="
                    + ((Registrant)mPinLockedRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mPersoLockedRegistrants: size=" + mPersoLockedRegistrants->Size());
        For (Int32 i = 0; i < mPersoLockedRegistrants->Size(); i++) {
            pw->Println("  mPersoLockedRegistrants[" + i + "]="
                    + ((Registrant)mPersoLockedRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mCurrentAppType=" + mCurrentAppType);
        pw->Println(" mUiccController=" + mUiccController);
        pw->Println(" mUiccCard=" + mUiccCard);
        pw->Println(" mUiccApplication=" + mUiccApplication);
        pw->Println(" mIccRecords=" + mIccRecords);
        pw->Println(" mCdmaSSM=" + mCdmaSSM);
        pw->Println(" mRadioOn=" + mRadioOn);
        pw->Println(" mQuietMode=" + mQuietMode);
        pw->Println(" mInitialized=" + mInitialized);
        pw->Println(" mExternalState=" + mExternalState);

        pw->Flush();
    }
}
