#ifndef __ELASTOS_IO_CDIRECTBYTEBUFFERHELPER_H__
#define __ELASTOS_IO_CDIRECTBYTEBUFFERHELPER_H__

#include "_Elastos_IO_CDirectByteBufferHelper.h"
#include "elastos/core/Singleton.h"
#include "Elastos.CoreLibrary.IO.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace IO {

CarClass(CDirectByteBufferHelper)
    , public Singleton
    , public IDirectByteBufferHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetNewInstance(
        /* [in] */ Int64 address,
        /* [in] */ Int32 capacity,
        /* [out] */ IDirectByteBuffer** instance);
};

}
}

#endif