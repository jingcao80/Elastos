
#ifndef __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOB_H__
#define __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOB_H__

#include "_Elastos_Sql_SQLite_CBlob.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CBlob)
    , public Object
    , public IBlob
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBlob();

    CARAPI GetInputStream(
        /* [out] */ IInputStream** instream);

    CARAPI GetOutputStream(
        /* [out] */ IOutputStream** outstream);

    CARAPI Close();

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 pos,
        /* [in] */ Int32 len,
        /* [out] */ Int32 * count);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 pos,
        /* [in] */ Int32 len,
        /* [out] */ Int32 * count);
public:
    Int32 mSize;

private:
    Int64 mHandle;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_ELASTOS_SQL_SQLLITE_CBLOB_H__
