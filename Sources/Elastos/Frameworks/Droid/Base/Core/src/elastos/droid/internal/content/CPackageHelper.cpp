
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Zip.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/pm/PackageParser.h"
#include "elastos/droid/internal/content/CPackageHelper.h"
#include "elastos/droid/internal/content/CNativeLibraryHelper.h"
#include "elastos/droid/internal/content/CNativeLibraryHelperHandle.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/storage/StorageResultCode.h"
#include "elastos/droid/os/storage/CStorageManagerHelper.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/CUserEnvironment.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Pm::PackageParser;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IUserEnvironment;
using Elastos::Droid::Os::CUserEnvironment;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Storage::StorageResultCode;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::CStorageManagerHelper;
using Elastos::Droid::Os::Storage::IStorageManagerHelper;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::IOutputStream;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::Zip::CZipEntry;
using Elastos::Utility::Zip::CZipFile;
using Elastos::Utility::Zip::CZipOutputStream;
using Elastos::Utility::Zip::IDeflaterOutputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

const Boolean CPackageHelper::localLOGV = FALSE;
const String CPackageHelper::TAG("CPackageHelper");

CAR_INTERFACE_IMPL(CPackageHelper, Singleton, IPackageHelper)
CAR_SINGLETON_IMPL(CPackageHelper)

ECode CPackageHelper::GetMountService(
    /* [out] */ IIMountService** mountService)
{
    VALIDATE_NOT_NULL(mountService)

    AutoPtr<IInterface> obj = ServiceManager::GetService(String("mount"));
    if (obj != NULL) {
        *mountService = IIMountService::Probe(obj);
        REFCOUNT_ADD(*mountService);
        return NOERROR;
    }
    else {
        Logger::E(TAG, "Can't get mount service");
        //throw new RemoteException("Could not contact mount service");
        return E_RUNTIME_EXCEPTION;
    }
}

ECode CPackageHelper::CreateSdDir(
    /* [in] */ Int64 sizeBytes,
    /* [in] */ const String& cid,
    /* [in] */ const String& sdEncKey,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isExternal,
    /* [out] */ String* dir)
{
    VALIDATE_NOT_NULL(dir)
    *dir = NULL;
    // Round up to nearest MB, plus another MB for filesystem overhead
    const Int32 sizeMb = (Int32)((sizeBytes + ITrafficStats::MB_IN_BYTES) / ITrafficStats::MB_IN_BYTES) + 1;
    // try {
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "MountService running?");
        return NOERROR;
    }

    if (localLOGV)
        Logger::I(TAG, "Size of container %d MB", sizeMb);

    Int32 rc = 0;
    if (FAILED(mountService->CreateSecureContainer(cid, sizeMb, String("ext4"), sdEncKey, uid, isExternal, &rc))) {
        Logger::E(TAG, "MountService running?");
        return NOERROR;
    }
    if (rc != StorageResultCode::OperationSucceeded) {
        Logger::E(TAG, "Failed to create secure container %s", cid.string());
        return NOERROR;
    }

    String cachePath;
    if (FAILED(mountService->GetSecureContainerPath(cid, &cachePath))){
        Logger::E(TAG, "MountService running?");
        return NOERROR;
    }
    if (localLOGV) {
        Logger::I(TAG, "Created secure container %s at %s", cid.string(), cachePath.string());
    }
    *dir = cachePath;
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.e(TAG, "MountService running?");
    // }
}

ECode CPackageHelper::ResizeSdDir(
    /* [in] */ Int64 sizeBytes,
    /* [in] */ const String& cid,
    /* [in] */ const String& sdEncKey,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // Round up to nearest MB, plus another MB for filesystem overhead
    const Int32 sizeMb = (Int32)((sizeBytes + ITrafficStats::MB_IN_BYTES) / ITrafficStats::MB_IN_BYTES) + 1;
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "MountService running?");
        return NOERROR;
    }
    Int32 rc;
    mountService->ResizeSecureContainer(cid, sizeMb, sdEncKey, &rc);
    if (rc == StorageResultCode::OperationSucceeded) {
        *result = TRUE;
        return NOERROR;
    }
    Logger::E(TAG, "Failed to create secure container %s", cid.string());
    return NOERROR;
}

