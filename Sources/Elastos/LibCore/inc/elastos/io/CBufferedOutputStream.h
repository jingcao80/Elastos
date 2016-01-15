
#ifndef __ELASTOS_IO_CBUFFEREDOUTPUTSTREAM_H__
#define __ELASTOS_IO_CBUFFEREDOUTPUTSTREAM_H__

#include "_Elastos_IO_CBufferedOutputStream.h"
#include "BufferedOutputStream.h"

namespace Elastos {
namespace IO {

CarClass(CBufferedOutputStream)
    , public BufferedOutputStream
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CBUFFEREDOUTPUTSTREAM_H__
