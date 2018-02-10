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

#include "elastos/droid/media/CMediaMetadata.h"
#include "elastos/droid/media/CMediaDescriptionBuilder.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CSparseArray.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {

CMediaMetadata::StaticInitializer::StaticInitializer()
{
    PREFERRED_DESCRIPTION_ORDER = ArrayOf<String>::Alloc(7);
    PREFERRED_DESCRIPTION_ORDER->Set(0, METADATA_KEY_TITLE);
    PREFERRED_DESCRIPTION_ORDER->Set(1, METADATA_KEY_ARTIST);
    PREFERRED_DESCRIPTION_ORDER->Set(2, METADATA_KEY_ALBUM);
    PREFERRED_DESCRIPTION_ORDER->Set(3, METADATA_KEY_ALBUM_ARTIST);
    PREFERRED_DESCRIPTION_ORDER->Set(4, METADATA_KEY_WRITER);
    PREFERRED_DESCRIPTION_ORDER->Set(5, METADATA_KEY_AUTHOR);
    PREFERRED_DESCRIPTION_ORDER->Set(6, METADATA_KEY_COMPOSER);

    PREFERRED_BITMAP_ORDER = ArrayOf<String>::Alloc(3);
    PREFERRED_BITMAP_ORDER->Set(0, METADATA_KEY_DISPLAY_ICON);
    PREFERRED_BITMAP_ORDER->Set(1, METADATA_KEY_ART);
    PREFERRED_BITMAP_ORDER->Set(2, METADATA_KEY_ALBUM_ART);

    PREFERRED_URI_ORDER = ArrayOf<String>::Alloc(3);
    PREFERRED_URI_ORDER->Set(0, METADATA_KEY_DISPLAY_ICON_URI);
    PREFERRED_URI_ORDER->Set(1, METADATA_KEY_ART_URI);
    PREFERRED_URI_ORDER->Set(2, METADATA_KEY_ALBUM_ART_URI);

    CArrayMap::New((IArrayMap**)&METADATA_KEYS_TYPE);
    AutoPtr<ICharSequence> cs;
    CString::New(METADATA_KEY_TITLE, (ICharSequence**)&cs);
    AutoPtr<IInteger32> iMETADATA_TYPE_TEXT;
    CInteger32::New(METADATA_TYPE_TEXT, (IInteger32**)&iMETADATA_TYPE_TEXT);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_ARTIST, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_DURATION, (ICharSequence**)&cs);
    AutoPtr<IInteger32> iMETADATA_TYPE_LONG;
    CInteger32::New(METADATA_TYPE_LONG, (IInteger32**)&iMETADATA_TYPE_LONG);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_LONG);

    cs = NULL;
    CString::New(METADATA_KEY_ALBUM, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_AUTHOR, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_WRITER, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_COMPOSER, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_COMPILATION, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_DATE, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_YEAR, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_LONG);

    cs = NULL;
    CString::New(METADATA_KEY_GENRE, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_TRACK_NUMBER, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_LONG);

    cs = NULL;
    CString::New(METADATA_KEY_NUM_TRACKS, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_LONG);

    cs = NULL;
    CString::New(METADATA_KEY_DISC_NUMBER, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_LONG);

    cs = NULL;
    CString::New(METADATA_KEY_ALBUM_ARTIST, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_ART, (ICharSequence**)&cs);
    AutoPtr<IInteger32> iMETADATA_TYPE_BITMAP;
    CInteger32::New(METADATA_TYPE_BITMAP, (IInteger32**)&iMETADATA_TYPE_BITMAP);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_BITMAP);

    cs = NULL;
    CString::New(METADATA_KEY_ART_URI, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_ALBUM_ART, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_BITMAP);

    cs = NULL;
    CString::New(METADATA_KEY_ALBUM_ART_URI, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_USER_RATING, (ICharSequence**)&cs);
    AutoPtr<IInteger32> iMETADATA_TYPE_RATING;
    CInteger32::New(METADATA_TYPE_RATING, (IInteger32**)&iMETADATA_TYPE_RATING);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_RATING);

    cs = NULL;
    CString::New(METADATA_KEY_RATING, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_RATING);

    cs = NULL;
    CString::New(METADATA_KEY_DISPLAY_TITLE, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_DISPLAY_SUBTITLE, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_DISPLAY_DESCRIPTION, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    cs = NULL;
    CString::New(METADATA_KEY_DISPLAY_ICON, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_BITMAP);

    cs = NULL;
    CString::New(METADATA_KEY_DISPLAY_ICON_URI, (ICharSequence**)&cs);
    METADATA_KEYS_TYPE->Put(cs, iMETADATA_TYPE_TEXT);

    CSparseArray::New((ISparseArray**)&EDITOR_KEY_MAPPING);

    cs = NULL;
    CString::New(METADATA_KEY_ART, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataEditor::BITMAP_KEY_ARTWORK, cs);

    cs = NULL;
    CString::New(METADATA_KEY_RATING, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataEditor::RATING_KEY_BY_OTHERS, cs);

    cs = NULL;
    CString::New(METADATA_KEY_USER_RATING, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataEditor::RATING_KEY_BY_USER, cs);

    cs = NULL;
    CString::New(METADATA_KEY_ALBUM, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_ALBUM, cs);

    cs = NULL;
    CString::New(METADATA_KEY_ALBUM_ARTIST, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_ALBUMARTIST, cs);

    cs = NULL;
    CString::New(METADATA_KEY_ARTIST, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_ARTIST, cs);

    cs = NULL;
    CString::New(METADATA_KEY_AUTHOR, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_AUTHOR, cs);

    cs = NULL;
    CString::New(METADATA_KEY_TRACK_NUMBER, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_CD_TRACK_NUMBER, cs);

    cs = NULL;
    CString::New(METADATA_KEY_COMPOSER, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_COMPOSER, cs);

    cs = NULL;
    CString::New(METADATA_KEY_COMPILATION, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_COMPILATION, cs);

    cs = NULL;
    CString::New(METADATA_KEY_DATE, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_DATE, cs);

    cs = NULL;
    CString::New(METADATA_KEY_DISC_NUMBER, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_DISC_NUMBER, cs);

    cs = NULL;
    CString::New(METADATA_KEY_DURATION, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_DURATION, cs);

    cs = NULL;
    CString::New(METADATA_KEY_GENRE, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_GENRE, cs);

    cs = NULL;
    CString::New(METADATA_KEY_NUM_TRACKS, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_NUM_TRACKS, cs);

    cs = NULL;
    CString::New(METADATA_KEY_TITLE, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_TITLE, cs);

    cs = NULL;
    CString::New(METADATA_KEY_WRITER, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_WRITER, cs);

    cs = NULL;
    CString::New(METADATA_KEY_YEAR, (ICharSequence**)&cs);
    EDITOR_KEY_MAPPING->Put(IMediaMetadataRetriever::METADATA_KEY_YEAR, cs);
}

