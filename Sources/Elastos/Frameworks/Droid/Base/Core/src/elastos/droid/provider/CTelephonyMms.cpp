#include "elastos/droid/provider/CTelephonyMms.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Utility.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyMms, Singleton, ITelephonyMms, ITelephonyBaseMmsColumns);
CAR_SINGLETON_IMPL(CTelephonyMms);

/**
 * Helper method to query this table.
 * @hide
 */
ECode CTelephonyMms::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor)
{
    return Telephony::Mms::Query(cr, projection, cursor);
}

/**
 * Helper method to query this table.
 * @hide
 */
ECode CTelephonyMms::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& where,
    /* [in] */ const String& orderBy,
    /* [out] */ ICursor** cursor)
{
    return Telephony::Mms::Query(cr, projection, where, orderBy, cursor);
}

/**
 * Helper method to extract email address from address string.
 * @hide
 */
ECode CTelephonyMms::ExtractAddrSpec(
    /* [in] */ const String& address,
    /* [out] */ String* addrSpec)
{
    return Telephony::Mms::ExtractAddrSpec(address, addrSpec);
}

/**
 * Is the specified address an email address?
 *
 * @param address the input address to test
 * @return TRUE if address is an email address; FALSE otherwise.
 * @hide
 */
ECode CTelephonyMms::IsEmailAddress(
    /* [in] */ const String& address,
    /* [out] */ Boolean* result)
{
    return Telephony::Mms::IsEmailAddress(address, result);
}

/**
 * Is the specified number a phone number?
 *
 * @param number the input number to test
 * @return TRUE if number is a phone number; FALSE otherwise.
 * @hide
 */
ECode CTelephonyMms::IsPhoneNumber(
    /* [in] */ const String& number,
    /* [out] */ Boolean* result)
{
    return Telephony::Mms::IsPhoneNumber(number, result);
}

ECode CTelephonyMms::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Mms::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyMms::GetREPORT_REQUEST_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Mms::REPORT_REQUEST_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyMms::GetREPORT_STATUS_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Mms::REPORT_STATUS_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyMms::GetNAME_ADDR_EMAIL_PATTERN(
    /* [out] */ IPattern** pattern)
{
    VALIDATE_NOT_NULL(pattern);
    *pattern = Telephony::Mms::NAME_ADDR_EMAIL_PATTERN;
    REFCOUNT_ADD(*pattern);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
