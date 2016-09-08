
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCPROVIDER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCPROVIDER_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/ContentProvider.h"

using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecord;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
class ECO_PUBLIC IccProvider: public ContentProvider
{
public:
    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Query(
        /* [in] */ IUri* url,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sort,
        /* [out] */ ICursor** result);

    // @Override
    CARAPI GetType(
        /* [in] */ IUri* url,
        /* [out] */ String* result);

    // @Override
    CARAPI Insert(
        /* [in] */ IUri* url,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** result);

    // @Override
    CARAPI Delete(
        /* [in] */ IUri* url,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

    // @Override
    CARAPI Update(
        /* [in] */ IUri* url,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

private:
    CARAPI_(AutoPtr<ICursor>) LoadAllSimContacts(
        /* [in] */ Int32 efType);

    CARAPI_(String) NormalizeValue(
        /* [in] */ const String& inVal);

    CARAPI_(AutoPtr<IMatrixCursor>) LoadFromEf(
        /* [in] */ Int32 efType,
        /* [in] */ Int64 subId);

    CARAPI_(Boolean) UpdateIccRecordInEf(
        /* [in] */ Int32 efType,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& pin2,
        /* [in] */ Int64 subId);

    /**
     * Loads an AdnRecord into a MatrixCursor. Must be called with mLock held.
     *
     * @param record the ADN record to load from
     * @param cursor the cursor to receive the results
     */
    CARAPI_(void) LoadRecord(
        /* [in] */ IAdnRecord* record,
        /* [in] */ IMatrixCursor* cursor,
        /* [in] */ Int32 id);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

    CARAPI_(Int64) GetRequestSubId(
        /* [in] */ IUri* url);

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitNames();

    static CARAPI_(AutoPtr<IUriMatcher>) InitUriMatcher();

public:
    static const String STR_TAG;
    static const String STR_NUMBER;
    static const String STR_EMAILS;
    static const String STR_ANRS;
    static const String STR_NEW_TAG;
    static const String STR_NEW_NUMBER;
    static const String STR_NEW_EMAILS;
    static const String STR_NEW_ANRS;
    static const String STR_PIN2;

protected:
    static const Int32 ADN;
    static const Int32 ADN_SUB;
    static const Int32 FDN;
    static const Int32 FDN_SUB;
    static const Int32 SDN;
    static const Int32 SDN_SUB;
    static const Int32 ADN_ALL;

private:
    static const String TAG;
    static const Boolean DBG;

    static const AutoPtr<ArrayOf<String> > ADDRESS_BOOK_COLUMN_NAMES;

    static const AutoPtr<IUriMatcher> URL_MATCHER;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCPROVIDER_H__
