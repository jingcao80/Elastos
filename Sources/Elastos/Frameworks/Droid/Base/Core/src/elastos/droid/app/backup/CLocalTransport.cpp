//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/app/backup/CLocalTransport.h"
#include "elastos/droid/app/backup/CBackupDataInput.h"
#include "elastos/droid/app/backup/CBackupDataOutput.h"
#include "elastos/droid/app/backup/CRestoreSet.h"
#include "elastos/droid/os/CEnvironment.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/utility/CBase64.h"

using Elastos::IO::CFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::Backup::IBackupDataInput;
using Elastos::Droid::App::Backup::CBackupDataInput;
using Elastos::Droid::App::Backup::IBackupDataOutput;
using Elastos::Droid::App::Backup::CBackupDataOutput;
using Elastos::Droid::App::Backup::CRestoreSet;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Utility::CBase64;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Backup {

const String CLocalTransport::TAG("Backup==>CLocalTransport");
const Boolean CLocalTransport::DEBUG = FALSE;
const String CLocalTransport::TRANSPORT_DIR_NAME("elastos.droid.internal.backup.CLocalTransport");
const String CLocalTransport::TRANSPORT_DESTINATION_STRING("Backing up to debug-only private cache");
const Int64 CLocalTransport::RESTORE_TOKEN = 1;

CLocalTransport::CLocalTransport()
    : mContext(NULL)
    , mDataDir(NULL)
    , mRestorePackages(NULL)
    , mRestorePackage(-1)
{

}

CLocalTransport::~CLocalTransport()
{

}

ECode CLocalTransport::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> file;
    env->GetDownloadCacheDirectory((IFile**)&file);
    CFile::New(file, String("backup"), (IFile**)&mDataDir);

    return NOERROR;
}

ECode CLocalTransport::ConfigurationIntent(
    /* [out] */ IIntent** intent)
{
    // The local transport is not user-configurable
    VALIDATE_NOT_NULL(intent);
    return NOERROR;
}

ECode CLocalTransport::CurrentDestinationString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);
    *string = TRANSPORT_DESTINATION_STRING;
    return NOERROR;
}

ECode CLocalTransport::TransportDirName(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);
    *string = TRANSPORT_DIR_NAME;
    return NOERROR;
}

ECode CLocalTransport::RequestBackupTime(
    /* [out] */ Int64* time)
{
    // any time is a good time for local backup
    VALIDATE_NOT_NULL(time);
    *time = 0;
    return NOERROR;
}


ECode CLocalTransport::InitializeDevice(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    if (DEBUG) {
        Logger::V(TAG, "wiping all data");
    }
    DeleteContents(mDataDir);
    *value = IBackupConstants::TRANSPORT_OK;

    return NOERROR;
}

ECode CLocalTransport::PerformBackup(
    /* [in] */ IPackageInfo* packageInfo,
    /* [in] */ IParcelFileDescriptor* inFd,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    //TODO Base64 doesn't implement, so we will do this fun next.
    /*
     *assert(0);
     *return NOERROR;
     */
    String packageName;
    packageInfo->GetPackageName(&packageName);
    if (DEBUG) {
        String datadir;
        mDataDir->ToString(&datadir);
        Logger::V(TAG, "performBackup() pkg=%s, datadir[%s]", packageName.string(), datadir.string());
    }

    AutoPtr<IFile> packageDir;
    CFile::New(mDataDir, packageName, (IFile**)&packageDir);
    Boolean tempResult;
    packageDir->Mkdirs(&tempResult);

    // Each 'record' in the restore set is kept in its own file, named by
    // the record key.  Wind through the data file, extracting individual
    // record operations and building a set of all the updates to apply
    // in this update.
    AutoPtr<IFileDescriptor> fDes;
    inFd->GetFileDescriptor((IFileDescriptor**)&fDes);
    AutoPtr<IBackupDataInput> changeSet;
    CBackupDataInput::New(fDes, (IBackupDataInput**)&changeSet);

    Int32 bufSize = 512;
    AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(bufSize);

    Boolean bHasNextHeader;
    while(changeSet->ReadNextHeader(&bHasNextHeader), bHasNextHeader == TRUE) {
        String key;
        changeSet->GetKey(&key);

        String base64Key;
        AutoPtr<IBase64> base64;
        CBase64::AcquireSingleton((IBase64**)&base64);
        AutoPtr<ArrayOf<Byte> > bytes = key.GetBytes();
        base64->EncodeToString(*bytes, IBase64::DEFAULT, &base64Key);

        AutoPtr<IFile> entityFile;
        CFile::New(packageDir, base64Key, (IFile**)&entityFile);

        Int32 dataSize;
        changeSet->GetDataSize(&dataSize);

        if (DEBUG) {
            Logger::V(TAG, "Got chanage set key=%s, size=%d, key64=%s", key.string(), dataSize, base64Key.string());
        }

        if (dataSize > 0) {
            Boolean bIsExist;
            entityFile->Exists(&bIsExist);
            if (bIsExist) {
                Boolean tempResult;
                entityFile->Delete(&tempResult);
            }

            AutoPtr<IFileOutputStream> entity;
            CFileOutputStream::New(entityFile, (IFileOutputStream**)&entity);
            if (dataSize > bufSize) {
                bufSize = dataSize;
                buf = ArrayOf<Byte>::Alloc(bufSize);
            }

            Int32 readSize;
            changeSet->ReadEntityData(buf, 0, dataSize, &readSize);
            if (DEBUG) {
                Logger::V(TAG, "data size:%d", dataSize);
            }
            FAIL_GOTO(entity->WriteBytes(*buf, 0, dataSize),ERROR);
            entity->Close();
        } else {
            Boolean tempResult;
            entityFile->Delete(&tempResult);
        }
    }

    return NOERROR;
ERROR:
    Logger::E(TAG, "Unable to updata key file ");
    *value = IBackupConstants::TRANSPORT_ERROR;
    return E_IO_EXCEPTION;
}

