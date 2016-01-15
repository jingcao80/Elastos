
#ifndef __ELASTOS_IO_CFILEOUTPUTSTREAM_H__
#define __ELASTOS_IO_CFILEOUTPUTSTREAM_H__

#include "_Elastos_IO_CFileOutputStream.h"
#include "FileOutputStream.h"

namespace Elastos {
namespace IO {

CarClass(CFileOutputStream)
    , public FileOutputStream
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CFILEOUTPUTSTREAM_H__
