#ifndef __ELASTOS_DROID_INTERNAL_OS_CATOMICFILE_H__
#define __ELASTOS_DROID_INTERNAL_OS_CATOMICFILE_H__

#include <Elastos.CoreLibrary.IO.h>
#include "_Elastos_Droid_Internal_Os_CAtomicFile.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFile;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileInputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CAtomicFile)
    , public Object
    , public IAtomicFile
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IFile* baseName);

    CARAPI GetBaseFile(
        /* [out] */ IFile** file);

    CARAPI StartWrite(
        /* [out] */ IFileOutputStream** stream);

    CARAPI FinishWrite(
        /* [in] */ IFileOutputStream* str);

    CARAPI FailWrite(
        /* [in] */ IFileOutputStream* str);

    CARAPI OpenAppend(
        /* [out] */ IFileOutputStream** stream);

    CARAPI Truncate();

    CARAPI Exists(
        /* [out] */ Boolean* exists);

    CARAPI Delete();

    CARAPI OpenRead(
        /* [out] */ IFileInputStream** stream);

    CARAPI ReadFully(
        /* [out] */ ArrayOf<Byte>** data);

public:
    AutoPtr<IFile> mBaseName;
    AutoPtr<IFile> mBackupName;
};

} // Os
} // Internal
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_INTERNAL_OS_CATOMICFILE_H__
