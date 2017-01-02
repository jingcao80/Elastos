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

#include "CRandomAccessFile.h"
#include "CFile.h"
#include "IoUtils.h"
#include "NioUtils.h"
#include "Math.h"
#include "Character.h"
#include "StringBuilder.h"
#include "OsConstants.h"
#include "IoBridge.h"
#include "CLibcore.h"
#include "CFileDescriptor.h"
#include "CModifiedUtf8.h"
#include "AutoLock.h"
#include "Memory.h"
#include "CCloseGuard.h"

using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::IStructStat;
using Elastos::Core::CCloseGuard;
using Elastos::Core::Character;
using Elastos::Core::StringBuilder;
using Elastos::IO::Channels::IChannel;
using Elastos::IO::Charset::IModifiedUtf8;
using Elastos::IO::Charset::CModifiedUtf8;
using Libcore::IO::IoBridge;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IoUtils;
using Libcore::IO::Memory;

namespace Elastos {
namespace IO {

CAR_OBJECT_IMPL(CRandomAccessFile)

CAR_INTERFACE_IMPL_4(CRandomAccessFile, Object, IRandomAccessFile, IDataInput, IDataOutput, ICloseable)

CRandomAccessFile::CRandomAccessFile()
    : mSyncMetadata(FALSE)
    , mMode(0)
{
    mScratch = ArrayOf<Byte>::Alloc(8);
}

CRandomAccessFile::~CRandomAccessFile()
{
    mScratch = NULL;
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
        mGuard->Close();
    }

    // can not call virtual method Close() on destructor.
    //
    AutoLock lock(this);

    Boolean isflag(FALSE);
    if (mChannel != NULL && (IChannel::Probe(mChannel)->IsOpen(&isflag) , isflag)) {
        ICloseable::Probe(mChannel)->Close();
        mChannel = NULL;
    }

    IoBridge::CloseAndSignalBlockedThreads(mFd);
}

ECode CRandomAccessFile::constructor(
    /* [in] */ IFile* file,
    /* [in] */ const String& mode)
{
    CCloseGuard::New((ICloseGuard**)&mGuard);

    Int32 flags;
    if (mode.Equals("r")) {
        flags = OsConstants::_O_RDONLY;
    }
    else if (mode.Equals("rw") || mode.Equals("rws") || mode.Equals("rwd")) {
        flags = OsConstants::_O_RDWR | OsConstants::_O_CREAT;
        if (mode.Equals("rws")) {
            // Sync file and metadata with every write
            mSyncMetadata = true;
        } else if (mode.Equals("rwd")) {
            // Sync file, but not necessarily metadata
            flags |= OsConstants::_O_SYNC;
        }
    }
    else {
//        throw new IllegalArgumentException("Invalid mode: " + mode);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mMode = flags;

    String path;
    file->GetPath(&path);
    AutoPtr<IFileDescriptor> fd;
    FAIL_RETURN(IoBridge::Open(path, flags, (IFileDescriptor**)&fd));

    Int32 ifd;
    fd->GetDescriptor(&ifd);
    CFileDescriptor::New((IFileDescriptor**)&mFd);
    mFd->SetDescriptor(ifd);

    // if we are in "rws" mode, attempt to sync file+metadata
    if (mSyncMetadata) {
        mFd->Sync();
    }

    return mGuard->Open(String("CRandomAccessFile::Close"));
}

ECode CRandomAccessFile::constructor(
    /* [in] */ const String& fileName,
    /* [in] */ const String& mode)
{
    AutoPtr<CFile> obj;
    FAIL_RETURN(CFile::NewByFriend(fileName, (CFile**)&obj));
    AutoPtr<IFile> file = (IFile*)obj;
    return constructor(file, mode);
}

ECode CRandomAccessFile::Close()
{
    mGuard->Close();

    AutoLock lock(this);

    Boolean isflag(FALSE);
    if (mChannel != NULL && (IChannel::Probe(mChannel)->IsOpen(&isflag) , isflag)) {
        ICloseable::Probe(mChannel)->Close();
        mChannel = NULL;
    }

    return IoBridge::CloseAndSignalBlockedThreads(mFd);
}

ECode CRandomAccessFile::GetChannel(
    /* [out] */ IFileChannel **channel)
{
    VALIDATE_NOT_NULL(channel)
    AutoLock lock(this);

    if (mChannel == NULL) {
        mChannel = NioUtils::NewFileChannel(this, mFd, mMode);
    }

    *channel = mChannel;
    REFCOUNT_ADD(*channel)
    return NOERROR;
}

ECode CRandomAccessFile::GetFD(
    /* [out] */ IFileDescriptor ** fd)
{
    VALIDATE_NOT_NULL(fd);

    *fd = mFd;
    REFCOUNT_ADD(*fd);
    return NOERROR;
}

ECode CRandomAccessFile::GetFilePointer(
    /* [out] */ Int64* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = -1;

    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);

    ECode ec = os->Lseek(mFd, 0ll, OsConstants::_SEEK_CUR, offset);
    if (FAILED(ec)) return E_IO_EXCEPTION;
    return NOERROR;
}

ECode CRandomAccessFile::GetLength(
    /* [out] */ Int64* len)
{
    VALIDATE_NOT_NULL(len);
    *len = 0;

    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);

