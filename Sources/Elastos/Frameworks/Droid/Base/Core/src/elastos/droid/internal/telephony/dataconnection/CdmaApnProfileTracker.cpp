/*
 * Copyright (c) 2010-2014, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManager;
using Elastos::Droid::Internal::Telephony::Dataconnection::ApnProfileOmh::IApnProfileTypeModem;
using Elastos::Droid::Internal::Telephony::Dataconnection::IApnSetting;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTracker;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;

/**
 * {@hide}
 */
public class CdmaApnProfileTracker extends Handler {
    protected final String LOG_TAG = "CDMA";

    private CDMAPhone mPhone;
    private CdmaSubscriptionSourceManager mCdmaSsm;

    /**
     * mApnProfilesList holds all the Apn profiles for cdma
     */
    private ArrayList<ApnSetting> mApnProfilesList = new ArrayList<ApnSetting>();

    private static const String[] mSupportedApnTypes = {
            PhoneConstants.APN_TYPE_DEFAULT,
            PhoneConstants.APN_TYPE_MMS,
            PhoneConstants.APN_TYPE_SUPL,
            PhoneConstants.APN_TYPE_DUN,
            PhoneConstants.APN_TYPE_HIPRI,
            PhoneConstants.APN_TYPE_FOTA,
            PhoneConstants.APN_TYPE_IMS,
            PhoneConstants.APN_TYPE_CBS };

    private static const String[] mDefaultApnTypes = {
            PhoneConstants.APN_TYPE_DEFAULT,
            PhoneConstants.APN_TYPE_MMS,
            PhoneConstants.APN_TYPE_SUPL,
            PhoneConstants.APN_TYPE_HIPRI,
            PhoneConstants.APN_TYPE_FOTA,
            PhoneConstants.APN_TYPE_IMS,
            PhoneConstants.APN_TYPE_CBS };

    // if we have no active ApnProfile this is NULL
    protected ApnSetting mActiveApn;

    /*
     * Context for read profiles for OMH.
     */
    private Int32 mOmhReadProfileContext = 0;

    /*
     * Count to track if all read profiles for OMH are completed or not.
     */
    private Int32 mOmhReadProfileCount = 0;

    // Temp. ApnProfile list from the modem.
    ArrayList<ApnSetting> mTempOmhApnProfilesList = new ArrayList<ApnSetting>();

    // Map of the service type to its priority
    HashMap<String, Integer> mOmhServicePriorityMap;

    /* Registrant list for objects interested in modem profile related events */
    private RegistrantList mModemApnProfileRegistrants = new RegistrantList();

    private static const Int32 EVENT_READ_MODEM_PROFILES = 0;
    private static const Int32 EVENT_GET_DATA_CALL_PROFILE_DONE = 1;
    private static const Int32 EVENT_LOAD_PROFILES = 2;

    /* Constructor */

    CdmaApnProfileTracker(CDMAPhone phone) {
        mPhone = phone;
        mCdmaSsm = CdmaSubscriptionSourceManager.GetInstance (phone->GetContext(), phone.mCi, this,
                EVENT_LOAD_PROFILES, NULL);

        mOmhServicePriorityMap = new HashMap<String, Integer>();

        SendMessage(ObtainMessage(EVENT_LOAD_PROFILES));
    }

    /**
     * Load the CDMA profiles
     */
    void LoadProfiles() {
        Log("loadProfiles...");
        mApnProfilesList->Clear();

        ReadApnProfilesFromModem();
    }


    /**
     * @param types comma delimited list of data service types
     * @return array of data service types
     */
    private String[] ParseTypes(String types) {
        String[] result;
        // If unset, set to DEFAULT.
        If (types == NULL || types->Equals("")) {
            result = new String[1];
            result[0] = PhoneConstants.APN_TYPE_ALL;
        } else {
            result = types->Split(",");
        }
        return result;
    }

    protected void Finalize() {
        Logger::D(LOG_TAG, "CdmaApnProfileTracker finalized");
    }

    CARAPI RegisterForModemProfileReady(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant(h, what, obj);
        mModemApnProfileRegistrants->Add(r);
    }

    CARAPI UnregisterForModemProfileReady(Handler h) {
        mModemApnProfileRegistrants->Remove(h);
    }

