#ifndef __ELASTOS_DROID_MEDIA_MEDIAMETADATAEDITOR_H__
#define __ELASTOS_DROID_MEDIA_MEDIAMETADATAEDITOR_H__

#include <Elastos.CoreLibrary.Core.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ISparseInt32Array;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * An abstract class for editing and storing metadata that can be published by
 * {@link RemoteControlClient}. See the {@link RemoteControlClient#editMetadata(boolean)}
 * method to instantiate a {@link RemoteControlClient.MetadataEditor} object.
 *
 * @deprecated Use {@link MediaMetadata} instead together with {@link MediaSession}.
 */
class MediaMetadataEditor
    : public Object
    , public IMediaMetadataEditor
{
public:
    MediaMetadataEditor();

    virtual ~MediaMetadataEditor();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Applies all of the metadata changes that have been set since the MediaMetadataEditor instance
     * was created or since {@link #clear()} was called.
     */
    CARAPI Apply();

    /**
     * Clears all the pending metadata changes set since the MediaMetadataEditor instance was
     * created or since this method was last called.
     * Note that clearing the metadata doesn't reset the editable keys
     * (use {@link #removeEditableKeys()} instead).
     */
    CARAPI Clear();

    /**
     * Flags the given key as being editable.
     * This should only be used by metadata publishers, such as {@link RemoteControlClient},
     * which will declare the metadata field as eligible to be updated, with new values
     * received through the {@link RemoteControlClient.OnMetadataUpdateListener} interface.
     * @param key the type of metadata that can be edited. The supported key is
     *     {@link #RATING_KEY_BY_USER}.
     */
    CARAPI AddEditableKey(
        /* [in] */ Int32 key);

    /**
     * Causes all metadata fields to be read-only.
     */
    CARAPI RemoveEditableKeys();

    /**
     * Retrieves the keys flagged as editable.
     * @return null if there are no editable keys, or an array containing the keys.
     */
    CARAPI GetEditableKeys(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Adds textual information.
     * Note that none of the information added after {@link #apply()} has been called,
     * will be available to consumers of metadata stored by the MediaMetadataEditor.
     * @param key The identifier of a the metadata field to set. Valid values are
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_ALBUM},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_ALBUMARTIST},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_TITLE},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_ARTIST},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_AUTHOR},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_COMPILATION},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_COMPOSER},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_DATE},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_GENRE},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_WRITER}.
     * @param value The text for the given key, or {@code null} to signify there is no valid
     *      information for the field.
     * @return Returns a reference to the same MediaMetadataEditor object, so you can chain put
     *      calls together.
     */
    CARAPI PutString(
        /* [in] */ Int32 key,
        /* [in] */ const String& value);

    /**
     * Adds numerical information.
     * Note that none of the information added after {@link #apply()} has been called
     * will be available to consumers of metadata stored by the MediaMetadataEditor.
     * @param key the identifier of a the metadata field to set. Valid values are
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_CD_TRACK_NUMBER},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_DISC_NUMBER},
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_DURATION} (with a value
     *      expressed in milliseconds),
     *      {@link android.media.MediaMetadataRetriever#METADATA_KEY_YEAR}.
     * @param value The long value for the given key
     * @return Returns a reference to the same MediaMetadataEditor object, so you can chain put
     *      calls together.
     * @throws IllegalArgumentException
     */
    CARAPI PutInt64(
        /* [in] */ Int32 key,
        /* [in] */ Int64 value);

    /**
     * Adds image.
     * @param key the identifier of the bitmap to set. The only valid value is
     *      {@link #BITMAP_KEY_ARTWORK}
     * @param bitmap The bitmap for the artwork, or null if there isn't any.
     * @return Returns a reference to the same MediaMetadataEditor object, so you can chain put
     *      calls together.
     * @throws IllegalArgumentException
     * @see android.graphics.Bitmap
     */
    CARAPI PutBitmap(
        /* [in] */ Int32 key,
        /* [in] */ IBitmap* bitmap);

    /**
     * Adds information stored as an instance.
     * Note that none of the information added after {@link #apply()} has been called
     * will be available to consumers of metadata stored by the MediaMetadataEditor.
     * @param key the identifier of a the metadata field to set. Valid keys for a:
     *     <ul>
     *     <li>{@link Bitmap} object are {@link #BITMAP_KEY_ARTWORK},</li>
     *     <li>{@link String} object are the same as for {@link #putString(int, String)}</li>
     *     <li>{@link Long} object are the same as for {@link #putLong(int, long)}</li>
     *     <li>{@link Rating} object are {@link #RATING_KEY_BY_OTHERS}
     *         and {@link #RATING_KEY_BY_USER}.</li>
     *     </ul>
     * @param value the metadata to add.
     * @return Returns a reference to the same MediaMetadataEditor object, so you can chain put
     *      calls together.
     * @throws IllegalArgumentException
     */
    CARAPI PutObject(
        /* [in] */ Int32 key,
        /* [in] */ IInterface* value);

    /**
     * Returns the long value for the key.
     * @param key one of the keys supported in {@link #putLong(int, long)}
     * @param defaultValue the value returned if the key is not present
     * @return the long value for the key, or the supplied default value if the key is not present
     * @throws IllegalArgumentException
     */
    CARAPI GetInt64(
        /* [in] */ Int32 key,
        /* [in] */ Int64 defaultValue,
        /* [out] */ Int64* result);

    /**
     * Returns the {@link String} value for the key.
     * @param key one of the keys supported in {@link #putString(int, String)}
     * @param defaultValue the value returned if the key is not present
     * @return the {@link String} value for the key, or the supplied default value if the key is
     *     not present
     * @throws IllegalArgumentException
     */
    CARAPI GetString(
        /* [in] */ Int32 key,
        /* [in] */ const String& defaultValue,
        /* [out] */ String* result);

    /**
     * Returns the {@link Bitmap} value for the key.
     * @param key the {@link #BITMAP_KEY_ARTWORK} key
     * @param defaultValue the value returned if the key is not present
     * @return the {@link Bitmap} value for the key, or the supplied default value if the key is
     *     not present
     * @throws IllegalArgumentException
     */
    CARAPI GetBitmap(
        /* [in] */ Int32 key,
        /* [in] */ IBitmap* defaultValue,
        /* [out] */ IBitmap** result);

    /**
     * Returns an object representation of the value for the key
     * @param key one of the keys supported in {@link #putObject(int, Object)}
     * @param defaultValue the value returned if the key is not present
     * @return the object for the key, as a {@link Long}, {@link Bitmap}, {@link String}, or
     *     {@link Rating} depending on the key value, or the supplied default value if the key is
     *     not present
     * @throws IllegalArgumentException
     */
    CARAPI GetObject(
        /* [in] */ Int32 key,
        /* [in] */ IInterface* defaultValue,
        /* [out] */ IInterface** result);

