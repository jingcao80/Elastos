
#ifndef __ELASTOS_UTILITY_ZIPOUTPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIPOUTPUTSTREAM_H__

#include "DeflaterOutputStream.h"
#include <elastos/utility/etl/HashSet.h>

using Elastos::Utility::Etl::HashSet;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayOutputStream;

namespace Elastos {
namespace Utility {
namespace Zip {

class ZipOutputStream
    : public DeflaterOutputStream
    , public IZipOutputStream
{
public:
    CAR_INTERFACE_DECL()

    ZipOutputStream();

    virtual ~ZipOutputStream();

    /**
     * Constructs a new {@code ZipOutputStream} with the specified output
     * stream.
     *
     * @param p1
     *            the {@code OutputStream} to write the data to.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* p1);

    /**
     * Closes the current {@code ZipEntry}, if any, and the underlying output
     * stream. If the stream is already closed this method does nothing.
     *
     * @throws IOException
     *             If an error occurs closing the stream.
     */
    //@Override
    CARAPI Close();

    /**
     * Closes the current {@code ZipEntry}. Any entry terminal data is written
     * to the underlying stream.
     *
     * @throws IOException
     *             If an error occurs closing the entry.
     */
    virtual CARAPI CloseEntry();

    /**
     * Indicates that all entries have been written to the stream. Any terminal
     * information is written to the underlying stream.
     *
     * @throws IOException
     *             if an error occurs while terminating the stream.
     */
    //@Override
    CARAPI Finish();

    /**
     * Writes entry information to the underlying stream. Data associated with
     * the entry can then be written using {@code write()}. After data is
     * written {@code closeEntry()} must be called to complete the writing of
     * the entry to the underlying stream.
     *
     * @param ze
     *            the {@code ZipEntry} to store.
     * @throws IOException
     *             If an error occurs storing the entry.
     * @see #write
     */
    virtual CARAPI PutNextEntry(
        /* [in] */ IZipEntry* ze);

    /**
     * Sets the {@code ZipFile} comment associated with the file being written.
     *
     * @param comment
     *            the comment associated with the file.
     */
    virtual CARAPI SetComment(
        /* [in] */ const String& comment);

    /**
     * Sets the compression level to be used for writing entry data. This level
     * may be set on a per entry basis. The level must have a value between -1
     * and 8 according to the {@code Deflater} compression level bounds.
     *
     * @param level
     *            the compression level (ranging from -1 to 8).
     * @see Deflater
     */
    virtual CARAPI SetLevel(
        /* [in] */ Int32 level);

    /**
     * Sets the compression method to be used when compressing entry data.
     * method must be one of {@code STORED} (for no compression) or {@code
     * DEFLATED}.
     *
     * @param method
     *            the compression method to use.
     */
    virtual CARAPI SetMethod(
        /* [in] */ Int32 method);

    /**
     * Writes data for the current entry to the underlying stream.
     *
     * @exception IOException
     *                If an error occurs writing to the stream
     */
    //@Override
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

private:
    CARAPI_(Int64) WriteInt64(
        /* [in] */ IOutputStream* os,
        /* [in] */ Int64 i);

    CARAPI_(Int32) WriteInt16(
        /* [in] */ IOutputStream* os,
        /* [in] */ Int32 i);

    CARAPI CheckOpen();

    CARAPI CheckSizeIsWithinShort(
        /* [in] */ const String& property,
        /* [in] */ ArrayOf<Byte>* bytes);

protected:
    static const Int32 ZIP_VERSION_2_0 = 20;// Zip specification version 2.0.

private:
    AutoPtr<ArrayOf<Byte> > mCommentBytes;

    HashSet<String> mEntries;

    Int32 mDefaultCompressionMethod;

    Int32 mCompressLevel;

    AutoPtr<IByteArrayOutputStream> mCDir;

    AutoPtr<IZipEntry> mCurrentEntry;

    AutoPtr<ICRC32> mCrc;

    Int32 mOffset;
    Int32 mCurOffset;

    /** The charset-encoded name for the current entry. */
    AutoPtr<ArrayOf<Byte> > mNameBytes;

    /** The charset-encoded comment for the current entry. */
    AutoPtr<ArrayOf<Byte> > mEntryCommentBytes;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIPOUTPUTSTREAM_H__
