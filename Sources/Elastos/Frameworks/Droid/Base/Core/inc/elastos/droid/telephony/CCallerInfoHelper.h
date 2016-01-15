
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CCallerInfoHelper.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CCallerInfoHelper)
{
public:
    CARAPI GetCallerInfo(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [out] */ ICallerInfo** callerInfo);

    CARAPI GetCallerInfo(
        /* [in] */ IContext* context,
        /* [in] */ IUri* contactRef,
        /* [out] */ ICallerInfo** callerInfo);

    CARAPI GetCallerInfo(
        /* [in] */ IContext* context,
        /* [in] */ IUri* contactRef,
        /* [in] */ ICursor* cursor,
        /* [out] */ ICallerInfo** callerInfo);

    CARAPI GetCallerId(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [out] */ String* res);

    CARAPI DoSecondaryLookupIfNecessary(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ ICallerInfo* previousResult,
        /* [out] */ ICallerInfo** res);

private:
    /**
     * @return The ISO 3166-1 two letters country code of the country the user
     *         is in.
     */
    static CARAPI GettCurrentCountryIso(
        /* [in] */ IContext* context,
        /* [in] */ ILocale* locale,
        /* [out] */ String* res);

    /**
     * Returns the column index to use to find the "person_id" field in
     * the specified cursor, based on the contact URI that was originally
     * queried.
     *
     * This is a helper function for the getCallerInfo() method that takes
     * a Cursor.  Looking up the person_id is nontrivial (compared to all
     * the other CallerInfo fields) since the column we need to use
     * depends on what query we originally ran.
     *
     * Watch out: be sure to not do any database access in this method, since
     * it's run from the UI thread (see comments below for more info.)
     *
     * @return the columnIndex to use (with cursor.getLong()) to get the
     * person_id, or -1 if we couldn't figure out what colum to use.
     *
     * TODO: Add a unittest for this method.  (This is a little tricky to
     * test, since we'll need a live contacts database to test against,
     * preloaded with at least some phone numbers and SIP addresses.  And
     * we'll probably have to hardcode the column indexes we expect, so
     * the test might break whenever the contacts schema changes.  But we
     * can at least make sure we handle all the URI patterns we claim to,
     * and that the mime types match what we expect...)
     */
    static CARAPI GetColumnIndexForPersonId(
        /* [in] */ IUri* contactRef,
        /* [in] */ ICursor* cursor,
        /* [out] */ Int32* res);

    static CARAPI Normalize(
        /* [in] */ const String& s,
        /* [out] */ String* res);

    static const Boolean VDBG = FALSE;
    static const String TAG;
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOHELPER_H__
