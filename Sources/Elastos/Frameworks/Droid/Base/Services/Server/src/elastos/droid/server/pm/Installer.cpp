
#include "pm/Installer.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::Internal::Os::CInstallerConnection;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Os::Build;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const String Installer::TAG("Installer");
const Boolean Installer::LOCAL_DEBUG;

Installer::Installer(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    CInstallerConnection::New((IInstallerConnection**)&mInstaller);
}

ECode Installer::OnStart()
{
    Slogger::I(TAG, "Waiting for installd to be ready.");
    Ping();
    return NOERROR;
}

Int32 Installer::Install(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid,
    /* [in] */ const String& seinfo)
{
    StringBuilder builder("install");
    builder.AppendChar(' ');
    builder.Append(name);
    builder.AppendChar(' ');
    builder.Append(uid);
    builder.AppendChar(' ');
    builder.Append(gid);
    builder.AppendChar(' ');
    builder.Append(!seinfo.IsNull() ? seinfo : "!");
    Int32 result;
    mInstaller->Execute(builder.ToString(), &result);
    return result;
}

Int32 Installer::Patchoat(
    /* [in] */ const String& apkPath,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isPublic,
    /* [in] */ const String& pkgName,
    /* [in] */ const String& instructionSet)
{
    if (!IsValidInstructionSet(instructionSet)) {
        Slogger::E(TAG, "Invalid instruction set: %s", instructionSet.string());
        return -1;
    }

    Int32 result;
    mInstaller->Patchoat(apkPath, uid, isPublic, pkgName, instructionSet, &result);
    return result
}

Int32 Installer::Patchoat(
    /* [in] */ const String& apkPath,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isPublic,
    /* [in] */ const String& instructionSet)
{
    if (!IsValidInstructionSet(instructionSet)) {
        Slogger::E(TAG, "Invalid instruction set: %s", instructionSet.string());
        return -1;
    }

    Int32 result;
    mInstaller->Patchoat(apkPath, uid, isPublic, instructionSet, &result);
    return result
}

Int32 Installer::Dexopt(
    /* [in] */ const String& apkPath,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isPublic,
    /* [in] */ const String& instructionSet)
{
    if (!IsValidInstructionSet(instructionSet)) {
        Slogger::E(TAG, "Invalid instruction set: %s", instructionSet.string());
        return -1;
    }

    Int32 result;
    mInstaller->Dexopt(apkPath, uid, isPublic, instructionSet, &result);
    return result;
}

Int32 Installer::Dexopt(
    /* [in] */ const String& apkPath,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isPublic,
    /* [in] */ const String& pkgName,
    /* [in] */ const String& instructionSet,
    /* [in] */ Boolean vmSafeMode)
{
    if (!IsValidInstructionSet(instructionSet)) {
        Slogger::E(TAG, "Invalid instruction set: %s", instructionSet.string());
        return -1;
    }

    Int32 result;
    mInstaller->Dexopt(apkPath, uid, isPublic, pkgName, instructionSet, vmSafeMode, &result);
    return result;
}

