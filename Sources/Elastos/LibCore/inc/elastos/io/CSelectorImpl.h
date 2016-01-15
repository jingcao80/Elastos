#ifndef __ELASTOS_IO_CSELECTORIMPL_H__
#define __ELASTOS_IO_CSELECTORIMPL_H__

#include "_Elastos_IO_CSelectorImpl.h"
#include "SelectorImpl.h"

namespace Elastos {
namespace IO {

CarClass(CSelectorImpl)
    , public SelectorImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CSELECTORIMPL_H__
