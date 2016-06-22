#include "elastos/droid/google/mms/utility/CSqliteWrapper.h"
#include "elastos/droid/google/mms/utility/SqliteWrapper.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

CAR_INTERFACE_IMPL(CSqliteWrapper, Singleton, ISqliteWrapper)
CAR_SINGLETON_IMPL(CSqliteWrapper)

ECode CSqliteWrapper::Query(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICursor> cursor = SqliteWrapper::Query(context, resolver, uri, projection, selection, selectionArgs, sortOrder);
    *result = cursor;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSqliteWrapper::Requery(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SqliteWrapper::Requery(context, cursor);
    return NOERROR;
}

ECode CSqliteWrapper::Update(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SqliteWrapper::Update(context, resolver, uri, values, where, selectionArgs);
    return NOERROR;
}

ECode CSqliteWrapper::Delete(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SqliteWrapper::Delete(context, resolver, uri, where, selectionArgs);
    return NOERROR;
}

ECode CSqliteWrapper::Insert(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUri> res = SqliteWrapper::Insert(context, resolver, uri, values);
    *result = res;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
