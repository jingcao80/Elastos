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

#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/FileBridge.h"
#include <elastos/utility/Arrays.h>
#include <elastos/droid/system/Os.h>
#include <elastos/droid/system/OsConstants.h>

using Elastos::Utility::Arrays;
using Elastos::Droid::System::Os;
using Elastos::Droid::System::OsConstants;
using Elastos::IO::ByteOrder_BIG_ENDIAN;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;
using Libcore::IO::IMemory;
using Libcore::IO::CMemory;

namespace Elastos {
namespace Droid {
namespace Os {

const String FileBridge::TAG("FileBridge");

const Int32 FileBridge::MSG_LENGTH = 8;

/** CMD_WRITE [len] [data] */
const Int32 FileBridge::CMD_WRITE = 1;
/** CMD_FSYNC */
const Int32 FileBridge::CMD_FSYNC = 2;
/** CMD_CLOSE */
const Int32 FileBridge::CMD_CLOSE = 3;

CAR_INTERFACE_IMPL(FileBridge::FileBridgeOutputStream, OutputStream, IFileBridgeOutputStream)

FileBridge::FileBridgeOutputStream::FileBridgeOutputStream(
    /* [in] */ IParcelFileDescriptor* clientPfd)
{
    mTemp = ArrayOf<Byte>::Alloc(MSG_LENGTH);
    mClientPfd = clientPfd;
    clientPfd->GetFileDescriptor((IFileDescriptor**)&mClient);
}

FileBridge::FileBridgeOutputStream::FileBridgeOutputStream(
    /* [in] */ IFileDescriptor* client)
{
    mTemp = ArrayOf<Byte>::Alloc(MSG_LENGTH);
    mClientPfd = NULL;
    mClient = client;
}

ECode FileBridge::FileBridgeOutputStream::Close()
{
    // try {
    ECode ec = WriteCommandAndBlock(FileBridge::CMD_CLOSE, String("close()"));
    // } finally {
    AutoPtr<IIoBridge> ib;
    CIoBridge::AcquireSingleton((IIoBridge**)&ib);
    ib->CloseAndSignalBlockedThreads(mClient);

    AutoPtr<IIoUtils> iu;
    CIoUtils::AcquireSingleton((IIoUtils**)&iu);
    iu->CloseQuietly(IFileDescriptor::Probe(mClientPfd));
    // }
    return ec;
}

ECode FileBridge::FileBridgeOutputStream::Fsync()
{
    return WriteCommandAndBlock(FileBridge::CMD_FSYNC, String("fsync()"));
}

ECode FileBridge::FileBridgeOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount)
{
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), byteOffset, byteCount))

    AutoPtr<IMemory> memory;
    CMemory::AcquireSingleton((IMemory**)&memory);
    memory->PokeInt32(mTemp, 0, CMD_WRITE, ByteOrder_BIG_ENDIAN);
    memory->PokeInt32(mTemp, 4, byteCount, ByteOrder_BIG_ENDIAN);
    AutoPtr<IIoBridge> ib;
    CIoBridge::AcquireSingleton((IIoBridge**)&ib);
    ib->Write(mClient, mTemp, 0, FileBridge::MSG_LENGTH);
    return ib->Write(mClient, buffer, byteOffset, byteCount);
}

ECode FileBridge::FileBridgeOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr<IStreams> stream;
    CStreams::AcquireSingleton((IStreams**)&stream);
    return stream->WriteSingleByte(this, oneByte);
}

ECode FileBridge::FileBridgeOutputStream::WriteCommandAndBlock(
    /* [in] */ Int32 cmd,
    /* [in] */ const String& cmdString)
{
    AutoPtr<IMemory> memory;
    CMemory::AcquireSingleton((IMemory**)&memory);
    memory->PokeInt32(mTemp, 0, cmd, ByteOrder_BIG_ENDIAN);
    AutoPtr<IIoBridge> ib;
    CIoBridge::AcquireSingleton((IIoBridge**)&ib);
    ib->Write(mClient, mTemp, 0, FileBridge::MSG_LENGTH);

    // Wait for server to ack
    Int32 readBytes;
    ib->Read(mClient, mTemp, 0, FileBridge::MSG_LENGTH, &readBytes);
    if (readBytes == FileBridge::MSG_LENGTH) {
        Int32 value;
        memory->PeekInt32(mTemp, 0, ByteOrder_BIG_ENDIAN, &value);
        if (value == cmd) {
            return NOERROR;
        }
    }

    return E_IO_EXCEPTION;
    // throw new IOException("Failed to execute " + cmdString + " across bridge");
}

FileBridge::FileBridge()
{
    // try {
    Elastos::Droid::System::Os::Socketpair(
        OsConstants::_AF_UNIX,
        OsConstants::_SOCK_STREAM, 0, mServer, mClient);
    // } catch (ErrnoException e) {
    //     throw new RuntimeException("Failed to create bridge");
    // }
}

Boolean FileBridge::IsClosed()
{
    return mClosed;
}

void FileBridge::ForceClose()
{
    AutoPtr<IIoUtils> iu;
    CIoUtils::AcquireSingleton((IIoUtils**)&iu);
    iu->CloseQuietly(mTarget);
    iu->CloseQuietly(mServer);
    iu->CloseQuietly(mClient);
    mClosed = true;
}

void FileBridge::SetTargetFile(
    /* [in] */ IFileDescriptor* target)
{
    mTarget = target;
}

AutoPtr<IFileDescriptor> FileBridge::GetClientSocket()
{
    return mClient;
}

ECode FileBridge::Run()
{
    AutoPtr<IMemory> memory;
    CMemory::AcquireSingleton((IMemory**)&memory);
    AutoPtr<ArrayOf<Byte> > temp = ArrayOf<Byte>::Alloc(8192);
    //try {
    AutoPtr<IIoBridge> ib;
    CIoBridge::AcquireSingleton((IIoBridge**)&ib);
    Int32 bytes;
    while ((ib->Read(mServer, temp, 0, MSG_LENGTH, &bytes), bytes) == MSG_LENGTH) {
        Int32 cmd = 0;
        memory->PeekInt32(temp, 0, ByteOrder_BIG_ENDIAN, &cmd);
        if (cmd == CMD_WRITE) {
            // Shuttle data into local file
            Int32 len = 0;
            memory->PeekInt32(temp, 4, ByteOrder_BIG_ENDIAN, &len);
            while (len > 0) {
                Int32 n;
                ib->Read(mServer, temp, 0, Elastos::Core::Math::Min(temp->GetLength(), len), &n);
                if (n == -1) {
                    // throw new IOException(
                    //         "Unexpected EOF; still expected " + len + " bytes");
                    return E_IO_EXCEPTION;
                }
                ib->Write(mTarget, temp, 0, n);
                len -= n;
            }
        }
        else if (cmd == CMD_FSYNC) {
            // Sync and echo back to confirm
            Elastos::Droid::System::Os::Fsync(mTarget);
            ib->Write(mServer, temp, 0, MSG_LENGTH);

        }
        else if (cmd == CMD_CLOSE) {
            // Close and echo back to confirm
            Elastos::Droid::System::Os::Fsync(mTarget);
            Elastos::Droid::System::Os::Close(mTarget);
            mClosed = true;
            ib->Write(mServer, temp, 0, MSG_LENGTH);
            break;
        }
    }

    // } catch (ErrnoException | IOException e) {
    //     Log.wtf(TAG, "Failed during bridge", e);
    // } finally {
    ForceClose();
    return NOERROR;
    // }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
