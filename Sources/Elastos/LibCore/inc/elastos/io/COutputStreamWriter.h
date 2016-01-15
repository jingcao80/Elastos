
#ifndef __ELASTOS_IO_COUTPUTSTREAMWRITER_H__
#define __ELASTOS_IO_COUTPUTSTREAMWRITER_H__

#include "_Elastos_IO_COutputStreamWriter.h"
#include "OutputStreamWriter.h"

namespace Elastos {
namespace IO {

CarClass(COutputStreamWriter)
    , public OutputStreamWriter
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_COUTPUTSTREAMWRITER_H__
