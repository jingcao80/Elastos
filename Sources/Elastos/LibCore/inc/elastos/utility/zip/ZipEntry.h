
#ifndef __ELASTOS_UTILITY_ZIPENTRY_H__
#define __ELASTOS_UTILITY_ZIPENTRY_H__

#include "Elastos.CoreLibrary.Utility.Zip.h"
#include "Object.h"

using Elastos::Core::ICloneable;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Utility {
namespace Zip {

class ZipEntry
    : public Object
    , public IZipEntry
    , public ICloneable
{
public:
    ZipEntry();

    virtual ~ZipEntry();

    CAR_INTERFACE_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& comment,
        /* [in] */ Int64 crc,
        /* [in] */ Int64 compressedSize,
        /* [in] */ Int64 size,
        /* [in] */ Int32 compressionMethod,
        /* [in] */ Int32 time,
        /* [in] */ Int32 modDate,
        /* [in] */ ArrayOf<Byte>* extra,
        /* [in] */ Int32 nameLength,
        /* [in] */ Int64 localHeaderRelOffset,
        /* [in] */ Int64 dataOffset);

    /**
     * Constructs a new {@code ZipEntry} with the specified name.
     *
     * @param name
     *            the name of the ZIP entry.
     * @throws IllegalArgumentException
     *             if the name length is outside the range (> 0xFFFF).
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Constructs a new {@code ZipEntry} using the values obtained from {@code
     * ze}.
     *
     * @param ze
     *            the {@code ZipEntry} from which to obtain values.
     */
    CARAPI constructor(
        /* [in] */ IZipEntry* ze);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* cdeHdrBuf,
        /* [in] */ IInputStream* cdStream,
        /* [in] */ ICharset* defaultCharset);

    /**
     * Gets the comment for this {@code ZipEntry}.
     *
     * @return the comment for this {@code ZipEntry}, or {@code null} if there
     *         is no comment. If we're reading an archive with
     *         {@code ZipInputStream} the comment is not available.
     */
    CARAPI GetComment(
        /* [out] */ String* result);

    /**
     * Gets the compressed size of this {@code ZipEntry}.
     *
     * @return the compressed size, or -1 if the compressed size has not been
     *         set.
     */
    CARAPI GetCompressedSize(
        /* [out] */ Int64* result);

    /**
     * Gets the checksum for this {@code ZipEntry}.
     *
     * @return the checksum, or -1 if the checksum has not been set.
     */
    CARAPI GetCrc(
        /* [out] */ Int64* result);

    /**
     * Gets the extra information for this {@code ZipEntry}.
     *
     * @return a byte array containing the extra information, or {@code null} if
     *         there is none.
     */
    CARAPI GetExtra(
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Gets the compression method for this {@code ZipEntry}.
     *
     * @return the compression method, either {@code DEFLATED}, {@code STORED}
     *         or -1 if the compression method has not been set.
     */
    CARAPI GetMethod(
        /* [out] */ Int32* result);

    /**
     * Gets the name of this {@code ZipEntry}.
     *
     * @return the entry name.
     */
    CARAPI GetName(
        /* [out] */ String* result);

    /**
     * Gets the uncompressed size of this {@code ZipEntry}.
     *
     * @return the uncompressed size, or {@code -1} if the size has not been
     *         set.
     */
    CARAPI GetSize(
        /* [out] */ Int64* result);

    /**
     * Gets the last modification time of this {@code ZipEntry}.
     *
     * @return the last modification time as the number of milliseconds since
     *         Jan. 1, 1970.
     */
    CARAPI GetTime(
        /* [out] */ Int64* result);

    /**
     * Determine whether or not this {@code ZipEntry} is a directory.
     *
     * @return {@code true} when this {@code ZipEntry} is a directory, {@code
     *         false} otherwise.
     */
    CARAPI IsDirectory(
        /* [out] */ Boolean* result);

    /**
     * Sets the comment for this {@code ZipEntry}.
     *
     * @param string
     *            the comment for this entry.
     */
    CARAPI SetComment(
        /* [in] */ const String& comment);

    /**
     * Sets the compressed size for this {@code ZipEntry}.
     *
     * @param value
     *            the compressed size (in bytes).
     */
    CARAPI SetCompressedSize(
        /* [in] */ Int64 value);

    /**
     * Sets the checksum for this {@code ZipEntry}.
     *
     * @param value
     *            the checksum for this entry.
     * @throws IllegalArgumentException
     *             if {@code value} is < 0 or > 0xFFFFFFFFL.
     */
    CARAPI SetCrc(
        /* [in] */ Int64 value);

    /**
     * Sets the extra information for this {@code ZipEntry}.
     *
     * @param data
     *            a byte array containing the extra information.
     * @throws IllegalArgumentException
     *             when the length of data is greater than 0xFFFF bytes.
     */
    CARAPI SetExtra(
        /* [in] */ ArrayOf<Byte>* data);

    /**
     * Sets the compression method for this {@code ZipEntry}.
     *
     * @param value
     *            the compression method, either {@code DEFLATED} or {@code
     *            STORED}.
     * @throws IllegalArgumentException
     *             when value is not {@code DEFLATED} or {@code STORED}.
     */
    CARAPI SetMethod(
        /* [in] */ Int32 value);

    /**
     * Sets the uncompressed size of this {@code ZipEntry}.
     *
     * @param value
     *            the uncompressed size for this entry.
     * @throws IllegalArgumentException
     *             if {@code value} < 0 or {@code value} > 0xFFFFFFFFL.
     */
    CARAPI SetSize(
        /* [in] */ Int64 value);

    /**
     * Sets the modification time of this {@code ZipEntry}.
     *
     * @param value
     *            the modification time as the number of milliseconds since Jan.
     *            1, 1970.
     */
    CARAPI SetTime(
        /* [in] */ Int64 value);

    /** @hide */
    CARAPI SetDataOffset(
        /* [in] */ Int64 value);

    CARAPI GetDataOffset(
        /* [out] */ Int64* value);

    CARAPI ToString(
        /* [out] */ String* value);

    CARAPI GetHashCode(
        /* [out] */ Int32* value);

protected:
    CARAPI CloneImpl(
        /* [in] */ IZipEntry* obj);

private:
    static Boolean ContainsNulByte(
        /* [in] */ ArrayOf<Byte>* bytes);

    static CARAPI ValidateStringLength(
        /* [in] */ const String& argument,
        /* [in] */ const String& string);

public:
    String mName;
    String mComment;

    Int64 mCrc; // Needs to be a long to distinguish -1 ("not set") from the 0xffffffff CRC32.
    Int64 mCompressedSize;
    Int64 mSize;

    Int32 mCompressionMethod;
    Int32 mTime;
    Int32 mModDate;

    AutoPtr<ArrayOf<Byte> > mExtra;

    Int32 mNameLength;
    Int64 mLocalHeaderRelOffset;
    Int64 mDataOffset;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIPENTRY_H__
