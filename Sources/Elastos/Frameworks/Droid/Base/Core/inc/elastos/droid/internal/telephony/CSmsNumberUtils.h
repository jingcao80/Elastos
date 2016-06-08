
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSNUMBERUTILS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSNUMBERUTILS_H__

#include "_Elastos_Droid_Internal_Telephony_CSmsNumberUtils.h"

#include "_Elastos.Droid.Internal.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

using Elastos::Droid::Content::IContext;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsNumberUtils)
    , public Singleton
    , public ISmsNumberUtils
{
private:
    class NumberEntry : public Object
    {
    public:
        NumberEntry(String number) {
            mNumber = number;
        }
    public:
        String mNumber;
        String mIDD;
        Int32 mCountryCode;
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     *  Filter the destination number if using VZW sim card.
     */
    CARAPI FilterDestAddr(
        /* [in] */ IPhoneBase* phoneBase,
        /* [in] */ const String& destAddr,
        /* [out] */ String* result);


private:
    /* Breaks the given number down and formats it according to the rules
     * for different number plans and differnt network.
     *
     * @param number destionation number which need to be format
     * @param activeMcc current network's mcc
     * @param networkType current network type
     *
     * @return the number after fromatting.
     */
    static String FormatNumber(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ const String& activeMcc,
        /* [in] */ Int32 networkType);

    /* Query International direct dialing from HbpcdLookup.db
     * for specified country code
     *
     * @param mcc current network's country code
     *
     * @return the IDD array list.
     */
    static AutoPtr<IArrayList> GetAllIDDs(
        /* [in] */ IContext* context,
        /* [in] */ const String& mcc);

    /* Verify if the the destination number is a NANP number
     *
     * @param numberEntry including number and IDD array
     * @param allIDDs the IDD array list of the current network's country code
     *
     * @return the number plan type related NANP
     */
    static Int32 CheckNANP(
        /* [in] */ NumberEntry* numberEntry,
        /* [in] */ IArrayList* allIDDs);

    static Boolean IsNANP(
        /* [in] */ const String& number);

    /* Verify if the the destination number is an internal number
     *
     * @param numberEntry including number and IDD array
     * @param allIDDs the IDD array list of the current network's country code
     *
     * @return the number plan type related international number
     */
    static Int32 CheckInternationalNumberPlan(
        /* [in] */ IContext* context,
        /* [in] */ NumberEntry* numberEntry,
        /* [in] */ IArrayList* allIDDs,
        /* [in] */ const String& homeIDD);

    /**
     *  Returns the country code from the given number.
     */
    static Int32 GetCountryCode(
        /* [in] */ IContext* context,
        /* [in] */ const String& number);

    /**
     *  Gets all country Codes information with given MCC.
     */
    static AutoPtr<ArrayOf<Int32> > GetAllCountryCodes(
        /* [in] */ IContext* context);

    static Boolean InExceptionListForNpCcAreaLocal(
        /* [in] */ NumberEntry* numberEntry);

    static String GetNumberPlanType(
        /* [in] */ Int32 state);

    /**
     * Returns the current network type
     */
    static Int32 GetNetworkType(
        /* [in] */ IPhoneBase* phoneBase);

    static Boolean IsInternationalRoaming(
        /* [in] */ IPhoneBase* phoneBase);

    static Boolean NeedToConvert(
        /* [in] */ IPhoneBase* phoneBase);

    static Boolean CompareGid1(
        /* [in] */ IPhoneBase* phoneBase,
        /* [in] */ const String& serviceGid1);

private:
    static const String TAG;
    static const Boolean DBG;

    static const String PLUS_SIGN;

    static const Int32 NANP_SHORT_LENGTH;
    static const Int32 NANP_MEDIUM_LENGTH;
    static const Int32 NANP_LONG_LENGTH;

    static const Int32 NANP_CC;
    static const String NANP_NDD;
    static const String NANP_IDD;

    static const Int32 MIN_COUNTRY_AREA_LOCAL_LENGTH;

    static const Int32 GSM_UMTS_NETWORK;
    static const Int32 CDMA_HOME_NETWORK;
    static const Int32 CDMA_ROAMING_NETWORK;

    static const Int32 NP_NONE;
    static const Int32 NP_NANP_BEGIN;

    /* <Phone Number>, <NXX>-<XXXX> N[2-9] */
    static const Int32 NP_NANP_LOCAL;

    /* <Area_code>-<Phone Number>, <NXX>-<NXX>-<XXXX> N[2-9] */
    static const Int32 NP_NANP_AREA_LOCAL;

    /* <1>-<Area_code>-<Phone Number>, 1-<NXX>-<NXX>-<XXXX> N[2-9] */
    static const Int32 NP_NANP_NDD_AREA_LOCAL;

    /* <+><U.S.Country_code><Area_code><Phone Number>, +1-<NXX>-<NXX>-<XXXX> N[2-9] */
    static const Int32 NP_NANP_NBPCD_CC_AREA_LOCAL;

    /* <Local_IDD><Country_code><Area_code><Phone Number>, 001-1-<NXX>-<NXX>-<XXXX> N[2-9] */
    static const Int32 NP_NANP_LOCALIDD_CC_AREA_LOCAL;

    /* <+><Home_IDD><Country_code><Area_code><Phone Number>, +011-1-<NXX>-<NXX>-<XXXX> N[2-9] */
    static const Int32 NP_NANP_NBPCD_HOMEIDD_CC_AREA_LOCAL;

    static const Int32 NP_INTERNATIONAL_BEGIN;
    /* <+>-<Home_IDD>-<Country_code>-<Area_code>-<Phone Number>, +011-86-25-86281234 */
    static const Int32 NP_NBPCD_HOMEIDD_CC_AREA_LOCAL;

    /* <Home_IDD>-<Country_code>-<Area_code>-<Phone Number>, 011-86-25-86281234 */
    static const Int32 NP_HOMEIDD_CC_AREA_LOCAL;

    /* <NBPCD>-<Country_code>-<Area_code>-<Phone Number>, +1-86-25-86281234 */
    static const Int32 NP_NBPCD_CC_AREA_LOCAL;

    /* <Local_IDD>-<Country_code>-<Area_code>-<Phone Number>, 00-86-25-86281234 */
    static const Int32 NP_LOCALIDD_CC_AREA_LOCAL;

    /* <Country_code>-<Area_code>-<Phone Number>, 86-25-86281234*/
    static const Int32 NP_CC_AREA_LOCAL;

    static AutoPtr<ArrayOf<Int32> > ALL_COUNTRY_CODES;
    static Int32 MAX_COUNTRY_CODES_LENGTH;
    static AutoPtr<IHashMap> IDDS_MAPS;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSNUMBERUTILS_H__