    AutoPtr<IStructStat> stat;
    ECode ec = os->Fstat(mFd, (IStructStat**)&stat);
    if (FAILED(ec)) return E_IO_EXCEPTION;

    return stat->GetSize(len);
}

ECode CRandomAccessFile::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = -1;

    Int32 byteCount;
    FAIL_RETURN(Read(mScratch, 0, 1, &byteCount))
    *value = byteCount != -1 ? (*mScratch)[0] & 0xff : -1;

    return NOERROR;
}

ECode CRandomAccessFile::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    return Read(buffer, 0, buffer->GetLength(), number);
}

ECode CRandomAccessFile::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    return IoBridge::Read(mFd, buffer, offset, length, number);
}

ECode CRandomAccessFile::ReadBoolean(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 temp;
    FAIL_RETURN(Read(&temp));
    if (temp < 0) {
//        throw new EOFException();
        return E_EOF_EXCEPTION;
    }
    *value = temp != 0;
    return NOERROR;
}

ECode CRandomAccessFile::ReadByte(
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 temp;
    FAIL_RETURN(Read(&temp));
    if (temp < 0) {
//        throw new EOFException();
        return E_EOF_EXCEPTION;
    }
    *value = (Byte)temp;
    return NOERROR;
}

ECode CRandomAccessFile::ReadChar(
    /* [out] */ Char32* value)
{
    VALIDATE_NOT_NULL(value);

    Byte firstChar;
    FAIL_RETURN(ReadByte(&firstChar));
    if ((firstChar & 0x80) == 0) { // ASCII
        *value = (Char32)firstChar;
        return NOERROR;
    }

    Char32 mask, toIgnoreMask;
    Int32 numToRead = 1;
    Char32 utf32 = firstChar;
    for (mask = 0x40, toIgnoreMask = 0xFFFFFF80;
         (firstChar & mask);
         numToRead++, toIgnoreMask |= mask, mask >>= 1) {
        // 0x3F == 00111111
        Byte ch;
        FAIL_RETURN(ReadByte(&ch));
        utf32 = (utf32 << 6) + (ch & 0x3F);
    }
    toIgnoreMask |= mask;
    utf32 &= ~(toIgnoreMask << (6 * (numToRead - 1)));

    *value = utf32;
    return NOERROR;
}

ECode CRandomAccessFile::ReadDouble(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    Int64 i64Value;
    FAIL_RETURN(ReadInt64(&i64Value));
    *value = Elastos::Core::Math::Int64BitsToDouble(i64Value);
    return NOERROR;
}

ECode CRandomAccessFile::ReadFloat(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    Int32 i32Value;
    FAIL_RETURN(ReadInt32(&i32Value));
    *value = Elastos::Core::Math::Int32BitsToFloat(i32Value);
    return NOERROR;
}

ECode CRandomAccessFile::ReadFully(
    /* [out] */ ArrayOf<Byte>* buffer)
{
    return ReadFully(buffer, 0, buffer->GetLength());
}

