
#include "elastos/droid/internal/content/CNativeLibraryHelperHandle.h"
#include "elastos/droid/internal/content/CNativeLibraryHelper.h"
// #include "elastos/droid/content/pm/PackageParser.h"

// using Elastos::Droid::Content::Pm::PackageParser;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::IO::EIID_ICloseable;

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
        assert(0 && "TODO: need PackageParser");
        return NOERROR;
        // AutoPtr<PackageParser::PackageLite> lite = PackageParser::ParsePackageLite(packageFile, 0);
        // return Create((Handle32)lite.Get());
    // } catch (PackageParserException e) {
    //     throw new IOException("Failed to parse package: " + packageFile, e);
    // }
}

ECode CNativeLibraryHelperHandle::Create(
    /* [in] */ Handle32 _pkg,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    // AutoPtr<PackageParser::Package> pkg = (PackageParser::Package*)_pkg;
    // Int32 flags;
    // pkg->mApplicationInfo->GetFlags(&flags);
    // return Create(pkg->GetAllCodePaths(), (flags & IApplicationInfo::FLAG_MULTIARCH) != 0);
    assert(0 && "TODO: need PackageParser");
    return NOERROR;
}

ECode CNativeLibraryHelperHandle::Create(
    /* [in] */ IPackageLite* _lite,
    /* [out] */ INativeLibraryHelperHandle** handle)
{
    // AutoPtr<PackageParser::PackageLite> lite = (PackageParser::PackageLite*)_lite;
    // return Create(lite->GetAllCodePaths(), lite->mMultiArch);
    assert(0 && "TODO: need PackageParser");
    return NOERROR;
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
    mGuard->Open(String("close"));
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
