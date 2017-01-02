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

#include "elastos/droid/server/PersistentDataBlockService.h"
#include "elastos/droid/server/CPersistentDataBlockBinderService.h"
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/R.h>
#include <elastos/droid/Manifest.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::R;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::Service::Persistentdata::EIID_IIPersistentDataBlockService;

using Elastos::IO::ICloseable;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::Channels::IFileChannel;
using Elastos::IO::IDataInput;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Server {

const String PersistentDataBlockService::TAG("PersistentDataBlockService");

const String PersistentDataBlockService::PERSISTENT_DATA_BLOCK_PROP("ro.frp.pst");
const Int32 PersistentDataBlockService::HEADER_SIZE = 8;
// Magic number to mark block device as adhering to the format consumed by this service
const Int32 PersistentDataBlockService::PARTITION_TYPE_MARKER = 0x1990;
// Limit to 100k as blocks larger than this might cause strain on Binder.
// TODO(anmorales): Consider splitting up too-large blocks in PersistentDataBlockManager
const Int32 PersistentDataBlockService::MAX_DATA_BLOCK_SIZE = 1024 * 100;

//===================================================================================
// PersistentDataBlockService::BinderService
//===================================================================================

CAR_INTERFACE_IMPL_2(PersistentDataBlockService::BinderService, Object, IIPersistentDataBlockService, IBinder)


ECode PersistentDataBlockService::BinderService::constructor(
    /* [in] */ ISystemService* service)
{
    mHost = (PersistentDataBlockService*)service;
    return NOERROR;
}

ECode PersistentDataBlockService::BinderService::Write(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;

    FAIL_RETURN(mHost->EnforceUid(Binder::GetCallingUid()))

    // Need to ensure we don't write over the last Byte
    Int64 maxBlockSize = mHost->GetBlockDeviceSize() - HEADER_SIZE - 1;
    if (data->GetLength() > maxBlockSize) {
        // partition is ~500k so shouldn't be a problem to downcast
        *result = (Int32) -maxBlockSize;
        return NOERROR;
    }

    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    AutoPtr<IByteBuffer> headerAndData;
    AutoPtr< ArrayOf<Byte> > array;
    AutoPtr<IFile> file;
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IDataOutputStream> outputStream;

    ECode ec = CFile::New(mHost->mDataBlockFile, (IFile**)&file);
    FAIL_GOTO(ec, _EXIT_)

    ec = CFileOutputStream::New(file, (IFileOutputStream**)&fos);
    FAIL_GOTO(ec, _EXIT_)

    ec = CDataOutputStream::New(IOutputStream::Probe(fos), (IDataOutputStream**)&outputStream);
    FAIL_GOTO(ec, _EXIT_)

    goto _CONTINUE_;

_EXIT_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::E(PersistentDataBlockService::TAG, "partition not available? %s",
            mHost->mDataBlockFile.string());
        *result = -1;
        return NOERROR;
    }
    else {
        Slogger::E(PersistentDataBlockService::TAG, "partition not available? ec=%08x", ec);
        return ec;
    }

_CONTINUE_:

    helper->Allocate(data->GetLength() + HEADER_SIZE, (IByteBuffer**)&headerAndData);
    headerAndData->PutInt32(PARTITION_TYPE_MARKER);
    headerAndData->PutInt32(data->GetLength());
    headerAndData->Put(data);

    {    AutoLock syncLock(mHost->mLock);
        headerAndData->GetArray((ArrayOf<Byte>**)&array);
        ec = IOutputStream::Probe(outputStream)->Write(array);
    }

    if (SUCCEEDED(ec)) {
        *result = data->GetLength();
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(PersistentDataBlockService::TAG, "failed writing to the persistent data block");
        *result = -1;
    }

    ICloseable::Probe(outputStream)->Close();
    return NOERROR;
}

