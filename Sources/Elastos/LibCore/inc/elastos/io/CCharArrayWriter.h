
#ifndef __ELASTOS_IO_CCHARARRAYWRITER_H__
#define __ELASTOS_IO_CCHARARRAYWRITER_H__

#include "_Elastos_IO_CCharArrayWriter.h"
#include "CharArrayWriter.h"

namespace Elastos {
namespace IO {

CarClass(CCharArrayWriter)
    , public CharArrayWriter
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CCHARARRAYWRITER_H__