ECode CRandomAccessFile::ReadFully(
    /* [out] */ ArrayOf<byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    if (buffer == NULL) {
        // throw new NullPointerException("buffer == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // avoid int overflow
    // BEGIN android-changed
    // Exception priorities (in case of multiple errors) differ from
    // RI, but are spec-compliant.
    // used (offset | length) < 0 instead of separate (offset < 0) and
    // (length < 0) check to safe one operation
    if ((offset | length) < 0 || offset > buffer->GetLength() - length) {
        // throw new IndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    // END android-changed
    Int32 number;
    while (length > 0) {
        FAIL_RETURN(Read(buffer, offset, length, &number));
        if (number < 0) {
            // throw new EOFException();
            return E_EOF_EXCEPTION;
        }
        offset += number;
        length -= number;
    }
    return NOERROR;
}

ECode CRandomAccessFile::ReadInt32(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(ReadFully(mScratch, 0, sizeof(Int32)));
    *value = Memory::PeekInt32(mScratch, 0, ByteOrder_BIG_ENDIAN);
    return NOERROR;
}

ECode CRandomAccessFile::ReadLine(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<StringBuilder> line = new StringBuilder(80);
    Boolean foundTerminator = FALSE;
    Int64 unreadPosition = 0;
    while (TRUE) {
        Int32 nextByte;
        FAIL_RETURN(Read(&nextByte));
        switch (nextByte) {
        case -1:
            if (line->GetLength() != 0) {
                *str = line->ToString();
            }
            else {
                *str = NULL;
            }
            return NOERROR;
        case (Byte)'\r':
            if (foundTerminator) {
                Seek(unreadPosition);
                *str = line->ToString();
                return NOERROR;
            }
            foundTerminator = TRUE;
            /* Have to be able to peek ahead one byte */
            GetFilePointer(&unreadPosition);
            break;
        case (Byte)'\n':
            *str = line->ToString();
            return NOERROR;
        default:
            if (foundTerminator) {
                Seek(unreadPosition);
                *str = line->ToString();
                return NOERROR;
            }
            line->AppendChar((Char32)nextByte);
        }
    }
    return NOERROR;
}

ECode CRandomAccessFile::ReadInt64(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(ReadFully(mScratch, 0, sizeof(Int64)));
    *value = Memory::PeekInt64(mScratch, 0, ByteOrder_BIG_ENDIAN);
    return NOERROR;
}

ECode CRandomAccessFile::ReadInt16(
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(ReadFully(mScratch, 0, sizeof(Int16)));
    *value = Memory::PeekInt16(mScratch, 0, ByteOrder_BIG_ENDIAN);
    return NOERROR;
}

ECode CRandomAccessFile::ReadUnsignedByte(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(Read(value));
    return *value < 0 ? E_EOF_EXCEPTION : NOERROR;
}

ECode CRandomAccessFile::ReadUnsignedInt16(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    Int16 temp;
    FAIL_RETURN(ReadInt16(&temp));
    *value = ((Int32)temp) & 0xFFFF;

    return NOERROR;
}

ECode CRandomAccessFile::ReadUTF(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Int32 utfSize = 0;
    FAIL_RETURN(ReadUnsignedInt16(&utfSize));
    if (utfSize == 0) {
        *str = "";
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(utfSize);
    Int32 readsize = 0;
    Read(buf, 0, buf->GetLength(), &readsize);
    if (readsize != buf->GetLength()) {
        // throw new EOFException();
        return E_EOF_EXCEPTION;
    }
    AutoPtr<IModifiedUtf8> mutf8help;
    CModifiedUtf8::AcquireSingleton((IModifiedUtf8**)&mutf8help);
    AutoPtr< ArrayOf<Char32> > charbuf = ArrayOf<Char32>::Alloc(utfSize);
    return mutf8help->Decode(buf, charbuf, 0, utfSize, str);
}

ECode CRandomAccessFile::Seek(
    /* [in] */ Int64 offset)
{
    if (offset < 0) {
        // seek position is negative
        // throw new IOException("offset < 0");
        return E_IO_EXCEPTION;
    }

    AutoPtr<CLibcore> ioObj;
    CLibcore::AcquireSingletonByFriend((CLibcore**)&ioObj);
    AutoPtr<ILibcore> libcore = (ILibcore*)ioObj.Get();
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);

    Int64 res;
    ECode ec = os->Lseek(mFd, offset, OsConstants::_SEEK_SET, &res);
    if (FAILED(ec)) return E_IO_EXCEPTION;
    return NOERROR;
}

ECode CRandomAccessFile::SetLength(
   /* [in] */ Int64 newLength)
{
    if (newLength < 0) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<CLibcore> ioObj;
    CLibcore::AcquireSingletonByFriend((CLibcore**)&ioObj);
    AutoPtr<ILibcore> libcore = (ILibcore*)ioObj.Get();
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);

    ECode ec = os->Ftruncate(mFd, newLength);
    if (FAILED(ec)) return E_IO_EXCEPTION;

    Int64 filePointer;
    FAIL_RETURN(GetFilePointer(&filePointer));
    if (filePointer > newLength) {
        FAIL_RETURN(Seek(newLength));
    }

    // if we are in "rws" mode, attempt to sync file+metadata
    if (mSyncMetadata) {
        mFd->Sync();
    }
    return NOERROR;
}

ECode CRandomAccessFile::SkipBytes(
    /* [in] */ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;

    if (count > 0) {
        Int64 currentPos, eof;
        GetFilePointer(&currentPos);
        GetLength(&eof);
        Int32 newCount = (Int32)((currentPos + count > eof) ? eof - currentPos
                : count);
        FAIL_RETURN(Seek(currentPos + newCount));
        *number = newCount;
        return NOERROR;
    }
    *number = 0;
    return NOERROR;
}

ECode CRandomAccessFile::Write(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    return Write(buffer, 0, buffer->GetLength());
}

ECode CRandomAccessFile::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    FAIL_RETURN(IoBridge::Write(mFd, buffer, offset, count));

    // if we are in "rws" mode, attempt to sync file+metadata
    if (mSyncMetadata) {
        mFd->Sync();
    }
    return NOERROR;
}

