#ifndef __ELASTOS_DROID_MEDIA_CMEDIAMETADATA_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAMETADATA_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Media_CMediaMetadata.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Contains metadata about an item, such as the title, artist, etc.
 */
CarClass(CMediaMetadata)
    , public Object
    , public IMediaMetadata
    , public IParcelable
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    friend class CMediaMetadataBuilder;

    CMediaMetadata();

    virtual ~CMediaMetadata();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBundle* bundle);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns true if the given key is contained in the metadata
     *
     * @param key a String key
     * @return true if the key exists in this metadata, false otherwise
     */
    CARAPI ContainsKey(
        /* [in] */ const String& key,
        /* [out] */ Boolean* result);

    /**
     * Returns the value associated with the given key, or null if no mapping of
     * the desired type exists for the given key or a null value is explicitly
     * associated with the key.
     *
     * @param key The key the value is stored under
     * @return a CharSequence value, or null
     */
    CARAPI GetText(
        /* [in] */ const String& key,
        /* [out] */ ICharSequence** result);

    /**
     * Returns the text value associated with the given key as a String, or null
     * if no mapping of the desired type exists for the given key or a null
     * value is explicitly associated with the key. This is equivalent to
     * calling {@link #getText getText().toString()} if the value is not null.
     *
     * @param key The key the value is stored under
     * @return a String value, or null
     */
    CARAPI GetString(
        /* [in] */ const String& key,
        /* [out] */ String* result);

    /**
     * Returns the value associated with the given key, or 0L if no long exists
     * for the given key.
     *
     * @param key The key the value is stored under
     * @return a long value
     */
    CARAPI GetInt64(
        /* [in] */ const String& key,
        /* [out] */ Int64* result);

    /**
     * Returns a {@link Rating} for the given key or null if no rating exists
     * for the given key.
     *
     * @param key The key the value is stored under
     * @return A {@link Rating} or null
     */
    CARAPI GetRating(
        /* [in] */ const String& key,
        /* [out] */ IRating** result);

    /**
     * Returns a {@link Bitmap} for the given key or null if no bitmap exists
     * for the given key.
     *
     * @param key The key the value is stored under
     * @return A {@link Bitmap} or null
     */
    CARAPI GetBitmap(
        /* [in] */ const String& key,
        /* [out] */ IBitmap** result);

    /**
     * Returns the number of fields in this metadata.
     *
     * @return The number of fields in the metadata.
     */
    CARAPI Size(
        /* [out] */ Int32* result);

    /**
     * Returns a Set containing the Strings used as keys in this metadata.
     *
     * @return a Set of String keys
     */
    CARAPI KeySet(
        /* [out] */ ISet** result);

    /**
     * Returns a simple description of this metadata for display purposes.
     *
     * @return A simple description of this metadata.
     */
    CARAPI GetDescription(
        /* [out] */ IMediaDescription** result);

    /**
     * Helper for getting the String key used by {@link MediaMetadata} from the
     * integer key that {@link MediaMetadataEditor} uses.
     *
     * @param editorKey The key used by the editor
     * @return The key used by this class or null if no mapping exists
     * @hide
     */
    static CARAPI GetKeyFromMetadataEditorKey(
        /* [in] */ Int32 editorKey,
        /* [out] */ String* result);

private:
    static String TAG;

    static AutoPtr<ArrayOf<String> > PREFERRED_DESCRIPTION_ORDER;

    static AutoPtr<ArrayOf<String> > PREFERRED_BITMAP_ORDER;

    static AutoPtr<ArrayOf<String> > PREFERRED_URI_ORDER;

    static const Int32 METADATA_TYPE_INVALID;
    static const Int32 METADATA_TYPE_LONG;
    static const Int32 METADATA_TYPE_TEXT;
    static const Int32 METADATA_TYPE_BITMAP;
    static const Int32 METADATA_TYPE_RATING;
    static AutoPtr<IArrayMap> METADATA_KEYS_TYPE;

    static AutoPtr<ISparseArray> EDITOR_KEY_MAPPING;

    AutoPtr<IBundle> mBundle;
    AutoPtr<IMediaDescription> mDescription;

    static const StaticInitializer sInitializer;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIAMETADATA_H__
