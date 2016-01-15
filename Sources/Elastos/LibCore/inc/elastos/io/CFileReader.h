
#ifndef __ELASTOS_IO_CFILEREADER_H__
#define __ELASTOS_IO_CFILEREADER_H__

#include "_Elastos_IO_CFileReader.h"
#include "FileReader.h"

namespace Elastos {
namespace IO {

CarClass(CFileReader)
    , public FileReader
{
public:
    CAR_OBJECT_DECL()
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CFILEREADER_H__
