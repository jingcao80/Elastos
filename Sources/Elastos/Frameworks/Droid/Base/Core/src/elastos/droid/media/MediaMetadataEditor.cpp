#include "elastos/droid/media/CMediaMetadataBuilder.h"
#include "elastos/droid/media/MediaMetadataEditor.h"
#include "elastos/droid/utility/CSparseInt32Array.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::IInteger64;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const String MediaMetadataEditor::TAG("MediaMetadataEditor");
const Int32 MediaMetadataEditor::METADATA_TYPE_INVALID = -1;
const Int32 MediaMetadataEditor::METADATA_TYPE_LONG = 0;
const Int32 MediaMetadataEditor::METADATA_TYPE_STRING = 1;
const Int32 MediaMetadataEditor::METADATA_TYPE_BITMAP = 2;
const Int32 MediaMetadataEditor::METADATA_TYPE_RATING = 3;
AutoPtr<ISparseInt32Array> MediaMetadataEditor::METADATA_KEYS_TYPE;

Boolean MediaMetadataEditor::InitStatic()
{
    CSparseInt32Array::New((ISparseInt32Array**)&METADATA_KEYS_TYPE);
    // NOTE: if adding to the list below, make sure you increment the array initialization size
    // keys with long values
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_CD_TRACK_NUMBER, METADATA_TYPE_LONG);
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_DISC_NUMBER, METADATA_TYPE_LONG);
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_DURATION, METADATA_TYPE_LONG);
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_YEAR, METADATA_TYPE_LONG);
    // keys with String values
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_ALBUM, METADATA_TYPE_STRING);
    METADATA_KEYS_TYPE->Put(
            IMediaMetadataRetriever::METADATA_KEY_ALBUMARTIST, METADATA_TYPE_STRING);
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_TITLE, METADATA_TYPE_STRING);
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_ARTIST, METADATA_TYPE_STRING);
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_AUTHOR, METADATA_TYPE_STRING);
    METADATA_KEYS_TYPE->Put(
            IMediaMetadataRetriever::METADATA_KEY_COMPILATION, METADATA_TYPE_STRING);
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_COMPOSER, METADATA_TYPE_STRING);
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_DATE, METADATA_TYPE_STRING);
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_GENRE, METADATA_TYPE_STRING);
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_WRITER, METADATA_TYPE_STRING);
    // keys with Bitmap values
    METADATA_KEYS_TYPE->Put(BITMAP_KEY_ARTWORK, METADATA_TYPE_BITMAP);
    // keys with Rating values
    METADATA_KEYS_TYPE->Put(RATING_KEY_BY_OTHERS, METADATA_TYPE_RATING);
    METADATA_KEYS_TYPE->Put(RATING_KEY_BY_USER, METADATA_TYPE_RATING);
    // Meta data for total number of tracks in Album
    METADATA_KEYS_TYPE->Put(IMediaMetadataRetriever::METADATA_KEY_NUM_TRACKS, METADATA_TYPE_LONG);
    return TRUE;
}

Boolean MediaMetadataEditor::mInit = InitStatic();

CAR_INTERFACE_IMPL(MediaMetadataEditor, Object, IMediaMetadataEditor)

MediaMetadataEditor::MediaMetadataEditor()
    : mEditableKeys(0)
    , mMetadataChanged(FALSE)
    , mApplied(FALSE)
    , mArtworkChanged(FALSE)
{
}

MediaMetadataEditor::~MediaMetadataEditor()
{
}

ECode MediaMetadataEditor::constructor()
{
    return NOERROR;
}

ECode MediaMetadataEditor::Apply()
{
    return NOERROR;
}

ECode MediaMetadataEditor::Clear()
{
    if (mApplied) {
        Logger::E(TAG, "Can't clear a previously applied MediaMetadataEditor");
        return NOERROR;
    }
    mEditorMetadata->Clear();
    mEditorArtwork = NULL;
    CMediaMetadataBuilder::New((IMediaMetadataBuilder**)&mMetadataBuilder);
    return NOERROR;
}

