#ifndef __ELASTOS_DROID_PRIVACY_SURROGATE_PRIVACYCURSOR_H__
#define __ELASTOS_DROID_PRIVACY_SURROGATE_PRIVACYCURSOR_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Core_server.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::EIID_ICursor;
using Elastos::Droid::Database::ICharArrayBuffer;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Privacy {
namespace Surrogate {

class PrivacyCursor :
    public ICursor,
    public ElRefBase
{
public:
    PrivacyCursor();

    /**
     *
     * @param realCursor may not be null
     * @param allowedIds
     */
    PrivacyCursor(
        /* [in] */ ICursor* realCursor,
        /* [in] */ ArrayOf<Int32>* allowedIds);

    /*
     * This constructor is only for android_id
     * @param blockedColumnAndroidID pass -11 if should block all, or the columnIndex where android_id is!
     */
    PrivacyCursor(
        /* [in] */ ICursor* realCursor,
        /* [in] */ Int32 blockedColumnAndroidID);

public:
    CAR_INTERFACE_DECL()

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI GetPosition(
        /* [out] */ Int32* position);

    CARAPI Move(
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* succeeded);

    CARAPI MoveToPosition(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* succeeded);

    CARAPI MoveToFirst(
        /* [out] */ Boolean* succeeded);

    CARAPI MoveToLast(
        /* [out] */ Boolean* succeeded);

    CARAPI MoveToNext(
        /* [out] */ Boolean* succeeded);

    CARAPI MoveToPrevious(
        /* [out] */ Boolean* succeeded);

    CARAPI IsFirst(
        /* [out] */ Boolean* isFirst);

    CARAPI IsLast(
        /* [out] */ Boolean* isLast);

    CARAPI IsBeforeFirst(
        /* [out] */ Boolean* isBeforeFirst);

    CARAPI IsAfterLast(
        /* [out] */ Boolean* isAfterLast);

    CARAPI GetColumnIndex(
        /* [in] */ const String& columnName,
        /* [out] */ Int32* columnIndex);

    CARAPI GetColumnIndexOrThrow(
        /* [in] */ const String& columnName,
        /* [out] */ Int32* columnIndex);

    CARAPI GetColumnName(
        /* [in] */ Int32 columnIndex,
        /* [out] */ String* columnName);

    CARAPI GetColumnNames(
        /* [out, callee] */ ArrayOf<String>** columnNames);

    CARAPI GetColumnCount(
        /* [out] */ Int32* columnCount);

    CARAPI GetBlob(
        /* [in] */ Int32 columnIndex,
        /* [out, callee] */ ArrayOf<Byte>** blob);

    CARAPI GetString(
        /* [in] */ Int32 columnIndex,
        /* [out] */ String* columnValue);

    CARAPI CopyStringToBuffer(
        /* [in] */ Int32 columnIndex,
        /* [in] */ ICharArrayBuffer* buf);

    CARAPI GetInt16(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int16* columnValue);

    CARAPI GetInt32(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int32* columnValue);

    CARAPI GetInt64(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int64* columnValue);

    CARAPI GetFloat(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Float* columnValue);

    CARAPI GetDouble(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Double* columnValue);

    CARAPI GetType(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Int32* index);

    CARAPI IsNull(
        /* [in] */ Int32 columnIndex,
        /* [out] */ Boolean* isNull);

    CARAPI Deactivate();

    CARAPI Requery(
        /* [out] */ Boolean* succeeded);

    CARAPI Close();

    CARAPI IsClosed(
        /* [out] */ Boolean* closed);

    CARAPI RegisterContentObserver(
        /* [in] */ IContentObserver* observer);

    CARAPI UnregisterContentObserver(
        /* [in] */ IContentObserver* observer);

    CARAPI RegisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    CARAPI UnregisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    CARAPI SetNotificationUri(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Elastos::Droid::Net::IUri* uri);

    CARAPI GetWantsAllOnMoveCalls(
        /* [out] */ Boolean* result);

    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    CARAPI Respond(
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** result);

private:
    /**
     * just create fakeID
     * @return fake ID or null if something went wrong
     */
    CARAPI_(String) GetFakeID();

    CARAPI_(Int32) GetContactId(
        /* [in] */ ICursor* c);

    CARAPI_(Boolean) IsAllowed(
        /* [in] */ Int32 id);

    CARAPI_(Boolean) IsAllowed(
        /* [in] */ ICursor* c);

    /**
     * TODO: switch to HashMap etc to speed this up?
     * @param realPos
     * @return
     */
    CARAPI_(Int32) GetMappedPos(
        /* [in] */ Int32 realPos);

private:
    /**
     *  {"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"};
     */
    const static String mask[16];

    const static String TAG;

    AutoPtr<ICursor> realCursor;

    AutoPtr<ArrayOf<Int32> > allowedIds;

    Int32 blockedColumnAndroidID;

    Boolean googleCursor;

    AutoPtr<ArrayOf<Int32> > allowedIdMapping;

};

} // namespace Surrogate
} // namespace Privacy
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_PRIVACY_SURROGATE_PRIVACYCURSOR_H__
