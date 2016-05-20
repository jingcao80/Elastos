
#include "CDatabaseHelper.h"
#include "Database.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_INTERFACE_IMPL(CDatabaseHelper, Object, IDatabaseHelper)

CAR_SINGLETON_IMPL(CDatabaseHelper);

ECode CDatabaseHelper::Complete(
    /* [in] */ const String& sql,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)
    *value = Database::Complete(sql);
    return NOERROR;
}

ECode CDatabaseHelper::Version(
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)
    *str = Database::Version();
    return NOERROR;
}

ECode CDatabaseHelper::ErrorString(
    /* [in] */ Int32 error_code,
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)
    *str = Database::ErrorString(error_code);
    return NOERROR;
}

ECode CDatabaseHelper::Status(
    /* [in] */ Int32 op,
    /* [in] */ ArrayOf<Int32>* info,
    /* [in] */ Boolean flag,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = Database::Status(op, info,flag);
    return NOERROR;
}

ECode CDatabaseHelper::LongFromJulian(
    /* [in] */ Double d,
    /* [out] */ Int64 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = Database::LongFromJulian(d);
    return NOERROR;
}

ECode CDatabaseHelper::LongFromJulian(
    /* [in] */ const String& s ,
    /* [out] */ Int64 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = Database::LongFromJulian(s);
    return NOERROR;
}

ECode CDatabaseHelper::JulianFromLong(
    /* [in] */ Int64 ms,
    /* [out] */ Double * value)
{
    VALIDATE_NOT_NULL(value)
    *value = Database::JulianFromLong(ms);
    return NOERROR;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
