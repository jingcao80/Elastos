
#include "elastos/droid/internal/content/CNativeLibraryHelperHandle.h"
#include "elastos/droid/internal/content/CNativeLibraryHelper.h"
#include "elastos/droid/content/pm/PackageParser.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::PackageParser;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CAR_OBJECT_IMPL(CNativeLibraryHelperHandle);
CAR_INTERFACE_IMPL_2(CNativeLibraryHelperHandle, Object, INativeLibraryHelperHandle, ICloseable);

ECode CNativeLibraryHelperHandle::Create(
    /* [in] */ IFile* packageFile,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    // try {
    AutoPtr<ArrayOf<Byte> > readBuffer = ArrayOf<Byte>::Alloc(PackageParser::CERTIFICATE_BUFFER_SIZE);
    AutoPtr<PackageParser::PackageLite> lite;
    if (FAILED(PackageParser::ParsePackageLite(packageFile, 0, readBuffer, (PackageParser::PackageLite**)&lite))) {
        Slogger::E("CNativeLibraryHelperHandle", "Failed to parse package: %p", packageFile);
        return E_IO_EXCEPTION;
    }
    return CreatePackageLite((Handle64)lite.Get(), handle);
    // } catch (PackageParserException e) {
    //     throw new IOException("Failed to parse package: " + packageFile, e);
    // }
}

ECode CNativeLibraryHelperHandle::CreatePackage(
    /* [in] */ Handle64 _pkg,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    AutoPtr<PackageParser::Package> pkg = (PackageParser::Package*)_pkg;
    Int32 flags;
    pkg->mApplicationInfo->GetFlags(&flags);
    return Create(pkg->GetAllCodePaths(), (flags & IApplicationInfo::FLAG_MULTIARCH) != 0, handle);
}

ECode CNativeLibraryHelperHandle::CreatePackageLite(
    /* [in] */ Handle64 _lite,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    AutoPtr<PackageParser::PackageLite> lite = (PackageParser::PackageLite*)_lite;
    return Create(lite->GetAllCodePaths(), lite->mMultiArch, handle);
}

ECode CNativeLibraryHelperHandle::Create(
    /* [in] */ List<String>* codePaths,
    /* [in] */ Boolean multiArch,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    Int32 size = codePaths->GetSize();
    AutoPtr<ArrayOf<Int64> > apkHandles = ArrayOf<Int64>::Alloc(size);
    List<String>::Iterator iter = codePaths->Begin();
    for (Int32 i = 0; iter != codePaths->End(); ++iter, i++) {
        String path = *iter;
        (*apkHandles)[i] = CNativeLibraryHelper::NativeOpenApk(path);
        if ((*apkHandles)[i] == 0) {
            // Unwind everything we've opened so far
            for (Int32 j = 0; j < i; j++) {
                CNativeLibraryHelper::NativeClose((*apkHandles)[j]);
            }
            // throw new IOException("Unable to open APK: " + path);
            return E_IO_EXCEPTION;
        }
    }

    return CNativeLibraryHelperHandle::New(apkHandles, multiArch, handle);
}

CNativeLibraryHelperHandle::CNativeLibraryHelperHandle()
    : mClosed(FALSE)
{
    AutoPtr<ICloseGuardHelper> cgHelper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&cgHelper);
    cgHelper->Get((ICloseGuard**)&mGuard);
}

CNativeLibraryHelperHandle::~CNativeLibraryHelperHandle()
{
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }

    if (!mClosed) {
        Close();
    }
}

ECode CNativeLibraryHelperHandle::constructor(
    /* [in] */ ArrayOf<Int64>* apkHandles,
    /* [in] */ Boolean multiArch)
{
    mApkHandles = apkHandles;
    mMultiArch = multiArch;
    mGuard->Open(String("CNativeLibraryHelperHandle::Close"));
    return NOERROR;
}

ECode CNativeLibraryHelperHandle::Close()
{
    for (Int32 i = 0; i < mApkHandles->GetLength(); i++) {
        Int64 apkHandle = (*mApkHandles)[i];
        CNativeLibraryHelper::NativeClose(apkHandle);
    }
    mGuard->Close();
    mClosed = TRUE;
    return NOERROR;
}

} // namespace Content
} // namespace Internal
} // namespace Droid
} // namespace Elastos
