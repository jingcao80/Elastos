
#ifndef __ELASTOS_IO_CPIPEDWRITER_H__
#define __ELASTOS_IO_CPIPEDWRITER_H__

#include "_Elastos_IO_CPipedWriter.h"
#include "PipedWriter.h"

namespace Elastos {
namespace IO {

CarClass(CPipedWriter)
    , public PipedWriter
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CPIPEDWRITER_H__