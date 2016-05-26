
#ifndef __ELASTOS_SQL_SQLITE_CVM_H__
#define __ELASTOS_SQL_SQLITE_CVM_H__

#include "_Elastos_Sql_SQLite_CVm.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CVm)
    , public Object
    , public IVm
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Step(
        /* [in] */ ICallback * cb,
        /* [out] */ Boolean * value);

    CARAPI Compile(
        /* [out] */ Boolean * value);

    CARAPI Stop();

    CARAPI Finalize();

    CVm();

    ~CVm();

public:
    Int64 mHandle;
    Int32 mError_code;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CVM_H__
