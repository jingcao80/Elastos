
#ifndef __ELASTOS_IO_CLINENUMBERREADER_H__
#define __ELASTOS_IO_CLINENUMBERREADER_H__

#include "_Elastos_IO_CLineNumberReader.h"
#include "LineNumberReader.h"

namespace Elastos {
namespace IO {

CarClass(CLineNumberReader)
    , public LineNumberReader
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CLINENUMBERREADER_H__
