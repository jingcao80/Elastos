
#ifndef __ELASTOS_IO_CINPUTSTREAMREADER_H__
#define __ELASTOS_IO_CINPUTSTREAMREADER_H__

#include "_Elastos_IO_CInputStreamReader.h"
#include "InputStreamReader.h"

namespace Elastos {
namespace IO {

CarClass(CInputStreamReader)
    , public InputStreamReader
{
public:
    CAR_OBJECT_DECL()
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CINPUTSTREAMREADER_H__