CAR_INTERFACE_IMPL_2(CMediaMetadata, Object, IMediaMetadata, IParcelable)

CAR_OBJECT_IMPL(CMediaMetadata)

String CMediaMetadata::TAG("MediaMetadata");
const Int32 CMediaMetadata::METADATA_TYPE_INVALID = -1;
const Int32 CMediaMetadata::METADATA_TYPE_LONG = 0;
const Int32 CMediaMetadata::METADATA_TYPE_TEXT = 1;
const Int32 CMediaMetadata::METADATA_TYPE_BITMAP = 2;
const Int32 CMediaMetadata::METADATA_TYPE_RATING = 3;
INIT_PROI_4 AutoPtr<ArrayOf<String> > CMediaMetadata::PREFERRED_DESCRIPTION_ORDER;
INIT_PROI_4 AutoPtr<ArrayOf<String> > CMediaMetadata::PREFERRED_BITMAP_ORDER;
INIT_PROI_4 AutoPtr<ArrayOf<String> > CMediaMetadata::PREFERRED_URI_ORDER;
INIT_PROI_4 AutoPtr<IArrayMap> CMediaMetadata::METADATA_KEYS_TYPE;
INIT_PROI_4 AutoPtr<ISparseArray> CMediaMetadata::EDITOR_KEY_MAPPING;
INIT_PROI_4 const CMediaMetadata::StaticInitializer CMediaMetadata::sInitializer;

CMediaMetadata::CMediaMetadata()
{
}

CMediaMetadata::~CMediaMetadata()
{
}

ECode CMediaMetadata::constructor()
{
    return NOERROR;
}

ECode CMediaMetadata::constructor(
    /* [in] */ IBundle* bundle)
{
    return CBundle::New(bundle, (IBundle**)&mBundle);
}

ECode CMediaMetadata::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mBundle = IBundle::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mDescription = IMediaDescription::Probe(obj);
    return NOERROR;
}

ECode CMediaMetadata::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mBundle);
    dest->WriteInterfacePtr(mDescription);
    return NOERROR;
}

ECode CMediaMetadata::ContainsKey(
    /* [in] */ const String& key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mBundle->ContainsKey(key, result);
}

ECode CMediaMetadata::GetText(
    /* [in] */ const String& key,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    return mBundle->GetCharSequence(key, result);
}

