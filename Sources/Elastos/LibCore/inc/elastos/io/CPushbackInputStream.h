
#ifndef __ELASTOS_IO_CPUSHBACKINPUTSTREAM_H__
#define __ELASTOS_IO_CPUSHBACKINPUTSTREAM_H__

#include "_Elastos_IO_CPushbackInputStream.h"
#include "PushbackInputStream.h"

namespace Elastos {
namespace IO {

CarClass(CPushbackInputStream)
    , public PushbackInputStream
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CPUSHBACKINPUTSTREAM_H__
