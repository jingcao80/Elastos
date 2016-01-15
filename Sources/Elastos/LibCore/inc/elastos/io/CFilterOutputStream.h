
#ifndef __ELASTOS_IO_CFILTEROUTPUTSTREAM_H__
#define __ELASTOS_IO_CFILTEROUTPUTSTREAM_H__

#include "_Elastos_IO_CFilterOutputStream.h"
#include "FilterOutputStream.h"

namespace Elastos {
namespace IO {

CarClass(CFilterOutputStream)
    , public FilterOutputStream
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CFILTEROUTPUTSTREAM_H__
