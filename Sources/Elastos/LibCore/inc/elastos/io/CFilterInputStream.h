
#ifndef __ELASTOS_IO_CFILTERINPUTSTREAM_H__
#define __ELASTOS_IO_CFILTERINPUTSTREAM_H__

#include "_Elastos_IO_CFilterInputStream.h"
#include "FilterInputStream.h"

namespace Elastos {
namespace IO {

CarClass(CFilterInputStream)
    , public FilterInputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CFILTERINPUTSTREAM_H__
