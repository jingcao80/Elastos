/*
 * Copyright (C) 2006, 2012 The Android Open Source Project
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
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IWindowManager;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::CommandsInterface::IRadioState;
using Elastos::Droid::Internal::Telephony::IccCardConstants::IState;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppType;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardStatus::ICardState;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardStatus::IPinState;
using Elastos::Droid::Internal::Telephony::Cat::ICatService;
using Elastos::Droid::Internal::Telephony::Cat::ICatServiceFactory;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMALTEPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManager;

using Elastos::Droid::Os::ISystemProperties;

using Elastos::Droid::Internal::IR;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;

/**
 * {@hide}
 */
public class UiccCard {
    protected static const String LOG_TAG = "UiccCard";
    protected static const Boolean DBG = TRUE;

    private static const String OPERATOR_BRAND_OVERRIDE_PREFIX = "operator_branding_";

    private final Object mLock = new Object();
    private CardState mCardState;
    private PinState mUniversalPinState;
    private Int32 mGsmUmtsSubscriptionAppIndex;
    private Int32 mCdmaSubscriptionAppIndex;
    private Int32 mImsSubscriptionAppIndex;
    private UiccCardApplication[] mUiccApplications =
            new UiccCardApplication[IccCardStatus.CARD_MAX_APPS];
    private Context mContext;
    private CommandsInterface mCi;
    private CatService mCatService;
    private Boolean mDestroyed = FALSE; //set to TRUE once this card is commanded to be disposed of.
    private RadioState mLastRadioState =  RadioState.RADIO_UNAVAILABLE;
    private UiccCarrierPrivilegeRules mCarrierPrivilegeRules;
    private UICCConfig mUICCConfig = NULL;

    private RegistrantList mAbsentRegistrants = new RegistrantList();
    private RegistrantList mCarrierPrivilegeRegistrants = new RegistrantList();

    private static const Int32 EVENT_CARD_REMOVED = 13;
    private static const Int32 EVENT_CARD_ADDED = 14;
    private static const Int32 EVENT_OPEN_LOGICAL_CHANNEL_DONE = 15;
    private static const Int32 EVENT_CLOSE_LOGICAL_CHANNEL_DONE = 16;
    private static const Int32 EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE = 17;
    private static const Int32 EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE = 18;
    private static const Int32 EVENT_SIM_IO_DONE = 19;
    private static const Int32 EVENT_CARRIER_PRIVILIGES_LOADED = 20;
    private static const Int32 EVENT_SIM_GET_ATR_DONE = 21;

    Int32 mSlotId;

    public UiccCard(Context c, CommandsInterface ci, IccCardStatus ics, Int32 slotId) {
        mCardState = ics.mCardState;
        mSlotId = slotId;
        Update(c, ci, ics);
    }

    protected UiccCard() {
    }

    CARAPI Dispose() {
        Synchronized (mLock) {
            If (DBG) Log("Disposing card");
            If (mCatService != NULL) CatServiceFactory->DisposeCatService(mSlotId);
            For (UiccCardApplication app : mUiccApplications) {
                If (app != NULL) {
                    app->Dispose();
                }
            }
            mCatService = NULL;
            mUiccApplications = NULL;
            mCarrierPrivilegeRules = NULL;
            mUICCConfig = NULL;
        }
    }

    CARAPI Update(Context c, CommandsInterface ci, IccCardStatus ics) {
        Synchronized (mLock) {
            If (mDestroyed) {
                Loge("Updated after destroyed! Fix me!");
                return;
            }
            CardState oldState = mCardState;
            mCardState = ics.mCardState;
            mUniversalPinState = ics.mUniversalPinState;
            mGsmUmtsSubscriptionAppIndex = ics.mGsmUmtsSubscriptionAppIndex;
            mCdmaSubscriptionAppIndex = ics.mCdmaSubscriptionAppIndex;
            mImsSubscriptionAppIndex = ics.mImsSubscriptionAppIndex;
            mContext = c;
            mCi = ci;
            //update applications
            If (mUICCConfig == NULL)
                mUICCConfig = new UICCConfig();
            If (DBG) Log(ics.mApplications.length + " applications");
            For ( Int32 i = 0; i < mUiccApplications.length; i++) {
                If (mUiccApplications[i] == NULL) {
                    //Create newly added Applications
                    If (i < ics.mApplications.length) {
                        mUiccApplications[i] = new UiccCardApplication(this,
                                ics.mApplications[i], mContext, mCi);
                    }
                } else If (i >= ics.mApplications.length) {
                    //Delete removed applications
                    mUiccApplications[i].Dispose();
                    mUiccApplications[i] = NULL;
                } else {
                    //Update the rest
                    mUiccApplications[i].Update(ics.mApplications[i], mContext, mCi);
                }
            }

            CreateAndUpdateCatService();

            // Reload the carrier privilege rules if necessary.
            Log("Before privilege rules: " + mCarrierPrivilegeRules + " : " + mCardState);
            If (mCarrierPrivilegeRules == NULL && mCardState == CardState.CARDSTATE_PRESENT) {
                mCarrierPrivilegeRules = new UiccCarrierPrivilegeRules(this,
                        mHandler->ObtainMessage(EVENT_CARRIER_PRIVILIGES_LOADED));
            } else If (mCarrierPrivilegeRules != NULL && mCardState != CardState.CARDSTATE_PRESENT) {
                mCarrierPrivilegeRules = NULL;
            }

            SanitizeApplicationIndexes();

            RadioState radioState = mCi->GetRadioState();
            If (DBG) Log("update: radioState=" + radioState + " mLastRadioState="
                    + mLastRadioState);
            // No notifications while radio is off or we just powering up
            If (radioState == RadioState.RADIO_ON && mLastRadioState == RadioState.RADIO_ON) {
                If (oldState != CardState.CARDSTATE_ABSENT &&
                        mCardState == CardState.CARDSTATE_ABSENT) {
                    If (DBG) Log("update: notify card removed");
                    mAbsentRegistrants->NotifyRegistrants();
                    mHandler->SendMessage(mHandler->ObtainMessage(EVENT_CARD_REMOVED, NULL));
                } else If (oldState == CardState.CARDSTATE_ABSENT &&
                        mCardState != CardState.CARDSTATE_ABSENT) {
                    If (DBG) Log("update: notify card added");
                    mHandler->SendMessage(mHandler->ObtainMessage(EVENT_CARD_ADDED, NULL));
                }
            }
            mLastRadioState = radioState;
        }
    }

    protected void CreateAndUpdateCatService() {
        If (mUiccApplications.length > 0 && mUiccApplications[0] != NULL) {
            // Initialize or Reinitialize CatService
            If (mCatService == NULL) {
                mCatService = CatServiceFactory->MakeCatService(mCi, mContext, this, mSlotId);
            }
        } else {
            If (mCatService != NULL) {
                CatServiceFactory->DisposeCatService(mSlotId);
            }
            mCatService = NULL;
        }
    }

    public CatService GetCatService() {
        return mCatService;
    }

    //@Override
    protected void Finalize() {
        If (DBG) Log("UiccCard finalized");
    }

    /**
     * This function makes sure that application indexes are valid
     * and resets invalid indexes. (This should never happen, but in case
     * RIL misbehaves we need to manage situation gracefully)
     */
    private void SanitizeApplicationIndexes() {
        mGsmUmtsSubscriptionAppIndex =
                CheckIndex(mGsmUmtsSubscriptionAppIndex, AppType.APPTYPE_SIM, AppType.APPTYPE_USIM);
        mCdmaSubscriptionAppIndex =
                CheckIndex(mCdmaSubscriptionAppIndex, AppType.APPTYPE_RUIM, AppType.APPTYPE_CSIM);
        mImsSubscriptionAppIndex =
                CheckIndex(mImsSubscriptionAppIndex, AppType.APPTYPE_ISIM, NULL);
    }

    private Int32 CheckIndex(Int32 index, AppType expectedAppType, AppType altExpectedAppType) {
        If (mUiccApplications == NULL || index >= mUiccApplications.length) {
            Loge("App index " + index + " is invalid since there are no applications");
            return -1;
        }

        If (index < 0) {
            // This is normal. (i.e. no application of this type)
            return -1;
        }

        If (mUiccApplications[index].GetType() != expectedAppType &&
            mUiccApplications[index].GetType() != altExpectedAppType) {
            Loge("App index " + index + " is invalid since it's not " +
                    expectedAppType + " and not " + altExpectedAppType);
            return -1;
        }

        // Seems to be valid
        return index;
    }

    /**
     * Notifies handler of any transition into State.ABSENT
     */
    CARAPI RegisterForAbsent(Handler h, Int32 what, Object obj) {
        Synchronized (mLock) {
            Registrant r = new Registrant (h, what, obj);

            mAbsentRegistrants->Add(r);

            If (mCardState == CardState.CARDSTATE_ABSENT) {
                r->NotifyRegistrant();
            }
        }
    }

    CARAPI UnregisterForAbsent(Handler h) {
        Synchronized (mLock) {
            mAbsentRegistrants->Remove(h);
        }
    }

    /**
     * Notifies handler when carrier privilege rules are loaded.
     */
    CARAPI RegisterForCarrierPrivilegeRulesLoaded(Handler h, Int32 what, Object obj) {
        Synchronized (mLock) {
            Registrant r = new Registrant (h, what, obj);

            mCarrierPrivilegeRegistrants->Add(r);

            If (AreCarrierPriviligeRulesLoaded()) {
                r->NotifyRegistrant();
            }
        }
    }

    CARAPI UnregisterForCarrierPrivilegeRulesLoaded(Handler h) {
        Synchronized (mLock) {
            mCarrierPrivilegeRegistrants->Remove(h);
        }
    }

    private void OnIccSwap(Boolean isAdded) {

        Boolean isHotSwapSupported = mContext->GetResources()->GetBoolean(
                R.bool.config_hotswapCapable);

        If (isHotSwapSupported) {
            Log("onIccSwap: isHotSwapSupported is TRUE, don't prompt for rebooting");
            return;
        }
        Log("onIccSwap: isHotSwapSupported is FALSE, prompt for rebooting");

        Synchronized (mLock) {
            // TODO: Here we assume the device can't handle SIM hot-swap
            //      and has to reboot. We may want to add a property,
            //      e.g. REBOOT_ON_SIM_SWAP, to indicate if modem support
            //      hot-swap.
            DialogInterface.OnClickListener listener = NULL;


            // TODO: SimRecords is not reset while SIM ABSENT (only reset while
            //       Radio_off_or_not_available). Have to reset in both both
            //       added or removed situation.
            listener = new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    Synchronized (mLock) {
                        If (which == DialogInterface.BUTTON_POSITIVE) {
                            If (DBG) Log("Reboot due to SIM swap");
                            PowerManager pm = (PowerManager) mContext
                                    .GetSystemService(Context.POWER_SERVICE);
                            pm->Reboot("SIM is added.");
                        }
                    }
                }

            };

            Resources r = Resources->GetSystem();

            String title = (isAdded) ? r->GetString(R::string::sim_added_title) :
                r->GetString(R::string::sim_removed_title);
            String message = (isAdded) ? r->GetString(R::string::sim_added_message) :
                r->GetString(R::string::sim_removed_message);
            String buttonTxt = r->GetString(R::string::sim_restart_button);

