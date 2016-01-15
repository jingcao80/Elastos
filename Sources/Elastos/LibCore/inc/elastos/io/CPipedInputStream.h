
#ifndef __ELASTOS_IO_CPIPEDINPUTSTREAM_H__
#define __ELASTOS_IO_CPIPEDINPUTSTREAM_H__

#include "_Elastos_IO_CPipedInputStream.h"
#include "PipedInputStream.h"

namespace Elastos {
namespace IO {

CarClass(CPipedInputStream)
    , public PipedInputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CPIPEDINPUTSTREAM_H__