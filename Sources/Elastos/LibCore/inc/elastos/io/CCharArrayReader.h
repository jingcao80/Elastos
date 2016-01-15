
#ifndef __ELASTOS_IO_CCHARARRAYREADER_H__
#define __ELASTOS_IO_CCHARARRAYREADER_H__

#include "_Elastos_IO_CCharArrayReader.h"
#include "CharArrayReader.h"

namespace Elastos {
namespace IO {

CarClass(CCharArrayReader)
    , public CharArrayReader
{
public:
    CAR_OBJECT_DECL()

};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CCHARARRAYREADER_H__
