
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CEnvironment.h"
#include "elastos/droid/os/Environment.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CEnvironment, Singleton, IEnvironment)

CAR_SINGLETON_IMPL(CEnvironment)

ECode CEnvironment::GetRootDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetRootDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetOemDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetOemDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetVendorDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetVendorDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetPrebundledDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetPrebundledDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetSystemSecureDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetSystemSecureDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetSecureDataDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetSecureDataDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetMediaStorageDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetMediaStorageDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetUserSystemDirectory(
    /* [in] */ Int32 userId,
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetUserSystemDirectory(userId);
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetUserConfigDirectory(
    /* [in] */ Int32 userId,
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetUserConfigDirectory(userId);
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::IsEncryptedFilesystemEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Environment::IsEncryptedFilesystemEnabled();
    return NOERROR;
}

ECode CEnvironment::GetDataDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetDataDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetExternalStorageDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetExternalStorageDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetSecondaryStorageDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetSecondaryStorageDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetLegacyExternalStorageDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetLegacyExternalStorageDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetLegacyExternalStorageObbDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetLegacyExternalStorageObbDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetEmulatedStorageSource(
    /* [in] */ Int32 userId,
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetEmulatedStorageSource(userId);
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetEmulatedStorageObbSource(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetEmulatedStorageObbSource();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetExternalStoragePublicDirectory(
    /* [in] */ const String& type,
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetExternalStoragePublicDirectory(type);
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::BuildExternalStorageAndroidDataDirs(
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    VALIDATE_NOT_NULL(files)
    AutoPtr< ArrayOf<IFile*> > f = Environment::BuildExternalStorageAndroidDataDirs();
    *files = f;
    REFCOUNT_ADD(*files)
    return NOERROR;
}

ECode CEnvironment::BuildExternalStorageAppDataDirs(
    /* [in] */ const String& packageName,
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    VALIDATE_NOT_NULL(files)
    AutoPtr< ArrayOf<IFile*> > f = Environment::BuildExternalStorageAppDataDirs(packageName);
    *files = f;
    REFCOUNT_ADD(*files)
    return NOERROR;
}

ECode CEnvironment::BuildExternalStorageAppMediaDirs(
    /* [in] */ const String& packageName,
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    VALIDATE_NOT_NULL(files)
    AutoPtr< ArrayOf<IFile*> > f = Environment::BuildExternalStorageAppMediaDirs(packageName);
    *files = f;
    REFCOUNT_ADD(*files)
    return NOERROR;
}

ECode CEnvironment::BuildExternalStorageAppObbDirs(
    /* [in] */ const String& packageName,
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    VALIDATE_NOT_NULL(files)
    AutoPtr< ArrayOf<IFile*> > f = Environment::BuildExternalStorageAppObbDirs(packageName);
    *files = f;
    REFCOUNT_ADD(*files)
    return NOERROR;
}

ECode CEnvironment::BuildExternalStorageAppFilesDirs(
    /* [in] */ const String& packageName,
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    VALIDATE_NOT_NULL(files)
    AutoPtr< ArrayOf<IFile*> > f = Environment::BuildExternalStorageAppFilesDirs(packageName);
    *files = f;
    REFCOUNT_ADD(*files)
    return NOERROR;
}

ECode CEnvironment::BuildExternalStorageAppCacheDirs(
    /* [in] */ const String& packageName,
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    VALIDATE_NOT_NULL(files)
    AutoPtr< ArrayOf<IFile*> > f = Environment::BuildExternalStorageAppCacheDirs(packageName);
    *files = f;
    REFCOUNT_ADD(*files)
    return NOERROR;
}

ECode CEnvironment::GetDownloadCacheDirectory(
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir)
    AutoPtr<IFile> f = Environment::GetDownloadCacheDirectory();
    *dir = f;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode CEnvironment::GetExternalStorageState(
    /* [out] */ String* state)
{
    VALIDATE_NOT_NULL(state)
    *state = Environment::GetExternalStorageState();
    return NOERROR;
}

ECode CEnvironment::GetStorageState(
    /* [in] */ IFile* path,
    /* [out] */ String* state)
{
    VALIDATE_NOT_NULL(state)
    *state = Environment::GetStorageState(path);
    return NOERROR;
}

ECode CEnvironment::GetSecondaryStorageState(
    /* [out] */ String* state)
{
    VALIDATE_NOT_NULL(state)
    *state = Environment::GetSecondaryStorageState();
    return NOERROR;
}

ECode CEnvironment::GetExternalStorageState(
    /* [in] */ IFile* path,
    /* [out] */ String* state)
{
    VALIDATE_NOT_NULL(state)
    *state = Environment::GetExternalStorageState(path);
    return NOERROR;
}

ECode CEnvironment::IsExternalStorageRemovable(
    /* [in] */ IFile* path,
    /* [out] */ Boolean* isRemovable)
{
    VALIDATE_NOT_NULL(isRemovable)
    *isRemovable = Environment::IsExternalStorageRemovable(path);
    return NOERROR;
}

ECode CEnvironment::IsNoEmulatedStorageExist(
    /* [out] */ Boolean* isExist)
{
    VALIDATE_NOT_NULL(isExist)
    *isExist = Environment::IsNoEmulatedStorageExist();
    return NOERROR;
}

ECode CEnvironment::IsExternalStorageRemovable(
    /* [out] */ Boolean* isRemovable)
{
    VALIDATE_NOT_NULL(isRemovable)
    *isRemovable = Environment::IsExternalStorageRemovable();
    return NOERROR;
}

ECode CEnvironment::IsExternalStorageEmulated(
    /* [out] */ Boolean* isEmulated)
{
    VALIDATE_NOT_NULL(isEmulated)
    *isEmulated = Environment::IsExternalStorageEmulated();
    return NOERROR;
}

ECode CEnvironment::IsExternalStorageEmulated(
    /* [in] */ IFile* path,
    /* [out] */ Boolean* isEmulated)
{
    VALIDATE_NOT_NULL(isEmulated)
    *isEmulated = Environment::IsExternalStorageEmulated(path);
    return NOERROR;
}

ECode CEnvironment::SetUserRequired(
    /* [in] */ Boolean userRequired)
{
    return Environment::SetUserRequired(userRequired);
}

ECode CEnvironment::BuildPaths(
    /* [in] */ ArrayOf<IFile*>* base,
    /* [in] */ ArrayOf<String>* segments,
    /* [out, callee] */ ArrayOf<IFile*>** files)
{
    VALIDATE_NOT_NULL(files)
    AutoPtr<ArrayOf<IFile*> > tmp = Environment::BuildPaths(base, segments);
    *files = tmp;
    REFCOUNT_ADD(*files)
    return NOERROR;
}

ECode CEnvironment::BuildPath(
    /* [in] */ IFile* base,
    /* [in] */ ArrayOf<String>* segments,
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)
    AutoPtr<IFile> tmp = Environment::BuildPath(base, segments);
    *file = tmp;
    REFCOUNT_ADD(*file)
    return NOERROR;
}

ECode CEnvironment::MaybeTranslateEmulatedPathToInternal(
    /* [in] */ IFile* path,
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)
    AutoPtr<IFile> tmp = Environment::MaybeTranslateEmulatedPathToInternal(path);
    *file = tmp;
    REFCOUNT_ADD(*file)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
