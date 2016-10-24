
#include "elastos/droid/defcontainer/DefaultContainerService.h"
#include "elastos/droid/defcontainer/CMediaContainerService.h"
#include "elastos/droid/defcontainer/MeasurementUtils.h"
#include "elastos/droid/os/Process.h"
#include <elastos/droid/os/Environment.h>
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/system/Os.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.Droid.Content.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IUserEnvironment;
using Elastos::Droid::Os::CUserEnvironment;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::CParcelFileDescriptorAutoCloseOutputStream;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageParserHelper;
using Elastos::Droid::Content::Pm::CPackageParserHelper;
using Elastos::Droid::Content::Pm::CPackageInfoLite;
using Elastos::Droid::Content::Pm::IVerifierInfo;
using Elastos::Droid::Content::Pm::IPackageCleanItem;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IObbScanner;
using Elastos::Droid::Content::Res::CObbScanner;
using Elastos::Droid::Content::Res::CAssetFileDescriptorAutoCloseOutputStream;
using Elastos::Droid::Internal::App::EIID_IIMediaContainerService;
using Elastos::Droid::Internal::Content::IPackageHelper;
using Elastos::Droid::Internal::Content::CPackageHelper;
using Elastos::Droid::Internal::Content::INativeLibraryHelper;
using Elastos::Droid::Internal::Content::CNativeLibraryHelper;
using Elastos::Droid::Internal::Content::INativeLibraryHelperHandleHelper;
using Elastos::Droid::Internal::Content::CNativeLibraryHelperHandleHelper;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::System::IStructStatVfs;
using Elastos::Core::Math;
using Elastos::IO::CFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::ICloseable;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace Droid {
namespace DefContainer {

//////////////////////////////////////////////////////////////////
//      DefaultContainerService::MediaContainerService
//////////////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL_2(DefaultContainerService::MediaContainerService, Object, IIMediaContainerService, IBinder)

DefaultContainerService::MediaContainerService::MediaContainerService()
{}

ECode DefaultContainerService::MediaContainerService::constructor(
    /* [in] */ IService* host)
{
    mHost = (DefaultContainerService*)host;
    return NOERROR;
}

ECode DefaultContainerService::MediaContainerService::CopyPackageToContainer(
    /* [in] */ const String& packagePath,
    /* [in] */ const String& containerId,
    /* [in] */ const String& key,
    /* [in] */ Boolean isExternal,
    /* [in] */ Boolean isForwardLocked,
    /* [in] */ const String& abiOverride,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (packagePath.IsNull() || containerId.IsNull()) {
        *result = NULL;
        return NOERROR;
    }

    if (isExternal) {
        // Make sure the sdcard is mounted.
        String status = Environment::GetExternalStorageState();
        if (!status.Equals(Environment::MEDIA_MOUNTED)) {
            //Slog.w(TAG, "Make sure sdcard is mounted.");
            *result = NULL;
            return NOERROR;
        }
    }

    AutoPtr<IPackageLite> pkg;
    AutoPtr<INativeLibraryHelperHandle> handle;
    //try {
    AutoPtr<IFile> packageFile;
    CFile::New(packagePath, (IFile**)&packageFile);
    AutoPtr<IPackageParserHelper> parser;
    CPackageParserHelper::AcquireSingleton((IPackageParserHelper**)&parser);
    parser->ParsePackageLite(packageFile, 0, (IPackageLite**)&pkg);
    AutoPtr<INativeLibraryHelperHandleHelper> helper;
    CNativeLibraryHelperHandleHelper::AcquireSingleton((INativeLibraryHelperHandleHelper**)&helper);
    ECode ec = helper->Create(pkg, (INativeLibraryHelperHandle**)&handle);

    mHost->CopyPackageToContainerInner(pkg, handle, containerId, key, isExternal,
            isForwardLocked, abiOverride, result);
    AutoPtr<IIoUtils> ioutils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
    ioutils->CloseQuietly(ICloseable::Probe(handle));
    if (ec == (ECode)E_PACKAGE_PARSER_EXCEPTION || ec == (ECode)E_IO_EXCEPTION)
    {
        //Slog.w(TAG, "Failed to copy package at " + packagePath, e);
        *result = NULL;
        ioutils->CloseQuietly(ICloseable::Probe(handle));
    }
    //} catch (PackageParserException | IOException e) {
        //return null;
    //} finally {
    //   IoUtils.closeQuietly(handle);
    //}
    return NOERROR;
}

ECode DefaultContainerService::MediaContainerService::CopyPackage(
    /* [in] */ const String& packagePath,
    /* [in] */ IIParcelFileDescriptorFactory* target,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (packagePath.IsNull() || target == NULL) {
            *result = IPackageManager::INSTALL_FAILED_INVALID_URI;
            return NOERROR;
        }

        AutoPtr<IPackageLite> pkg;
        //try {
        AutoPtr<IFile> packageFile;
        CFile::New(packagePath, (IFile**)&packageFile);
        AutoPtr<IPackageParserHelper> parser;
        CPackageParserHelper::AcquireSingleton((IPackageParserHelper**)&parser);
        ECode ec = parser->ParsePackageLite(packageFile, 0, (IPackageLite**)&pkg);
        ECode ec1 = mHost->CopyPackageInner(pkg, target, result);
        if (ec == (ECode)E_PACKAGE_PARSER_EXCEPTION || ec == (ECode)E_IO_EXCEPTION
            || ec1 == (ECode)E_REMOTE_EXCEPTION) {
            //Slog.w(TAG, "Failed to copy package at " + packagePath + ": " + e);
            *result = IPackageManager::INSTALL_FAILED_INSUFFICIENT_STORAGE;
            return NOERROR;
        }
        return NOERROR;
        //} catch (PackageParserException | IOException | RemoteException e) {
            //Slog.w(TAG, "Failed to copy package at " + packagePath + ": " + e);
        //}
}

ECode DefaultContainerService::MediaContainerService::GetMinimalPackageInfo(
    /* [in] */ const String& packagePath,
    /* [in] */ Int32 flags,
    /* [in] */ const String& abiOverride,
    /* [out] */ IPackageInfoLite** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> context = IContext::Probe(mHost);
    Boolean isForwardLocked = (flags & IPackageManager::INSTALL_FORWARD_LOCK) != 0;

    AutoPtr<IPackageInfoLite> ret;
    CPackageInfoLite::New((IPackageInfoLite**)&ret);
    if (packagePath.IsNull()) {
        //Slog.i(TAG, "Invalid package file " + packagePath);
        ret->SetRecommendedInstallLocation(IPackageHelper::RECOMMEND_FAILED_INVALID_APK);
        *result = ret;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<IFile> packageFile;
    CFile::New(packagePath, (IFile**)&packageFile);
    AutoPtr<IPackageLite> pkg;
    Int64 sizeBytes;
    //try {
    AutoPtr<IPackageParserHelper> parser;
    CPackageParserHelper::AcquireSingleton((IPackageParserHelper**)&parser);
    ECode ec = parser->ParsePackageLite(packageFile, 0, (IPackageLite**)&pkg);
    AutoPtr<IPackageHelper> helper;
    CPackageHelper::AcquireSingleton((IPackageHelper**)&helper);
    ECode ec1 = NOERROR;
    if (SUCCEEDED(ec)) {
        ec1 = helper->CalculateInstalledSize(pkg, isForwardLocked, abiOverride, &sizeBytes);
    }
    if (ec == (ECode)E_PACKAGE_PARSER_EXCEPTION || ec1 == (ECode)E_IO_EXCEPTION) {
        //Slog.w(TAG, "Failed to parse package at " + packagePath + ": " + e);
        Boolean exists;
        if (packageFile->Exists(&exists), !exists) {
            ret->SetRecommendedInstallLocation(IPackageHelper::RECOMMEND_FAILED_INVALID_URI);
        } else {
            ret->SetRecommendedInstallLocation(IPackageHelper::RECOMMEND_FAILED_INVALID_APK);
        }
        *result = ret;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    //} catch (PackageParserException | IOException e) {
        // Slog.w(TAG, "Failed to parse package at " + packagePath + ": " + e);

        // if (!packageFile.exists()) {
        //     ret.recommendedInstallLocation = PackageHelper.RECOMMEND_FAILED_INVALID_URI;
        // } else {
        //     ret.recommendedInstallLocation = PackageHelper.RECOMMEND_FAILED_INVALID_APK;
        // }

        // return ret;
    //}
    String objString;
    pkg->GetPackageName(&objString);
    ret->SetPackageName(objString);
    Int32 objInt;
    pkg->GetVersionCode(&objInt);
    ret->SetVersionCode(objInt);
    pkg->GetInstallLocation(&objInt);
    ret->SetInstallLocation(objInt);
    AutoPtr< ArrayOf<IVerifierInfo*> > objArray;
    pkg->GetVerifiers((ArrayOf<IVerifierInfo*>**)&objArray);
    ret->SetVerifiers(objArray);
    Int32 recommendedInstallLocation;
    helper->ResolveInstallLocation(
        context, objString, objInt, sizeBytes, flags, &recommendedInstallLocation);
    Boolean objBool;
    pkg->GetMultiArch(&objBool);
    ret->SetMultiArch(objBool);

    *result = ret;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode DefaultContainerService::MediaContainerService::GetObbInfo(
    /* [in] */ const String& filename,
    /* [out] */ IObbInfo** result)
{
    VALIDATE_NOT_NULL(result)
    //try {
    AutoPtr<IObbScanner> helper;
    CObbScanner::AcquireSingleton((IObbScanner**)&helper);
    ECode ec = helper->GetObbInfo(filename, result);
    if (ec == (ECode)E_IO_EXCEPTION) {
        // Slog.d(TAG, "Couldn't get OBB info for " + filename);
        *result = NULL;
    }
    return NOERROR;
    //} catch (IOException e) {
        // Slog.d(TAG, "Couldn't get OBB info for " + filename);
        // return null;
    //}
}

ECode DefaultContainerService::MediaContainerService::CalculateDirectorySize(
    /* [in] */ const String& directory,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    AutoPtr<IFile> path;
    CFile::New(directory, (IFile**)&path);
    AutoPtr<IFile> dir = Environment::MaybeTranslateEmulatedPathToInternal(path);
    Boolean exists, isDirectory;
    if ((dir->Exists(&exists), exists)
        && (dir->IsDirectory(&isDirectory), isDirectory)) {
        String targetPath;
        dir->GetAbsolutePath(&targetPath);
        *result = MeasurementUtils::MeasureDirectory(targetPath);
        return NOERROR;
    } else {
        *result = 0L;
        return NOERROR;
    }
}

ECode DefaultContainerService::MediaContainerService::GetFileSystemStats(
    /* [in] */ const String& path,
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    VALIDATE_NOT_NULL(result)
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    AutoPtr<IStructStatVfs> stat;
    Elastos::Droid::System::Os::StatVfs(path, (IStructStatVfs**)&stat);
    Int64 f_blocks, f_bsize, f_bavail;
    stat->GetBlocks(&f_blocks);
    stat->GetBsize(&f_bsize);
    stat->GetBavail(&f_bavail);
    Int64 totalSize = f_blocks * f_bsize;
    Int64 availSize = f_bavail * f_bsize;
    AutoPtr< ArrayOf<Int64> > obj = ArrayOf<Int64>::Alloc(2);
    (*obj)[0] = totalSize;
    (*obj)[1] = availSize;
    *result = obj;
    REFCOUNT_ADD(*result)
    return NOERROR;
    //try {
        // final long totalSize = stat.f_blocks * stat.f_bsize;
        // final long availSize = stat.f_bavail * stat.f_bsize;
        // return new long[] { totalSize, availSize };
    //} catch (ErrnoException e) {
        //throw new IllegalStateException(e);
    //}
}

ECode DefaultContainerService::MediaContainerService::ClearDirectory(
    /* [in] */ const String& path)
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    AutoPtr<IFile> directory;
    CFile::New(path, (IFile**)&directory);
    Boolean exists, isDirectory;
    if ((directory->Exists(&exists), exists)
        && (directory->IsDirectory(&isDirectory), isDirectory)) {
        mHost->EraseFiles(directory);
    }
    return NOERROR;
}

ECode DefaultContainerService::MediaContainerService::CalculateInstalledSize(
    /* [in] */ const String& packagePath,
    /* [in] */ Boolean isForwardLocked,
    /* [in] */ const String& abiOverride,
    /* [out] */ Int64* result)
{
    AutoPtr<IFile> packageFile;
    CFile::New(packagePath, (IFile**)&packageFile);
    AutoPtr<IPackageLite> pkg;
    AutoPtr<IPackageParserHelper> parser;
    CPackageParserHelper::AcquireSingleton((IPackageParserHelper**)&parser);
    ECode ec = parser->ParsePackageLite(packageFile, 0, (IPackageLite**)&pkg);
    AutoPtr<IPackageHelper> helper;
    CPackageHelper::AcquireSingleton((IPackageHelper**)&helper);
    ECode ec1 = helper->CalculateInstalledSize(pkg, isForwardLocked, abiOverride, result);
    if (ec == (ECode)E_PACKAGE_PARSER_EXCEPTION || ec1 == (ECode)E_IO_EXCEPTION) {
        //Slog.w(TAG, "Failed to calculate installed size: " + e);
        *result = Elastos::Core::Math::INT64_MAX_VALUE;
        return NOERROR;
    }
    return NOERROR;
    //try {
        // pkg = PackageParser.parsePackageLite(packageFile, 0);
        // return PackageHelper.calculateInstalledSize(pkg, isForwardLocked, abiOverride);
    //} catch (PackageParserException | IOException e) {
        // Slog.w(TAG, "Failed to calculate installed size: " + e);
        // return Long.MAX_VALUE;
    //}
}

ECode DefaultContainerService::MediaContainerService::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

//////////////////////////////////////////////////////////////////
//                      DefaultContainerService
//////////////////////////////////////////////////////////////////

DefaultContainerService::DefaultContainerService()
{
    CMediaContainerService::New(this, (IIMediaContainerService**)&mBinder);
}

ECode DefaultContainerService::constructor()
{
    IntentService::constructor(String("DefaultContainerService"));
    SetIntentRedelivery(TRUE);
    return NOERROR;
}

ECode DefaultContainerService::EraseFiles(
    /* [in] */ ArrayOf<IFile*> * paths)
{
    for (Int32 i = 0; i < paths->GetLength(); ++i) {
        EraseFiles((*paths)[i]);
    }
    return NOERROR;
}

ECode DefaultContainerService::EraseFiles(
    /* [in] */ IFile* path)
{
    Boolean isDirectory;
    if (path->IsDirectory(&isDirectory), isDirectory) {
        AutoPtr< ArrayOf<String> > files;
        path->List((ArrayOf<String>**)&files);
        if (NULL != files) {
            for (Int32 i = 0; i < files->GetLength(); ++i) {
                AutoPtr<IFile> obj;
                CFile::New(path, (*files)[i], (IFile**)&obj);
                EraseFiles(obj);
            }
        }
    }
    return path->Delete();
}

ECode DefaultContainerService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    *binder = IBinder::Probe(mBinder);
    REFCOUNT_ADD(*binder)
    return NOERROR;
}

ECode DefaultContainerService::OnHandleIntent(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IPackageManager::ACTION_CLEAN_EXTERNAL_STORAGE.Equals(action)) {
        AutoPtr<IServiceManager> manager;
        CServiceManager::AcquireSingleton((IServiceManager**)&manager);
        AutoPtr<IInterface> obj;
        manager->GetService(String("package"), (IInterface**)&obj);
        AutoPtr<IIPackageManager> pm = IIPackageManager::Probe(obj);
        AutoPtr<IPackageCleanItem> item, nextItem;
        //try {
        while ((pm->NextPackageToClean(item, (IPackageCleanItem**)&nextItem), nextItem) != NULL) {
            item = nextItem;
            nextItem = NULL;
            AutoPtr<IUserEnvironment> userEnv;
            Int32 userId;
            item->GetUserId(&userId);
            CUserEnvironment::New(userId, (IUserEnvironment**)&userEnv);
            String packageName;
            item->GetPackageName(&packageName);
            AutoPtr< ArrayOf<IFile*> > obj;
            userEnv->BuildExternalStorageAppDataDirs(packageName, (ArrayOf<IFile*>**)&obj);
            EraseFiles(obj);
            obj = NULL;
            userEnv->BuildExternalStorageAppMediaDirs(packageName, (ArrayOf<IFile*>**)&obj);
            EraseFiles(obj);
            Boolean andCode;
            if (item->GetAndCode(&andCode), andCode) {
                obj = NULL;
                userEnv->BuildExternalStorageAppObbDirs(
                    packageName, (ArrayOf<IFile*>**)&obj);
                EraseFiles(obj);
            }
        }
        //} catch (RemoteException e) {
    }
    return NOERROR;
}

ECode DefaultContainerService::CopyPackageToContainerInner(
    /* [in] */ IPackageLite* pkg,
    /* [in] */ INativeLibraryHelperHandle* handle,
    /* [in] */ const String& newCid,
    /* [in] */ const String& key,
    /* [in] */ Boolean isExternal,
    /* [in] */ Boolean isForwardLocked,
    /* [in] */ const String& abiOverride,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // Calculate container size, rounding up to nearest MB and adding an
    // extra MB for filesystem overhead
    AutoPtr<IPackageHelper> helper;
    CPackageHelper::AcquireSingleton((IPackageHelper**)&helper);

    Int64 sizeBytes;
    ECode ec = helper->CalculateInstalledSize(pkg, handle,
            isForwardLocked, abiOverride, &sizeBytes);
    // Create new container
    String newMountPath;
    helper->CreateSdDir(sizeBytes, newCid, key, Process::MyUid(), isExternal, &newMountPath);

    if (newMountPath.IsNull()) {
        return E_IO_EXCEPTION;
        //throw new IOException("Failed to create container " + newCid);
    }
    AutoPtr<IFile> targetDir;
    CFile::New(newMountPath, (IFile**)&targetDir);

    //try {
    // Copy all APKs
    String path;
    pkg->GetBaseCodePath(&path);
    ECode ec1 = CopyFile(path, targetDir, String("base.apk"), isForwardLocked);
    AutoPtr< ArrayOf<String> > splitNames, paths;
    pkg->GetSplitNames((ArrayOf<String>**)&splitNames);
    pkg->GetSplitCodePaths((ArrayOf<String>**)&paths);
    ECode ec2 = NOERROR;
    if (!ArrayUtils::IsEmpty(splitNames.Get())) {
        for (Int32 i = 0; i < splitNames->GetLength(); ++i) {
            ec2 = CopyFile((*paths)[i], targetDir, String("split_") +
                (*splitNames)[i] + String(".apk"), isForwardLocked);
        }
    }

    // Extract native code
    AutoPtr<IFile> libraryRoot;
    CFile::New(targetDir, INativeLibraryHelper::LIB_DIR_NAME, (IFile**)&libraryRoot);
    Int32 res;
    AutoPtr<INativeLibraryHelper> nlh;
    CNativeLibraryHelper::AcquireSingleton((INativeLibraryHelper**)&nlh);
    nlh->CopyNativeBinariesWithOverride(handle, libraryRoot, abiOverride, &res);
    if (res != IPackageManager::INSTALL_SUCCEEDED) {
        //throw new IOException("Failed to extract native code, res=" + res);
        return E_IO_EXCEPTION;
    }

    Boolean finalizeSdDir;
    if (helper->FinalizeSdDir(newCid, &finalizeSdDir), finalizeSdDir) {
        //throw new IOException("Failed to finalize " + newCid);
        return E_IO_EXCEPTION;
    }

    Boolean isContainerMounted;
    if(helper->IsContainerMounted(newCid, &isContainerMounted), isContainerMounted) {
        helper->UnMountSdDir(newCid, &isContainerMounted);
    }

    Boolean isError;
    if (ec1 == (ECode)E_ERRNO_EXCEPTION || ec2 == (ECode)E_ERRNO_EXCEPTION) {
        helper->DestroySdDir(newCid, &isError);
        return E_IO_EXCEPTION;
    }
    if (ec == (ECode)E_IO_EXCEPTION) {
        helper->DestroySdDir(newCid, &isError);
        return E_IO_EXCEPTION;
    }
    //} catch (ErrnoException e) {
    //    PackageHelper.destroySdDir(newCid);
    //    throw e.rethrowAsIOException();
    //} catch (IOException e) {
    //    PackageHelper.destroySdDir(newCid);
    //    throw e;
    //}

    *result = newMountPath;
    return NOERROR;
}

ECode DefaultContainerService::CopyPackageInner(
    /* [in] */ IPackageLite* pkg,
    /* [in] */ IIParcelFileDescriptorFactory* target,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    String str;
    pkg->GetBaseCodePath(&str);
    Boolean isEpk = str.EndWith(".epk");
    CopyFile(str, target, isEpk ? String("base.epk") : String("base.apk"));
    AutoPtr<ArrayOf<String> > splitNames, splitCodePaths;
    pkg->GetSplitNames((ArrayOf<String>**)&splitNames);
    if (!ArrayUtils::IsEmpty(splitNames.Get())) {
        pkg->GetSplitCodePaths((ArrayOf<String>**)&splitCodePaths);
        for (Int32 i = 0; i < splitNames->GetLength(); i++) {
            CopyFile((*splitCodePaths)[i], target,
                String("split_") + (*splitNames)[i] + (isEpk ? ".epk" : ".apk"));
        }
    }

    *result = IPackageManager::INSTALL_SUCCEEDED;
    return NOERROR;
}

ECode DefaultContainerService::CopyFile(
    /* [in] */ const String& sourcePath,
    /* [in] */ IIParcelFileDescriptorFactory* target,
    /* [in] */ const String& targetName)
{
    AutoPtr<IInputStream> in;
    AutoPtr<IOutputStream> out;
    //try {
    ECode ec = CFileInputStream::New(sourcePath, (IInputStream**)&in);
    AutoPtr<IParcelFileDescriptor> descriptor;
    target->Open(targetName, IParcelFileDescriptor::MODE_READ_WRITE,
        (IParcelFileDescriptor**)&descriptor);
    CParcelFileDescriptorAutoCloseOutputStream::New(descriptor, (IOutputStream**)&out);
    AutoPtr<IStreams> stream;
    CStreams::AcquireSingleton((IStreams**)&stream);
    Int32 size;
    ec = stream->Copy(in, out, &size);
    if (FAILED(ec)) {
        goto errin;
    }
errin:
    AutoPtr<IIoUtils> ioutils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioutils);
    ioutils->CloseQuietly(ICloseable::Probe(out));
    ioutils->CloseQuietly(ICloseable::Probe(in));
    //} finally {
    //    IoUtils.closeQuietly(out);
    //    IoUtils.closeQuietly(in);
    //}
    return NOERROR;
}

ECode DefaultContainerService::CopyFile(
    /* [in] */ const String& sourcePath,
    /* [in] */ IFile* targetDir,
    /* [in] */ const String& targetName,
    /* [in] */ Boolean isForwardLocked)
{
    AutoPtr<IFile> sourceFile, targetFile;
    CFile::New(sourcePath, (IFile**)&sourceFile);
    CFile::New(targetDir, targetName, (IFile**)&targetFile);

    //Slog.d(TAG, "Copying " + sourceFile + " to " + targetFile);
    if (!FileUtils::CopyFile(sourceFile, targetFile)) {
        //throw new IOException("Failed to copy " + sourceFile + " to " + targetFile);
        return E_IO_EXCEPTION;
    }

    if (isForwardLocked) {
        String publicTargetName;
        AutoPtr<IPackageHelper> helper;
        CPackageHelper::AcquireSingleton((IPackageHelper**)&helper);
        helper->ReplaceEnd(targetName, String(".apk"), String(".zip"), &publicTargetName);
        AutoPtr<IFile> publicTargetFile;
        CFile::New(targetDir, publicTargetName, (IFile**)&publicTargetFile);
        Int64 size;
        helper->ExtractPublicFiles(sourceFile, publicTargetFile, &size);
        String obj;
        targetFile->GetAbsolutePath(&obj);
        Elastos::Droid::System::Os::Chmod(obj, 0640);
        publicTargetFile->GetAbsolutePath(&obj);
        Elastos::Droid::System::Os::Chmod(obj, 0644);
    } else {
        String obj;
        targetFile->GetAbsolutePath(&obj);
        Elastos::Droid::System::Os::Chmod(obj, 0644);
    }
    return NOERROR;
}

} //namespace DefContainer
} //namespace Droid
} //namespace Elastos