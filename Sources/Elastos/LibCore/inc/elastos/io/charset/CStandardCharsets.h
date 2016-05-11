#ifndef __ELASTOS_IO_CHARSET_ELASTOS_IO_CSTANDARDCHARSETS_H__
#define __ELASTOS_IO_CHARSET_ELASTOS_IO_CSTANDARDCHARSETS_H__

#include "_Elastos_IO_Charset_CStandardCharsets.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {
namespace Charset {

CarClass(CStandardCharsets)
    , public Singleton
    , public IStandardCharsets
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetISO_8859_1(
        /* [out] */ ICharset** outcharset);

    CARAPI GetUS_ASCII(
        /* [out] */ ICharset** outcharset);

    CARAPI GetUTF_8(
        /* [out] */ ICharset** outcharset);

    CARAPI GetUTF_16(
        /* [out] */ ICharset** outcharset);

    CARAPI GetUTF_16BE(
        /* [out] */ ICharset** outcharset);

    CARAPI GetUTF_16LE(
        /* [out] */ ICharset** outcharset);
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARSET_ELASTOS_IO_CSTANDARDCHARSETS_H__
