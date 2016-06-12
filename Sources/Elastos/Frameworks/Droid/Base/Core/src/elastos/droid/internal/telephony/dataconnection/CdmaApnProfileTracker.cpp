
#include "elastos/droid/internal/telephony/dataconnection/CdmaApnProfileTracker.h"
#include <_Elastos.Droid.Core.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(CdmaApnProfileTracker, Handler, ICdmaApnProfileTracker)

AutoPtr<ArrayOf<String> > CdmaApnProfileTracker::mSupportedApnTypes = InitSupportedApnTypes();
AutoPtr<ArrayOf<String> > CdmaApnProfileTracker::mDefaultApnTypes = InitDefaultApnTypes();
const Int32 CdmaApnProfileTracker::EVENT_READ_MODEM_PROFILES = 0;
const Int32 CdmaApnProfileTracker::EVENT_GET_DATA_CALL_PROFILE_DONE = 1;
const Int32 CdmaApnProfileTracker::EVENT_LOAD_PROFILES = 2;

CdmaApnProfileTracker::CdmaApnProfileTracker()
    : LOG__TAG("CDMA")
    , mOmhReadProfileContext(0)
    , mOmhReadProfileCount(0)
{
    CArrayList::New((IArrayList**)&mTempOmhApnProfilesList);
    CArrayList::New((IArrayList**)&mApnProfilesList);
    assert(0 && "TODO CRegistrantList IRegistrantList");
    // CRegistrantList::New((IRegistrantList**)&mModemApnProfileRegistrants);
}

ECode CdmaApnProfileTracker::constructor(
    /* [in] */ ICDMAPhone* phone)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mPhone = phone;
        mCdmaSsm = CdmaSubscriptionSourceManager.getInstance (phone.getContext(), phone.mCi, this,
                EVENT_LOAD_PROFILES, null);
        mOmhServicePriorityMap = new HashMap<String, Integer>();
        sendMessage(obtainMessage(EVENT_LOAD_PROFILES));

#endif
}

ECode CdmaApnProfileTracker::LoadProfiles()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("loadProfiles...");
        mApnProfilesList.clear();
        readApnProfilesFromModem();

#endif
}

ECode CdmaApnProfileTracker::ParseTypes(
    /* [in] */ const String& types,
    /* [out, callee] */ ArrayOf<String>** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String[] result;
        // If unset, set to DEFAULT.
        if (types == null || types.equals("")) {
            result = new String[1];
            result[0] = PhoneConstants.APN_TYPE_ALL;
        } else {
            result = types.split(",");
        }
        return result;

#endif
}

ECode CdmaApnProfileTracker::Finalize()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Log.d(LOG__TAG, "CdmaApnProfileTracker finalized");

#endif
}

ECode CdmaApnProfileTracker::RegisterForModemProfileReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Registrant r = new Registrant(h, what, obj);
        mModemApnProfileRegistrants.add(r);

#endif
}

ECode CdmaApnProfileTracker::UnregisterForModemProfileReady(
    /* [in] */ IHandler* h)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mModemApnProfileRegistrants.remove(h);

#endif
}

ECode CdmaApnProfileTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!mPhone.mIsTheCurrentActivePhone) {
            Log.d(LOG__TAG, "Ignore CDMA msgs since CDMA phone is inactive");
            return;
        }
        switch (msg.what) {
            case EVENT_LOAD_PROFILES:
                loadProfiles();
                break;
            case EVENT_READ_MODEM_PROFILES:
                onReadApnProfilesFromModem();
                break;
            case EVENT_GET_DATA_CALL_PROFILE_DONE:
                onGetDataCallProfileDone((AsyncResult) msg.obj, (int)msg.arg1);
                break;
            default:
                break;
        }

#endif
}

ECode CdmaApnProfileTracker::ReadApnProfilesFromModem()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        sendMessage(obtainMessage(EVENT_READ_MODEM_PROFILES));

#endif
}

ECode CdmaApnProfileTracker::OnReadApnProfilesFromModem()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("OMH: onReadApnProfilesFromModem()");
        mOmhReadProfileContext++;
        mOmhReadProfileCount = 0; // Reset the count and list(s)
        /* Clear out the modem profiles lists (main and temp) which were read/saved */
        mTempOmhApnProfilesList.clear();
        mOmhServicePriorityMap.clear();
        // For all the service types known in modem, read the data profies
        for (ApnProfileTypeModem p : ApnProfileTypeModem.values()) {
            log("OMH: Reading profiles for:" + p.getid());
            mOmhReadProfileCount++;
            mPhone.mCi.getDataCallProfile(p.getid(),
                            obtainMessage(EVENT_GET_DATA_CALL_PROFILE_DONE, //what
                            mOmhReadProfileContext, //arg1
                            0 , //arg2  -- ignore
                            p));//userObj
        }

