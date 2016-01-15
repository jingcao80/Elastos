
#ifndef __ELASTOS_IO_CPIPEDOUTPUTSTREAM_H__
#define __ELASTOS_IO_CPIPEDOUTPUTSTREAM_H__

#include "_Elastos_IO_CPipedOutputStream.h"
#include "PipedOutputStream.h"

namespace Elastos {
namespace IO {

CarClass(CPipedOutputStream)
    , public PipedOutputStream
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CPIPEDOUTPUTSTREAM_H__