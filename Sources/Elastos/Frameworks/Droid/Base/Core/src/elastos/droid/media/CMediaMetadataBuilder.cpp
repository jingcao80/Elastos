#include "elastos/droid/media/CMediaMetadata.h"
#include "elastos/droid/media/CMediaMetadataBuilder.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaMetadataBuilder, Object, IMediaMetadataBuilder)

CAR_OBJECT_IMPL(CMediaMetadataBuilder)

CMediaMetadataBuilder::CMediaMetadataBuilder()
{
}

CMediaMetadataBuilder::~CMediaMetadataBuilder()
{
}

ECode CMediaMetadataBuilder::constructor()
{
    return CBundle::New((IBundle**)&mBundle);
}

ECode CMediaMetadataBuilder::constructor(
    /* [in] */ IMediaMetadata* source)
{
    return CBundle::New(((CMediaMetadata*)source)->mBundle, (IBundle**)&mBundle);
}

ECode CMediaMetadataBuilder::PutText(
    /* [in] */ const String& key,
    /* [in] */ ICharSequence* value)
{
    AutoPtr<ICharSequence> cs;
    CString::New(key, (ICharSequence**)&cs);
    Boolean b;
    if ((CMediaMetadata::METADATA_KEYS_TYPE->ContainsKey(cs, &b), b)) {
        AutoPtr<IInterface> obj;
        CMediaMetadata::METADATA_KEYS_TYPE->Get(cs, (IInterface**)&obj);
        Int32 val;
        IInteger32::Probe(obj)->GetValue(&val);
        if (val != CMediaMetadata::METADATA_TYPE_TEXT) {
            // throw new IllegalArgumentException("The " + key
            //         + " key cannot be used to put a CharSequence");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return mBundle->PutCharSequence(key, value);
}

ECode CMediaMetadataBuilder::PutString(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoPtr<ICharSequence> cs;
    CString::New(key, (ICharSequence**)&cs);
    Boolean b;
    if ((CMediaMetadata::METADATA_KEYS_TYPE->ContainsKey(cs, &b), b)) {
        AutoPtr<IInterface> obj;
        CMediaMetadata::METADATA_KEYS_TYPE->Get(cs, (IInterface**)&obj);
        Int32 val;
        IInteger32::Probe(obj)->GetValue(&val);
        if (val != CMediaMetadata::METADATA_TYPE_TEXT) {
            // throw new IllegalArgumentException("The " + key
            //         + " key cannot be used to put a String");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    cs = NULL;
    CString::New(value, (ICharSequence**)&cs);
    return mBundle->PutCharSequence(key, cs);
}

ECode CMediaMetadataBuilder::PutLong(
    /* [in] */ const String& key,
    /* [in] */ Int64 value)
{
    AutoPtr<ICharSequence> cs;
    CString::New(key, (ICharSequence**)&cs);
    Boolean b;
    if ((CMediaMetadata::METADATA_KEYS_TYPE->ContainsKey(cs, &b), b)) {
        AutoPtr<IInterface> obj;
        CMediaMetadata::METADATA_KEYS_TYPE->Get(cs, (IInterface**)&obj);
        Int32 val;
        IInteger32::Probe(obj)->GetValue(&val);
        if (val != CMediaMetadata::METADATA_TYPE_LONG) {
            // throw new IllegalArgumentException("The " + key
            //         + " key cannot be used to put a long");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    return mBundle->PutInt64(key, value);
}

ECode CMediaMetadataBuilder::PutRating(
    /* [in] */ const String& key,
    /* [in] */ IRating* value)
{
    AutoPtr<ICharSequence> cs;
    CString::New(key, (ICharSequence**)&cs);
    Boolean b;
    if ((CMediaMetadata::METADATA_KEYS_TYPE->ContainsKey(cs, &b), b)) {
        AutoPtr<IInterface> obj;
        CMediaMetadata::METADATA_KEYS_TYPE->Get(cs, (IInterface**)&obj);
        Int32 val;
        IInteger32::Probe(obj)->GetValue(&val);
        if (val != CMediaMetadata::METADATA_TYPE_RATING) {
            // throw new IllegalArgumentException("The " + key
            //         + " key cannot be used to put a Rating");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    return mBundle->PutParcelable(key, IParcelable::Probe(value));
}

ECode CMediaMetadataBuilder::PutBitmap(
    /* [in] */ const String& key,
    /* [in] */ IBitmap* value)
{
    AutoPtr<ICharSequence> cs;
    CString::New(key, (ICharSequence**)&cs);
    Boolean b;
    if ((CMediaMetadata::METADATA_KEYS_TYPE->ContainsKey(cs, &b), b)) {
        AutoPtr<IInterface> obj;
        CMediaMetadata::METADATA_KEYS_TYPE->Get(cs, (IInterface**)&obj);
        Int32 val;
        IInteger32::Probe(obj)->GetValue(&val);
        if (val != CMediaMetadata::METADATA_TYPE_BITMAP) {
            // throw new IllegalArgumentException("The " + key
            //         + " key cannot be used to put a Bitmap");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    return mBundle->PutParcelable(key, IParcelable::Probe(value));
}

ECode CMediaMetadataBuilder::Build(
    /* [out] */ IMediaMetadata** result)
{
    return CMediaMetadata::New(mBundle, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