ECode CMediaMetadata::GetString(
    /* [in] */ const String& key,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> text;
    GetText(key, (ICharSequence**)&text);
    if (text != NULL) {
        return text->ToString(result);
    }
    *result = NULL;
    return NOERROR;
}

ECode CMediaMetadata::GetInt64(
    /* [in] */ const String& key,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mBundle->GetInt64(key, 0, result);
}

ECode CMediaMetadata::GetRating(
    /* [in] */ const String& key,
    /* [out] */ IRating** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IRating> rating;
    // try {
    AutoPtr<IParcelable> obj;
    mBundle->GetParcelable(key, (IParcelable**)&obj);
    rating = IRating::Probe(obj);
    // } catch (Exception e) {
    //     // ignore, value was not a bitmap
    //     Log.w(TAG, "Failed to retrieve a key as Rating.", e);
    // }
    *result = rating;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaMetadata::GetBitmap(
    /* [in] */ const String& key,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IBitmap> bmp;
    // try {
    AutoPtr<IParcelable> obj;
    mBundle->GetParcelable(key, (IParcelable**)&obj);
    bmp = IBitmap::Probe(obj);
    // } catch (Exception e) {
    //     // ignore, value was not a bitmap
    //     Log.w(TAG, "Failed to retrieve a key as Bitmap.", e);
    // }
    *result = bmp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaMetadata::Size(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mBundle->GetSize(result);
}

ECode CMediaMetadata::KeySet(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result)
    return mBundle->GetKeySet(result);
}

ECode CMediaMetadata::GetDescription(
    /* [out] */ IMediaDescription** result)
{
    VALIDATE_NOT_NULL(result)
    if (mDescription != NULL) {
        *result = mDescription;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    String mediaId;
    GetString(METADATA_KEY_MEDIA_ID, &mediaId);

    AutoPtr<ArrayOf<ICharSequence*> > text = ArrayOf<ICharSequence*>::Alloc(3);
    AutoPtr<IBitmap> icon;
    AutoPtr<IUri> iconUri;

    // First handle the case where display data is set already
    AutoPtr<ICharSequence> displayText;
    GetText(METADATA_KEY_DISPLAY_TITLE, (ICharSequence**)&displayText);
    if (!TextUtils::IsEmpty(displayText)) {
        // If they have a display title use only display data, otherwise use
        // our best bets
        text->Set(0, displayText);
        AutoPtr<ICharSequence> cs;
        GetText(METADATA_KEY_DISPLAY_SUBTITLE, (ICharSequence**)&cs);
        text->Set(1, cs);
        cs = NULL;
        GetText(METADATA_KEY_DISPLAY_DESCRIPTION, (ICharSequence**)&cs);
        text->Set(2, cs);
    }
    else {
        // Use whatever fields we can
        Int32 textIndex = 0;
        Int32 keyIndex = 0;
        while (textIndex < text->GetLength() && keyIndex < PREFERRED_DESCRIPTION_ORDER->GetLength()) {
            AutoPtr<ICharSequence> next;
            GetText((*PREFERRED_DESCRIPTION_ORDER)[keyIndex++], (ICharSequence**)&next);
            if (!TextUtils::IsEmpty(next)) {
                // Fill in the next empty bit of text
                text->Set(textIndex++, next);
            }
        }
    }

    // Get the best art bitmap we can find
    for (Int32 i = 0; i < PREFERRED_BITMAP_ORDER->GetLength(); i++) {
        AutoPtr<IBitmap> next;
        GetBitmap((*PREFERRED_BITMAP_ORDER)[i], (IBitmap**)&next);
        if (next != NULL) {
            icon = next;
            break;
        }
    }

    // Get the best Uri we can find
    for (Int32 i = 0; i < PREFERRED_URI_ORDER->GetLength(); i++) {
        String next;
        GetString((*PREFERRED_URI_ORDER)[i], &next);
        if (!TextUtils::IsEmpty(next)) {
            Uri::Parse(next, (IUri**)&iconUri);
            break;
        }
    }

    AutoPtr<IMediaDescriptionBuilder> bob;
    CMediaDescriptionBuilder::New((IMediaDescriptionBuilder**)&bob);
    bob->SetMediaId(mediaId);
    bob->SetTitle((*text)[0]);
    bob->SetSubtitle((*text)[1]);
    bob->SetDescription((*text)[2]);
    bob->SetIconBitmap(icon);
    bob->SetIconUri(iconUri);
    bob->Build((IMediaDescription**)&mDescription);

    *result = mDescription;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaMetadata::GetKeyFromMetadataEditorKey(
    /* [in] */ Int32 editorKey,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    EDITOR_KEY_MAPPING->Get(editorKey, NULL, (IInterface**)&obj);
    return ICharSequence::Probe(obj)->ToString(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
