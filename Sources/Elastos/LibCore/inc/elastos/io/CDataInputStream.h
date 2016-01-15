
#ifndef __ELASTOS_IO_CDATAINPUTSTREAM_H__
#define __ELASTOS_IO_CDATAINPUTSTREAM_H__

#include "_Elastos_IO_CDataInputStream.h"
#include "DataInputStream.h"

namespace Elastos {
namespace IO {

CarClass(CDataInputStream)
    , public DataInputStream
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CDATAINPUTSTREAM_H__
