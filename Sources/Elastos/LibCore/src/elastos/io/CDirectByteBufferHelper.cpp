#include "elastos/io/CDirectByteBufferHelper.h"
#include "elastos/io/DirectByteBuffer.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CDirectByteBufferHelper, Singleton, IDirectByteBufferHelper)

CAR_SINGLETON_IMPL(CDirectByteBufferHelper)

ECode CDirectByteBufferHelper::GetNewInstance(
    /* [in] */ Int64 address,
    /* [in] */ Int32 capacity,
    /* [out] */ IDirectByteBuffer** instance)
{
    AutoPtr<DirectByteBuffer> buf = new DirectByteBuffer();
    buf->constructor(address, capacity);
    *instance = buf;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

}
}