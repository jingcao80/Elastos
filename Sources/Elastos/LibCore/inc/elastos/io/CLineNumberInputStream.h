
#ifndef __ELASTOS_IO_CLINENUMBERINPUTSTREAM_H__
#define __ELASTOS_IO_CLINENUMBERINPUTSTREAM_H__

#include "_Elastos_IO_CLineNumberInputStream.h"
#include "LineNumberInputStream.h"

namespace Elastos {
namespace IO {

CarClass(CLineNumberInputStream)
    , public LineNumberInputStream
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CLINENUMBERINPUTSTREAM_H__