ECode CPackageHelper::MountSdDir(
    /* [in] */ const String& cid,
    /* [in] */ const String& key,
    /* [in] */ Int32 ownerUid,
    /* [out] */ String* dir)
{
    return MountSdDir(cid, key, ownerUid, TRUE, dir);
}

ECode CPackageHelper::MountSdDir(
    /* [in] */ const String& cid,
    /* [in] */ const String& key,
    /* [in] */ Int32 ownerUid,
    /* [in] */ Boolean readOnly,
    /* [out] */ String* dir)
{
    VALIDATE_NOT_NULL(dir)
    *dir = NULL;
    // try {
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "MountService running?");
        return NOERROR;
    }
    Int32 rc = 0;
    if (FAILED(mountService->MountSecureContainer(cid, key, ownerUid, readOnly, &rc))) {
        Logger::E(TAG, "MountService running?");
        return NOERROR;
    }
    if (rc != StorageResultCode::OperationSucceeded) {
        Logger::I(TAG, "Failed to mount container %s rc: %s", cid.string(), StringUtils::ToString(rc).string());
        return NOERROR;
    }

    if (FAILED(mountService->GetSecureContainerPath(cid, dir))) {
        Logger::E(TAG, "MountService running?");
        return NOERROR;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.e(TAG, "MountService running?");
    // }
}

ECode CPackageHelper::UnMountSdDir(
    /* [in] */ const String& cid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // try {
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "MountService running?");
        return NOERROR;
    }
    Int32 rc = 0;
    if (FAILED(mountService->UnmountSecureContainer(cid, TRUE, &rc))) {
        Logger::E(TAG, "MountService running?");
        return NOERROR;
    }
    if (rc != StorageResultCode::OperationSucceeded) {
        Logger::E(TAG, "Failed to unmount %s with rc %s", cid.string(), StringUtils::ToString(rc).string());
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.e(TAG, "MountService running?");
    // }
}

ECode CPackageHelper::RenameSdDir(
    /* [in] */ const String& oldId,
    /* [in] */ const String& newId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // try {
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "Failed to rename %s to %s", oldId.string(), newId.string());
        return NOERROR;
    }
    Int32 rc = 0;
    if (FAILED(mountService->RenameSecureContainer(oldId, newId, &rc))) {
        Logger::E(TAG, "Failed to rename %s to %s", oldId.string(), newId.string());
        return NOERROR;
    }
    if (rc != StorageResultCode::OperationSucceeded) {
        Logger::E(TAG, "Failed to rename %s to %s with rc %s", oldId.string(),
                newId.string(), StringUtils::ToString(rc).string());
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.i(TAG, "Failed ot rename  " + oldId + " to " + newId +
    //             " with exception : " + e);
    // }
}

ECode CPackageHelper::GetSdDir(
    /* [in] */ const String& cid,
    /* [out] */ String* dir)
{
    VALIDATE_NOT_NULL(dir)
    *dir = NULL;
    // try {
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "Failed to get container path for %s", cid.string());
        return NOERROR;
    }

    if (FAILED(mountService->GetSecureContainerPath(cid, dir))) {
        Logger::E(TAG, "Failed to get container path for %s", cid.string());
        return NOERROR;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Failed to get container path for " + cid +
    //         " with exception " + e);
    // }
}

ECode CPackageHelper::GetSdFilesystem(
    /* [in] */ const String& cid,
    /* [out] */ String* fs)
{
    VALIDATE_NOT_NULL(fs)
    *fs = NULL;
    // try {
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "Failed to get container path for %s", cid.string());
        return NOERROR;
    }

    if (FAILED(mountService->GetSecureContainerFilesystemPath(cid, fs))) {
        Logger::E(TAG, "Failed to get container path for %s", cid.string());
        return NOERROR;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Failed to get container path for " + cid +
    //         " with exception " + e);
    // }
}

