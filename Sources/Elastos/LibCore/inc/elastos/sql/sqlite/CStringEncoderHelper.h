
#ifndef __ELASTOS_SQL_SQLITE_CSTRINGENCODERHELPER_H__
#define __ELASTOS_SQL_SQLITE_CSTRINGENCODERHELPER_H__

#include "_Elastos_Sql_SQLite_CStringEncoderHelper.h"
#include "Singleton.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CStringEncoderHelper)
    , public Singleton
    , public IStringEncoderHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI Encode(
        /* [in] */ ArrayOf<Byte> * a,
        /* [out] */ String * str);

    CARAPI Decode(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Byte> ** outarray);

    CARAPI EncodeX(
        /* [in] */ ArrayOf<Byte> * a,
        /* [out] */ String * str);
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CSTRINGENCODERHELPER_H__
