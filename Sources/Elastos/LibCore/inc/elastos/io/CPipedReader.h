
#ifndef __ELASTOS_IO_CPIPEDREADER_H__
#define __ELASTOS_IO_CPIPEDREADER_H__

#include "_Elastos_IO_CPipedReader.h"
#include "PipedReader.h"

namespace Elastos {
namespace IO {

CarClass(CPipedReader)
    , public PipedReader
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CPIPEDREADER_H__