Int32 Installer::Idmap(
    /* [in] */ const String& targetApkPath,
    /* [in] */ const String& overlayApkPath,
    /* [in] */ Int32 uid)
{
    StringBuilder builder("idmap");
    builder.AppendChar(' ');
    builder.Append(targetApkPath);
    builder.AppendChar(' ');
    builder.Append(overlayApkPath);
    builder.AppendChar(' ');
    builder.Append(uid);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::Movedex(
    /* [in] */ const String& srcPath,
    /* [in] */ const String& dstPath,
    /* [in] */ const String& instructionSet)
{
    if (!IsValidInstructionSet(instructionSet)) {
        Slogger::E(TAG, "Invalid instruction set: %s", instructionSet.string());
        return -1;
    }

    StringBuilder builder("movedex");
    builder.AppendChar(' ');
    builder.Append(srcPath);
    builder.AppendChar(' ');
    builder.Append(dstPath);
    builder.AppendChar(' ');
    builder.Append(instructionSet);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::Rmdex(
    /* [in] */ const String& codePath,
    /* [in] */ const String& instructionSet)
{
    if (!IsValidInstructionSet(instructionSet)) {
        Slogger::E(TAG, "Invalid instruction set: %s", instructionSet.string());
        return -1;
    }

    StringBuilder builder("rmdex");
    builder.AppendChar(' ');
    builder.Append(codePath);
    builder.AppendChar(' ');
    builder.Append(instructionSet);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::Remove(
    /* [in] */ const String& name,
    /* [in] */ Int32 userId)
{
    StringBuilder builder("remove");
    builder.AppendChar(' ');
    builder.Append(name);
    builder.AppendChar(' ');
    builder.Append(userId);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::Rename(
    /* [in] */ const String& oldname,
    /* [in] */ const String& newname)
{
    StringBuilder builder("rename");
    builder.AppendChar(' ');
    builder.Append(oldname);
    builder.AppendChar(' ');
    builder.Append(newname);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::FixUid(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    StringBuilder builder("fixuid");
    builder.AppendChar(' ');
    builder.Append(name);
    builder.AppendChar(' ');
    builder.Append(uid);
    builder.AppendChar(' ');
    builder.Append(gid);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::DeleteCacheFiles(
    /* [in] */ const String& name,
    /* [in] */ Int32 userId)
{
    StringBuilder builder("rmcache");
    builder.AppendChar(' ');
    builder.Append(name);
    builder.AppendChar(' ');
    builder.Append(userId);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::DeleteCodeCacheFiles(
    /* [in] */ const String& name,
    /* [in] */ Int32 userId)
{
    StringBuilder builder("rmcodecache");
    builder.AppendChar(' ');
    builder.Append(name);
    builder.AppendChar(' ');
    builder.Append(userId);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::CreateUserData(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 userId,
    /* [in] */ const String& seinfo)
{
    StringBuilder builder("mkuserdata");
    builder.AppendChar(' ');
    builder.Append(name);
    builder.AppendChar(' ');
    builder.Append(uid);
    builder.AppendChar(' ');
    builder.Append(userId);
    builder.AppendChar(' ');
    builder.Append(!seinfo.IsNull() ? seinfo : "!");
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::CreateUserConfig(
    /* [in] */ Int32 userId)
{
    StringBuilder builder("mkuserconfig");
    builder.AppendChar(' ');
    builder.Append(userId);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::RemoveUserDataDirs(
    /* [in] */ Int32 userId)
{
    StringBuilder builder;
    builder.AppendCStr("rmuser");
    builder.AppendChar(' ');
    builder.Append(userId);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::ClearUserData(
    /* [in] */ const String& name,
    /* [in] */ Int32 userId)
{
    StringBuilder builder;
    builder.AppendCStr("rmuserdata");
    builder.AppendChar(' ');
    builder.Append(name);
    builder.AppendChar(' ');
    builder.Append(userId);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::MarkBootComplete(
    /* [in] */ const String& instructionSet)
{
    if (!IsValidInstructionSet(instructionSet)) {
        Slogger::E(TAG, "Invalid instruction set: %s", instructionSet.string());
        return -1;
    }

    StringBuilder builder("markbootcomplete");
    builder.AppendChar(' ');
    builder.Append(instructionSet);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Boolean Installer::Ping()
{
    Int32 result;
    if (mInstaller->Execute(String("ping"), &result), result < 0) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

Int32 Installer::FreeCache(
    /* [in] */ Int64 freeStorageSize)
{
    StringBuilder builder;
    builder.Append("freecache");
    builder.AppendChar(' ');
    builder.Append(StringUtils::Int64ToString(freeStorageSize));
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Int32 Installer::GetSizeInfo(
    /* [in] */ const String& pkgName,
    /* [in] */ Int32 persona,
    /* [in] */ const String& apkPath,
    /* [in] */ const String& libDirPath,
    /* [in] */ const String& fwdLockApkPath,
    /* [in] */ const String& asecPath,
    /* [in] */ ArrayOf<String>* instructionSets,
    /* [in] */ IPackageStats* pStats)
{
    for (Int32 i = 0; i < instructionSets->GetLength(); ++i) {
        String instructionSet = (*instructionSets)[i];
        if (!IsValidInstructionSet(instructionSet)) {
            Slogger::E(TAG, "Invalid instruction set: %s", instructionSet.string());
            return -1;
        }
    }

    StringBuilder builder;
    builder.Append("getsize");
    builder.AppendChar(' ');
    builder.Append(pkgName);
    builder.AppendChar(' ');
    builder.Append(persona);
    builder.AppendChar(' ');
    builder.Append(apkPath);
    builder.AppendChar(' ');
    // TODO: Extend getSizeInfo to look at the full subdirectory tree,
    // not just the first level.
    builder.Append(!libDirPath.IsNull() ? libDirPath : "!");
    builder.AppendChar(' ');
    builder.Append(!fwdLockApkPath.IsNull() ? fwdLockApkPath : "!");
    builder.AppendChar(' ');
    builder.Append(!asecPath.IsNull() ? asecPath : "!");
    builder.AppendChar(' ');
    // TODO: Extend getSizeInfo to look at *all* instrution sets, not
    // just the primary.
    builder.Append((*instructionSets)[0]);

    String s;
    mInstaller->Transaction(builder.ToString(), &s);
    AutoPtr< ArrayOf<String> > res;
    StringUtils::Split(s, String(" "), (ArrayOf<String>**)&res);

    if ((res == NULL) || (res->GetLength() != 5)) {
        return -1;
    }
    // try {
    pStats->SetCodeSize(StringUtils::ParseInt64((*res)[1]));
    pStats->SetDataSize(StringUtils::ParseInt64((*res)[2]));
    pStats->SetCacheSize(StringUtils::ParseInt64((*res)[3]));
    pStats->SetExternalCodeSize(StringUtils::ParseInt64((*res)[4]));
    return StringUtils::ParseInt32((*res)[0], 10, -1);
    // } catch (NumberFormatException e) {
    //     return -1;
    // }
}

Int32 Installer::MoveFiles()
{
    Int32 result;
    mInstaller->Execute(String("movefiles"), &result);
    return result;
}

Int32 Installer::LinkNativeLibraryDirectory(
    /* [in] */ const String& dataPath,
    /* [in] */ const String& nativeLibPath32,
    /* [in] */ Int32 userId)
{
    if (dataPath.IsNull()) {
        Slogger::E(TAG, "linkNativeLibraryDirectory dataPath is null");
        return -1;
    }
    else if (nativeLibPath32.IsNull()) {
        Slogger::E(TAG, "linkNativeLibraryDirectory nativeLibPath is null");
        return -1;
    }

    StringBuilder builder;
    builder.Append("linklib ");
    builder.Append(dataPath);
    builder.AppendChar(' ');
    builder.Append(nativeLibPath32);
    builder.AppendChar(' ');
    builder.Append(userId);

    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result;
}

Boolean Installer::RestoreconData(
    /* [in] */ const String& pkgName,
    /* [in] */ const String& seinfo,
    /* [in] */ Int32 uid)
{
    StringBuilder builder("restorecondata");
    builder.AppendChar(' ');
    builder.Append(pkgName);
    builder.AppendChar(' ');
    builder.Append(!seinfo.IsNull() ? seinfo : "!");
    builder.AppendChar(' ');
    builder.Append(uid);
    Int32 result;
    mInstaller->Execute(builder.toString(), &result);
    return result == 0;
}

Boolean Installer::IsValidInstructionSet(
    /* [in] */ const String& instructionSet)
{
    if (instructionSet.IsNull()) {
        return FALSE;
    }

    for (Int32 i = 0; i < Build::SUPPORTED_ABIS->GetLength(); ++i) {
        assert(0);
        // if (instructionSet.equals(VMRuntime.getInstructionSet(abi))) {
        //     return true;
        // }
    }

    return FALSE;
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