#endif
}

ECode CdmaApnProfileTracker::OnGetDataCallProfileDone(
    /* [in] */ IAsyncResult* ar,
    /* [in] */ Int32 context)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (context != mOmhReadProfileContext) {
            //we have other onReadOmhDataprofiles() on the way.
            return;
        }
        if (ar.exception != null) {
            log("OMH: Exception in onGetDataCallProfileDone:" + ar.exception);
            mOmhReadProfileCount--;
            return;
        }
        // ApnProfile list from the modem for a given SERVICE_TYPE. These may
        // be from RUIM in case of OMH
        ArrayList<ApnSetting> dataProfileListModem = (ArrayList<ApnSetting>)ar.result;
        ApnProfileTypeModem modemProfile = (ApnProfileTypeModem)ar.userObj;
        mOmhReadProfileCount--;
        if (dataProfileListModem != null && dataProfileListModem.size() > 0) {
            String serviceType;
            /* For the modem service type, get the android DataServiceType */
            serviceType = modemProfile.getDataServiceType();
            log("OMH: # profiles returned from modem:" + dataProfileListModem.size()
                    + " for " + serviceType);
            mOmhServicePriorityMap.put(serviceType,
                    omhListGetArbitratedPriority(dataProfileListModem, serviceType));
            for (ApnSetting apn : dataProfileListModem) {
                /* Store the modem profile type in the data profile */
                ((ApnProfileOmh)apn).setApnProfileTypeModem(modemProfile);
                /* Look through mTempOmhApnProfilesList for existing profile id's
                 * before adding it. This implies that the (similar) profile with same
                 * priority already exists.
                 */
                ApnProfileOmh omhDuplicateDp = getDuplicateProfile(apn);
                if (null == omhDuplicateDp) {
                    mTempOmhApnProfilesList.add(apn);
                    ((ApnProfileOmh)apn).addServiceType(ApnProfileTypeModem.
                            getApnProfileTypeModem(serviceType));
                } else {
                    /*  To share the already established data connection
                     * (say between SUPL and DUN) in cases such as below:
                     *  Ex:- SUPL+DUN [profile id 201, priority 1]
                     *  'apn' instance is found at this point. Add the non-provisioned
                     *   service type to this 'apn' instance
                     */
                    log("OMH: Duplicate Profile " + omhDuplicateDp);
                    omhDuplicateDp.addServiceType(ApnProfileTypeModem.
                            getApnProfileTypeModem(serviceType));
                }
            }
        }
        //(Re)Load APN List
        if (mOmhReadProfileCount == 0) {
            log("OMH: Modem omh profile read complete.");
            addServiceTypeToUnSpecified();
            mApnProfilesList.addAll(mTempOmhApnProfilesList);
            mModemApnProfileRegistrants.notifyRegistrants();
        }
        return;

#endif
}

ECode CdmaApnProfileTracker::GetDuplicateProfile(
    /* [in] */ IApnSetting* apn,
    /* [out] */ IApnProfileOmh** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (ApnSetting dataProfile : mTempOmhApnProfilesList) {
            if (((ApnProfileOmh)apn).getProfileId() ==
                ((ApnProfileOmh)dataProfile).getProfileId()){
                return (ApnProfileOmh)dataProfile;
            }
        }
        return null;

#endif
}

ECode CdmaApnProfileTracker::GetApnProfile(
    /* [in] */ const String& serviceType,
    /* [out] */ IApnSetting** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("getApnProfile: serviceType="+serviceType);
        ApnSetting profile = null;
        // Go through all the profiles to find one
        for (ApnSetting apn: mApnProfilesList) {
            if (apn.canHandleType(serviceType)) {
                profile = apn;
                break;
            }
        }
        log("getApnProfile: return profile=" + profile);
        return profile;

#endif
}

ECode CdmaApnProfileTracker::GetOmhApnProfilesList(
    /* [out] */ IArrayList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("getOmhApnProfilesList:" + mApnProfilesList);
        return mApnProfilesList;

#endif
}

ECode CdmaApnProfileTracker::AddServiceTypeToUnSpecified()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (String apntype : mSupportedApnTypes) {
            if(!mOmhServicePriorityMap.containsKey(apntype)) {
                // ServiceType :apntype is not provisioned in the card,
                // Look through the profiles read from the card to locate
                // the UNSPECIFIED profile and add the service type to it.
                for (ApnSetting apn : mTempOmhApnProfilesList) {
                    if (((ApnProfileOmh)apn).getApnProfileTypeModem() ==
                                ApnProfileTypeModem.PROFILE_TYPE_UNSPECIFIED) {
                        ((ApnProfileOmh)apn).addServiceType(ApnProfileTypeModem.
                                getApnProfileTypeModem(apntype));
                        log("OMH: Service Type added to UNSPECIFIED is : " +
                                ApnProfileTypeModem.getApnProfileTypeModem(apntype));
                        break;
                    }
                }
            }
        }

#endif
}

ECode CdmaApnProfileTracker::OmhListGetArbitratedPriority(
    /* [in] */ IArrayList* dataProfileListModem,
    /* [in] */ const String& serviceType,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ApnSetting profile = null;
        for (ApnSetting apn : dataProfileListModem) {
            if (!((ApnProfileOmh) apn).isValidPriority()) {
                log("[OMH] Invalid priority... skipping");
                continue;
            }
            if (profile == null) {
                profile = apn; // first hit
            } else {
                if (serviceType == PhoneConstants.APN_TYPE_SUPL) {
                    // Choose the profile with lower priority
                    profile = ((ApnProfileOmh) apn).isPriorityLower(((ApnProfileOmh) profile)
                            .getPriority()) ? apn : profile;
                } else {
                    // Choose the profile with higher priority
                    profile = ((ApnProfileOmh) apn).isPriorityHigher(((ApnProfileOmh) profile)
                            .getPriority()) ? apn : profile;
                }
            }
        }
        return ((ApnProfileOmh) profile).getPriority();

#endif
}

ECode CdmaApnProfileTracker::ClearActiveApnProfile()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mActiveApn = null;

#endif
}

ECode CdmaApnProfileTracker::IsApnTypeActive(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mActiveApn != null && mActiveApn.canHandleType(type);

#endif
}

ECode CdmaApnProfileTracker::IsApnTypeAvailable(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        for (String s : mSupportedApnTypes) {
            if (TextUtils.equals(type, s)) {
                return true;
            }
        }
        return false;

#endif
}

ECode CdmaApnProfileTracker::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Log.d(LOG__TAG, "[CdmaApnProfileTracker] " + s);

#endif
}

ECode CdmaApnProfileTracker::Loge(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Log.e(LOG__TAG, "[CdmaApnProfileTracker] " + s);

#endif
}

AutoPtr<ArrayOf<String> > CdmaApnProfileTracker::InitSupportedApnTypes()
{
    AutoPtr<ArrayOf<String> > rev = ArrayOf<String>::Alloc(8);
    (*rev)[0] = IPhoneConstants::APN_TYPE_DEFAULT;
    (*rev)[1] = IPhoneConstants::APN_TYPE_MMS;
    (*rev)[2] = IPhoneConstants::APN_TYPE_SUPL;
    (*rev)[3] = IPhoneConstants::APN_TYPE_DUN;
    (*rev)[4] = IPhoneConstants::APN_TYPE_HIPRI;
    (*rev)[5] = IPhoneConstants::APN_TYPE_FOTA;
    (*rev)[6] = IPhoneConstants::APN_TYPE_IMS;
    (*rev)[7] = IPhoneConstants::APN_TYPE_CBS;
    return rev;
}

AutoPtr<ArrayOf<String> > CdmaApnProfileTracker::InitDefaultApnTypes()
{
    AutoPtr<ArrayOf<String> > rev = ArrayOf<String>::Alloc(7);
    (*rev)[0] = IPhoneConstants::APN_TYPE_DEFAULT;
    (*rev)[1] = IPhoneConstants::APN_TYPE_MMS;
    (*rev)[2] = IPhoneConstants::APN_TYPE_SUPL;
    (*rev)[3] = IPhoneConstants::APN_TYPE_HIPRI;
    (*rev)[4] = IPhoneConstants::APN_TYPE_FOTA;
    (*rev)[5] = IPhoneConstants::APN_TYPE_IMS;
    (*rev)[6] = IPhoneConstants::APN_TYPE_CBS;
    return rev;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
