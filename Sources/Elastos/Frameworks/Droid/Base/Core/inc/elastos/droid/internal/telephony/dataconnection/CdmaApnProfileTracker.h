
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDMAAPNPROFILETRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDMAAPNPROFILETRACKER_H__

#include "_Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <_Elastos.Droid.Core.h>
#include <Elastos.Droid.Os.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Process.h>

using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManager;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::Process;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * {@hide}
 */
class CdmaApnProfileTracker
    : public Handler
    , public ICdmaApnProfileTracker
{
public:
    CAR_INTERFACE_DECL()

    CdmaApnProfileTracker();

    /* Constructor */
    CARAPI constructor(
        /* [in] */ ICDMAPhone* phone);

    /**
     * Load the CDMA profiles
     */
    CARAPI LoadProfiles();

    CARAPI Finalize();

    CARAPI RegisterForModemProfileReady(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForModemProfileReady(
        /* [in] */ IHandler* h);

    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI GetApnProfile(
        /* [in] */ const String& serviceType,
        /* [out] */ IApnSetting** result);

    CARAPI GetOmhApnProfilesList(
        /* [out] */ IArrayList** result);

    CARAPI ClearActiveApnProfile();

    CARAPI IsApnTypeActive(
        /* [in] */ const String& type,
        /* [out] */ Boolean* result);

    CARAPI IsApnTypeAvailable(
        /* [in] */ const String& type,
        /* [out] */ Boolean* result);

    CARAPI Log(
        /* [in] */ const String& s);

    CARAPI Loge(
        /* [in] */ const String& s);

private:
    /**
     * @param types comma delimited list of data service types
     * @return array of data service types
     */
    CARAPI ParseTypes(
        /* [in] */ const String& types,
        /* [out, callee] */ ArrayOf<String>** result);

    /*
     * Trigger modem read for data profiles
     */
    CARAPI ReadApnProfilesFromModem();

    /*
     * Reads all the data profiles from the modem
     */
    CARAPI OnReadApnProfilesFromModem();

    /*
     * Process the response for the RIL request GET_DATA_CALL_PROFILE.
     * Save the profile details received.
     */
    CARAPI OnGetDataCallProfileDone(
        /* [in] */ IAsyncResult* ar,
        /* [in] */ Int32 context);

    /*
     * returns the object 'OMH dataProfile' if a match with the same profile id
     * exists in the enumerated list of OMH profile list
     */
    CARAPI GetDuplicateProfile(
        /* [in] */ IApnSetting* apn,
        /* [out] */ IApnProfileOmh** result);

    /* For all the OMH service types not present in the card, add them to the
     * UNSPECIFIED/DEFAULT data profile.
     */
    CARAPI AddServiceTypeToUnSpecified();

    /*
     * Retrieves the highest priority for all APP types except SUPL. Note that
     * for SUPL, retrieve the least priority among its profiles.
     */
    CARAPI OmhListGetArbitratedPriority(
        /* [in] */ IArrayList* dataProfileListModem,
        /* [in] */ const String& serviceType,
        /* [out] */ Int32* result);

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitSupportedApnTypes();

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitDefaultApnTypes();

public:
    const String LOG__TAG;

    // if we have no active ApnProfile this is null
    AutoPtr<IApnSetting> mActiveApn;

    // Temp. ApnProfile list from the modem.
    AutoPtr<IArrayList> mTempOmhApnProfilesList;

    // Map of the service type to its priority
    AutoPtr<IHashMap> mOmhServicePriorityMap;

private:
    AutoPtr<ICDMAPhone> mPhone;

    AutoPtr<ICdmaSubscriptionSourceManager> mCdmaSsm;

    /**
     * mApnProfilesList holds all the Apn profiles for cdma
     */
    AutoPtr<IArrayList> mApnProfilesList;

    static AutoPtr<ArrayOf<String> > mSupportedApnTypes;

    static AutoPtr<ArrayOf<String> > mDefaultApnTypes;

    /*
     * Context for read profiles for OMH.
     */
    Int32 mOmhReadProfileContext;

    /*
     * Count to track if all read profiles for OMH are completed or not.
     */
    Int32 mOmhReadProfileCount;

    /* Registrant list for objects interested in modem profile related events */
    AutoPtr<IRegistrantList> mModemApnProfileRegistrants;;

    static const Int32 EVENT_READ_MODEM_PROFILES;

    static const Int32 EVENT_GET_DATA_CALL_PROFILE_DONE;

    static const Int32 EVENT_LOAD_PROFILES;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDMAAPNPROFILETRACKER_H__
