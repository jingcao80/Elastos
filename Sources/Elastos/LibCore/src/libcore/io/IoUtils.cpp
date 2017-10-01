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

#include "Elastos.CoreLibrary.Net.h"
#include "IoUtils.h"
#include "CFileDescriptor.h"
#include "CFile.h"
#include "CRandomAccessFile.h"
#include "CLibcore.h"
#include "IoBridge.h"
#include "StringBuilder.h"
#include "CRandom.h"
#include "CSystem.h"
#include <fcntl.h>

using Elastos::Utility::CRandom;
using Elastos::Utility::EIID_IRandom;
using Elastos::Core::StringBuilder;
using Elastos::Core::CSystem;
using Elastos::Droid::System::IStructStat;
using Elastos::IO::CFile;
using Elastos::IO::CFileDescriptor;

namespace Libcore {
namespace IO {

//=============================================================
// IoUtils::FileReader
//=============================================================
IoUtils::FileReader::FileReader()
    : mUnknownLength(FALSE)
    , mCount(0)
{
}

ECode IoUtils::FileReader::Init(
    /* [in] */ const String& absolutePath)
{
    // We use IoBridge.open because callers might differentiate
    // between a FileNotFoundException and a general IOException.
    //
    // NOTE: This costs us an additional call to fstat(2) to test whether
    // "absolutePath" is a directory or not. We can eliminate it
    // at the cost of copying some code from IoBridge.open.
    // try {
    ECode ec = IoBridge::Open(absolutePath, O_RDONLY, (IFileDescriptor**)&mFd);
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        return E_IO_EXCEPTION;
    }
    // } catch (FileNotFoundException fnfe) {
    //     throw fnfe;
    // }

    Int32 capacity = 8192;

    // try {
    AutoPtr<IOs> os = CLibcore::sOs;
    AutoPtr<IStructStat> stat;

    ec = os->Fstat(mFd, (IStructStat**)&stat);
    if (FAILED(ec)) {
        IoUtils::CloseQuietly(mFd);
        // ALOGE("FileReader::Init IOException: create FileReader: %s", absolutePath.string());
        return E_IO_EXCEPTION;
    // throw exception.rethrowAsIOException();
    }
    else {
        // Like RAF & other APIs, we assume that the file size fits
        // into a 32 bit integer.
        // capacity = (Int32) stat.st_size;
        Int64 size;
        ec = stat->GetSize(&size);
        if (FAILED(ec)) return E_IO_EXCEPTION;

        capacity = (Int32)size;
        if (capacity == 0) {
            mUnknownLength = TRUE;
            capacity = 8192;
        }
    }
    // } catch (ErrnoException exception) {
        // throw exception.rethrowAsIOException();
    // }


    mBytes = ArrayOf<Byte>::Alloc(capacity);
    return NOERROR;
}

ECode IoUtils::FileReader::ReadFully()
{
    VALIDATE_NOT_NULL(mFd)

    Int32 read;
    Int32 capacity = mBytes->GetLength();
    // try {
    AutoPtr<IOs> os = CLibcore::sOs;
    ECode ec = NOERROR;
    while ((ec = os->Read(mFd, mBytes, mCount, capacity - mCount, &read), read) != 0 && SUCCEEDED(ec)) {
        mCount += read;
        if (mCount == capacity) {
            if (mUnknownLength) {
                // If we don't know the length of this file, we need to continue
                // reading until we reach EOF. Double the capacity in preparation.
                Int32 newCapacity = capacity * 2;
                AutoPtr<ArrayOf<Byte> > newBytes = ArrayOf<Byte>::Alloc(newCapacity);
                newBytes->Copy(mBytes, 0, capacity);
                mBytes = newBytes;
                capacity = newCapacity;
            } else {
                // We know the length of this file and we've read the right number
                // of bytes from it, return.
                break;
            }
        }
    }

    if (FAILED(ec)) {
        ec = E_IO_EXCEPTION;
    }

    // } catch (ErrnoException e) {
        // throw e.rethrowAsIOException();
    // } finally {
    IoUtils::CloseQuietly(mFd);
    // }
    return ec;
}

AutoPtr<ArrayOf<Byte> > IoUtils::FileReader::ToByteArray()
{
    if (mCount == mBytes->GetLength()) {
        return mBytes;
    }

    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(mCount);
    result->Copy(mBytes, 0, mCount);
    return result;
}

String IoUtils::FileReader::ToString(
    /* [in] */ ICharset* cs)
{
    String str((char*)mBytes->GetPayload(), mCount);
    return str;
    // return new String(bytes, 0, count, cs);
}

//=============================================================
// IoUtils
//=============================================================
AutoPtr<IRandom> IoUtils::TEMPORARY_DIRECTORY_PRNG;

/**
 * Calls close(2) on 'fd'. Also resets the internal int to -1.
 */
ECode IoUtils::Close(
    /* [in] */ IFileDescriptor* fd)
{
    // try {
    Boolean isValid;
    if (fd != NULL && (fd->Valid(&isValid), isValid)) {
        AutoPtr<IOs> os = CLibcore::sOs;
        ECode ec = os->Close(fd);
        if (FAILED(ec)) {
            return E_IO_EXCEPTION;
        }
    }
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     throw errnoException.rethrowAsIOException();
    // }
}

/**
 * Closes 'closeable', ignoring any exceptions. Does nothing if 'closeable' is null.
 */
ECode IoUtils::CloseQuietly(
    /* [in] */ ICloseable* closeable)
{
    if (closeable != NULL) {
        // try {
        closeable->Close();
        // } catch (IOException ignored) {
        // }
    }
    return NOERROR;
}

ECode IoUtils::CloseQuietly(
    /* [in] */ ISocket* socket)
{
    if (socket != NULL) {
        // try {
        return socket->Close();
        // } catch (Exception ignored) {
        // }
    }
    return NOERROR;
}

ECode IoUtils::CloseQuietly(
    /* [in] */ IFileDescriptor* fd)
{
    return Close(fd);
}

/**
 * Sets 'fd' to be blocking or non-blocking, according to the state of 'blocking'.
 */
ECode IoUtils::SetBlocking(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Boolean blocking)
{
    // try {
    Int32 flags;
    AutoPtr<IOs> os = CLibcore::sOs;
    ECode ec = (os->FcntlVoid(fd, F_GETFL, &flags));
    if (FAILED(ec)) return E_IO_EXCEPTION;

    if (!blocking) {
        flags |= O_NONBLOCK;
    }
    else {
        flags &= ~O_NONBLOCK;
    }

    Int32 temp;
    ec = os->FcntlInt64(fd, F_SETFL, flags, &temp);
    if (FAILED(ec)) return E_IO_EXCEPTION;
    return NOERROR;
    // } catch (ErrnoException errnoException) {
    //     throw errnoException.rethrowAsIOException();
    // }
}

/**
 * Returns the contents of 'path' as a byte array.
 */
ECode IoUtils::ReadFileAsByteArray(
    /* [in] */ const String& absolutePath,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = NULL;

    AutoPtr<FileReader> reader = new FileReader();
    FAIL_RETURN(reader->Init(absolutePath))
    FAIL_RETURN(reader->ReadFully())

    AutoPtr<ArrayOf<Byte> > tmp = reader->ToByteArray();
    *bytes = tmp;
    REFCOUNT_ADD(*bytes);
    return NOERROR;
}

/**
 * Returns the contents of 'path' as a string. The contents are assumed to be UTF-8.
 */
ECode IoUtils::ReadFileAsString(
    /* [in] */ const String& absolutePath,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);

    AutoPtr<FileReader> reader = new FileReader();
    FAIL_RETURN(reader->Init(absolutePath))
    FAIL_RETURN(reader->ReadFully())

    AutoPtr<ICharset> cs;//TODO = StandardCharsets.UTF_8;
    *result = reader->ToString(cs);
    return NOERROR;
}

ECode IoUtils::DeleteContents(
    /* [in] */ IFile* dir)
{
    VALIDATE_NOT_NULL(dir)

    AutoPtr<ArrayOf<IFile*> > files;
    dir->ListFiles((ArrayOf<IFile*>**)&files);
    Boolean result;
    if (files != NULL) {
        Boolean isDir;
        for (Int32 i = 0; i < files->GetLength(); ++i) {
            IFile* file = (*files)[i];
            if (file->IsDirectory(&isDir), isDir) {
                DeleteContents(file);
            }
            file->Delete(&result);
        }
    }
    return NOERROR;
}

ECode IoUtils::CreateTemporaryDirectory(
    /* [in] */ const String& prefix,
    /* [out] */ IFile** file)
{
    VALIDATE_NOT_NULL(file)
    *file = NULL;

    if (TEMPORARY_DIRECTORY_PRNG == NULL) {
        AutoPtr<IRandom> r;
        CRandom::New((IRandom**)&r);
        TEMPORARY_DIRECTORY_PRNG = r;
    }

    String prop;
    AutoPtr<CSystem> system;
    CSystem::AcquireSingletonByFriend((CSystem**)&system);
    system->GetProperty(String("java.io.tmpdir"), &prop);

    Int32 ri;
    Boolean bval;
    while (TRUE) {
        TEMPORARY_DIRECTORY_PRNG->NextInt32(&ri);
        StringBuilder sb(prefix);
        sb.Append(ri);
        String candidateName = sb.ToString();
        AutoPtr<IFile> result;
        CFile::New(prop, candidateName, (IFile**)&result);
        if (result->Mkdir(&bval), bval) {
            *file = result;
            REFCOUNT_ADD(*file)
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode IoUtils::CanOpenReadOnly(
    /* [in] */ const String& path,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // try {
    // Use open(2) rather than stat(2) so we require fewer permissions. http://b/6485312.
    AutoPtr<IFileDescriptor> fd;
    AutoPtr<IOs> os = CLibcore::sOs;
    FAIL_RETURN(os->Open(path, O_RDONLY, 0, (IFileDescriptor**)&fd))
    FAIL_RETURN(os->Close(fd))
    *result = TRUE;
    // } catch (ErrnoException errnoException) {
        // return FALSE;
    // }
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
