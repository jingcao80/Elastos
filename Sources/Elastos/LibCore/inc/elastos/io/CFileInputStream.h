
#ifndef __ELASTOS_IO_CFILEINPUTSTREAM_H__
#define __ELASTOS_IO_CFILEINPUTSTREAM_H__

#include "_Elastos_IO_CFileInputStream.h"
#include "FileInputStream.h"

namespace Elastos {
namespace IO {

CarClass(CFileInputStream)
    , public FileInputStream
{
public:
    CAR_OBJECT_DECL()
    
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CFILEINPUTSTREAM_H__
