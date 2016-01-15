
#ifndef __ELASTOS_IO_CPUSHBACKREADER_H__
#define __ELASTOS_IO_CPUSHBACKREADER_H__

#include "_Elastos_IO_CPushbackReader.h"
#include "PushbackReader.h"

namespace Elastos {
namespace IO {

CarClass(CPushbackReader)
    , public PushbackReader
{
public:
    CAR_OBJECT_DECL()
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CPUSHBACKREADER_H__
