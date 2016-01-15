#include "elastos/droid/app/backup/CBackupDataOutput.h"
#include <elastos/utility/logging/Logger.h>
#include <androidfw/BackupHelpers.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

const String CBackupDataOutput::TAG("CBackupDataOutput");
const Boolean CBackupDataOutput::DEBUG = FALSE;

CAR_INTERFACE_IMPL(CBackupDataOutput, Object, IBackupDataOutput)

CAR_OBJECT_IMPL(CBackupDataOutput)

CBackupDataOutput::CBackupDataOutput()
    : mBackupWriter(0)
{
}

CBackupDataOutput::~CBackupDataOutput()
{
    NativeDtor();
}

ECode CBackupDataOutput::constructor(
    /* [in] */ IFileDescriptor* fd)
{
    if (NULL == fd) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mBackupWriter = NativeCtor(fd);
    if (mBackupWriter == 0) {
        // throw new RuntimeException("Native initialization failed with fd=" + fd);
        Logger::E(TAG, "Native initialization failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CBackupDataOutput::WriteEntityHeader(
    /* [in] */ const String& key,
    /* [in] */ Int32 dataSize,
    /* [out] */ Int32* writtenSize)
{
    VALIDATE_NOT_NULL(writtenSize);
    Int32 result = NativeWriteEntityHeader(key, dataSize);
    if (result >= 0) {
        *writtenSize = result;
        return NOERROR;
    }
    else {
        //throw new IOException("result=0x" + Integer.toHexString(result));
        Logger::E(TAG, "result=0x%x", result);
        return E_IO_EXCEPTION;
    }
}

ECode CBackupDataOutput::WriteEntityData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 size,
    /* [out] */ Int32* writtenSize)
{
    VALIDATE_NOT_NULL(writtenSize);
    Int32 result = NativeWriteEntityData(data, size);
    if (result >= 0) {
        *writtenSize = result;
        return NOERROR;
    }
    else {
        //throw new IOException("result=0x" + Integer.toHexString(result));
        Logger::E(TAG, "result=0x%x", result);
        return E_IO_EXCEPTION;
    }
}

ECode CBackupDataOutput::SetKeyPrefix(
    /* [in] */ const String& keyPrefix)
{
    NativeSetKeyPrefix(keyPrefix);
    return NOERROR;
}


Int32 CBackupDataOutput::GetNativeBackupDataWriter()
{
    return mBackupWriter;
}

Int32 CBackupDataOutput::NativeCtor(
    /* [in] */ IFileDescriptor* fdes)
{
    Int32 fd = 0;
    fdes->GetDescriptor(&fd);
    if (fd == -1) {
        return 0;
    }
    return (Int32)(new android::BackupDataWriter(fd));
}

void CBackupDataOutput::NativeDtor()
{
    delete (android::BackupDataWriter*)mBackupWriter;
}

Int32 CBackupDataOutput::NativeWriteEntityHeader(
    /* [in] */ const String& key,
    /* [in] */ Int32 dataSize)
{
    int err;
    android::BackupDataWriter* writer = (android::BackupDataWriter*)mBackupWriter;

    if (key.IsNull()) {
        return -1;
    }

    err = writer->WriteEntityHeader(android::String8(key.string()), dataSize);
    return err;
}

Int32 CBackupDataOutput::NativeWriteEntityData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 size)
{
    int err;
    android::BackupDataWriter* writer = (android::BackupDataWriter*)mBackupWriter;

    if (data == NULL) {
        return -1;
    }

    if (data->GetLength() < size) {
        // size mismatch
        return -1;
    }

    err = writer->WriteEntityData(data->GetPayload(), size);
    return err;
}

void CBackupDataOutput::NativeSetKeyPrefix(
    /* [in] */ const String& key)
{
    int err;
    android::BackupDataWriter* writer = (android::BackupDataWriter*)mBackupWriter;

    const char* keyPrefixUTF = key.string();
    android::String8 keyPrefix(keyPrefixUTF ? keyPrefixUTF : "");

    writer->SetKeyPrefix(keyPrefix);
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