            AlertDialog dialog = new AlertDialog->Builder(mContext)
            .SetTitle(title)
            .SetMessage(message)
            .SetPositiveButton(buttonTxt, listener)
            .Create();
            dialog->GetWindow()->SetType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
            dialog->Show();
        }
    }

    protected Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg){
            If (mDestroyed) {
                Loge("Received message " + msg + "[" + msg.what
                        + "] while being destroyed. Ignoring.");
                return;
            }

            Switch (msg.what) {
                case EVENT_CARD_REMOVED:
                    OnIccSwap(FALSE);
                    break;
                case EVENT_CARD_ADDED:
                    OnIccSwap(TRUE);
                    break;
                case EVENT_OPEN_LOGICAL_CHANNEL_DONE:
                case EVENT_CLOSE_LOGICAL_CHANNEL_DONE:
                case EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE:
                case EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE:
                case EVENT_SIM_IO_DONE:
                case EVENT_SIM_GET_ATR_DONE:
                    AsyncResult ar = (AsyncResult)msg.obj;
                    If (ar.exception != NULL) {
                       If (DBG)
                         Log("Error in SIM access with exception" + ar.exception);
                    }
                    AsyncResult->ForMessage((Message)ar.userObj, ar.result, ar.exception);
                    ((Message)ar.userObj).SendToTarget();
                    break;
                case EVENT_CARRIER_PRIVILIGES_LOADED:
                    OnCarrierPriviligesLoadedMessage();
                    break;
                default:
                    Loge("Unknown Event " + msg.what);
            }
        }
    };

    private void OnCarrierPriviligesLoadedMessage() {
        Synchronized (mLock) {
            mCarrierPrivilegeRegistrants->NotifyRegistrants();
        }
    }

    public Boolean IsApplicationOnIcc(IccCardApplicationStatus.AppType type) {
        Synchronized (mLock) {
            For (Int32 i = 0 ; i < mUiccApplications.length; i++) {
                If (mUiccApplications[i] != NULL && mUiccApplications[i].GetType() == type) {
                    return TRUE;
                }
            }
            return FALSE;
        }
    }

    public CardState GetCardState() {
        Synchronized (mLock) {
            return mCardState;
        }
    }

    public PinState GetUniversalPinState() {
        Synchronized (mLock) {
            return mUniversalPinState;
        }
    }

    public Int32 GetSlotId() {
        Synchronized (mLock) {
            return mSlotId;
        }
    }

    public UiccCardApplication GetApplication(Int32 family) {
        Synchronized (mLock) {
            Int32 index = IccCardStatus.CARD_MAX_APPS;
            Switch (family) {
                case UiccController.APP_FAM_3GPP:
                    index = mGsmUmtsSubscriptionAppIndex;
                    break;
                case UiccController.APP_FAM_3GPP2:
                    index = mCdmaSubscriptionAppIndex;
                    break;
                case UiccController.APP_FAM_IMS:
                    index = mImsSubscriptionAppIndex;
                    break;
            }
            If (index >= 0 && index < mUiccApplications.length) {
                return mUiccApplications[index];
            }
            return NULL;
        }
    }

    public UiccCardApplication GetApplicationIndex(Int32 index) {
        Synchronized (mLock) {
            If (index >= 0 && index < mUiccApplications.length) {
                return mUiccApplications[index];
            }
            return NULL;
        }
    }

    /**
     * Returns the SIM application of the specified type.
     *
     * @param type ICC application Type (@see com.android.internal.telephony.PhoneConstants#APPTYPE_xxx)
     * @return application corresponding to type or a NULL if no match found
     */
    public UiccCardApplication GetApplicationByType(Int32 type) {
        Synchronized (mLock) {
            For (Int32 i = 0 ; i < mUiccApplications.length; i++) {
                If (mUiccApplications[i] != NULL &&
                        mUiccApplications[i].GetType()->Ordinal() == type) {
                    return mUiccApplications[i];
                }
            }
            return NULL;
        }
    }

    /**
     * Exposes {@link CommandsInterface.iccOpenLogicalChannel}
     */
    CARAPI IccOpenLogicalChannel(String AID, Message response) {
        mCi->IccOpenLogicalChannel(AID,
                mHandler->ObtainMessage(EVENT_OPEN_LOGICAL_CHANNEL_DONE, response));
    }

    /**
     * Exposes {@link CommandsInterface.iccCloseLogicalChannel}
     */
    CARAPI IccCloseLogicalChannel(Int32 channel, Message response) {
        mCi->IccCloseLogicalChannel(channel,
                mHandler->ObtainMessage(EVENT_CLOSE_LOGICAL_CHANNEL_DONE, response));
    }

    /**
     * Exposes {@link CommandsInterface.iccTransmitApduLogicalChannel}
     */
    CARAPI IccTransmitApduLogicalChannel(Int32 channel, Int32 cla, Int32 command,
            Int32 p1, Int32 p2, Int32 p3, String data, Message response) {
        mCi->IccTransmitApduLogicalChannel(channel, cla, command, p1, p2, p3,
                data, mHandler->ObtainMessage(EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE, response));
    }

    /**
     * Exposes {@link CommandsInterface.iccTransmitApduBasicChannel}
     */
    CARAPI IccTransmitApduBasicChannel(Int32 cla, Int32 command,
            Int32 p1, Int32 p2, Int32 p3, String data, Message response) {
        mCi->IccTransmitApduBasicChannel(cla, command, p1, p2, p3,
                data, mHandler->ObtainMessage(EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE, response));
    }

    /**
     * Exposes {@link CommandsInterface.iccIO}
     */
    CARAPI IccExchangeSimIO(Int32 fileID, Int32 command, Int32 p1, Int32 p2, Int32 p3,
            String pathID, Message response) {
        mCi->IccIO(command, fileID, pathID, p1, p2, p3, NULL, NULL,
                mHandler->ObtainMessage(EVENT_SIM_IO_DONE, response));
    }

    /**
     * Exposes {@link CommandsInterface.getAtr}
     */
    CARAPI GetAtr(Message response) {
        mCi->GetAtr(mHandler->ObtainMessage(EVENT_SIM_GET_ATR_DONE, response));
    }

    /**
     * Exposes {@link CommandsInterface.sendEnvelopeWithStatus}
     */
    CARAPI SendEnvelopeWithStatus(String contents, Message response) {
        mCi->SendEnvelopeWithStatus(contents, response);
    }

    /* Returns number of applications on this card */
    public Int32 GetNumApplications() {
        Int32 count = 0;
        For (UiccCardApplication a : mUiccApplications) {
            If (a != NULL) {
                count++;
            }
        }
        return count;
    }

    /**
     * Returns TRUE iff carrier priveleges rules are NULL (dont need to be loaded) or loaded.
     */
    public Boolean AreCarrierPriviligeRulesLoaded() {
        return mCarrierPrivilegeRules != NULL
            && mCarrierPrivilegeRules->AreCarrierPriviligeRulesLoaded();
    }

    /**
     * Exposes {@link UiccCarrierPrivilegeRules.getCarrierPrivilegeStatus}.
     */
    public Int32 GetCarrierPrivilegeStatus(Signature signature, String packageName) {
        return mCarrierPrivilegeRules == NULL ?
            TelephonyManager.CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED :
            mCarrierPrivilegeRules->GetCarrierPrivilegeStatus(signature, packageName);
    }

    /**
     * Exposes {@link UiccCarrierPrivilegeRules.getCarrierPrivilegeStatus}.
     */
    public Int32 GetCarrierPrivilegeStatus(PackageManager packageManager, String packageName) {
        return mCarrierPrivilegeRules == NULL ?
            TelephonyManager.CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED :
            mCarrierPrivilegeRules->GetCarrierPrivilegeStatus(packageManager, packageName);
    }

    /**
     * Exposes {@link UiccCarrierPrivilegeRules.getCarrierPrivilegeStatusForCurrentTransaction}.
     */
    public Int32 GetCarrierPrivilegeStatusForCurrentTransaction(PackageManager packageManager) {
        return mCarrierPrivilegeRules == NULL ?
            TelephonyManager.CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED :
            mCarrierPrivilegeRules->GetCarrierPrivilegeStatusForCurrentTransaction(packageManager);
    }

    /**
     * Exposes {@link UiccCarrierPrivilegeRules.getCarrierPackageNamesForIntent}.
     */
    public List<String> GetCarrierPackageNamesForIntent(
            PackageManager packageManager, Intent intent) {
        return mCarrierPrivilegeRules == NULL ? NULL :
            mCarrierPrivilegeRules->GetCarrierPackageNamesForIntent(
                    packageManager, intent);
    }

    public Boolean SetOperatorBrandOverride(String brand) {
        Log("setOperatorBrandOverride: " + brand);
        Log("current iccId: " + GetIccId());

        String iccId = GetIccId();
        If (TextUtils->IsEmpty(iccId)) {
            return FALSE;
        }

        SharedPreferences.Editor spEditor =
                PreferenceManager->GetDefaultSharedPreferences(mContext).Edit();
        String key = OPERATOR_BRAND_OVERRIDE_PREFIX + iccId;
        If (brand == NULL) {
            spEditor->Remove(key).Commit();
        } else {
            spEditor->PutString(key, brand).Commit();
        }
        return TRUE;
    }

    public String GetOperatorBrandOverride() {
        String iccId = GetIccId();
        If (TextUtils->IsEmpty(iccId)) {
            return NULL;
        }
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(mContext);
        return sp->GetString(OPERATOR_BRAND_OVERRIDE_PREFIX + iccId, NULL);
    }

    public String GetIccId() {
        // ICCID should be same across all the apps.
        For (UiccCardApplication app : mUiccApplications) {
            If (app != NULL) {
                IccRecords ir = app->GetIccRecords();
                If (ir != NULL && ir->GetIccId() != NULL) {
                    return ir->GetIccId();
                }
            }
        }
        return NULL;
    }
    public UICCConfig GetUICCConfig() {
        return mUICCConfig;
    }

    void OnRefresh(IccRefreshResponse refreshResponse){
        For ( Int32 i = 0; i < mUiccApplications.length; i++) {
            If (mUiccApplications[i] != NULL) {
                // Let the app know that the refresh occurred
                mUiccApplications[i].OnRefresh(refreshResponse);
            }
        }
    }

    protected void Log(String msg) {
        Rlog->D(LOG_TAG, msg);
    }

    private void Loge(String msg) {
        Rlog->E(LOG_TAG, msg);
    }

    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("UiccCard:");
        pw->Println(" mCi=" + mCi);
        pw->Println(" mDestroyed=" + mDestroyed);
        pw->Println(" mLastRadioState=" + mLastRadioState);
        pw->Println(" mCatService=" + mCatService);
        pw->Println(" mAbsentRegistrants: size=" + mAbsentRegistrants->Size());
        For (Int32 i = 0; i < mAbsentRegistrants->Size(); i++) {
            pw->Println("  mAbsentRegistrants[" + i + "]="
                    + ((Registrant)mAbsentRegistrants->Get(i)).GetHandler());
        }
        For (Int32 i = 0; i < mCarrierPrivilegeRegistrants->Size(); i++) {
            pw->Println("  mCarrierPrivilegeRegistrants[" + i + "]="
                    + ((Registrant)mCarrierPrivilegeRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mCardState=" + mCardState);
        pw->Println(" mUniversalPinState=" + mUniversalPinState);
        pw->Println(" mGsmUmtsSubscriptionAppIndex=" + mGsmUmtsSubscriptionAppIndex);
        pw->Println(" mCdmaSubscriptionAppIndex=" + mCdmaSubscriptionAppIndex);
        pw->Println(" mImsSubscriptionAppIndex=" + mImsSubscriptionAppIndex);
        pw->Println(" mImsSubscriptionAppIndex=" + mImsSubscriptionAppIndex);
        pw->Println(" mUiccApplications: length=" + mUiccApplications.length);
        For (Int32 i = 0; i < mUiccApplications.length; i++) {
            If (mUiccApplications[i] == NULL) {
                pw->Println("  mUiccApplications[" + i + "]=" + NULL);
            } else {
                pw->Println("  mUiccApplications[" + i + "]="
                        + mUiccApplications[i].GetType() + " " + mUiccApplications[i]);
            }
        }
        pw->Println();
        // Print details of all applications
        For (UiccCardApplication app : mUiccApplications) {
            If (app != NULL) {
                app->Dump(fd, pw, args);
                pw->Println();
            }
        }
        // Print details of all IccRecords
        For (UiccCardApplication app : mUiccApplications) {
            If (app != NULL) {
                IccRecords ir = app->GetIccRecords();
                If (ir != NULL) {
                    ir->Dump(fd, pw, args);
                    pw->Println();
                }
            }
        }
        pw->Flush();
    }
}
