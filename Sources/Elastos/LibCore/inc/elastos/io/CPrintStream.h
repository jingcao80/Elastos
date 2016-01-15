
#ifndef __ELASTOS_IO_CPRINTSTREAM_H__
#define __ELASTOS_IO_CPRINTSTREAM_H__

#include "_Elastos_IO_CPrintStream.h"
#include "PrintStream.h"

namespace Elastos {
namespace IO {

CarClass(CPrintStream)
    , public PrintStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CPRINTSTREAM_H__
