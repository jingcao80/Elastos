
#ifndef __ELASTOS_SQL_SQLITE_CFUNCTIONCONTEXT_H__
#define __ELASTOS_SQL_SQLITE_CFUNCTIONCONTEXT_H__

#include "_Elastos_Sql_SQLite_CFunctionContext.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CFunctionContext)
    , public Object
    , public IFunctionContext
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI SetResult(
        /* [in] */ const String& ret);

    CARAPI SetResult(
        /* [in] */ Int32 i);

    CARAPI SetResult(
        /* [in] */ Double d);

    CARAPI SetError(
        /* [in] */ const String& err);

    CARAPI SetResult(
        /* [in] */ const ArrayOf<Byte>& b);

    CARAPI SetResultZeroblob(
        /* [in] */ Int32 n);

    CARAPI Count(
        /* [out] */ Int32* count);
    CFunctionContext();

public:
    Int64 mHandle;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CFUNCTIONCONTEXT_H__
