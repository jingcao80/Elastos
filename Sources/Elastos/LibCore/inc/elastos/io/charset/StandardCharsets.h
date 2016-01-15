#ifndef __ELASTOS_IO_CHARSET_ELASTOS_IO_STANDARDCHARSETS_H__
#define __ELASTOS_IO_CHARSET_ELASTOS_IO_STANDARDCHARSETS_H__

#include "Charset.h"

namespace Elastos {
namespace IO {
namespace Charset {

class StandardCharsets
{
private:
    StandardCharsets();

public:
    /**
    * The ISO-8859-1 charset.
    */
    static const AutoPtr<ICharset> ISO_8859_1;

    /**
    * The US-ASCII charset.
    */
    static const AutoPtr<ICharset> US_ASCII;

    /**
    * The UTF-8 charset.
    */
    static const AutoPtr<ICharset> UTF_8;

    /**
    * The UTF-16 charset.
    */
    static const AutoPtr<ICharset> UTF_16;

    /**
    * The UTF-16BE (big-endian) charset.
    */
    static const AutoPtr<ICharset> UTF_16BE;

    /**
    * The UTF-16LE (little-endian) charset.
    */
    static const AutoPtr<ICharset> UTF_16LE;

private:
    static Boolean Init();
    static Boolean sIsInit;
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARSET_ELASTOS_IO_STANDARDCHARSETS_H__
