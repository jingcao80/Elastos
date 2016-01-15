#ifndef __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTOR_H__
#define __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTOR_H__

#include "_Elastos_Droid_Os_CParcelFileDescriptor.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"

using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CParcelFileDescriptor)
    , public ParcelFileDescriptor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTOR_H__
