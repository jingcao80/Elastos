
#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYBLACKLIST_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYBLACKLIST_H__

#include "_Elastos_Droid_Provider_CTelephonyBlacklist.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

/**
 * Contains phone numbers that are blacklisted
 * for phone and/or message purposes.
 * @hide
 */
CarClass(CTelephonyBlacklist)
    , public Singleton
    , public ITelephonyBlacklist
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** result);

    CARAPI GetCONTENT_FILTER_BYNUMBER_URI(
        /* [out] */ IUri** result);

    CARAPI GetCONTENT_PHONE_URI(
        /* [out] */ IUri** result);

    CARAPI GetCONTENT_MESSAGE_URI(
        /* [out] */ IUri** result);

private:
    static CARAPI_(AutoPtr<IUri>) InitUri(
        /* [in] */ const String& uriStr);

public:
    /**
     * The content:// style URL for this table
     */
    static AutoPtr<IUri> CONTENT_URI;

    /**
     * The content:// style URL for filtering this table by number.
     * When using this, make sure the number is correctly encoded
     * when appended to the Uri.
     */
    static AutoPtr<IUri> CONTENT_FILTER_BYNUMBER_URI;

    /**
     * The content:// style URL for filtering this table on phone numbers
     */
    static AutoPtr<IUri> CONTENT_PHONE_URI;

    /**
     * The content:// style URL for filtering this table on message numbers
     */
    static AutoPtr<IUri> CONTENT_MESSAGE_URI;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_PROVIDER_CTELEPHONYBLACKLIST_H__
