
#include "elastos/droid/os/CDropBoxManagerEntry.h"
#include "elastos/droid/os/CParcelFileDescriptorAutoCloseInputStream.h"

using Elastos::IO::EIID_IInputStream;
using Elastos::Droid::Os::CParcelFileDescriptorAutoCloseInputStream;
using Elastos::Droid::Os::IParcelFileDescriptorAutoCloseInputStream;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CDropBoxManagerEntry, Object, IDropBoxManagerEntry)

CAR_OBJECT_IMPL(CDropBoxManagerEntry)

ECode CDropBoxManagerEntry::constructor()
{
    return NOERROR;
}

ECode CDropBoxManagerEntry::constructor(
    /* [in] */ const String& tag,
    /* [in] */ Int64 millis)
{
    if (tag.IsNull()) {
        Slogger::E("CDropBoxManagerEntry", "tag == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mTag = tag;
    mTimeMillis = millis;
    mData = NULL;
    mFileDescriptor = NULL;
    mFlags = IDropBoxManager::IS_EMPTY;
    return NOERROR;
}

ECode CDropBoxManagerEntry::constructor(
    /* [in] */ const String& tag,
    /* [in] */ Int64 millis,
    /* [in] */ const String& text)
{
    if (tag.IsNull()) {
        Slogger::E("CDropBoxManagerEntry", "tag == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (text.IsNull()) {
        Slogger::E("CDropBoxManagerEntry", "text == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    mTag = tag;
    mTimeMillis = millis;

    mData = ArrayOf<Byte>::Alloc((Byte*)text.string(), text.GetByteLength());
    mFileDescriptor = NULL;
    mFlags = IDropBoxManager::IS_TEXT;

    return NOERROR;
}

ECode CDropBoxManagerEntry::constructor(
    /* [in] */ const String& tag,
    /* [in] */ Int64 millis,
    /* [in] */ ArrayOf<Byte>* arrayData,
    /* [in] */ Int32 flags)
{
    if (tag.IsNull()) {
        Slogger::E("CDropBoxManagerEntry", "tag == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (((flags & IDropBoxManager::IS_EMPTY) != 0) != (arrayData == NULL)) {
        Slogger::E("CDropBoxManagerEntry", "Bad flags: %d", flags);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mTag = tag;
    mTimeMillis = millis;
    mData = arrayData;
    mFileDescriptor = NULL;
    mFlags = flags;

    return NOERROR;
}

ECode CDropBoxManagerEntry::constructor(
    /* [in] */ const String& tag,
    /* [in] */ Int64 millis,
    /* [in] */ IParcelFileDescriptor* pfdData,
    /* [in] */ Int32 flags)
{
    if (tag.IsNull()) {
        Slogger::E("CDropBoxManagerEntry", "tag == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (((flags & IDropBoxManager::IS_EMPTY) != 0) != (pfdData == NULL)) {
        Slogger::E("CDropBoxManagerEntry", "Bad flags: %d", flags);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mTag = tag;
    mTimeMillis = millis;
    mData = NULL;
    mFileDescriptor = pfdData;
    mFlags = flags;

    return NOERROR;
}

ECode CDropBoxManagerEntry::constructor(
    /* [in] */ const String& tag,
    /* [in] */ Int64 millis,
    /* [in] */ IFile* fileData,
    /* [in] */ Int32 flags)
{
    if (tag.IsNull()) {
        Slogger::E("CDropBoxManagerEntry", "tag == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if ((flags & IDropBoxManager::IS_EMPTY) != 0) {
        Slogger::E("CDropBoxManagerEntry", "Bad flags: %d", flags);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mTag = tag;
    mTimeMillis = millis;
    mData = NULL;
    ParcelFileDescriptor::Open(fileData, IParcelFileDescriptor::MODE_READ_ONLY,
            (IParcelFileDescriptor**)&mFileDescriptor);
    mFlags = flags;

    return NOERROR;
}

ECode CDropBoxManagerEntry::Close()
{
    if (mFileDescriptor != NULL) {
        mFileDescriptor->Close();
    }

    return NOERROR;
}

ECode CDropBoxManagerEntry::GetTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = mTag;
    return NOERROR;
}

ECode CDropBoxManagerEntry::GetTimeMillis(
    /* [out] */ Int64* timeMillis)
{
    VALIDATE_NOT_NULL(timeMillis);
    *timeMillis = mTimeMillis;
    return NOERROR;
}

ECode CDropBoxManagerEntry::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags & ~IDropBoxManager::IS_GZIPPED;
    return NOERROR;
}

ECode CDropBoxManagerEntry::GetText(
    /* [in] */ Int32 maxBytes,
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);
    if ((mFlags & IDropBoxManager::IS_TEXT) == 0) {
        *text = String(NULL);
        return NOERROR;
    }
    if (mData != NULL) {
        StringBuilder sb("");
        for (Int32 i = 0; i < Elastos::Core::Math::Min(maxBytes, mData->GetLength()); i++) {
            sb += (Char32)((*mData)[i]);
        }
        *text = sb.ToString();
        return NOERROR;
    }

    AutoPtr<IInputStream> is = NULL;
    //try {
    GetInputStream((IInputStream**)&is);
    if (is == NULL) {
        *text = String(NULL);
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(maxBytes);
    Int32 readBytes = 0;
    Int32 n = 0;
    while (n >= 0 && (readBytes += n) < maxBytes) {
        is->ReadBytes(buf, readBytes, maxBytes - readBytes, &n);
    }

    StringBuilder sb("");
    for (Int32 i = 0; i < Elastos::Core::Math::Min(readBytes, buf->GetLength()); i++) {
        sb += (Char32)((*buf)[i]);
    }
    *text = sb.ToString();
   //} catch (IOException e) {
   //     return null;
    //} finally {
        //try {
            if (is != NULL)
                is->Close();
        //} catch (IOException e) {}
    //}
    return NOERROR;
}

ECode CDropBoxManagerEntry::GetInputStream(
    /* [out] */ IInputStream** stream)
{
    VALIDATE_NOT_NULL(stream);
    *stream = NULL;

    AutoPtr<IInputStream> is;
    if (mData != NULL) {
        CByteArrayInputStream::New(mData, (IInputStream**)&is);
    }
    else if (mFileDescriptor != NULL) {
        AutoPtr<IParcelFileDescriptorAutoCloseInputStream> aci;
        CParcelFileDescriptorAutoCloseInputStream::New(mFileDescriptor, (IParcelFileDescriptorAutoCloseInputStream**)&aci);
        is = IInputStream::Probe(aci);
    }
    else {
        *stream = NULL;
        return NOERROR;
    }
    if ((mFlags & IDropBoxManager::IS_GZIPPED) != 0) {
        AutoPtr<IGZIPInputStream> gzipis;
        // TODO:
        //CGZIPInputStream::New(is, (IGZIPInputStream**)&gzipis);
        *stream = IInputStream::Probe(gzipis);
        REFCOUNT_ADD(*stream);
        return NOERROR;
    }
    else {
        *stream = is;
        REFCOUNT_ADD(*stream);
        return NOERROR;
    }
}

ECode CDropBoxManagerEntry::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    return E_NOT_IMPLEMENTED;
}

ECode CDropBoxManagerEntry::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteString(mTag);
    out->WriteInt64(mTimeMillis);
    if (mFileDescriptor != NULL) {
        out->WriteInt32(mFlags & ~IDropBoxManager::HAS_BYTE_ARRAY);  // Clear bit just to be safe
        assert(IParcelable::Probe(mFileDescriptor) != NULL);
        IParcelable::Probe(mFileDescriptor)->WriteToParcel(out);
    }
    else
    {
        out->WriteInt32(mFlags | IDropBoxManager::HAS_BYTE_ARRAY);
        //out.writeByteArray(mData);
    }
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