ECode MediaMetadataEditor::AddEditableKey(
    /* [in] */ Int32 key)
{
    if (mApplied) {
        Logger::E(TAG, "Can't change editable keys of a previously applied MetadataEditor");
        return NOERROR;
    }
    // only one editable key at the moment, so we're not wasting memory on an array
    // of editable keys to check the validity of the key, just hardcode the supported key.
    if (key == RATING_KEY_BY_USER) {
        mEditableKeys |= (KEY_EDITABLE_MASK & key);
        mMetadataChanged = TRUE;
    }
    else {
        Logger::E(TAG, "Metadata key %d cannot be edited", key);
    }
    return NOERROR;
}

ECode MediaMetadataEditor::RemoveEditableKeys()
{
    if (mApplied) {
        Logger::E(TAG, "Can't remove all editable keys of a previously applied MetadataEditor");
        return NOERROR;
    }
    if (mEditableKeys != 0) {
        mEditableKeys = 0;
        mMetadataChanged = TRUE;
    }
    return NOERROR;
}

ECode MediaMetadataEditor::GetEditableKeys(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    // only one editable key supported here
    if (mEditableKeys == RATING_KEY_BY_USER) {
        AutoPtr<ArrayOf<Int32> > keys = ArrayOf<Int32>::Alloc(1);
        keys->Set(0, RATING_KEY_BY_USER);
        *result = keys;
        REFCOUNT_ADD(*result);
    }
    else {
        *result = NULL;
    }
    return NOERROR;
}

