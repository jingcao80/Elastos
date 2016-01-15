
#ifndef __ELASTOS_IO_CSTRINGBUFFERINPUTSTREAM_H__
#define __ELASTOS_IO_CSTRINGBUFFERINPUTSTREAM_H__

#include "_Elastos_IO_CStringBufferInputStream.h"
#include "StringBufferInputStream.h"

namespace Elastos {
namespace IO {

CarClass(CStringBufferInputStream)
    , public StringBufferInputStream
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CSTRINGBUFFERINPUTSTREAM_H__
