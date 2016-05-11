
#ifndef __LIBCORE_IO_CLIBCORE_H__
#define __LIBCORE_IO_CLIBCORE_H__

#include "core/Singleton.h"
#include "_Libcore_IO_CLibcore.h"

using Elastos::Core::Singleton;

namespace Libcore {
namespace IO {

CarClass(CLibcore)
    , public Singleton
    , public ILibcore
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetOs(
        /* [out] */ IOs ** os);

public:
    static AutoPtr<IOs> sOs;
};

} // namespace IO
} // namespace Libcore

#endif // __LIBCORE_IO_CLIBCORE_H__
