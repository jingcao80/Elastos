
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_MCCTABLE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_MCCTABLE_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::IComparable;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Mobile Country Code
 *
 * {@hide}
 */
class MccTable
{
public:
    class MccEntry
        : public Object
        , public IComparable
    {
    public:
        CAR_INTERFACE_DECL()

        MccEntry(
            /* [in] */ Int32 mnc,
            /* [in] */ const String& iso,
            /* [in] */ Int32 smallestDigitsMCC);

        MccEntry(
            /* [in] */ Int32 mnc,
            /* [in] */ const String iso,
            /* [in] */ Int32 smallestDigitsMCC,
            /* [in] */ const String language);

        // @Override
        CARAPI CompareTo(
            /* [in] */ IInterface* o,
            /* [out] */ Int32* result);

    private:
        CARAPI_(void) Init(
            /* [in] */ Int32 mnc,
            /* [in] */ const String iso,
            /* [in] */ Int32 smallestDigitsMCC,
            /* [in] */ const String language);

    public:
        Int32 mMcc;
        String mIso;
        Int32 mSmallestDigitsMnc;
        String mLanguage;
    };

public:
    /**
     * Returns a default time zone ID for the given MCC.
     * @param mcc Mobile Country Code
     * @return default TimeZone ID, or NULL if not specified
     */
    static CARAPI_(String) DefaultTimeZoneForMcc(
        /* [in] */ Int32 mcc);

    /**
     * Given a GSM Mobile Country Code, returns
     * an ISO two-character country code if available.
     * Returns "" if unavailable.
     */
    static CARAPI_(String) CountryCodeForMcc(
        /* [in] */ Int32 mcc);

    /**
     * Given a GSM Mobile Country Code, returns
     * an ISO 2-3 character language code if available.
     * Returns NULL if unavailable.
     */
    static CARAPI_(String) DefaultLanguageForMcc(
        /* [in] */ Int32 mcc);

    /**
     * Given a GSM Mobile Country Code, returns
     * the smallest number of digits that M if available.
     * Returns 2 if unavailable.
     */
    static CARAPI_(Int32) SmallestDigitsMccForMnc(
        /* [in] */ Int32 mcc);

    /**
     * Updates MCC and MNC device configuration information for application retrieving
     * correct version of resources.  If MCC is 0, MCC and MNC will be ignored (not set).
     * @param context Context to act on.
     * @param mccmnc truncated imsi with just the MCC and MNC - MNC assumed to be from 4th to end
     * @param fromServiceState true if coming from the radio service state, false if from SIM
     */
    static CARAPI_(void) UpdateMccMncConfiguration(
        /* [in] */ IContext* context,
        /* [in] */ const String& mccmnc,
        /* [in] */ Boolean fromServiceState);

    /**
     * Return Locale for the language and country or NULL if no good match.
     *
     * @param context Context to act on.
     * @param language Two character language code desired
     * @param country Two character country code desired
     *
     * @return Locale or NULL if no appropriate value
     */
    static CARAPI_(AutoPtr<ILocale>) GetLocaleForLanguageCountry(
        /* [in] */ IContext* context,
        /* [in] */ const String& language,
        /* [in] */ const String& country);

    /**
     * Utility code to set the system locale if it's not set already
     * @param context Context to act on.
     * @param language Two character language code desired
     * @param country Two character country code desired
     *
     *  {@hide}
     */
    static CARAPI_(void) SetSystemLocale(
        /* [in] */ IContext* context,
        /* [in] */ const String& language,
        /* [in] */ const String& country);

private:
    static CARAPI_(AutoPtr<MccEntry>) EntryForMcc(
        /* [in] */ Int32 mcc);

    /**
     * If the timezone is not already set, set it based on the MCC of the SIM.
     * @param context Context to act on.
     * @param mcc Mobile Country Code of the SIM or SIM-like entity (build prop on CDMA)
     */
    static CARAPI_(void) SetTimezoneFromMccIfNeeded(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mcc);

    /**
     * Get Locale based on the MCC of the SIM.
     * @param context Context to act on.
     * @param mcc Mobile Country Code of the SIM or SIM-like entity (build prop on CDMA)
     *
     * @return locale for the mcc or NULL if none
     */
    static CARAPI_(AutoPtr<ILocale>) GetLocaleFromMcc(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mcc);

    /**
     * Set the country code for wifi.  This sets allowed wifi channels based on the
     * country of the carrier we see.  If we can't see any, reset to 0 so we don't
     * broadcast on forbidden channels.
     * @param context Context to act on.
     * @param mcc Mobile Country Code of the operator.  0 if not known
     */
    static CARAPI_(void) SetWifiCountryCodeFromMcc(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mcc);

    static CARAPI_(AutoPtr<IArrayList>) InitTable();

    static CARAPI_(AutoPtr<IComparable>) GetObj(
    /* [in] */ MccEntry* base);

private:
    static const String TAG;

    static AutoPtr<IArrayList> sTable; //ArrayList<MccEntry>
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_MCCTABLE_H__
