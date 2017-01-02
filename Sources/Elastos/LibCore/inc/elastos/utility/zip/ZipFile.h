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

#ifndef __ELASTOS_UTILITY_ZIPFILE_H__
#define __ELASTOS_UTILITY_ZIPFILE_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"
#include "InflaterInputStream.h"
#include "InputStream.h"
#include "AutoLock.h"

using Elastos::Core::ICloseGuard;
using Elastos::IO::IRandomAccessFile;
using Elastos::IO::InputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IFile;
using Elastos::IO::ICloseable;
using Elastos::Utility::ILinkedHashMap;

namespace Elastos {
namespace Utility {
namespace Zip {

/**
 * This class provides random read access to a <i>ZIP-archive</i> file.
 * <p>
 * While {@code ZipInputStream} provides stream based read access to a
 * <i>ZIP-archive</i>, this class implements more efficient (file based) access
 * and makes use of the <i>central directory</i> within a <i>ZIP-archive</i>.
 * <p>
 * Use {@code ZipOutputStream} if you want to create an archive.
 * <p>
 * A temporary ZIP file can be marked for automatic deletion upon closing it.
 *
 * @see ZipEntry
 * @see ZipOutputStream
 */
class ZipFile
    : public Object
    , public IZipFile
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Wrap a stream around a RandomAccessFile.  The RandomAccessFile is shared
     * among all streams returned by getInputStream(), so we have to synchronize
     * access to it.  (We can optimize this by adding buffering here to reduce
     * collisions.)
     *
     * <p>We could support mark/reset, but we don't currently need them.
     */
    class RAFStream
        : public InputStream
        , public IZipFileRAFStream
    {
    public:
        CAR_INTERFACE_DECL()

        RAFStream(
            /* [in] */ IRandomAccessFile* raf,
            /* [in] */ Int64 initialOffset);

        RAFStream(
            /* [in] */ IRandomAccessFile* raf,
            /* [in] */ Int64 initialOffset,
            /* [in] */ Int64 endOffset);

        CARAPI Available(
            /* [out] */ Int32* number);

        CARAPI Read(
            /* [out] */ Int32* value);

        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 byteOffset,
            /* [in] */ Int32 byteCount,
            /* [out] */ Int32* number);

        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [out] */ Int32* number);

        CARAPI Skip(
            /* [in] */ Int64 byteCount,
            /* [out] */ Int64* number);

        CARAPI Fill(
            /* [in] */ IInflater* inflater,
            /* [in] */ Int32 nativeEndBufSize,
            /* [out] */ Int32* result);

    public:
        AutoPtr<IRandomAccessFile> mSharedRaf;
        Int64 mOffset;
        Int64 mEndOffset;
    };

    class ZipInflaterInputStream
        : public InflaterInputStream
    {
    public:
        ZipInflaterInputStream(
            /* [in] */ IInputStream* is,
            /* [in] */ IInflater* inf,
            /* [in] */ Int32 bsize,
            /* [in] */ IZipEntry* entry);

        //@Override
        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 off,
            /* [in] */ Int32 nbytes,
            /* [out] */ Int32* number);

        //@Override
        CARAPI Available(
            /* [out] */ Int32* number);

    public:
        AutoPtr<IZipEntry> mEntry;
        Int64 mBytesRead;
    };

    class Enumeration
        : public Object
        , public IEnumeration
    {
    public:
        CAR_INTERFACE_DECL()

        Enumeration(
            /* [in] */ IIterator* it,
            /* [in] */ ZipFile* zipFile);

        CARAPI HasMoreElements(
            /* [out] */ Boolean * value);

        CARAPI GetNextElement(
            /* [out] */ IInterface ** inter);

    public:
        AutoPtr<IIterator> mIt;
    private:
        ZipFile* mHost;
    };

public:
    ZipFile();

    virtual ~ZipFile();

    /**
     * Constructs a new {@code ZipFile} with the specified file.
     *
     * @param file
     *            the file to read from.
     * @throws ZipException
     *             if a ZIP error occurs.
     * @throws IOException
     *             if an {@code IOException} occurs.
     */
    CARAPI constructor(
        /* [in] */ IFile* file);

    /**
     * Opens a file as <i>ZIP-archive</i>. "mode" must be {@code OPEN_READ} or
     * {@code OPEN_DELETE} . The latter sets the "delete on exit" flag through a
     * file.
     *
     * @param file
     *            the ZIP file to read.
     * @param mode
     *            the mode of the file open operation.
     * @throws IOException
     *             if an {@code IOException} occurs.
     */
    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ Int32 mode);

    /**
     * Opens a ZIP archived file.
     *
     * @param name
     *            the name of the ZIP file.
     * @throws IOException
     *             if an IOException occurs.
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Closes this ZIP file. This method is idempotent.
     *
     * @throws IOException
     *             if an IOException occurs.
     */
    CARAPI Close();

    /**
     * Returns an enumeration of the entries. The entries are listed in the
     * order in which they appear in the ZIP archive.
     *
     * @return the enumeration of the entries.
     * @throws IllegalStateException if this ZIP file has been closed.
     */
    CARAPI GetEntries(
        /* [out] */ IEnumeration** entries);


    CARAPI GetComment(
        /* [out] */ String* comment);
    /**
     * Gets the ZIP entry with the specified name from this {@code ZipFile}.
     *
     * @param entryName
     *            the name of the entry in the ZIP file.
     * @return a {@code ZipEntry} or {@code null} if the entry name does not
     *         exist in the ZIP file.
     * @throws IllegalStateException if this ZIP file has been closed.
     */
    CARAPI GetEntry(
        /* [in] */ const String& entryName,
        /* [out] */ IZipEntry** entry);

    /**
     * Returns an input stream on the data of the specified {@code ZipEntry}.
     *
     * @param entry
     *            the ZipEntry.
     * @return an input stream of the data contained in the {@code ZipEntry}.
     * @throws IOException
     *             if an {@code IOException} occurs.
     * @throws IllegalStateException if this ZIP file has been closed.
     */
    CARAPI GetInputStream(
        /* [in] */ IZipEntry* entry,
        /* [out] */ IInputStream** is);

    /**
     * Gets the file name of this {@code ZipFile}.
     *
     * @return the file name of this {@code ZipFile}.
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the number of {@code ZipEntries} in this {@code ZipFile}.
     *
     * @return the number of entries in this file.
     * @throws IllegalStateException if this ZIP file has been closed.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

protected:
    CARAPI CheckNotClosed();

    /**
     * Find the central directory and read the contents.
     *
     * <p>The central directory can be followed by a variable-length comment
     * field, so we have to scan through it backwards.  The comment is at
     * most 64K, plus we have 18 bytes for the end-of-central-dir stuff
     * itself, plus apparently sometimes people throw random junk on the end
     * just for the fun of it.
     *
     * <p>This is all a little wobbly.  If the wrong value ends up in the EOCD
     * area, we're hosed. This appears to be the way that everybody handles
     * it though, so we're in good company if this fails.
     */
    CARAPI ReadCentralDir();

public:
    /**
     * General Purpose Bit Flags, Bit 3.
     * If this bit is set, the fields crc-32, compressed
     * size and uncompressed size are set to zero in the
     * local header.  The correct values are put in the
     * data descriptor immediately following the compressed
     * data.  (Note: PKZIP version 2.04g for DOS only
     * recognizes this bit for method 8 compression, newer
     * versions of PKZIP recognize this bit for any
     * compression method.)
     */
    static const Int32 GPBF_DATA_DESCRIPTOR_FLAG; // android-added

    /**
     * General Purpose Bit Flags, Bit 11.
     * Language encoding flag (EFS).  If this bit is set,
     * the filename and comment fields for this file
     * must be encoded using UTF-8.
     */
    static const Int32 GPBF_UTF8_FLAG; // android-added

    static const Int32 GPBF_UNSUPPORTED_MASK;
private:
    String mFileName;

    AutoPtr<IFile> mFileToDeleteOnClose;

    AutoPtr<IRandomAccessFile> mRaf;
    static Object sLock;

    AutoPtr<IMap> mEntries;

    String mComment;
    AutoPtr<ICloseGuard> mGuard;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIPFILE_H__
