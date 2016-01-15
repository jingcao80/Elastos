
#ifndef __ELASTOS_IO_CSTRINGREADER_H__
#define __ELASTOS_IO_CSTRINGREADER_H__

#include "_Elastos_IO_CStringReader.h"
#include "StringReader.h"

namespace Elastos {
namespace IO {

CarClass(CStringReader)
    , public StringReader
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CSTRINGREADER_H__