//@Override
ECode PersistentDataBlockService::BinderService::Read(
    /* [out, callee] */ ArrayOf<Byte>** data)
{
    VALIDATE_NOT_NULL(data)
    *data = NULL;

    FAIL_RETURN(mHost->EnforceUid(Binder::GetCallingUid()))

    AutoPtr<IFile> file;
    AutoPtr<IFileInputStream> fos;
    AutoPtr<IDataInputStream> inputStream;
    Int32 totalDataSize = 0, read;
    AutoPtr<ArrayOf<Byte> > bytes;

    ECode ec = CFile::New(mHost->mDataBlockFile, (IFile**)&file);
    FAIL_GOTO(ec, _EXIT_)

    ec = CFileInputStream::New(file, (IFileInputStream**)&fos);
    FAIL_GOTO(ec, _EXIT_)

    ec = CDataInputStream::New(IInputStream::Probe(fos), (IDataInputStream**)&inputStream);
    FAIL_GOTO(ec, _EXIT_)

    goto _CONTINUE_;

_EXIT_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::E(PersistentDataBlockService::TAG, "partition not available? %s",
            mHost->mDataBlockFile.string());
        return NOERROR;
    }
    else {
        Slogger::E(PersistentDataBlockService::TAG, "partition not available? ec=%08x", ec);
        return ec;
    }

_CONTINUE_:

    {    AutoLock syncLock(mHost->mLock);
        ec = mHost->GetTotalDataSizeLocked(inputStream, &totalDataSize);
        FAIL_GOTO(ec, _EXIT2_)

        if (totalDataSize == 0) {
            bytes = ArrayOf<Byte>::Alloc(0);
            *data = bytes;
            REFCOUNT_ADD(*data)
            return NOERROR;
        }

        bytes = ArrayOf<Byte>::Alloc(totalDataSize);
        ec = IInputStream::Probe(inputStream)->Read(bytes, 0, totalDataSize, &read);
        FAIL_GOTO(ec, _EXIT2_)

        if (read < totalDataSize) {
            // something went wrong, not returning potentially corrupt data
            Slogger::E(PersistentDataBlockService::TAG,
                "failed to read entire data block. bytes read: %d/%d",
                read, totalDataSize);
            *data = NULL;
            ICloseable::Probe(inputStream)->Close();
            return NOERROR;
        }
    }

_EXIT2_:
    if (SUCCEEDED(ec)) {
        *data = bytes;
        REFCOUNT_ADD(*data)
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "failed to read data");
        *data = NULL;
    }

    ICloseable::Probe(inputStream)->Close();
    return NOERROR;
}

ECode PersistentDataBlockService::BinderService::Wipe()
{
    FAIL_RETURN(mHost->EnforceOemUnlockPermission())

    {    AutoLock syncLock(mHost->mLock);
        Int32 ret = mHost->NativeWipe(mHost->mDataBlockFile);

        if (ret < 0) {
            Slogger::E(TAG, "failed to wipe persistent partition");
        }
    }
    return NOERROR;
}

//@Override
ECode PersistentDataBlockService::BinderService::SetOemUnlockEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IActivityManagerHelper> amHelper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&amHelper);

    // do not allow monkey to flip the flag
    Boolean bval;
    amHelper->IsUserAMonkey(&bval);
    if (bval) {
        return NOERROR;
    }

    FAIL_RETURN(mHost->EnforceOemUnlockPermission())

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    AutoPtr<IByteBuffer> data;
    AutoPtr<ArrayOf<Byte> > array;
    AutoPtr<IFile> file;
    AutoPtr<IFileOutputStream> outputStream;
    AutoPtr<IFileChannel> channel;
    Int32 number;

    ECode ec = CFile::New(mHost->mDataBlockFile, (IFile**)&file);
    FAIL_GOTO(ec, _EXIT_)

    ec = CFileOutputStream::New(file, (IFileOutputStream**)&outputStream);
    FAIL_GOTO(ec, _EXIT_)

    goto _CONTINUE_;

_EXIT_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::E(PersistentDataBlockService::TAG, "partition not available? %s",
            mHost->mDataBlockFile.string());
        return NOERROR;
    }
    else {
        Slogger::E(PersistentDataBlockService::TAG, "partition not available? ec=%08x", ec);
        return ec;
    }

