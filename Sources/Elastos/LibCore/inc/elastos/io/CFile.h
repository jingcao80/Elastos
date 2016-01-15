#ifndef __ELASTOS_IO_CFILE_H__
#define __ELASTOS_IO_CFILE_H__

#include "_Elastos_IO_CFile.h"
#include "File.h"

namespace Elastos {
namespace IO {

CarClass(CFile)
    , public File
{
public:
    CAR_OBJECT_DECL()
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CFILE_H__
