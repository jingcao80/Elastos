
#ifndef __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOBW_H__
#define __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOBW_H__

#include "_Elastos_Sql_SQLite_CBlobW.h"
#include "CBlob.h"
#include <OutputStream.h>

using Elastos::IO::OutputStream;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CBlobW)
    , public OutputStream
    , public IBlobW
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBlobW();

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI WriteBytes(
        /* [in] */ ArrayOf<Byte>* buffer);

    CARAPI WriteBytes(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Close();

    CARAPI constructor(
        /* [in] */ Elastos::Sql::SQLite::IBlob* blob);

    CARAPI Flush();

private:
    AutoPtr<CBlob> mBlob;
    Int32 mPos;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOBW_H__
