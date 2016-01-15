
#include "elastos/droid/database/CDatabaseInsertHelper.h"

namespace Elastos {
namespace Droid {
namespace Database {

ECode CDatabaseInsertHelper::constructor(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ const String& tableName)
{
    return DatabaseUtils::InsertHelper::constructor(db, tableName);
}

ECode CDatabaseInsertHelper::GetColumnIndex(
    /* [in] */ const String& key,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    return DatabaseUtils::InsertHelper::GetColumnIndex(key, index);
}

ECode CDatabaseInsertHelper::BindDouble(
    /* [in] */ Int32 index,
    /* [in] */ Double value)
{
    return DatabaseUtils::InsertHelper::BindDouble(index, value);
}

ECode CDatabaseInsertHelper::BindFloat(
    /* [in] */ Int32 index,
    /* [in] */ Float value)
{
    return DatabaseUtils::InsertHelper::BindFloat(index, value);
}

ECode CDatabaseInsertHelper::BindInt64(
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    return DatabaseUtils::InsertHelper::BindInt64(index, value);
}

ECode CDatabaseInsertHelper::BindInt32(
    /* [in] */ Int32 index,
    /* [in] */ Int32 value)
{
    return DatabaseUtils::InsertHelper::BindInt32(index, value);
}

ECode CDatabaseInsertHelper::BindBoolean(
    /* [in] */ Int32 index,
    /* [in] */ Boolean value)
{
    return DatabaseUtils::InsertHelper::BindBoolean(index, value);
}

ECode CDatabaseInsertHelper::BindNull(
    /* [in] */ Int32 index)
{
    return DatabaseUtils::InsertHelper::BindNull(index);
}

ECode CDatabaseInsertHelper::BindBytes(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* value)
{
    return DatabaseUtils::InsertHelper::BindBytes(index, value);
}

ECode CDatabaseInsertHelper::BindString(
    /* [in] */ Int32 index,
    /* [in] */ const String& value)
{
    return DatabaseUtils::InsertHelper::BindString(index, value);
}

ECode CDatabaseInsertHelper::Insert(
    /* [in] */ IContentValues* values,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return DatabaseUtils::InsertHelper::Insert(values, result);
}

ECode CDatabaseInsertHelper::Execute(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return DatabaseUtils::InsertHelper::Execute(result);
}

ECode CDatabaseInsertHelper::PrepareForInsert()
{
    return DatabaseUtils::InsertHelper::PrepareForInsert();
}

ECode CDatabaseInsertHelper::PrepareForReplace()
{
    return DatabaseUtils::InsertHelper::PrepareForReplace();
}

ECode CDatabaseInsertHelper::Replace(
    /* [in] */ IContentValues* values,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return DatabaseUtils::InsertHelper::Replace(values, result);
}

ECode CDatabaseInsertHelper::Close()
{
    return DatabaseUtils::InsertHelper::Close();
}

} //Database
} //Droid
} //Elastos