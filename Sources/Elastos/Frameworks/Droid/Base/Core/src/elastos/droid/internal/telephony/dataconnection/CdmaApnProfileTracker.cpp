
#include "elastos/droid/internal/telephony/dataconnection/CdmaApnProfileTracker.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/cdma/CdmaSubscriptionSourceManager.h"
#include "elastos/droid/internal/telephony/dataconnection/ApnProfileOmh.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/CRegistrantList.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/text/TextUtils.h"
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.Os.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::Cdma::CdmaSubscriptionSourceManager;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::CRegistrantList;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

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
    CRegistrantList::New((IRegistrantList**)&mModemApnProfileRegistrants);
}

ECode CdmaApnProfileTracker::constructor(
    /* [in] */ ICDMAPhone* phone)
{
    mPhone = phone;
    AutoPtr<IContext> context;
    IPhone::Probe(phone)->GetContext((IContext**)&context);
    AutoPtr<ICommandsInterface> ci;
    mCdmaSsm = CdmaSubscriptionSourceManager::GetInstance(context, ((PhoneBase*) phone)->mCi, this,
            EVENT_LOAD_PROFILES, NULL);
    CHashMap::New((IHashMap**)&mOmhServicePriorityMap);
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_LOAD_PROFILES, (IMessage**)&msg);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode CdmaApnProfileTracker::LoadProfiles()
{
    Log("loadProfiles...");
    mApnProfilesList->Clear();
    ReadApnProfilesFromModem();
    return NOERROR;
}

ECode CdmaApnProfileTracker::ParseTypes(
    /* [in] */ const String& types,
    /* [out, callee] */ ArrayOf<String>** _result)
{
    VALIDATE_NOT_NULL(_result)

    AutoPtr<ArrayOf<String> > result;
    // If unset, set to DEFAULT.
    if (types == NULL || types.Equals("")) {
        result = ArrayOf<String>::Alloc(1);
        (*result)[0] = IPhoneConstants::APN_TYPE_ALL;
    } else {
        StringUtils::Split(types, ",", (ArrayOf<String>**)&result);
    }
    *_result = result;
    REFCOUNT_ADD(*_result)
    return NOERROR;
}

ECode CdmaApnProfileTracker::Finalize()
{
    Logger::D(LOG__TAG, "CdmaApnProfileTracker finalized");
    return NOERROR;
}

ECode CdmaApnProfileTracker::RegisterForModemProfileReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    ((RegistrantList*) mModemApnProfileRegistrants.Get())->Add(r);
    return NOERROR;
}

ECode CdmaApnProfileTracker::UnregisterForModemProfileReady(
    /* [in] */ IHandler* h)
{
    ((RegistrantList*) mModemApnProfileRegistrants.Get())->Remove(h);
    return NOERROR;
}

ECode CdmaApnProfileTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (!((PhoneBase*)mPhone.Get())->mIsTheCurrentActivePhone) {
        Logger::D(LOG__TAG, "Ignore CDMA msgs since CDMA phone is inactive");
        return NOERROR;
    }
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    switch (msgWhat) {
        case EVENT_LOAD_PROFILES:
            LoadProfiles();
            break;
        case EVENT_READ_MODEM_PROFILES:
            OnReadApnProfilesFromModem();
            break;
        case EVENT_GET_DATA_CALL_PROFILE_DONE:
            OnGetDataCallProfileDone(IAsyncResult::Probe(msgObj), msgArg1);
            break;
        default:
            break;
    }
    return NOERROR;
}

ECode CdmaApnProfileTracker::ReadApnProfilesFromModem()
{
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_READ_MODEM_PROFILES, (IMessage**)&msg);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode CdmaApnProfileTracker::OnReadApnProfilesFromModem()
{
    Log("OMH: onReadApnProfilesFromModem()");
    mOmhReadProfileContext++;
    mOmhReadProfileCount = 0; // Reset the count and list(s)
    /* Clear out the modem profiles lists (main and temp) which were read/saved */
    mTempOmhApnProfilesList->Clear();
    mOmhServicePriorityMap->Clear();
    // For all the service types known in modem, read the data profies
    AutoPtr<ArrayOf<IApnProfileTypeModem*> > values = ApnProfileOmh::ApnProfileTypeModem::GetValues();
    for (Int32 i = 0; i < values->GetLength(); ++i) {
        AutoPtr<IApnProfileTypeModem> p = (*values)[i];
        Int32 id;
        p->Getid(&id);
        Log("OMH: Reading profiles for:%d", id);
        mOmhReadProfileCount++;
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_GET_DATA_CALL_PROFILE_DONE, //what
                mOmhReadProfileContext, //arg1
                0 , //arg2  -- ignore
                p, //userObj
                (IMessage**)&msg);
        ((PhoneBase*) mPhone.Get())->mCi->GetDataCallProfile(id, msg);
    }
    return NOERROR;
}

ECode CdmaApnProfileTracker::OnGetDataCallProfileDone(
    /* [in] */ IAsyncResult* ar,
    /* [in] */ Int32 context)
{
    if (context != mOmhReadProfileContext) {
        //we have other onReadOmhDataprofiles() on the way.
        return NOERROR;
    }
    if (((AsyncResult*) ar)->mException != NULL) {
        Log("OMH: Exception in onGetDataCallProfileDone:%s", TO_CSTR(((AsyncResult*) ar)->mException));
        mOmhReadProfileCount--;
        return NOERROR;
    }
    // ApnProfile list from the modem for a given SERVICE_TYPE. These may
    // be from RUIM in case of OMH
    AutoPtr<IArrayList> dataProfileListModem = IArrayList::Probe(((AsyncResult*) ar)->mResult);
    AutoPtr<IApnProfileTypeModem> modemProfile = IApnProfileTypeModem::Probe(((AsyncResult*) ar)->mUserObj);
    mOmhReadProfileCount--;
    if (dataProfileListModem != NULL) {
        Int32 size;
        dataProfileListModem->GetSize(&size);
        if (size > 0) {
            String serviceType;
            /* For the modem service type, get the android DataServiceType */
            modemProfile->GetDataServiceType(&serviceType);
            Log("OMH: # profiles returned from modem:%d for %s", size,
                    serviceType.string());
            Int32 priority;
            OmhListGetArbitratedPriority(dataProfileListModem, serviceType, &priority);
            AutoPtr<IInteger32> i32;
            CInteger32::New(priority, (IInteger32**)&i32);
            mOmhServicePriorityMap->Put(StringUtils::ParseCharSequence(serviceType), i32);
            AutoPtr<IIterator> it;
            dataProfileListModem->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IApnSetting> apn = IApnSetting::Probe(obj);
                /* Store the modem profile type in the data profile */
                IApnProfileOmh::Probe(apn)->SetApnProfileTypeModem(modemProfile);
                /* Look through mTempOmhApnProfilesList for existing profile id's
                 * before adding it. This implies that the (similar) profile with same
                 * priority already exists.
                 */
                AutoPtr<IApnProfileOmh> omhDuplicateDp;
                GetDuplicateProfile(apn, (IApnProfileOmh**)&omhDuplicateDp);
                if (NULL == omhDuplicateDp) {
                    mTempOmhApnProfilesList->Add(apn);
                    AutoPtr<IApnProfileTypeModem> modem = ApnProfileOmh::ApnProfileTypeModem::GetApnProfileTypeModem(serviceType);
                    IApnProfileOmh::Probe(apn)->AddServiceType(modem);
                } else {
                    /*  To share the already established data connection
                     * (say between SUPL and DUN) in cases such as below:
                     *  Ex:- SUPL+DUN [profile id 201, priority 1]
                     *  'apn' instance is found at this point. Add the non-provisioned
                     *   service type to this 'apn' instance
                     */
                    Log("OMH: Duplicate Profile %s", TO_CSTR(omhDuplicateDp));
                    AutoPtr<IApnProfileTypeModem> modem = ApnProfileOmh::ApnProfileTypeModem::GetApnProfileTypeModem(serviceType);
                    omhDuplicateDp->AddServiceType(modem);
                }
            }
        }
    }
    //(Re)Load APN List
    if (mOmhReadProfileCount == 0) {
        Log("OMH: Modem omh profile read complete.");
        AddServiceTypeToUnSpecified();
        mApnProfilesList->AddAll(ICollection::Probe(mTempOmhApnProfilesList));
        ((RegistrantList*) mModemApnProfileRegistrants.Get())->NotifyRegistrants();
    }
    return NOERROR;
}