ECode MediaMetadataEditor::PutString(
    /* [in] */ Int32 key,
    /* [in] */ const String& value)
{
    if (mApplied) {
        Logger::E(TAG, "Can't edit a previously applied MediaMetadataEditor");
        return NOERROR;
    }
    Int32 val;
    METADATA_KEYS_TYPE->Get(key, METADATA_TYPE_INVALID, &val);
    if (val != METADATA_TYPE_STRING) {
        // throw(new IllegalArgumentException("Invalid type 'String' for key "+ key));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mEditorMetadata->PutString(StringUtils::ToString(key), value);
    mMetadataChanged = TRUE;
    return NOERROR;
}

ECode MediaMetadataEditor::PutInt64(
    /* [in] */ Int32 key,
    /* [in] */ Int64 value)
{
    if (mApplied) {
        Logger::E(TAG, "Can't edit a previously applied MediaMetadataEditor");
        return NOERROR;
    }
    Int32 val;
    METADATA_KEYS_TYPE->Get(key, METADATA_TYPE_INVALID, &val);
    if (val != METADATA_TYPE_LONG) {
        // throw(new IllegalArgumentException("Invalid type 'long' for key "+ key));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mEditorMetadata->PutInt64(StringUtils::ToString(key), value);
    mMetadataChanged = TRUE;
    return NOERROR;
}

ECode MediaMetadataEditor::PutBitmap(
    /* [in] */ Int32 key,
    /* [in] */ IBitmap* bitmap)
{
    if (mApplied) {
        Logger::E(TAG, "Can't edit a previously applied MediaMetadataEditor");
        return NOERROR;
    }
    if (key != BITMAP_KEY_ARTWORK) {
        // throw(new IllegalArgumentException("Invalid type 'Bitmap' for key "+ key));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mEditorArtwork = bitmap;
    mArtworkChanged = TRUE;
    return NOERROR;
}

ECode MediaMetadataEditor::PutObject(
    /* [in] */ Int32 key,
    /* [in] */ IInterface* value)
{
    if (mApplied) {
        Logger::E(TAG, "Can't edit a previously applied MediaMetadataEditor");
        return NOERROR;
    }
    Int32 val;
    METADATA_KEYS_TYPE->Get(key, METADATA_TYPE_INVALID, &val);
    switch(val) {
        case METADATA_TYPE_LONG: {
            if (IInteger64::Probe(value) != NULL) {
                Int64 v;
                IInteger64::Probe(value)->GetValue(&v);
                return PutInt64(key, v);
            }
            else {
                // throw(new IllegalArgumentException("Not a non-null Long for key "+ key));
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        case METADATA_TYPE_STRING: {
            if ((value == NULL) || (ICharSequence::Probe(value) != NULL)) {
                String str;
                ICharSequence::Probe(value)->ToString(&str);
                return PutString(key, str);
            }
            else {
                // throw(new IllegalArgumentException("Not a String for key "+ key));
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        case METADATA_TYPE_RATING:
            mEditorMetadata->PutParcelable(StringUtils::ToString(key), IParcelable::Probe(value));
            mMetadataChanged = TRUE;
            break;
        case METADATA_TYPE_BITMAP:
            if ((value == NULL) || (IBitmap::Probe(value) != NULL))  {
                return PutBitmap(key, IBitmap::Probe(value));
            }
            else {
                // throw(new IllegalArgumentException("Not a Bitmap for key "+ key));
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        default:
            // throw(new IllegalArgumentException("Invalid key "+ key));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode MediaMetadataEditor::GetInt64(
    /* [in] */ Int32 key,
    /* [in] */ Int64 defaultValue,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    Int32 val;
    METADATA_KEYS_TYPE->Get(key, METADATA_TYPE_INVALID, &val);
    if (val != METADATA_TYPE_LONG) {
        // throw(new IllegalArgumentException("Invalid type 'long' for key "+ key));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mEditorMetadata->GetInt64(StringUtils::ToString(key), defaultValue, result);
}

ECode MediaMetadataEditor::GetString(
    /* [in] */ Int32 key,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);
    Int32 val;
    METADATA_KEYS_TYPE->Get(key, METADATA_TYPE_INVALID, &val);
    if (val != METADATA_TYPE_STRING) {
        // throw(new IllegalArgumentException("Invalid type 'String' for key "+ key));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mEditorMetadata->GetString(StringUtils::ToString(key), defaultValue, result);
}

ECode MediaMetadataEditor::GetBitmap(
    /* [in] */ Int32 key,
    /* [in] */ IBitmap* defaultValue,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (key != BITMAP_KEY_ARTWORK) {
        // throw(new IllegalArgumentException("Invalid type 'Bitmap' for key "+ key));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = (mEditorArtwork != NULL ? mEditorArtwork.Get() : defaultValue);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode MediaMetadataEditor::GetObject(
    /* [in] */ Int32 key,
    /* [in] */ IInterface* defaultValue,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 val;
    METADATA_KEYS_TYPE->Get(key, METADATA_TYPE_INVALID, &val);
    Boolean b;
    switch (val) {
        case METADATA_TYPE_LONG:
            if (mEditorMetadata->ContainsKey(StringUtils::ToString(key), &b), b) {
                Int64 val;
                mEditorMetadata->GetInt64(StringUtils::ToString(key), &val);
                AutoPtr<IInteger64> i64;
                CInteger64::New(val, (IInteger64**)&i64);
                *result = i64;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
            else {
                *result = defaultValue;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        case METADATA_TYPE_STRING: {
            if (mEditorMetadata->ContainsKey(StringUtils::ToString(key), &b), b) {
                String str;
                mEditorMetadata->GetString(StringUtils::ToString(key), &str);
                AutoPtr<ICharSequence> cs;
                CString::New(str, (ICharSequence**)&cs);
                *result = cs;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
            else {
                *result = defaultValue;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
        case METADATA_TYPE_RATING:
            if (mEditorMetadata->ContainsKey(StringUtils::ToString(key), &b), b) {
                AutoPtr<IParcelable> p;
                mEditorMetadata->GetParcelable(StringUtils::ToString(key), (IParcelable**)&p);
                *result = p;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
            else {
                *result = defaultValue;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        case METADATA_TYPE_BITMAP:
            // only one key for Bitmap supported, value is not stored in mEditorMetadata Bundle
            if (key == BITMAP_KEY_ARTWORK) {
                *result = (mEditorArtwork != NULL ? mEditorArtwork.Get() : defaultValue);
                REFCOUNT_ADD(*result)
                return NOERROR;
            } // else: fall through to invalid key handling
        default:
            // throw(new IllegalArgumentException("Invalid key "+ key));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
