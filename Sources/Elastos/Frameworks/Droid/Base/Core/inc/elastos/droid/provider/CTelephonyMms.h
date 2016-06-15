#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYMMS_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYMMS_H__

#include "_Elastos_Droid_Provider_CTelephonyMms.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonyMms)
    , public Singleton
    , public ITelephonyMms
    , public ITelephonyBaseMmsColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    /**
     * Helper method to query this table.
     * @hide
     */
    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

    /**
     * Helper method to query this table.
     * @hide
     */
    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& where,
        /* [in] */ const String& orderBy,
        /* [out] */ ICursor** cursor);

    /**
     * Helper method to extract email address from address string.
     * @hide
     */
    CARAPI ExtractAddrSpec(
        /* [in] */ const String& address,
        /* [out] */ String* addrSpec);

    /**
     * Is the specified address an email address?
     *
     * @param address the input address to test
     * @return TRUE if address is an email address; FALSE otherwise.
     * @hide
     */
    CARAPI IsEmailAddress(
        /* [in] */ const String& address,
        /* [out] */ Boolean* result);

    /**
     * Is the specified number a phone number?
     *
     * @param number the input number to test
     * @return TRUE if number is a phone number; FALSE otherwise.
     * @hide
     */
    CARAPI IsPhoneNumber(
        /* [in] */ const String& number,
        /* [out] */ Boolean* result);

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetREPORT_REQUEST_URI(
        /* [out] */ IUri** uri);

    CARAPI GetREPORT_STATUS_URI(
        /* [out] */ IUri** uri);

    CARAPI GetNAME_ADDR_EMAIL_PATTERN(
        /* [out] */ IPattern** pattern);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYMMS_H__