ECode CPackageHelper::FinalizeSdDir(
    /* [in] */ const String& cid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // try {
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "Failed to finalize container %s", cid.string());
        return NOERROR;
    }
    Int32 rc = 0;
    if (FAILED(mountService->FinalizeSecureContainer(cid, &rc))) {
        Logger::E(TAG, "Failed to finalize container %s", cid.string());
        return NOERROR;
    }
    if (rc != StorageResultCode::OperationSucceeded) {
        Logger::E(TAG, "Failed to finalize container %s", cid.string());
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Failed to finalize container " + cid +
    //             " with exception " + e);
    // }
}

ECode CPackageHelper::DestroySdDir(
    /* [in] */ const String& cid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // try {
    if (localLOGV) {
        Logger::I(TAG, "Forcibly destroying container %s", cid.string());
    }

    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "Failed to destroy container %s", cid.string());
        return NOERROR;
    }
    Int32 rc = 0;
    if (FAILED(mountService->DestroySecureContainer(cid, TRUE, &rc))) {
        Logger::E(TAG, "Failed to destroy container %s", cid.string());
        return NOERROR;
    }
    if (rc != StorageResultCode::OperationSucceeded) {
        Logger::I(TAG, "Failed to destroy container %s", cid.string());
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Failed to destroy container " + cid +
    //             " with exception " + e);
    // }
}

ECode CPackageHelper::GetSecureContainerList(
    /* [out, callee] */ ArrayOf<String>** ids)
{
    VALIDATE_NOT_NULL(ids)
    *ids = NULL;
    // try {
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "Failed to get secure container list");
        return NOERROR;
    }
    if (FAILED(mountService->GetSecureContainerList(ids))) {
        Logger::E(TAG, "Failed to get secure container list");
        return NOERROR;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Failed to get secure container list with exception" +
    //             e);
    // }
}

ECode CPackageHelper::IsContainerMounted(
    /* [in] */ const String& cid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // try {
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "Failed to find out if container %s mounted", cid.string());
        return NOERROR;
    }
    Boolean isMounted;
    if (FAILED(mountService->IsSecureContainerMounted(cid, &isMounted))) {
        Logger::E(TAG, "Failed to find out if container %s mounted", cid.string());
        return NOERROR;
    }
    return isMounted;
    // } catch (RemoteException e) {
    //        Log.e(TAG, "Failed to find out if container " + cid + " mounted");
    // }
}

ECode CPackageHelper::ExtractPublicFiles(
    /* [in] */ IFile* apkFile,
    /* [in] */ IFile* publicZipFile,
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)

    AutoPtr<IFileOutputStream> fstr;
    AutoPtr<IZipOutputStream> publicZipOutStream;
    AutoPtr<IIoUtils> ioUtils;

    if (publicZipFile != NULL) {
        ASSERT_SUCCEEDED(CFileOutputStream::New(publicZipFile, (IFileOutputStream**)&fstr))
        ASSERT_SUCCEEDED(CZipOutputStream::New(IOutputStream::Probe(fstr), (IZipOutputStream**)&publicZipOutStream))
        // Logger::D(TAG, "Extracting " + apkFile + " to " + publicZipFile);
    }

    *size = 0;

    // try {
    AutoPtr<IZipFile> privateZip;
    AutoPtr<IEnumeration> entries;
    String packagePath;
    apkFile->GetAbsolutePath(&packagePath);
    if (FAILED(CZipFile::New(packagePath, (IZipFile**)&privateZip))) goto Exit2;
    // try {
    // Copy manifest, resources.arsc and res directory to public zip
    if (FAILED(privateZip->GetEntries((IEnumeration**)&entries))) goto Exit1;
    if (entries != NULL) {
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        AutoPtr<IArrayList> arrayList;
        collections->List(entries, (IArrayList**)&arrayList);
        Int32 length;
        arrayList->GetSize(&length);
        AutoPtr<IZipEntry> zipEntry;
        String zipEntryName;
        for (Int32 i = 0; i < length; i++) {
            zipEntry = NULL;
            arrayList->Get(i, (IInterface**)&zipEntry);
            zipEntry->GetName(&zipEntryName);
            if (String("AndroidManifest.xml").Equals(zipEntryName)
                    || String("resources.arsc").Equals(zipEntryName)
                    || zipEntryName.StartWith("res/")) {
                Int64 entrySize = 0;
                zipEntry->GetSize(&entrySize);
                *size += entrySize;
                if (publicZipFile != NULL) {
                    if (FAILED(CopyZipEntry(zipEntry, privateZip, publicZipOutStream))) goto Exit1;
                }
            }
        }
    }

