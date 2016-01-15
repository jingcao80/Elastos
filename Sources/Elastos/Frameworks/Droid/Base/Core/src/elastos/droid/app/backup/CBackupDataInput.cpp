#include "elastos/droid/app/backup/CBackupDataInput.h"
#include <elastos/utility/logging/Logger.h>
#include <androidfw/BackupHelpers.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

//===============================================================
//               CBackupDataInput::EntityHeader
//===============================================================

CBackupDataInput::EntityHeader::EntityHeader()
    : mDataSize(0)
{
}

//===============================================================
//                     CBackupDataInput
//===============================================================

const String CBackupDataInput::TAG("CBackupDataInput");
const Boolean CBackupDataInput::DEBUG = FALSE;

CAR_INTERFACE_IMPL(CBackupDataInput, Object, IBackupDataInput)

CAR_OBJECT_IMPL(CBackupDataInput)

CBackupDataInput::CBackupDataInput()
    : mHeaderReady(FALSE)
    , mBackupReader(0)
{
    mHeader = new EntityHeader();
}

CBackupDataInput::~CBackupDataInput()
{
    NativeDtor();
}

ECode CBackupDataInput::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    if (fd == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mBackupReader = NativeCtor(fd);
    if (mBackupReader == 0) {
        // throw new RuntimeException("Native initialization failed with fd=" + fd);
        Logger::E(TAG, "Native initialization failed ");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CBackupDataInput::ReadNextHeader(
    /* [out] */ Boolean* next)
{
    VALIDATE_NOT_NULL(next);
    Int32 result = NativeReadNextHeader(mHeader);
    if (result == 0) {
        //read sucessfully
        mHeaderReady = true;
        *next = TRUE;
        return NOERROR;
    }
    else if (result > 0) {
        //done
        mHeaderReady = FALSE;
        *next = FALSE;
        return NOERROR;
    }
    else {
        //error
        mHeaderReady = FALSE;
        // throw new IOException("failed: 0x" + Integer.toHexString(result));
        Logger::E(TAG, "failed: 0x%x", result);
        return E_IO_EXCEPTION;
    }
}

ECode CBackupDataInput::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);
    if (mHeaderReady) {
        *key = mHeader->mKey;
        return NOERROR;
    }
    else {
        //throw new IllegalStateException("Entity header not read");
        Logger::E(TAG, "Entity header not read");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CBackupDataInput::GetDataSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    if (mHeaderReady) {
        *size = mHeader->mDataSize;
        return NOERROR;
    }
    else {
        //throw new IllegalStateException("Entity header not read");
        Logger::E(TAG, "Entity header not read");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CBackupDataInput::ReadEntityData(
    /* [in, out] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [out] */ Int32* readSize)
{
    VALIDATE_NOT_NULL(readSize);
    if (mHeaderReady) {
        Int32 result = NativeReadEntityData(data, offset, size);
        if (result >= 0) {
            *readSize = result;
            return NOERROR;
        }
        else {
            //throw new IOException("result=0x" + Integer.toHexString(result));
            Logger::E(TAG, "result=0x%x", result);
            return E_IO_EXCEPTION;
        }
    }
    else {
        //throw new IllegalStateException("Entity header not read");
        Logger::E(TAG, "Entity header not read");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CBackupDataInput::SkipEntityData()
{
    if (mHeaderReady) {
        NativeSkipEntityData();
        return NOERROR;
    }
    else {
        //throw new IllegalStateException("Entity header not read");
        Logger::E(TAG, "Entity header not read");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode CBackupDataInput::GetBackupReader(
    /* [out] */ Int32* backupReader)
{
    VALIDATE_NOT_NULL(backupReader);
    *backupReader = mBackupReader;
    return NOERROR;
}

Int32 CBackupDataInput::NativeCtor(
    /* [in] */ IFileDescriptor* fdes)
{
    Int32 fd;
    fdes->GetDescriptor(&fd);
    return (Int32)(new android::BackupDataReader(fd));
}

void CBackupDataInput::NativeDtor()
{
    delete (android::BackupDataReader*)mBackupReader;
}

Int32 CBackupDataInput::NativeReadNextHeader(
    /* [in] */ EntityHeader* entity)
{
    int err;
    bool done;
    android::BackupDataReader* reader = (android::BackupDataReader*)mBackupReader;

    int type = 0;

    err = reader->ReadNextHeader(&done, &type);
    if (done) {
        return 1;
    }

    if (err != 0) {
        return err < 0 ? err : -1;
    }

    switch (type) {
        case android::BACKUP_HEADER_ENTITY_V1:
        {
            android::String8 key;
            size_t dataSize;
            err = reader->ReadEntityHeader(&key, &dataSize);
            if (err != 0) {
                return err < 0 ? err : -1;
            }
            //TODO: Set the fields in the entity object
            entity->mKey = key.string();
            entity->mDataSize = dataSize;
            return 0;
        }
    default:
        Logger::E(TAG, "Unknown header type: %p", type);
        return -1;
    }

    return 1;
}


Int32 CBackupDataInput::NativeReadEntityData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size)
{
    int err;
    android::BackupDataReader* reader = (android::BackupDataReader*)mBackupReader;

    if (data == NULL) {
        return -2;
    }
    if (data->GetLength() < (size + offset)) {
        //size mismatch
        return -1;
    }
    err = reader->ReadEntityData(data->GetPayload() + offset, size);
    return err;

}

Int32 CBackupDataInput::NativeSkipEntityData()
{
    int err;
    android::BackupDataReader* reader = (android::BackupDataReader*)mBackupReader;

    err = reader->SkipEntityData();
    return err;
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
