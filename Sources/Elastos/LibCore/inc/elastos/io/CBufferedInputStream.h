
#ifndef __ELASTOS_IO_CBUFFEREDINPUTSTREAM_H__
#define __ELASTOS_IO_CBUFFEREDINPUTSTREAM_H__

#include "_Elastos_IO_CBufferedInputStream.h"
#include "BufferedInputStream.h"

namespace Elastos {
namespace IO {

CarClass(CBufferedInputStream)
    , public BufferedInputStream
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CBUFFEREDINPUTSTREAM_H__
