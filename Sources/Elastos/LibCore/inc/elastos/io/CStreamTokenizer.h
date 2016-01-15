
#ifndef __ELASTOS_IO_CSTREAMTOKENIZER_H__
#define __ELASTOS_IO_CSTREAMTOKENIZER_H__

#include "_Elastos_IO_CStreamTokenizer.h"
#include "StreamTokenizer.h"

namespace Elastos {
namespace IO {

CarClass(CStreamTokenizer)
    , public StreamTokenizer
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CSTREAMTOKENIZER_H__
