
#ifndef __ELASTOS_IO_CHARSET_ELASTOS_IO_CCODINGERRORACTIONHELPER_H__
#define __ELASTOS_IO_CHARSET_ELASTOS_IO_CCODINGERRORACTIONHELPER_H__

#include "_Elastos_IO_Charset_CCodingErrorActionHelper.h"
#include "Singleton.h"

namespace Elastos {
namespace IO {
namespace Charset {

CarClass(CCodingErrorActionHelper)
    , public Singleton
    , public ICodingErrorActionHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetIGNORE(
        /* [out] */ ICodingErrorAction** ignore);

    CARAPI GetREPLACE(
        /* [out] */ ICodingErrorAction** replace);

    CARAPI GetREPORT(
        /* [out] */ ICodingErrorAction** report);
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHARSET_ELASTOS_IO_CCODINGERRORACTIONHELPER_H__