ECode CLocalTransport::ClearBackupData(
    /* [in] */ IPackageInfo* packageInfo,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    String pkgName;
    packageInfo->GetPackageName(&pkgName);
    if (DEBUG) {
        Logger::V(TAG, "ClearBackupData() pkg=%s", pkgName.string());
    }

    AutoPtr<IFile> packageDir;
    CFile::New(mDataDir, pkgName, (IFile**)&packageDir);

    AutoPtr<ArrayOf<IFile*> > fileSet;
    packageDir->ListFiles((ArrayOf<IFile*>**)&fileSet);
    if (NULL != fileSet) {
        Int32 cnt = fileSet->GetLength();
        Boolean bTemp;
        for(Int32 i = 0; i < cnt; ++i) {
            (*fileSet)[i]->Delete(&bTemp);
        }
        packageDir->Delete(&bTemp);
    }
    *value = IBackupConstants::TRANSPORT_OK;
    return NOERROR;
}


ECode CLocalTransport::FinishBackup(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    if (DEBUG) {
        Logger::V(TAG, "FinishBackup()");
    }
    *value = IBackupConstants::TRANSPORT_OK;
    return NOERROR;
}

ECode CLocalTransport::GetAvailableRestoreSets(
    /* [out, callee] */ ArrayOf<IRestoreSet*>** result)
{
    VALIDATE_NOT_NULL(result);
    // one hardcoded restore set
    AutoPtr<IRestoreSet> set;
    CRestoreSet::New(String("Local disk image"), String("flash"), RESTORE_TOKEN, (IRestoreSet**)&set);
    AutoPtr<ArrayOf<IRestoreSet*> > _array = ArrayOf<IRestoreSet*>::Alloc(1);
    _array->Set(0, set);
    *result = _array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CLocalTransport::GetCurrentRestoreSet(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    // The hardcoded restore set always has the same token
    *value = RESTORE_TOKEN;
    return NOERROR;
}

ECode CLocalTransport::StartRestore(
    /* [in] */ Int64 token,
    /* [in] */ ArrayOf<IPackageInfo*>* packages,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    if(DEBUG) {
        Logger::V(TAG, "StartRestore() token=%d", token);
    }
    mRestorePackages = packages;
    mRestorePackage = -1;
    *value = IBackupConstants::TRANSPORT_OK;
    return NOERROR;
}

ECode CLocalTransport::NextRestorePackage(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (NULL == mRestorePackages) {
        Logger::E(TAG, "StartRestore not called");
        assert(0);
    }

    while (++mRestorePackage < mRestorePackages->GetLength()) {
        String name;
        (*mRestorePackages)[mRestorePackage]->GetPackageName(&name);
        AutoPtr<IFile> file;
        CFile::New(mDataDir, name, (IFile**)&file);
        Boolean bIsDir;
        file->IsDirectory(&bIsDir);
        if (bIsDir) {
            if (DEBUG) Logger::V(TAG, "NextRestorePackage() = %s", name.string());
            *result = name;
            return NOERROR;
        }
    }
    if (DEBUG) {
        Logger::V(TAG, "no more packages to restore");
    }
    *result = String("");
    return NOERROR;
}

ECode CLocalTransport::GetRestoreData(
    /* [in] */ IParcelFileDescriptor* outFd,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    Logger::E(TAG, "GetRestoreData will soonly realized");

    if (NULL == mRestorePackages) {
        Logger::E(TAG, "StartRestore not called");
        assert(0);
    }
    if (mRestorePackages < 0) {
        Logger::E(TAG, "NextRestorePackage not called");
        assert(0);
    }

    String name;
    (*mRestorePackages)[mRestorePackage]->GetPackageName(&name);
    AutoPtr<IFile> packageDir;
    CFile::New(mDataDir, name, (IFile**)&packageDir);

    // The restore set is the concatenation of the individual record blobs,
    // each of which is a file in the package's directory
    AutoPtr<ArrayOf<IFile*> > blobs;
    packageDir->ListFiles((ArrayOf<IFile*>**)&blobs);
    if (NULL == blobs) {
        // nextRestorePackage() ensures the dir exists, so this is an error
        String str;
        packageDir->ToString(&str);
        Logger::E(TAG, "Error listing directory: %s", str.string());
        *value = IBackupConstants::TRANSPORT_ERROR;
        assert(0);
    }

    // We expect at least some data if the directory exists in the first place
    if (DEBUG) Logger::V(TAG, "GetRestoreData() found %d key files",blobs->GetLength());
    AutoPtr<IBackupDataOutput> out;
    AutoPtr<IFileDescriptor> fDes;
    outFd->GetFileDescriptor((IFileDescriptor**)&fDes);
    CBackupDataOutput::New(fDes, (IBackupDataOutput**)&out);

    Int32 cnt = blobs->GetLength();
    Int64 size64;
    Int32 size;
    //try{
    ECode ec = NOERROR;
    for (Int32 i = 0; i < cnt; ++i) {
        AutoPtr<IFileInputStream> in;
        ec = CFileInputStream::New((*blobs)[i], (IFileInputStream**)&in);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Logger::E(TAG, "Unable to read backup records");
            *value = IBackupConstants::TRANSPORT_ERROR;
            return E_IO_EXCEPTION;
        }
        //try{
        (*blobs)[i]->GetLength(&size64);
        size = (Int32)size64;
        AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(size);
        Int32 len;
        ec = in->ReadBytes(buf, &len);
        if(FAILED(ec)) {
            in->Close();
            Logger::E(TAG, "Unable to read backup records");
            *value = IBackupConstants::TRANSPORT_ERROR;
            return E_IO_EXCEPTION;
        }

        AutoPtr<IBase64> base64;
        CBase64::AcquireSingleton((IBase64**)&base64);
        String fileName;
        (*blobs)[i]->GetName(&fileName);
        AutoPtr<ArrayOf<Byte> > result;
        base64->Decode(fileName, IBase64::DEFAULT, (ArrayOf<Byte>**)&result);
        String key((char*)result->GetPayload());

        if (DEBUG) Logger::V(TAG, "... filename:%s, key=%s, size=%d",fileName.string(), key.string(), size);

        Int32 writtenSize;
        out->WriteEntityHeader(key, size, &writtenSize);
        out->WriteEntityData(buf, size, &writtenSize);
        //} finally{
        in->Close();
        //}

    }
    *value = IBackupConstants::TRANSPORT_OK;
    return NOERROR;
    //} catch (IOException e) {
    // Log.e(TAG, "Unable to read backup records", e);
    // return BackupConstants.TRANSPORT_ERROR;
    //}
    //
}

ECode CLocalTransport::FinishRestore()
{
    if (DEBUG) Logger::V(TAG, "FinishRestore()");
    return NOERROR;
}


void CLocalTransport::DeleteContents(
    /* [in] */ IFile* dirname)
{
    AutoPtr<ArrayOf<IFile*> > contents;
    dirname->ListFiles((ArrayOf<IFile*>**)&contents);
    if (contents != NULL) {
        Int32 cnt = contents->GetLength();
        Boolean bIsDir;
        Boolean bTemp;
        for (Int32 i = 0; i < cnt; ++i) {
            (*contents)[i]->IsDirectory(&bIsDir);
            if (bIsDir) {
                // delete the directory's contents then fall through
                // and delete the directory itself.
                DeleteContents((*contents)[i]);
            }
            (*contents)[i]->Delete(&bTemp);
        }
    }
}

} // Backup
} // Internal
} // Droid
} // namespace Elastos
