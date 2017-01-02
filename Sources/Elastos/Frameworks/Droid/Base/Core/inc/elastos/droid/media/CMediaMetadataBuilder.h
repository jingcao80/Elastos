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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIAMETADATABUILDER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAMETADATABUILDER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_Media_CMediaMetadataBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/CBundle.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Use to build MediaMetadata objects. The system defined metadata keys must
 * use the appropriate data type.
 */
CarClass(CMediaMetadataBuilder)
    , public Object
    , public IMediaMetadataBuilder
{
public:
    CMediaMetadataBuilder();

    virtual ~CMediaMetadataBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Create an empty Builder. Any field that should be included in the
     * {@link MediaMetadata} must be added.
     */
    CARAPI constructor();

    /**
     * Create a Builder using a {@link MediaMetadata} instance to set the
     * initial values. All fields in the source metadata will be included in
     * the new metadata. Fields can be overwritten by adding the same key.
     *
     * @param source
     */
    CARAPI constructor(
        /* [in] */ IMediaMetadata* bundle);

    /**
     * Put a CharSequence value into the metadata. Custom keys may be used,
     * but if the METADATA_KEYs defined in this class are used they may only
     * be one of the following:
     * <ul>
     * <li>{@link #METADATA_KEY_TITLE}</li>
     * <li>{@link #METADATA_KEY_ARTIST}</li>
     * <li>{@link #METADATA_KEY_ALBUM}</li>
     * <li>{@link #METADATA_KEY_AUTHOR}</li>
     * <li>{@link #METADATA_KEY_WRITER}</li>
     * <li>{@link #METADATA_KEY_COMPOSER}</li>
     * <li>{@link #METADATA_KEY_DATE}</li>
     * <li>{@link #METADATA_KEY_GENRE}</li>
     * <li>{@link #METADATA_KEY_ALBUM_ARTIST}</li>
     * <li>{@link #METADATA_KEY_ART_URI}</li>
     * <li>{@link #METADATA_KEY_ALBUM_ART_URI}</li>
     * <li>{@link #METADATA_KEY_DISPLAY_TITLE}</li>
     * <li>{@link #METADATA_KEY_DISPLAY_SUBTITLE}</li>
     * <li>{@link #METADATA_KEY_DISPLAY_DESCRIPTION}</li>
     * <li>{@link #METADATA_KEY_DISPLAY_ICON_URI}</li>
     * </ul>
     *
     * @param key The key for referencing this value
     * @param value The CharSequence value to store
     * @return The Builder to allow chaining
     */
    CARAPI PutText(
        /* [in] */ const String& key,
        /* [in] */ ICharSequence* value);

    /**
     * Put a String value into the metadata. Custom keys may be used, but if
     * the METADATA_KEYs defined in this class are used they may only be one
     * of the following:
     * <ul>
     * <li>{@link #METADATA_KEY_TITLE}</li>
     * <li>{@link #METADATA_KEY_ARTIST}</li>
     * <li>{@link #METADATA_KEY_ALBUM}</li>
     * <li>{@link #METADATA_KEY_AUTHOR}</li>
     * <li>{@link #METADATA_KEY_WRITER}</li>
     * <li>{@link #METADATA_KEY_COMPOSER}</li>
     * <li>{@link #METADATA_KEY_DATE}</li>
     * <li>{@link #METADATA_KEY_GENRE}</li>
     * <li>{@link #METADATA_KEY_ALBUM_ARTIST}</li>
     * <li>{@link #METADATA_KEY_ART_URI}</li>
     * <li>{@link #METADATA_KEY_ALBUM_ART_URI}</li>
     * <li>{@link #METADATA_KEY_DISPLAY_TITLE}</li>
     * <li>{@link #METADATA_KEY_DISPLAY_SUBTITLE}</li>
     * <li>{@link #METADATA_KEY_DISPLAY_DESCRIPTION}</li>
     * <li>{@link #METADATA_KEY_DISPLAY_ICON_URI}</li>
     * </ul>
     *
     * @param key The key for referencing this value
     * @param value The String value to store
     * @return The Builder to allow chaining
     */
    CARAPI PutString(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    /**
     * Put a long value into the metadata. Custom keys may be used, but if
     * the METADATA_KEYs defined in this class are used they may only be one
     * of the following:
     * <ul>
     * <li>{@link #METADATA_KEY_DURATION}</li>
     * <li>{@link #METADATA_KEY_TRACK_NUMBER}</li>
     * <li>{@link #METADATA_KEY_NUM_TRACKS}</li>
     * <li>{@link #METADATA_KEY_DISC_NUMBER}</li>
     * <li>{@link #METADATA_KEY_YEAR}</li>
     * </ul>
     *
     * @param key The key for referencing this value
     * @param value The long value to store
     * @return The Builder to allow chaining
     */
    CARAPI PutLong(
        /* [in] */ const String& key,
        /* [in] */ Int64 value);

    /**
     * Put a {@link Rating} into the metadata. Custom keys may be used, but
     * if the METADATA_KEYs defined in this class are used they may only be
     * one of the following:
     * <ul>
     * <li>{@link #METADATA_KEY_RATING}</li>
     * <li>{@link #METADATA_KEY_USER_RATING}</li>
     * </ul>
     *
     * @param key The key for referencing this value
     * @param value The Rating value to store
     * @return The Builder to allow chaining
     */
    CARAPI PutRating(
        /* [in] */ const String& key,
        /* [in] */ IRating* value);

    /**
     * Put a {@link Bitmap} into the metadata. Custom keys may be used, but
     * if the METADATA_KEYs defined in this class are used they may only be
     * one of the following:
     * <ul>
     * <li>{@link #METADATA_KEY_ART}</li>
     * <li>{@link #METADATA_KEY_ALBUM_ART}</li>
     * <li>{@link #METADATA_KEY_DISPLAY_ICON}</li>
     * </ul>
     *
     * @param key The key for referencing this value
     * @param value The Bitmap to store
     * @return The Builder to allow chaining
     */
    CARAPI PutBitmap(
        /* [in] */ const String& key,
        /* [in] */ IBitmap* value);

    /**
     * Creates a {@link MediaMetadata} instance with the specified fields.
     *
     * @return The new MediaMetadata instance
     */
    CARAPI Build(
        /* [out] */ IMediaMetadata** result);

private:
    AutoPtr<IBundle> mBundle;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIAMETADATABUILDER_H__
