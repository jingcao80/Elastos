#ifndef __ELASTOS_IO_CSTRINGWRITER_H__
#define __ELASTOS_IO_CSTRINGWRITER_H__

#include "_Elastos_IO_CStringWriter.h"
#include "StringWriter.h"

namespace Elastos {
namespace IO {

CarClass(CStringWriter)
    , public StringWriter
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CSTRINGWRITER_H__
