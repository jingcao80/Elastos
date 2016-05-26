#ifndef __ELASTOS_IO_CFILEDESCRIPTORHELPER_H__
#define __ELASTOS_IO_CFILEDESCRIPTORHELPER_H__

#include "_Elastos_IO_CFileDescriptorHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace IO {

CarClass(CFileDescriptorHelper)
    , public Singleton
    , public IFileDescriptorHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInFile(
        /* [out] */ IFileDescriptor** inFile);

    CARAPI GetOutFile(
        /* [out] */ IFileDescriptor** outFile);

    CARAPI GetErrFile(
        /* [out] */ IFileDescriptor** errFile);
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CFILEDESCRIPTORHELPER_H__
