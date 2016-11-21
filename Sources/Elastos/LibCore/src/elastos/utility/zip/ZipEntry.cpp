
#include "ZipEntry.h"
#include "CGregorianCalendar.h"
#include "CDate.h"
#include "CStreams.h"
#include "HeapBufferIterator.h"
#include "CZipEntry.h"
#include "ZipFile.h"
#include "Arrays.h"
#include <cutils/log.h>

using Libcore::IO::IStreams;
using Libcore::IO::CStreams;
using Libcore::IO::IBufferIterator;
using Libcore::IO::HeapBufferIterator;
using Elastos::IO::ByteOrder_LITTLE_ENDIAN;
using Elastos::Core::EIID_ICloneable;

namespace Elastos {
namespace Utility {
namespace Zip {


CAR_INTERFACE_IMPL_2(ZipEntry, Object, IZipEntry, ICloneable)

ZipEntry::ZipEntry()
    : mCrc(-1)
    , mCompressedSize(-1)
    , mSize(-1)
    , mCompressionMethod(-1)
    , mTime(-1)
    , mModDate(-1)
    , mExtra(NULL)
    , mNameLength(-1)
    , mLocalHeaderRelOffset(-1)
    , mDataOffset(-1)
{}

ZipEntry::~ZipEntry()
{
}

ECode ZipEntry::constructor(
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
    /* [in] */ Int64 dataOffset)
{
    mName = name;
    mComment = comment;
    mCrc = crc;
    mCompressedSize = compressedSize;
    mSize = size;
    mCompressionMethod = compressionMethod;
    mTime = time;
    mModDate = modDate;
    mExtra = extra;
    mNameLength = nameLength;
    mLocalHeaderRelOffset = localHeaderRelOffset;
    mDataOffset = dataOffset;
    return NOERROR;
}

ECode ZipEntry::constructor(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
//        throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    FAIL_RETURN(ValidateStringLength(String("Name"), name))

    mName = name;
    return NOERROR;
}

ECode ZipEntry::constructor(
    /* [in] */ IZipEntry* other)
{
    VALIDATE_NOT_NULL(other)
    CZipEntry* ze = (CZipEntry*)other;
    mName = ze->mName;
    mComment = ze->mComment;
    mTime = ze->mTime;
    mSize = ze->mSize;
    mCompressedSize = ze->mCompressedSize;
    mCrc = ze->mCrc;
    mCompressionMethod = ze->mCompressionMethod;
    mModDate = ze->mModDate;
    mExtra = ze->mExtra;
    mNameLength = ze->mNameLength;
    mLocalHeaderRelOffset = ze->mLocalHeaderRelOffset;
    return NOERROR;
}

ECode ZipEntry::constructor(
    /* in */ ArrayOf<Byte>* cdeHdrBuf,
    /* in */ IInputStream* cdStream,
    /* [in] */ ICharset* defaultCharset)
{
    VALIDATE_NOT_NULL(cdeHdrBuf)
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    FAIL_RETURN(streams->ReadFully(cdStream, cdeHdrBuf, 0, cdeHdrBuf->GetLength()));

    AutoPtr<IBufferIterator> it = HeapBufferIterator::Iterator(
        cdeHdrBuf, 0, cdeHdrBuf->GetLength(), ByteOrder_LITTLE_ENDIAN);

    Int32 sig;
    it->ReadInt32(&sig);
    if (sig != IZipConstants::CENSIG) {
        ALOGE("ZipException: ZipEntry::constructor Central Directory Entry not found., sig:%d,CENSIG:%d", sig, IZipConstants::CENSIG);
        return E_ZIP_EXCEPTION;
//         throw new ZipException("Central Directory Entry not found");
    }

    Int16 temp16;
    it->Seek(8);
    it->ReadInt16(&temp16);
    Int32 gpbf = temp16 & 0xffff;

    if ((gpbf & ZipFile::GPBF_UNSUPPORTED_MASK) != 0) {
        ALOGE("ZipException: ZipEntry::constructor Invalid General Purpose Bit Flag: %08x", gpbf);
        // throw new ZipException("Invalid General Purpose Bit Flag: " + gpbf);
        return E_ZIP_EXCEPTION;
    }

    // If the GPBF_UTF8_FLAG is set then the character encoding is UTF-8 whatever the default
    // provided.
    AutoPtr<ICharset> charset = defaultCharset;
    if ((gpbf & ZipFile::GPBF_UTF8_FLAG) != 0) {
        charset = NULL;//TODO upgrade StandardCharsets.UTF_8;
    }

    it->ReadInt16(&temp16);
    mCompressionMethod = temp16 & 0xffff;
    it->ReadInt16(&temp16);
    mTime = temp16 & 0xffff;
    it->ReadInt16(&temp16);
    mModDate = temp16 & 0xffff;

    // These are 32-bit values in the file, but 64-bit fields in this object.
    Int32 temp;
    it->ReadInt32(&temp);
    mCrc = ((Int64) temp) & 0xffffffffL;
    it->ReadInt32(&temp);
    mCompressedSize = ((Int64) temp) & 0xffffffffL;
    it->ReadInt32(&temp);
    mSize = ((Int64) temp) & 0xffffffffL;

    it->ReadInt16(&temp16);
    mNameLength = temp16 & 0xffff;
    it->ReadInt16(&temp16);
    Int32 extraLength = temp16 & 0xffff;
    it->ReadInt16(&temp16);
    Int32 commentByteCount = temp16 & 0xffff;

    // This is a 32-bit value in the file, but a 64-bit field in this object.
    it->Seek(42);
    it->ReadInt32(&temp);
    mLocalHeaderRelOffset = ((Int64) temp) & 0xffffffffL;

    AutoPtr< ArrayOf<Byte> > nameBytes = ArrayOf<Byte>::Alloc(mNameLength);
    FAIL_RETURN(streams->ReadFully(cdStream, nameBytes, 0, mNameLength));
    if (ContainsNulByte(nameBytes)) {
        ALOGE("ZipException: Filename contains NUL byte: %s", Arrays::ToString(nameBytes).string());
        //throw new ZipException("Filename contains NUL byte: " + Arrays.toString(nameBytes));
        return E_ZIP_EXCEPTION;
    }
    // name = new String(nameBytes, 0, nameBytes.length, charset);
    mName = String((const char*)nameBytes->GetPayload(), mNameLength);

    if (extraLength > 0) {
        mExtra = ArrayOf<Byte>::Alloc(extraLength);
        streams->ReadFully(cdStream, mExtra, 0, extraLength);
    }

    if (commentByteCount > 0) {
        AutoPtr<ArrayOf<Byte> > commentBytes = ArrayOf<Byte>::Alloc(commentByteCount);
        streams->ReadFully(cdStream, commentBytes, 0, commentByteCount);
        //comment = new String(commentBytes, 0, commentBytes.length, charset);
        mComment = String((const char*)commentBytes->GetPayload(), commentByteCount);
    }
    return NOERROR;
}

ECode ZipEntry::Clone(
    /* [out] */ IInterface** obj)
{
    AutoPtr<IZipEntry> ze;
    CZipEntry::New(mName, (IZipEntry**)&ze);
    return CloneImpl(ze);
}


ECode ZipEntry::CloneImpl(
    /* [in] */ IZipEntry* obj)
{
    VALIDATE_NOT_NULL(obj)
    CZipEntry* ze = (CZipEntry*)obj;
    ze->mName = mName;
    ze->mComment = mComment;
    ze->mCrc = mCrc;
    ze->mCompressedSize = mCompressedSize;
    ze->mSize = mSize;
    ze->mCompressionMethod = mCompressionMethod;
    ze->mTime = mTime;
    ze->mModDate = mModDate;
    ze->mExtra = mExtra != NULL ? mExtra->Clone() : NULL;
    ze->mNameLength = mNameLength;
    ze->mLocalHeaderRelOffset = mLocalHeaderRelOffset;
    ze->mDataOffset = mDataOffset;
    return NOERROR;
}

ECode ZipEntry::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = mName.GetHashCode();
    return NOERROR;
}

ECode ZipEntry::GetComment(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mComment;
    return NOERROR;
}

ECode ZipEntry::GetCompressedSize(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCompressedSize;
    return NOERROR;
}

ECode ZipEntry::GetCrc(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCrc;
    return NOERROR;
}

ECode ZipEntry::GetExtra(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtra;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ZipEntry::GetMethod(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCompressionMethod;
    return NOERROR;
}

ECode ZipEntry::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mName;
    return NOERROR;
}

ECode ZipEntry::GetSize(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSize;
    return NOERROR;
}

ECode ZipEntry::GetTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 time = -1;
    if (mTime != -1) {
        AutoPtr<ICalendar> cal;
        CGregorianCalendar::New((ICalendar**)&cal);
        cal->Set(ICalendar::MILLISECOND, 0);
        cal->Set(1980 + ((mModDate >> 9) & 0x7f), ((mModDate >> 5) & 0xf) - 1,
            mModDate & 0x1f, (mTime >> 11) & 0x1f, (mTime >> 5) & 0x3f,
            (mTime & 0x1f) << 1);
        AutoPtr<IDate> d;
        cal->GetTime((IDate**)&d);
        d->GetTime(&time);
    }
    *result = time;
    return NOERROR;
}

ECode ZipEntry::IsDirectory(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 length = mName.GetLength();
    assert(length > 0);
    *result = mName.GetChar(length - 1) == '/';
    return NOERROR;
}

ECode ZipEntry::SetComment(
    /* [in] */ const String& comment)
{
    if (comment.IsNull()) {
        mComment = NULL;
        return NOERROR;
    }

    FAIL_RETURN(ValidateStringLength(String("Comment"), comment))

    mComment = comment;
    return NOERROR;
}

ECode ZipEntry::SetCompressedSize(
    /* [in] */ Int64 value)
{
    mCompressedSize = value;
    return NOERROR;
}

ECode ZipEntry::SetCrc(
    /* [in] */ Int64 value)
{
    if (value >= 0 && value <= 0xFFFFFFFFll) {
        mCrc = value;
        return NOERROR;
    }
    else {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode ZipEntry::SetExtra(
    /* [in] */ ArrayOf<Byte>* data)
{
    if (data != NULL && data->GetLength() > 0xFFFF) {
        ALOGE("IllegalArgumentException: Extra data too long: %d", data->GetLength());
        // throw new IllegalArgumentException("Extra data too long: " + data.length);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mExtra = data;
    return NOERROR;
}

ECode ZipEntry::SetMethod(
    /* [in] */ Int32 value)
{
    if (value != IZipEntry::STORED && value != IZipEntry::DEFLATED) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCompressionMethod = value;
    return NOERROR;
}

ECode ZipEntry::SetSize(
    /* [in] */ Int64 value)
{
    if (value >= 0 && value <= 0xFFFFFFFFll) {
        mSize = value;
        return NOERROR;
    }
    else {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode ZipEntry::SetTime(
    /* [in] */ Int64 value)
{
    AutoPtr<ICalendar> cal;
    CGregorianCalendar::New((ICalendar**)&cal);
    AutoPtr<IDate> date;
    CDate::New(value, (IDate**)&date);
    cal->SetTime(date);
    Int32 year;
    cal->Get(ICalendar::YEAR, &year);
    if (year < 1980) {
        mModDate = 0x21;
        mTime = 0;
    }
    else {
        mModDate = cal->Get(ICalendar::DATE, &mModDate);
        Int32 month;
        cal->Get(ICalendar::MONTH, &month);
        mModDate = ((month + 1) << 5) | mModDate;
        mModDate = ((year - 1980) << 9) | mModDate;
        Int32 time;
        cal->Get(ICalendar::SECOND, &time);
        mTime = time >> 1;
        cal->Get(ICalendar::MINUTE, &time);
        mTime = (time << 5) | mTime;
        cal->Get(ICalendar::HOUR_OF_DAY, &time);
        mTime = (time << 11) | mTime;
    }

    return NOERROR;
}

ECode ZipEntry::SetDataOffset(
    /* [in] */ Int64 value)
{
    mDataOffset = value;
    return NOERROR;
}

ECode ZipEntry::GetDataOffset(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mDataOffset;
    return NOERROR;
}

ECode ZipEntry::ToString(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mName;
    return NOERROR;
}

Boolean ZipEntry::ContainsNulByte(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    assert(bytes);
    for (Int32 i = 0; i < bytes->GetLength(); ++i) {
        if ((*bytes)[i] == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode ZipEntry::ValidateStringLength(
    /* [in] */ const String& argument,
    /* [in] */ const String& string)
{
    // This check is not perfect: the character encoding is determined when the entry is
    // written out. UTF-8 is probably a worst-case: most alternatives should be single byte per
    // character.
    //byte[] bytes = string.getBytes(StandardCharsets.UTF_8);
    AutoPtr<ArrayOf<Byte> > bytes = string.GetBytes();
    if (bytes->GetLength() > 0xffff) {
        ALOGE("IllegalArgumentException: %s : %s too long.", argument.string(), string.string());
        // throw new IllegalArgumentException(argument + " too long: " + bytes.length);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Zip
} // namespace Utility
} // namespace Elastos
