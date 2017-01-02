//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CONTACTINFOHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CONTACTINFOHELPER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/dialer/calllog/ContactInfo.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Dialer::Service::ICachedNumberLookupService;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

/**
 * Utility class to look up the contact information for a given number.
 */
class ContactInfoHelper : public Object
{
public:
    ContactInfoHelper(
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
    CARAPI_(AutoPtr<ContactInfo>) LookupNumber(
        /* [in] */ const String& number,
        /* [in] */ const String& countryIso);

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
    CARAPI_(Boolean) IsBusiness(
        /* [in] */ Int32 sourceType);

     /**
     * This function looks at a contact's source and determines if the user can
     * mark caller ids from this source as invalid.
     *
     * @param sourceType The source type to be checked
     * @param objectId The ID of the Contact object.
     * @return true if contacts from this source can be marked with an invalid caller id
     */
    CARAPI_(Boolean) CanReportAsInvalid(
        /* [in] */ Int32 sourceType,
        /* [in] */ const String& objectId);

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
    CARAPI_(AutoPtr<ContactInfo>) LookupContactFromUri(
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
    CARAPI_(AutoPtr<ContactInfo>) QueryContactInfoForSipAddress(
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
    CARAPI_(AutoPtr<ContactInfo>) QueryContactInfoForPhoneNumber(
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

    static const AutoPtr<ICachedNumberLookupService> sCachedNumberLookupService;
            // = ObjectFactory.newCachedNumberLookupService();
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CONTACTINFOHELPER_H__