    CARAPI HandleMessage (Message msg) {

        If (!mPhone.mIsTheCurrentActivePhone) {
            Logger::D(LOG_TAG, "Ignore CDMA msgs since CDMA phone is inactive");
            return;
        }

        Switch (msg.what) {
            case EVENT_LOAD_PROFILES:
                LoadProfiles();
                break;
            case EVENT_READ_MODEM_PROFILES:
                OnReadApnProfilesFromModem();
                break;

            case EVENT_GET_DATA_CALL_PROFILE_DONE:
                OnGetDataCallProfileDone((AsyncResult) msg.obj, (Int32)msg.arg1);
                break;

            default:
                break;
        }
    }

    /*
     * Trigger modem read for data profiles
     */
    private void ReadApnProfilesFromModem() {
        SendMessage(ObtainMessage(EVENT_READ_MODEM_PROFILES));
    }

    /*
     * Reads all the data profiles from the modem
     */
    private void OnReadApnProfilesFromModem() {
        Log("OMH: OnReadApnProfilesFromModem()");
        mOmhReadProfileContext++;

        mOmhReadProfileCount = 0; // Reset the count and List(s)
        /* Clear out the modem profiles Lists (main and temp) which were read/saved */
        mTempOmhApnProfilesList->Clear();
        mOmhServicePriorityMap->Clear();

        // For all the service types known in modem, read the data profies
        For (ApnProfileTypeModem p : ApnProfileTypeModem->Values()) {
            Log("OMH: Reading profiles for:" + p->Getid());
            mOmhReadProfileCount++;
            mPhone.mCi->GetDataCallProfile(p->Getid(),
                            ObtainMessage(EVENT_GET_DATA_CALL_PROFILE_DONE, //what
                            mOmhReadProfileContext, //arg1
                            0 , //arg2  -- ignore
                            p));//userObj
        }

    }

    /*
     * Process the response for the RIL request GET_DATA_CALL_PROFILE.
     * Save the profile details received.
     */
    private void OnGetDataCallProfileDone(AsyncResult ar, Int32 context) {
        If (context != mOmhReadProfileContext) {
            //we have other OnReadOmhDataprofiles() on the way.
            return;
        }

        If (ar.exception != NULL) {
            Log("OMH: Exception in onGetDataCallProfileDone:" + ar.exception);
            mOmhReadProfileCount--;
            return;
        }

        // ApnProfile list from the modem for a given SERVICE_TYPE. These may
        // be from RUIM in case of OMH
        ArrayList<ApnSetting> dataProfileListModem = (ArrayList<ApnSetting>)ar.result;

        ApnProfileTypeModem modemProfile = (ApnProfileTypeModem)ar.userObj;

        mOmhReadProfileCount--;

        If (dataProfileListModem != NULL && dataProfileListModem->Size() > 0) {
            String serviceType;

            /* For the modem service type, get the android DataServiceType */
            serviceType = modemProfile->GetDataServiceType();

            Log("OMH: # profiles returned from modem:" + dataProfileListModem->Size()
                    + " for " + serviceType);

            mOmhServicePriorityMap->Put(serviceType,
                    OmhListGetArbitratedPriority(dataProfileListModem, serviceType));

            For (ApnSetting apn : dataProfileListModem) {

                /* Store the modem profile type in the data profile */
                ((ApnProfileOmh)apn).SetApnProfileTypeModem(modemProfile);

                /* Look through mTempOmhApnProfilesList for existing profile id's
                 * before adding it. This implies that The (similar) profile with same
                 * priority already exists.
                 */
                ApnProfileOmh omhDuplicateDp = GetDuplicateProfile(apn);
                If (NULL == omhDuplicateDp) {
                    mTempOmhApnProfilesList->Add(apn);
                    ((ApnProfileOmh)apn).AddServiceType(ApnProfileTypeModem.
                            GetApnProfileTypeModem(serviceType));
                } else {
                    /*  To share the already established data connection
                     * (say between SUPL and DUN) in cases such as below:
                     *  Ex:- SUPL+DUN [profile id 201, priority 1]
                     *  'apn' instance is found at this point. Add the non-provisioned
                     *   service type to this 'apn' instance
                     */
                    Log("OMH: Duplicate Profile " + omhDuplicateDp);
                    omhDuplicateDp->AddServiceType(ApnProfileTypeModem.
                            GetApnProfileTypeModem(serviceType));
                }
            }
        }

        //(Re)Load APN List
        If (mOmhReadProfileCount == 0) {
            Log("OMH: Modem omh profile read complete.");
            AddServiceTypeToUnSpecified();
            mApnProfilesList->AddAll(mTempOmhApnProfilesList);
            mModemApnProfileRegistrants->NotifyRegistrants();
        }

        return;
    }

