
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CONTACTINFOHELPER_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CONTACTINFOHELPER_H__

#include "Elastos.Droid.App.h"

using Elastos::Apps::Dialer::Service::ICachedNumberLookupService;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

/**
 * Utility class to look up the contact information for a given number.
 */
class ContactInfoHelper
    : public Object
    , public IContactInfoHelper
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& currentCountryIso);

    /**
     * Returns the contact information for the given number.
     * <p>
     * If the number does not match any contact, returns a contact info containing only the number
     * and the formatted number.
     * <p>
     * If an error occurs during the lookup, it returns null.
     *
     * @param number the number to look up
     * @param countryIso the country associated with this number
     */
    CARAPI LookupNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& countryIso,
        /* [out] */ IContactInfo** result);

    /**
     * Parses the given URI to determine the original lookup key of the contact.
     */
    static CARAPI_(String) GetLookupKeyFromUri(
        /* [in] */ IUri* lookupUri);

    /**
     * Given a contact's sourceType, return true if the contact is a business
     *
     * @param sourceType sourceType of the contact. This is usually populated by
     *        {@link #mCachedNumberLookupService}.
     */
    CARAPI IsBusiness(
        /* [in] */ Int32 sourceType,
        /* [out] */ Boolean* result);

     /**
     * This function looks at a contact's source and determines if the user can
     * mark caller ids from this source as invalid.
     *
     * @param sourceType The source type to be checked
     * @param objectId The ID of the Contact object.
     * @return true if contacts from this source can be marked with an invalid caller id
     */
    CARAPI CanReportAsInvalid(
        /* [in] */ Int32 sourceType,
        /* [in] */ const String& objectId,
        /* [out] */ Boolean* result);

private:
    /**
     * Creates a JSON-encoded lookup uri for a unknown number without an associated contact
     *
     * @param number - Unknown phone number
     * @return JSON-encoded URI that can be used to perform a lookup when clicking on the quick
     *         contact card.
     */
    static CARAPI_(AutoPtr<IUri>) CreateTemporaryContactUri(
        /* [in] */ const String& number);

    /**
     * Looks up a contact using the given URI.
     * <p>
     * It returns null if an error occurs, {@link ContactInfo#EMPTY} if no matching contact is
     * found, or the {@link ContactInfo} for the given contact.
     * <p>
     * The {@link ContactInfo#formattedNumber} field is always set to {@code null} in the returned
     * value.
     */
    CARAPI_(AutoPtr<IContactInfo>) LookupContactFromUri(
        /* [in] */ IUri* uri);

    /**
     * Determines the contact information for the given SIP address.
     * <p>
     * It returns the contact info if found.
     * <p>
     * If no contact corresponds to the given SIP address, returns {@link ContactInfo#EMPTY}.
     * <p>
     * If the lookup fails for some other reason, it returns null.
     */
    CARAPI_(AutoPtr<IContactInfo>) QueryContactInfoForSipAddress(
        /* [in] */ const String& sipAddress);

     /**
     * Determines the contact information for the given phone number.
     * <p>
     * It returns the contact info if found.
     * <p>
     * If no contact corresponds to the given phone number, returns {@link ContactInfo#EMPTY}.
     * <p>
     * If the lookup fails for some other reason, it returns null.
     */
    CARAPI_(AutoPtr<IContactInfo>) QueryContactInfoForPhoneNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& countryIso);

    /**
     * Format the given phone number
     *
     * @param number the number to be formatted.
     * @param normalizedNumber the normalized number of the given number.
     * @param countryIso the ISO 3166-1 two letters country code, the country's convention will be
     *        used to format the number if the normalized phone is null.
     *
     * @return the formatted number, or the given number if it was formatted.
     */
    CARAPI_(String) FormatPhoneNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& normalizedNumber,
        /* [in] */ const String& countryIso);

private:
    AutoPtr<IContext> mContext;
    String mCurrentCountryIso;

    static const AutoPtr<ICachedNumberLookupService> mCachedNumberLookupService;
            // = ObjectFactory.newCachedNumberLookupService();
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CONTACTINFOHELPER_H__