ECode CdmaApnProfileTracker::GetDuplicateProfile(
    /* [in] */ IApnSetting* apn,
    /* [out] */ IApnProfileOmh** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IIterator> it;
    mTempOmhApnProfilesList->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnSetting> dataProfile = IApnSetting::Probe(obj);
        Int32 apnId;
        apn->GetProfileId(&apnId);
        Int32 dataId;
        dataProfile->GetProfileId(&dataId);
        if (apnId == dataId){
            *result = IApnProfileOmh::Probe(dataProfile);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode CdmaApnProfileTracker::GetApnProfile(
    /* [in] */ const String& serviceType,
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)

    Log("getApnProfile: serviceType=%s", serviceType.string());
    AutoPtr<IApnSetting> profile;
    // Go through all the profiles to find one
    AutoPtr<IIterator> it;
    mApnProfilesList->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnSetting> apn = IApnSetting::Probe(obj);
        Boolean canHandleType;
        apn->CanHandleType(serviceType, &canHandleType);
        if (canHandleType) {
            profile = apn;
            break;
        }
    }
    Log("getApnProfile: return profile=%s", TO_CSTR(profile));
    *result = profile;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CdmaApnProfileTracker::GetOmhApnProfilesList(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result)

    Log("getOmhApnProfilesList:%s", TO_CSTR(mApnProfilesList));
    *result = mApnProfilesList;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CdmaApnProfileTracker::AddServiceTypeToUnSpecified()
{
    for (Int32 i = 0; i < mSupportedApnTypes->GetLength(); ++i) {
        String apntype = (*mSupportedApnTypes)[i];
        Boolean containsKey;
        mOmhServicePriorityMap->ContainsKey(StringUtils::ParseCharSequence(apntype), &containsKey);
        if (!containsKey) {
            // ServiceType :apntype is not provisioned in the card,
            // Look through the profiles read from the card to locate
            // the UNSPECIFIED profile and add the service type to it.
            AutoPtr<IIterator> it;
            mTempOmhApnProfilesList->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IApnSetting> apn = IApnSetting::Probe(obj);
                AutoPtr<IApnProfileTypeModem> modem;
                IApnProfileOmh::Probe(apn)->GetApnProfileTypeModem((IApnProfileTypeModem**)&modem);
                if (modem == ApnProfileOmh::ApnProfileTypeModem::PROFILE_TYPE_UNSPECIFIED) {
                    IApnProfileOmh::Probe(apn)->AddServiceType(
                            ApnProfileOmh::ApnProfileTypeModem::GetApnProfileTypeModem(apntype));
                    Log("OMH: Service Type added to UNSPECIFIED is : %s",
                            TO_CSTR(ApnProfileOmh::ApnProfileTypeModem::GetApnProfileTypeModem(apntype)));
                    break;
                }
            }
        }
    }
    return NOERROR;
}

ECode CdmaApnProfileTracker::OmhListGetArbitratedPriority(
    /* [in] */ IArrayList* dataProfileListModem,
    /* [in] */ const String& serviceType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IApnSetting> profile;
    AutoPtr<IIterator> it;
    dataProfileListModem->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IApnSetting> apn = IApnSetting::Probe(obj);
        Boolean isValidPriority;
        IApnProfileOmh::Probe(apn)->IsValidPriority(&isValidPriority);
        if (!isValidPriority) {
            Log("[OMH] Invalid priority... skipping");
            continue;
        }
        if (profile == NULL) {
            profile = apn; // first hit
        } else {
            if (serviceType == IPhoneConstants::APN_TYPE_SUPL) {
                // Choose the profile with lower priority
                Int32 priority;
                IApnProfileOmh::Probe(profile)->GetPriority(&priority);
                Boolean isPriorityLower;
                IApnProfileOmh::Probe(apn)->IsPriorityLower(priority, &isPriorityLower);
                profile = isPriorityLower ? apn : profile;
            } else {
                // Choose the profile with higher priority
                Int32 priority;
                IApnProfileOmh::Probe(profile)->GetPriority(&priority);
                Boolean isPriorityHigher;
                IApnProfileOmh::Probe(apn)->IsPriorityHigher(priority, &isPriorityHigher);
                profile = isPriorityHigher ? apn : profile;
            }
        }
    }
    return IApnProfileOmh::Probe(profile)->GetPriority(result);
}

ECode CdmaApnProfileTracker::ClearActiveApnProfile()
{
    mActiveApn = NULL;
    return NOERROR;
}

ECode CdmaApnProfileTracker::IsApnTypeActive(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean canHandleType;
    mActiveApn->CanHandleType(type, &canHandleType);
    *result = mActiveApn != NULL && canHandleType;
    return NOERROR;
}

ECode CdmaApnProfileTracker::IsApnTypeAvailable(
    /* [in] */ const String& type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    for (Int32 i = 0; i < mSupportedApnTypes->GetLength(); ++i) {
        String s = (*mSupportedApnTypes)[i];
        if (TextUtils::Equals(type, s)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode CdmaApnProfileTracker::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    Logger::D(LOG__TAG, "[CdmaApnProfileTracker] %s", msgBuf);
    return NOERROR;
}

ECode CdmaApnProfileTracker::Loge(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    Logger::E(LOG__TAG, "[CdmaApnProfileTracker] %s", msgBuf);
    return NOERROR;
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