    /*
     * returns the object 'OMH dataProfile' if a match with the same profile id
     * exists in the enumerated list of OMH profile list
     */
    private ApnProfileOmh GetDuplicateProfile(ApnSetting apn) {
        For (ApnSetting dataProfile : mTempOmhApnProfilesList) {
            If (((ApnProfileOmh)apn).GetProfileId() ==
                ((ApnProfileOmh)dataProfile).GetProfileId()){
                Return (ApnProfileOmh)dataProfile;
            }
        }
        return NULL;
    }

    public ApnSetting GetApnProfile(String serviceType) {
        Log("getApnProfile: serviceType="+serviceType);
        ApnSetting profile = NULL;

        // Go through all the profiles to find one
        For (ApnSetting apn: mApnProfilesList) {
            If (apn->CanHandleType(serviceType)) {
                profile = apn;
                break;
            }
        }

        Log("getApnProfile: return profile=" + profile);
        return profile;
    }

    public ArrayList<ApnSetting> GetOmhApnProfilesList() {
        Log("getOmhApnProfilesList:" + mApnProfilesList);
        return mApnProfilesList;
    }

    /* For all the OMH service types not present in the card, add them to the
     * UNSPECIFIED/DEFAULT data profile.
     */
    private void AddServiceTypeToUnSpecified() {
        For (String apntype : mSupportedApnTypes) {
            If(!mOmhServicePriorityMap->ContainsKey(apntype)) {

                // ServiceType :apntype is not provisioned in the card,
                // Look through the profiles read from the card to locate
                // the UNSPECIFIED profile and add the service type to it.
                For (ApnSetting apn : mTempOmhApnProfilesList) {
                    If (((ApnProfileOmh)apn).GetApnProfileTypeModem() ==
                                ApnProfileTypeModem.PROFILE_TYPE_UNSPECIFIED) {
                        ((ApnProfileOmh)apn).AddServiceType(ApnProfileTypeModem.
                                GetApnProfileTypeModem(apntype));
                        Log("OMH: Service Type added to UNSPECIFIED is : " +
                                ApnProfileTypeModem->GetApnProfileTypeModem(apntype));
                        break;
                    }
                }
            }
        }
    }

    /*
     * Retrieves the highest priority for all APP types except SUPL. Note that
     * for SUPL, retrieve the least priority among its profiles.
     */
    private Int32 OmhListGetArbitratedPriority(
            ArrayList<ApnSetting> dataProfileListModem,
            String serviceType) {
        ApnSetting profile = NULL;

        For (ApnSetting apn : dataProfileListModem) {
            If (!((ApnProfileOmh) apn).IsValidPriority()) {
                Log("[OMH] Invalid priority... skipping");
                continue;
            }

            If (profile == NULL) {
                profile = apn; // first hit
            } else {
                If (serviceType == PhoneConstants.APN_TYPE_SUPL) {
                    // Choose the profile with lower priority
                    profile = ((ApnProfileOmh) apn).IsPriorityLower(((ApnProfileOmh) profile)
                            .GetPriority()) ? apn : profile;
                } else {
                    // Choose the profile with higher priority
                    profile = ((ApnProfileOmh) apn).IsPriorityHigher(((ApnProfileOmh) profile)
                            .GetPriority()) ? apn : profile;
                }
            }
        }
        Return ((ApnProfileOmh) profile).GetPriority();
    }

    CARAPI ClearActiveApnProfile() {
        mActiveApn = NULL;
    }

    public Boolean IsApnTypeActive(String type) {
        return mActiveApn != NULL && mActiveApn->CanHandleType(type);
    }

    protected Boolean IsApnTypeAvailable(String type) {
        For (String s : mSupportedApnTypes) {
            If (TextUtils->Equals(type, s)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    protected void Log(String s) {
        Logger::D(LOG_TAG, "[CdmaApnProfileTracker] " + s);
    }

    protected void Loge(String s) {
        Logger::E(LOG_TAG, "[CdmaApnProfileTracker] " + s);
    }
}

