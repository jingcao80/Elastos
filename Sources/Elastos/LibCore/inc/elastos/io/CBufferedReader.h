
#ifndef __ELASTOS_IO_CBUFFEREDREADER_H__
#define __ELASTOS_IO_CBUFFEREDREADER_H__

#include "_Elastos_IO_CBufferedReader.h"
#include "BufferedReader.h"

namespace Elastos {
namespace IO {

CarClass(CBufferedReader)
    , public BufferedReader
{
public:
    CAR_OBJECT_DECL()
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CBUFFEREDREADER_H__