_CONTINUE_:

    ec = outputStream->GetChannel((IFileChannel**)&channel);
    FAIL_GOTO(ec, _EXIT2_)

    ec = channel->Position(mHost->GetBlockDeviceSize() - 1);
    FAIL_GOTO(ec, _EXIT2_)

    helper->Allocate(1, (IByteBuffer**)&data);
    ec = data->Put(enabled ? (Byte) 1 : (Byte) 0);
    FAIL_GOTO(ec, _EXIT2_)

    ec = IBuffer::Probe(data)->Flip();
    FAIL_GOTO(ec, _EXIT2_)

    {    AutoLock syncLock(mHost->mOemLock);
        ec = channel->Write(data, &number);
        FAIL_GOTO(ec, _EXIT2_)
    }

_EXIT2_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "unable to access persistent partition");
    }

    ioUtils->CloseQuietly(ICloseable::Probe(outputStream));
    return ec;
}

ECode PersistentDataBlockService::BinderService::GetOemUnlockEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    FAIL_RETURN(mHost->EnforceOemUnlockPermission())

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    AutoPtr<IFile> file;
    AutoPtr<IFileInputStream> fos;
    AutoPtr<IDataInputStream> inputStream;
    Byte read = 0;
    Int64 number;

    ECode ec = CFile::New(mHost->mDataBlockFile, (IFile**)&file);
    FAIL_GOTO(ec, _EXIT_)

    ec = CFileInputStream::New(file, (IFileInputStream**)&fos);
    FAIL_GOTO(ec, _EXIT_)

    ec = CDataInputStream::New(IInputStream::Probe(fos), (IDataInputStream**)&inputStream);
    FAIL_GOTO(ec, _EXIT_)

    goto _CONTINUE_;

_EXIT_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::E(PersistentDataBlockService::TAG, "partition not available? %s",
            mHost->mDataBlockFile.string());
        return NOERROR;
    }
    else {
        Slogger::E(PersistentDataBlockService::TAG, "partition not available? ec=%08x", ec);
        return ec;
    }

_CONTINUE_:

    ec = IInputStream::Probe(inputStream)->Skip(mHost->GetBlockDeviceSize() - 1, &number);
    FAIL_GOTO(ec, _EXIT2_)

    {    AutoLock syncLock(mHost->mOemLock);
        ec = IDataInput::Probe(inputStream)->ReadByte(&read);
        FAIL_GOTO(ec, _EXIT2_)

        *result = read != 0;
    }

_EXIT2_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "unable to access persistent partition");
        ec = NOERROR;
    }

    ioUtils->CloseQuietly(ICloseable::Probe(inputStream));
    return ec;
}

//@Override
ECode PersistentDataBlockService::BinderService::GetDataBlockSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    FAIL_RETURN(mHost->EnforceUid(Binder::GetCallingUid()))

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    AutoPtr<IFile> file;
    AutoPtr<IFileInputStream> fos;
    AutoPtr<IDataInputStream> inputStream;

    ECode ec = CFile::New(mHost->mDataBlockFile, (IFile**)&file);
    FAIL_GOTO(ec, _EXIT_)

    ec = CFileInputStream::New(file, (IFileInputStream**)&fos);
    FAIL_GOTO(ec, _EXIT_)

    ec = CDataInputStream::New(IInputStream::Probe(fos), (IDataInputStream**)&inputStream);
    FAIL_GOTO(ec, _EXIT_)

    goto _CONTINUE_;

_EXIT_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::E(PersistentDataBlockService::TAG, "partition not available? %s",
            mHost->mDataBlockFile.string());
        return NOERROR;
    }
    else {
        Slogger::E(PersistentDataBlockService::TAG, "partition not available? ec=%08x", ec);
        return ec;
    }

_CONTINUE_:

    {    AutoLock syncLock(mHost->mLock);
        ec = mHost->GetTotalDataSizeLocked(inputStream, result);
    }

    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "error reading data block size");
        *result = 0;
    }

    ioUtils->CloseQuietly(ICloseable::Probe(inputStream));
    return NOERROR;
}

//@Override
ECode PersistentDataBlockService::BinderService::GetMaximumDataBlockSize(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 actualSize = mHost->GetBlockDeviceSize() - HEADER_SIZE - 1;
    *result = actualSize <= MAX_DATA_BLOCK_SIZE ? actualSize : MAX_DATA_BLOCK_SIZE;
    return NOERROR;
}

ECode PersistentDataBlockService::BinderService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//===================================================================================
// PersistentDataBlockService
//===================================================================================

