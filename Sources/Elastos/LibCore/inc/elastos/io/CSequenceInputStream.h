
#ifndef __ELASTOS_IO_CSEQUENCEINPUTSTREAM_H__
#define __ELASTOS_IO_CSEQUENCEINPUTSTREAM_H__

#include "_Elastos_IO_CSequenceInputStream.h"
#include "SequenceInputStream.h"

namespace Elastos {
namespace IO {

CarClass(CSequenceInputStream)
    , public SequenceInputStream
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CSEQUENCEINPUTSTREAM_H__