Exit1:
    privateZip->Close();
    if (publicZipFile != NULL) {
       IDeflaterOutputStream::Probe(publicZipOutStream)->Finish();
       IFlushable::Probe(publicZipOutStream)->Flush();
       FileUtils::Sync(fstr);
       ICloseable::Probe(publicZipOutStream)->Close();
       String path;
       publicZipFile->GetAbsolutePath(&path);
       FileUtils::SetPermissions(path, FileUtils::sS_IRUSR
           | FileUtils::sS_IWUSR | FileUtils::sS_IRGRP | FileUtils::sS_IROTH, -1, -1);
    }

Exit2:
    ASSERT_SUCCEEDED(CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils));
    ioUtils->CloseQuietly(ICloseable::Probe(publicZipOutStream));
    return NOERROR;
}

ECode CPackageHelper::CopyZipEntry(
    /* [in] */ IZipEntry* zipEntry,
    /* [in] */ IZipFile* inZipFile,
    /* [in] */ IZipOutputStream* outZipStream)
{
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(4096);
    Int32 num = 0;

    AutoPtr<IZipEntry> newEntry;
    AutoPtr<IIoUtils> ioUtils;

    Int32 ret = 0;
    if (zipEntry->GetMethod(&ret), ret == IZipEntry::STORED) {
        // Preserve the STORED method of the input entry.
        FAIL_RETURN(CZipEntry::New(zipEntry, (IZipEntry**)&newEntry))
    }
    else {
        // Create a new entry so that the compressed len is recomputed.
        String name;
        FAIL_RETURN(zipEntry->GetName(&name))
        FAIL_RETURN(CZipEntry::New(name, (IZipEntry**)&newEntry))
    }
    FAIL_RETURN(outZipStream->PutNextEntry(newEntry))

    AutoPtr<IInputStream> data;
    FAIL_RETURN(inZipFile->GetInputStream(zipEntry, (IInputStream**)&data))
    // try {
    while ((data->Read(buffer, &num), num) > 0) {
        if (FAILED(IOutputStream::Probe(outZipStream)->Write(buffer, 0, num))) goto Exit1;
    }
    if (FAILED(IFlushable::Probe(outZipStream)->Flush())) goto Exit1;
    // } finally {
    //     IoUtils.closeQuietly(data);
    // }
Exit1:
    ASSERT_SUCCEEDED(CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils))
    FAIL_RETURN(ioUtils->CloseQuietly(ICloseable::Probe(data)));
    return NOERROR;
}