ECode CRandomAccessFile::Write(
    /* [in] */ Int32 oneByte)
{
    (*mScratch)[0] = (Byte)(oneByte & 0xFF);
    return Write(mScratch, 0, 1);
}

ECode CRandomAccessFile::WriteBoolean(
    /* [in] */ Boolean value)
{
    return Write(value? 1 : 0);
}

ECode CRandomAccessFile::WriteByte(
    /* [in] */ Int32 value)
{
    return Write(value & 0xff);
}

ECode CRandomAccessFile::WriteChar(
    /* [in] */ Int32 value)
{
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(4);
    Int32 len;
    Character::ToChars((Char32)value, *buffer, 0, &len);
    return Write(buffer, 0, len);
}

ECode CRandomAccessFile::WriteBytes(
    /* [in] */ const String& str)
{
    if (str.IsNullOrEmpty())
        return NOERROR;

    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(str.GetLength());
    for (Int32 index = 0; index < str.GetLength(); index++) {
        bytes->Set(index, (Byte)(str.GetChar(index) & 0xFF));
    }
    return Write(bytes);
}

ECode CRandomAccessFile::WriteChars(
    /* [in] */ const String& str)
{
    if (str.IsNullOrEmpty())
        return NOERROR;

    // write(str.getBytes("UTF-16BE"));
    AutoPtr<ArrayOf<Byte> > chs = str.GetBytes();
    return Write(chs);
}

ECode CRandomAccessFile::WriteDouble(
    /* [in] */ Double value)
{
    return WriteInt64(Elastos::Core::Math::DoubleToInt64Bits(value));
}

ECode CRandomAccessFile::WriteFloat(
    /* [in] */ Float value)
{
    return WriteInt32(Elastos::Core::Math::FloatToInt32Bits(value));
}

ECode CRandomAccessFile::WriteInt32(
    /* [in] */ Int32 value)
{
    Memory::PokeInt32(mScratch, 0, value, ByteOrder_BIG_ENDIAN);
    return Write(mScratch, 0, sizeof(Int32));
}

ECode CRandomAccessFile::WriteInt64(
    /* [in] */ Int64 value)
{
    Memory::PokeInt64(mScratch, 0, value, ByteOrder_BIG_ENDIAN);
    return Write(mScratch, 0, sizeof(Int64));
}

ECode CRandomAccessFile::WriteInt16(
    /* [in] */ Int32 value)
{
    Memory::PokeInt16(mScratch, 0, value, ByteOrder_BIG_ENDIAN);
    return Write(mScratch, 0, sizeof(Int32));
}

ECode CRandomAccessFile::WriteUTF(
    /* [in] */ const String& str)
{
    AutoPtr<IModifiedUtf8> utf8help;
    CModifiedUtf8::AcquireSingleton((IModifiedUtf8**)&utf8help);
    AutoPtr<ArrayOf<Byte> > bytes;
    utf8help->Encode(str, (ArrayOf<Byte>**)&bytes);
    return Write(bytes);
}

} // namespace IO
} // namespace Elastos