PersistentDataBlockService::PersistentDataBlockService()
    : mAllowedAppId(-1)
    , mBlockDeviceSize(0)
{
}

PersistentDataBlockService::~PersistentDataBlockService()
{}

ECode PersistentDataBlockService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);

    CPersistentDataBlockBinderService::New(this, (IBinder**)&mService);

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);

    mContext = context;
    sysProp->Get(PERSISTENT_DATA_BLOCK_PROP, &mDataBlockFile);
    mBlockDeviceSize = -1; // Load lazily
    mAllowedAppId = GetAllowedAppId(UserHandle::USER_OWNER);
    return NOERROR;
}

ECode PersistentDataBlockService::OnStart()
{
    PublishBinderService(IContext::PERSISTENT_DATA_BLOCK_SERVICE, mService);
    return NOERROR;
}

Int32 PersistentDataBlockService::GetAllowedAppId(
    /* [in] */ Int32 userHandle)
{
    String allowedPackage;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetString(R::string::config_persistentDataPackageName, &allowedPackage);
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 allowedUid = -1;
    ECode ec = pm->GetPackageUid(allowedPackage, userHandle, &allowedUid);
    if (ec == (ECode)E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
        // not expected
        Slogger::E(TAG, "not able to find package %s", allowedPackage.string());
    }
    return UserHandle::GetAppId(allowedUid);
}

ECode PersistentDataBlockService::EnforceOemUnlockPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::OEM_UNLOCK_STATE,
        String("Can't access OEM unlock state"));
}

ECode PersistentDataBlockService::EnforceUid(
    /* [in] */ Int32 callingUid)
{
    if (UserHandle::GetAppId(callingUid) != mAllowedAppId) {
        Slogger::E(TAG, "uid %d not allowed to access PST", callingUid);
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode PersistentDataBlockService::GetTotalDataSizeLocked(
    /* [in] */ IDataInputStream* inputStream,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 totalDataSize;
    Int32 blockId;
    FAIL_RETURN(IDataInput::Probe(inputStream)->ReadInt32(&blockId))
    if (blockId == PARTITION_TYPE_MARKER) {
        IDataInput::Probe(inputStream)->ReadInt32(&totalDataSize);
    }
    else {
        totalDataSize = 0;
    }
    *result = totalDataSize;
    return NOERROR;
}

Int64 PersistentDataBlockService::GetBlockDeviceSize()
{
    {    AutoLock syncLock(mLock);
        if (mBlockDeviceSize == -1) {
            mBlockDeviceSize = NativeGetBlockDeviceSize(mDataBlockFile);
        }
    }

    return mBlockDeviceSize;
}

//============================================================================
// native codes
//============================================================================
#include <utils/misc.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <utils/Log.h>

#include <inttypes.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>


uint64_t get_block_device_size(int fd)
{
    uint64_t size = 0;
    int ret;

    ret = ioctl(fd, BLKGETSIZE64, &size);

    if (ret)
        return 0;

    return size;
}

int wipe_block_device(int fd)
{
    uint64_t range[2];
    int ret;
    uint64_t len = get_block_device_size(fd);

    range[0] = 0;
    range[1] = len;

    if (range[1] == 0)
        return 0;

    ret = ioctl(fd, BLKSECDISCARD, &range);
    if (ret < 0) {
        ALOGE("Something went wrong secure discarding block: %s\n", strerror(errno));
        range[0] = 0;
        range[1] = len;
        ret = ioctl(fd, BLKDISCARD, &range);
        if (ret < 0) {
            ALOGE("Discard failed: %s\n", strerror(errno));
            return -1;
        } else {
            ALOGE("Wipe via secure discard failed, used non-secure discard instead\n");
            return 0;
        }

    }

    return ret;
}

Int64 PersistentDataBlockService::NativeGetBlockDeviceSize(
    /* [in] */ const String& path)
{
    int fd = open(path.string(), O_RDONLY);

    if (fd < 0)
        return 0;

    return get_block_device_size(fd);
}

Int32 PersistentDataBlockService::NativeWipe(
    /* [in] */ const String& path)
{
    int fd = open(path.string(), O_WRONLY);

    if (fd < 0)
        return 0;

    return wipe_block_device(fd);
}


}// Server
}// Droid
}// Elastos
