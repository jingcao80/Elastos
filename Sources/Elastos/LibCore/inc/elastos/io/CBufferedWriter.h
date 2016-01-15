
#ifndef __ELASTOS_IO_CBUFFEREDWRITER_H__
#define __ELASTOS_IO_CBUFFEREDWRITER_H__

#include "_Elastos_IO_CBufferedWriter.h"
#include "BufferedWriter.h"

namespace Elastos {
namespace IO {

CarClass(CBufferedWriter)
    ,public BufferedWriter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CBUFFEREDWRITER_H__
