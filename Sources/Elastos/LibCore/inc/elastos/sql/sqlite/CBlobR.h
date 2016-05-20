
#ifndef __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOBR_H__
#define __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOBR_H__

#include "_Elastos_Sql_SQLite_CBlobR.h"
#include "CBlob.h"
#include "InputStream.h"

using Elastos::IO::InputStream;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CBlobR)
    , public InputStream
    , public IBlobR
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Available(
        /* [out] */ Int32* number);

    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    CARAPI Read(
         /* [out] */ Int32* value);

    CARAPI ReadBytes(
        /* [out] */ ArrayOf<Byte> * buffer,
        /* [out] */ Int32* number);

    CARAPI ReadBytes(
        /* [out] */ ArrayOf<Byte> * b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [out] */ Int32* number);

    CARAPI Reset();

    CARAPI Close();

    CARAPI Skip(
        /* [in] */ Int64 n,
        /* [out] */ Int64* number);

    CARAPI constructor(
        /* [in] */ Elastos::Sql::SQLite::IBlob* blob);

private:
    AutoPtr<CBlob> mBlob;
    Int32 mPos;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOBR_H__
