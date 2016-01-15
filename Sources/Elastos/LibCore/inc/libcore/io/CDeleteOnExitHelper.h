#ifndef __LIBCORE_IO_CDELETEONEXITHELPER_H__
#define __LIBCORE_IO_CDELETEONEXITHELPER_H__

#include "Singleton.h"
#include "_Libcore_IO_CDeleteOnExitHelper.h"


namespace Libcore {
namespace IO {

CarClass(CDeleteOnExitHelper)
    , public Singleton
    , public IDeleteOnExitHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [out] */ IDeleteOnExit** instance);
};

} // namespace IO
} // namespace Libcore
#endif
