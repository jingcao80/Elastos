#ifndef __ELASTOS_DROID_SERVER_PM_INSTALLER_H__
#define __ELASTOS_DROID_SERVER_PM_INSTALLER_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/server/SystemService.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Droid::Content::Pm::IPackageStats;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Internal::Os::IInstallerConnection;
using Elastos::Droid::Server::SystemService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class Installer : public SystemService
{
public:
    Installer(
        /* [in] */ IContext* context);

    CARAPI OnStart();

    CARAPI_(Int32) Install(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid,
        /* [in] */ const String& seinfo);

    CARAPI_(Int32) Patchoat(
        /* [in] */ const String& apkPath,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean isPublic,
        /* [in] */ const String& pkgName,
        /* [in] */ const String& instructionSet);

    CARAPI_(Int32) Patchoat(
        /* [in] */ const String& apkPath,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean isPublic,
        /* [in] */ const String& instructionSet);

    CARAPI_(Int32) Dexopt(
        /* [in] */ const String& apkPath,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean isPublic,
        /* [in] */ const String& instructionSet);

    CARAPI_(Int32) Dexopt(
        /* [in] */ const String& apkPath,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean isPublic,
        /* [in] */ const String& pkgName,
        /* [in] */ const String& instructionSet,
        /* [in] */ Boolean vmSafeMode);

    CARAPI_(Int32) Idmap(
        /* [in] */ const String& targetApkPath,
        /* [in] */ const String& overlayApkPath,
        /* [in] */ const String& cachePath,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 targetHash,
        /* [in] */ Int32 overlayHash);

    CARAPI_(Int32) Aapt(
        /* [in] */ const String& themeApkPath,
        /* [in] */ const String& internalPath,
        /* [in] */ const String& resTablePath,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pkgId,
        /* [in] */ Int32 minSdkVersion,
        /* [in] */ const String& commonResourcesPath);

    CARAPI_(Int32) Movedex(
        /* [in] */ const String& srcPath,
        /* [in] */ const String& dstPath,
        /* [in] */ const String& instructionSet);

    CARAPI_(Int32) Rmdex(
        /* [in] */ const String& codePath,
        /* [in] */ const String& instructionSet);

    CARAPI_(Int32) Remove(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId);

    CARAPI_(Int32) Rename(
        /* [in] */ const String& oldname,
        /* [in] */ const String& newname);

    CARAPI_(Int32) FixUid(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid);

    CARAPI_(Int32) DeleteCacheFiles(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId);

    CARAPI_(Int32) DeleteCodeCacheFiles(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId);

    CARAPI_(Int32) CreateUserData(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 userId,
        /* [in] */ const String& seinfo);

    CARAPI_(Int32) CreateUserConfig(
        /* [in] */ Int32 userId);

    CARAPI_(Int32) RemoveUserDataDirs(
        /* [in] */ Int32 userId);

    CARAPI_(Int32) ClearUserData(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId);

    CARAPI_(Int32) MarkBootComplete(
        /* [in] */ const String& instructionSet);

    CARAPI_(Boolean) Ping();

    CARAPI_(Int32) FreeCache(
        /* [in] */ Int64 freeStorageSize);

    CARAPI_(Int32) GetSizeInfo(
        /* [in] */ const String& pkgName,
        /* [in] */ Int32 persona,
        /* [in] */ const String& apkPath,
        /* [in] */ const String& libDirPath,
        /* [in] */ const String& fwdLockApkPath,
        /* [in] */ const String& asecPath,
        /* [in] */ ArrayOf<String>* instructionSets,
        /* [in] */ IPackageStats* pStats);

    CARAPI_(Int32) MoveFiles();

    /**
     * Links the 32 bit native library directory in an application's data directory to the
     * real location for backward compatibility. Note that no such symlink is created for
     * 64 bit shared libraries.
     *
     * @return -1 on error
     */
    CARAPI_(Int32) LinkNativeLibraryDirectory(
        /* [in] */ const String& dataPath,
        /* [in] */ const String& nativeLibPath32,
        /* [in] */ Int32 userId);

    CARAPI_(Boolean) RestoreconData(
        /* [in] */ const String& pkgName,
        /* [in] */ const String& seinfo,
        /* [in] */ Int32 uid);

private:
    /**
     * Returns true iff. {@code instructionSet} is a valid instruction set.
     */
    static CARAPI_(Boolean) IsValidInstructionSet(
        /* [in] */ const String& instructionSet);

private:
    static const String TAG;

    static const Boolean LOCAL_DEBUG = FALSE;

    AutoPtr<IInstallerConnection> mInstaller;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_INSTALLER_H__
