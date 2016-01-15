
#ifndef __ELASTOS_SQL_SQLITE_STRINGENCODER_H__
#define __ELASTOS_SQL_SQLITE_STRINGENCODER_H__

#include <elastos.h>

namespace Elastos {
namespace Sql {
namespace SQLite {

class StringEncoder
{
public:
    static String Encode(
        /* [in] */ ArrayOf<Byte> * a);

    static String EncodeX(
        /* [in] */ ArrayOf<Byte> * a);

    static AutoPtr<ArrayOf<Byte> > Decode(
        /* [in] */ const String& s);

private:
    static AutoPtr<ArrayOf<Byte> > ByteCopy(
        /* [in] */ ArrayOf<Byte> * source,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Byte> * target);

    static Boolean Init();

private:
    static Boolean initflag;

protected:
    static AutoPtr<ArrayOf<Char32> > xdigits;
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_STRINGENCODER_H__
