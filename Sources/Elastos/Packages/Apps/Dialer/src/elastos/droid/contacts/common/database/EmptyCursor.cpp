
#include "elastos/droid/contacts/common/database/EmptyCursor.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Database {

ECode EmptyCursor::constructor(
    /* [in] */ ArrayOf<String>* columns)
{
    mColumns = columns;
    return NOERROR;
}

ECode EmptyCursor::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;
    return NOERROR;
}

ECode EmptyCursor::GetColumnNames(
    /* [out, callee] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names)
    *names = mColumns;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode EmptyCursor::GetString(
    /* [in] */ Int32 column,
    /* [out] */ String* str)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::GetInt16(
    /* [in] */ Int32 column,
    /* [out] */ Int16* value)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::GetInt32(
    /* [in] */ Int32 column,
    /* [out] */ Int32* value)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::GetInt64(
    /* [in] */ Int32 column,
    /* [out] */ Int64* value)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::GetFloat(
    /* [in] */ Int32 column,
    /* [out] */ Float* value)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::GetDouble(
    /* [in] */ Int32 column,
    /* [out] */ Double* value)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::IsNull(
    /* [in] */ Int32 column,
    /* [out] */ Boolean* isNull)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

} // namespace Database
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