ECode CPackageHelper::FixSdPermissions(
    /* [in] */ const String& cid,
    /* [in] */ Int32 gid,
    /* [in] */ const String& filename,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // try {
    AutoPtr<IIMountService> mountService;
    if (FAILED(GetMountService((IIMountService**)&mountService))) {
        Logger::E(TAG, "Failed to fixperms container %s", cid.string());
        return NOERROR;
    }
    Int32 rc = 0;
    if (FAILED(mountService->FixPermissionsSecureContainer(cid, gid, filename, &rc))) {
        Logger::E(TAG, "Failed to fixperms container %s", cid.string());
        return NOERROR;
    }
    if (rc != StorageResultCode::OperationSucceeded) {
        Logger::E(TAG, "Failed to fixperms container %s", cid.string());
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
    // } catch (RemoteException e) {
    //     Logger::E(TAG, "Failed to fixperms container " + cid + " with exception " + e);
    // }
}

/**
 * Given a requested {@link PackageInfo#installLocation} and calculated
 * install size, pick the actual location to install the app.
 */
ECode CPackageHelper::ResolveInstallLocation(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 installLocation,
    /* [in] */ Int64 sizeBytes,
    /* [in] */ Int32 installFlags,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IApplicationInfo> existingInfo;

    AutoPtr<IPackageManager> pkgManager;
    FAIL_RETURN(context->GetPackageManager((IPackageManager**)&pkgManager));
    pkgManager->GetApplicationInfo(packageName, IPackageManager::GET_UNINSTALLED_PACKAGES,
        (IApplicationInfo**)&existingInfo);

    Int32 prefer;
    Boolean checkBoth;
    if ((installFlags & IPackageManager::INSTALL_INTERNAL) != 0) {
        prefer = RECOMMEND_INSTALL_INTERNAL;
        checkBoth = FALSE;
    }
    else if ((installFlags & IPackageManager::INSTALL_EXTERNAL) != 0) {
        prefer = RECOMMEND_INSTALL_EXTERNAL;
        checkBoth = FALSE;
    }
    else if (installLocation == IPackageInfo::INSTALL_LOCATION_INTERNAL_ONLY) {
        prefer = RECOMMEND_INSTALL_INTERNAL;
        checkBoth = FALSE;
    }
    else if (installLocation == IPackageInfo::INSTALL_LOCATION_PREFER_EXTERNAL) {
        prefer = RECOMMEND_INSTALL_EXTERNAL;
        checkBoth = TRUE;
    }
    else if (installLocation == IPackageInfo::INSTALL_LOCATION_AUTO) {
        // When app is already installed, prefer same medium
        if (existingInfo != NULL) {
            Int32 flags;
            existingInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0) {
                prefer = RECOMMEND_INSTALL_EXTERNAL;
            }
            else {
                prefer = RECOMMEND_INSTALL_INTERNAL;
            }
        }
        else {
            prefer = RECOMMEND_INSTALL_INTERNAL;
        }
        checkBoth = TRUE;
    }
    else {
        prefer = RECOMMEND_INSTALL_INTERNAL;
        checkBoth = FALSE;
    }

    Boolean externalIsEmulated = Environment::IsExternalStorageEmulated();
    Boolean noemulated = Environment::IsNoEmulatedStorageExist();
    AutoPtr<IStorageManagerHelper> smHelper;
    CStorageManagerHelper::AcquireSingleton((IStorageManagerHelper**)&smHelper);
    AutoPtr<IStorageManager> storage;
    smHelper->From(context, (IStorageManager**)&storage);

    Boolean fitsOnInternal = FALSE;
    if (checkBoth || prefer == RECOMMEND_INSTALL_INTERNAL) {
        AutoPtr<IFile> target = Environment::GetDataDirectory();
        Int64 size;
        storage->GetStorageBytesUntilLow(target, &size);
        fitsOnInternal = (sizeBytes <= size);
    }

    Boolean fitsOnExternal = FALSE;
    if (!externalIsEmulated && (checkBoth || prefer == RECOMMEND_INSTALL_EXTERNAL)) {
        AutoPtr<IUserEnvironment> ue;
        CUserEnvironment::New(IUserHandle::USER_OWNER, (IUserEnvironment**)&ue);
        AutoPtr<IFile> target;
        ue->GetExternalStorageDirectory((IFile**)&target);
        // External is only an option when size is known
        if (sizeBytes > 0) {
            Int64 size;
            storage->GetStorageBytesUntilLow(target, &size);
            fitsOnExternal = (sizeBytes <= size);
        }
    }

    if (noemulated && (checkBoth || prefer == RECOMMEND_INSTALL_EXTERNAL)) {
        AutoPtr<IUserEnvironment> ue;
        CUserEnvironment::New(IUserHandle::USER_OWNER, (IUserEnvironment**)&ue);
        AutoPtr<IFile> target;
        ue->GetSecondaryStorageDirectory((IFile**)&target);
        // External is only an option when size is known
        if (sizeBytes > 0) {
            Int64 size = 0;
            storage->GetStorageBytesUntilLow(target, &size);
            fitsOnExternal = fitsOnExternal | (sizeBytes <= size);
        }
    }

    if (prefer == RECOMMEND_INSTALL_INTERNAL) {
        if (fitsOnInternal) {
            *result = IPackageHelper::RECOMMEND_INSTALL_INTERNAL;
            return NOERROR;
        }
    }
    else if (prefer == RECOMMEND_INSTALL_EXTERNAL) {
        if (fitsOnExternal) {
            *result = IPackageHelper::RECOMMEND_INSTALL_EXTERNAL;
            return NOERROR;
        }
    }

    if (checkBoth) {
        if (fitsOnInternal) {
            *result = IPackageHelper::RECOMMEND_INSTALL_INTERNAL;
            return NOERROR;
        }
        else if (fitsOnExternal) {
            *result = IPackageHelper::RECOMMEND_INSTALL_EXTERNAL;
            return NOERROR;
        }
    }

    /*
     * If they requested to be on the external media by default, return that
     * the media was unavailable. Otherwise, indicate there was insufficient
     * storage space available.
     */
    if (!externalIsEmulated && (checkBoth || prefer == RECOMMEND_INSTALL_EXTERNAL)
        && !Environment::MEDIA_MOUNTED.Equals(Environment::GetExternalStorageState())) {
        *result = IPackageHelper::RECOMMEND_MEDIA_UNAVAILABLE;
        return NOERROR;
    }
    else if (noemulated && (checkBoth || prefer == RECOMMEND_INSTALL_EXTERNAL)
            && !Environment::MEDIA_MOUNTED.Equals(Environment::GetSecondaryStorageState())) {
        *result = IPackageHelper::RECOMMEND_MEDIA_UNAVAILABLE;
        return NOERROR;
    }
    else {
        *result = IPackageHelper::RECOMMEND_FAILED_INSUFFICIENT_STORAGE;
        return NOERROR;
    }
}