private:
    static CARAPI_(Boolean) InitStatic();

protected:
    /**
     * @hide
     * Mask of editable keys.
     */
    Int64 mEditableKeys;

    /**
     * @hide
     */
    Boolean mMetadataChanged;

    /**
     * @hide
     */
    Boolean mApplied;

    /**
     * @hide
     */
    Boolean mArtworkChanged;

    /**
     * @hide
     */
    AutoPtr<IBitmap> mEditorArtwork;

    /**
     * @hide
     */
    AutoPtr<IBundle> mEditorMetadata;

    /**
     * @hide
     */
    AutoPtr<IMediaMetadataBuilder> mMetadataBuilder;

    /**
     * @hide
     */
    static const Int32 METADATA_TYPE_INVALID;
    /**
     * @hide
     */
    static const Int32 METADATA_TYPE_LONG;

    /**
     * @hide
     */
    static const Int32 METADATA_TYPE_STRING;

    /**
     * @hide
     */
    static const Int32 METADATA_TYPE_BITMAP;

    /**
     * @hide
     */
    static const Int32 METADATA_TYPE_RATING;

    /**
     * @hide
     */
    static AutoPtr<ISparseInt32Array> METADATA_KEYS_TYPE;

private:
    static const String TAG;
    static Boolean mInit;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_MEDIAMETADATAEDITOR_H__
