#ifndef __ELASTOS_IO_CHARSET_CCODERRESULTHELPER_H__
#define __ELASTOS_IO_CHARSET_CCODERRESULTHELPER_H__

#include "_Elastos_IO_Charset_CCoderResultHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {
namespace Charset {

CarClass(CCoderResultHelper)
    , public Singleton
    , public ICoderResultHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetUNDERFLOW(
        /* [out] */ ICoderResult** result);

    CARAPI GetOVERFLOW(
        /* [out] */ ICoderResult** result);

    CARAPI MalformedForLength(
        /* [in] */ Int32 length,
        /* [out] */ ICoderResult** result);

    CARAPI UnmappableForLength(
        /* [in] */ Int32 length,
        /* [out] */ ICoderResult** result);
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARSET_CCODERRESULTHELPER_H__
