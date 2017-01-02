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

#ifndef __LIBCORE_IO_IOUTILS_H__
#define __LIBCORE_IO_IOUTILS_H__

#include "Elastos.CoreLibrary.h"

using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::Charset::ICharset;
using Elastos::Net::ISocket;
using Elastos::Utility::IRandom;

namespace Libcore {
namespace IO {

class IoUtils
{
public:
    /**
     * Calls close(2) on 'fd'. Also resets the internal int to -1. Does nothing if 'fd' is null
     * or invalid.
     */
    static CARAPI Close(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Closes 'closeable', ignoring any checked exceptions. Does nothing if 'closeable' is null.
     */
    static CARAPI CloseQuietly(
        /* [in] */ ICloseable* closeable);

    /**
     * Closes 'socket', ignoring any exceptions. Does nothing if 'socket' is null.
     */
    static CARAPI CloseQuietly(
        /* [in] */ ISocket* socket);

    /**
     * Closes 'fd', ignoring any exceptions. Does nothing if 'fd' is null or invalid.
     */
    static CARAPI CloseQuietly(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Sets 'fd' to be blocking or non-blocking, according to the state of 'blocking'.
     */
    static CARAPI SetBlocking(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Boolean blocking);

    /**
     * Returns the contents of 'path' as a byte array.
     */
    static CARAPI ReadFileAsByteArray(
        /* [in] */ const String& path,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    /**
     * Returns the contents of 'path' as a string. The contents are assumed to be UTF-8.
     */
    static CARAPI ReadFileAsString(
        /* [in] */ const String& path,
        /* [out] */ String* result);

    /**
     * Do not use. Use createTemporaryDirectory instead.
     *
     * Used by frameworks/base unit tests to clean up a temporary directory.
     * Deliberately ignores errors, on the assumption that test cleanup is only
     * supposed to be best-effort.
     *
     * @deprecated Use {@link #createTemporaryDirectory} instead.
     */
    static CARAPI DeleteContents(
        /* [in] */ IFile* dir);

    /**
     * Creates a unique new temporary directory under "java.io.tmpdir".
     */
    static CARAPI CreateTemporaryDirectory(
        /* [in] */ const String& prefix,
        /* [out] */ IFile** file);

    /**
     * Do not use. This is for System.loadLibrary use only.
     *
     * Checks whether {@code path} can be opened read-only. Similar to File.exists, but doesn't
     * require read permission on the parent, so it'll work in more cases, and allow you to
     * remove read permission from more directories. Everyone else should just open(2) and then
     * use the fd, but the loadLibrary API is broken by its need to ask ClassLoaders where to
     * find a .so rather than just calling dlopen(3).
     */
     static CARAPI CanOpenReadOnly(
        /* [in] */ const String& path,
        /* [out] */ Boolean* result);

protected:
    /**
     * A convenience class for reading the contents of a file into a {@code String}
     * or a {@code byte[]}. This class attempts to minimize the number of allocations
     * and copies required to read this data.
     *
     * For the case where we know the "true" length of a file (most ordinary files)
     * we allocate exactly one byte[] and copy data into that. Calls to
     * {@link #toByteArray} will then return the internal array and <b>not</b> a copy.
     *
     * <b>Note that an absolute path must be supplied. Expect your reads to fail
     * if one isn't.</b>
     */
    class FileReader : public ElRefBase
    {
    public:
        FileReader();

        CARAPI Init(
            /* [in] */ const String& absolutePath);

        CARAPI ReadFully();

        AutoPtr<ArrayOf<Byte> > ToByteArray();

        String ToString(
            /* [in] */ ICharset* cs);

    private:
        AutoPtr<IFileDescriptor> mFd;
        Boolean mUnknownLength;

        AutoPtr<ArrayOf<Byte> > mBytes;
        Int32 mCount;

    };

private:
    IoUtils();
    IoUtils(const IoUtils&);

private:
    static AutoPtr<IRandom> TEMPORARY_DIRECTORY_PRNG;
};

} // namespace IO
} // namespace Libcore

#endif //__LIBCORE_IO_IOUTILS_H__
