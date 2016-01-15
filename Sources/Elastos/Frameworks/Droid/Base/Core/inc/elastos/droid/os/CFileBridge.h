#ifndef __ELASTOS_DROID_OS_CFILEBIRDGE_H__
#define __ELASTOS_DROID_OS_CFILEBIRDGE_H__

#include "_Elastos_Droid_Os_CFileBridge.h"
#include "elastos/droid/os/FileBridge.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CFileBridge) , public FileBridge
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CFILEBIRDGE_H__