ECode CPackageHelper::CalculateInstalledSize(
    /* [in] */ Handle64 pkg,
    /* [in] */ Boolean isForwardLocked,
    /* [in] */ const String& abiOverride,
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    AutoPtr<INativeLibraryHelperHandle> handle;
    CNativeLibraryHelperHandle::CreatePackage(pkg, (INativeLibraryHelperHandle**)&handle);
    ECode ec = CalculateInstalledSize(pkg, handle, isForwardLocked, abiOverride, size);
    AutoPtr<IIoUtils> ioUtils;
    ASSERT_SUCCEEDED(CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils));
    ioUtils->CloseQuietly(ICloseable::Probe(handle));
    return ec;
}

ECode CPackageHelper::CalculateInstalledSize(
    /* [in] */ Handle64 pkg,
    /* [in] */ INativeLibraryHelperHandle* handle,
    /* [in] */ Boolean isForwardLocked,
    /* [in] */ const String& abiOverride,
    /* [out] */ Int64* sizeBytes)
{
    VALIDATE_NOT_NULL(sizeBytes)
    *sizeBytes = 0;

    // Include raw APKs, and possibly unpacked resources
    AutoPtr<PackageParser::PackageLite> lite = (PackageParser::PackageLite*)pkg;
    AutoPtr<List<String> > codePaths = lite->GetAllCodePaths();
    List<String>::Iterator iter = codePaths->Begin();
    for (; iter != codePaths->End(); ++iter) {
        AutoPtr<IFile> codeFile;
        CFile::New(*iter, (IFile**)&codeFile);
        Int64 length = 0;
        codeFile->GetLength(&length);
        *sizeBytes += length;

        if (isForwardLocked) {
            Int64 size;
            ExtractPublicFiles(codeFile, NULL, &size);
            *sizeBytes += size;
        }
    }

    // Include all relevant native code
    AutoPtr<INativeLibraryHelper> nlHelper;
    CNativeLibraryHelper::AcquireSingleton((INativeLibraryHelper**)&nlHelper);
    Int64 size;
    nlHelper->SumNativeBinariesWithOverride(handle, abiOverride, &size);
    *sizeBytes += size;

    return NOERROR;
}

ECode CPackageHelper::ReplaceEnd(
    /* [in] */ const String& str,
    /* [in] */ const String& before,
    /* [in] */ const String& after,
    /* [out] */ String* result)
{
    if (!str.EndWith(before)) {
        Logger::D(TAG, "Expected %s to end with %s", str.string(), before.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = str.Substring(0, str.GetLength() - before.GetLength()) + after;
    return NOERROR;
}

} // namespace Content
} // namespace Internal
} // namespace Droid
} // namespace Elastos
