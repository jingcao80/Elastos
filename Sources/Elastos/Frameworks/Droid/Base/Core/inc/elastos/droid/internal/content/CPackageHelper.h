
#ifndef __ELASTOS_DROID_INTERNAL_CONTENT_CPACKAGEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_CONTENT_CPACKAGEHELPER_H__

#include "_Elastos_Droid_Internal_Content_CPackageHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageLite;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Core::Singleton;
using Elastos::IO::IFile;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Zip::IZipFile;
using Elastos::Utility::Zip::IZipOutputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CarClass(CPackageHelper)
    , public Singleton
    , public IPackageHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetMountService(
        /* [out] */ IIMountService** mountService);

    CARAPI CreateSdDir(
        /* [in] */ Int64 sizeBytes,
        /* [in] */ const String& cid,
        /* [in] */ const String& sdEncKey,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean isExternal,
        /* [out] */ String* dir);

    CARAPI ResizeSdDir(
        /* [in] */ Int64 sizeBytes,
        /* [in] */ const String& cid,
        /* [in] */ const String& sdEncKey,
        /* [out] */ Boolean* result);

    CARAPI MountSdDir(
        /* [in] */ const String& cid,
        /* [in] */ const String& key,
        /* [in] */ Int32 ownerUid,
        /* [out] */ String* dir);

    CARAPI MountSdDir(
        /* [in] */ const String& cid,
        /* [in] */ const String& key,
        /* [in] */ Int32 ownerUid,
        /* [in] */ Boolean readOnly,
        /* [out] */ String* dir);

    CARAPI UnMountSdDir(
        /* [in] */ const String& cid,
        /* [out] */ Boolean* result);

    CARAPI RenameSdDir(
        /* [in] */ const String& oldId,
        /* [in] */ const String& newId,
        /* [out] */ Boolean* result);

    CARAPI GetSdDir(
        /* [in] */ const String& cid,
        /* [out] */ String* dir);

    CARAPI GetSdFilesystem(
        /* [in] */ const String& cid,
        /* [out] */ String* str);

    CARAPI FinalizeSdDir(
        /* [in] */ const String& cid,
        /* [out] */ Boolean* result);

    CARAPI DestroySdDir(
        /* [in] */ const String& cid,
        /* [out] */ Boolean* result);

    CARAPI GetSecureContainerList(
        /* [out, callee] */ ArrayOf<String>** ids);

    CARAPI IsContainerMounted(
        /* [in] */ const String& cid,
        /* [out] */ Boolean* isMounted);

    CARAPI ExtractPublicFiles(
        /* [in] */ IFile* apkFile,
        /* [in] */ IFile* publicZipFile,
        /* [out] */ Int64* size);

    CARAPI FixSdPermissions(
        /* [in] */ const String& cid,
        /* [in] */ Int32 gid,
        /* [in] */ const String& filename,
        /* [out] */ Boolean* result);

    /**
     * Given a requested {@link PackageInfo#installLocation} and calculated
     * install size, pick the actual location to install the app.
     */
    CARAPI ResolveInstallLocation(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 installLocation,
        /* [in] */ Int64 sizeBytes,
        /* [in] */ Int32 installFlags,
        /* [out] */ Int32* result);

    CARAPI CalculateInstalledSize(
        /* [in] */ IPackageLite* pkg,
        /* [in] */ Boolean isForwardLocked,
        /* [in] */ const String& abiOverride,
        /* [out] */ Int64* size);

    CARAPI CalculateInstalledSize(
        /* [in] */ IPackageLite* pkg,
        /* [in] */ INativeLibraryHelperHandle* handle,
        /* [in] */ Boolean isForwardLocked,
        /* [in] */ const String& abiOverride,
        /* [out] */ Int64* size);

    CARAPI ReplaceEnd(
        /* [in] */ const String& str,
        /* [in] */ const String& before,
        /* [in] */ const String& after,
        /* [out] */ String* result);

private:
    CARAPI CopyZipEntry(
        /* [in] */ IZipEntry* zipEntry,
        /* [in] */ IZipFile* inZipFile,
        /* [in] */ IZipOutputStream* outZipStream);

private:
    static const Boolean localLOGV;
    static const String TAG;
};

} // namespace Content
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_CONTENT_CPACKAGEHELPER_H__
