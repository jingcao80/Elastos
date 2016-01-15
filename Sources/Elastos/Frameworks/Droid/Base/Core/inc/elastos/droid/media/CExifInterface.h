
#ifndef __ELASTOS_DROID_MEDIA_CEXIFINTERFACE_H__
#define __ELASTOS_DROID_MEDIA_CEXIFINTERFACE_H__

#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_CExifInterface.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/Object.h>

using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CExifInterface)
    , public Object
    , public IExifInterface
{
public:
    CExifInterface();

    virtual ~CExifInterface();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Reads Exif tags from the specified JPEG file.
     */
    CARAPI constructor(
        /* [in] */ const String& filename);

    /**
     * Returns the value of the specified tag or {@code null} if there
     * is no such tag in the JPEG file.
     *
     * @param tag the name of the tag.
     */
    CARAPI GetAttribute(
        /* [in] */ const String& tag,
        /* [out] */ String* result);

    /**
     * Returns the integer value of the specified tag. If there is no such tag
     * in the JPEG file or the value cannot be parsed as integer, return
     * <var>defaultValue</var>.
     *
     * @param tag the name of the tag.
     * @param defaultValue the value to return if the tag is not available.
     */
    CARAPI GetAttributeInt32(
        /* [in] */ const String& tag,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32* result);

    /**
     * Returns the double value of the specified rational tag. If there is no
     * such tag in the JPEG file or the value cannot be parsed as double, return
     * <var>defaultValue</var>.
     *
     * @param tag the name of the tag.
     * @param defaultValue the value to return if the tag is not available.
     */
    CARAPI GetAttributeDouble(
        /* [in] */ const String& tag,
        /* [in] */ Double defaultValue,
        /* [out] */ Double* result);

    /**
     * Set the value of the specified tag.
     *
     * @param tag the name of the tag.
     * @param value the value of the tag.
     */
    CARAPI SetAttribute(
        /* [in] */ const String& tag,
        /* [in] */ const String& value);

    /**
     * Save the tag data into the JPEG file. This is expensive because it involves
     * copying all the JPG data from one file to another and deleting the old file
     * and renaming the other. It's best to use {@link #setAttribute(String,String)}
     * to set all attributes to write and make a single call rather than multiple
     * calls for each attribute.
     */
    CARAPI SaveAttributes();

    /**
     * Returns true if the JPEG file has a thumbnail.
     */
    CARAPI HasThumbnail(
        /* [out] */ Boolean* result);

    /**
     * Returns the thumbnail inside the JPEG file, or {@code null} if there is no thumbnail.
     * The returned data is in JPEG format and can be decoded using
     * {@link android.graphics.BitmapFactory#decodeByteArray(byte[],int,int)}
     */
    CARAPI GetThumbnail(
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Returns the offset and length of thumbnail inside the JPEG file, or
     * {@code null} if there is no thumbnail.
     *
     * @return two-element array, the offset in the first value, and length in
     *         the second, or {@code null} if no thumbnail was found.
     * @hide
     */
    CARAPI GetThumbnailRange(
        /* [out, callee] */ ArrayOf<Int64>** result);

    /**
     * Stores the latitude and longitude value in a float array. The first element is
     * the latitude, and the second element is the longitude. Returns false if the
     * Exif tags are not available.
     */
    CARAPI GetLatLong(
        /* [in] */ ArrayOf<Float>* output,
        /* [out] */ Boolean* result);

    /**
     * Return the altitude in meters. If the exif tag does not exist, return
     * <var>defaultValue</var>.
     *
     * @param defaultValue the value to return if the tag is not available.
     */
    CARAPI GetAltitude(
        /* [in] */ Double defaultValue,
        /* [out] */ Double* result);

    /**
     * Returns number of milliseconds since Jan. 1, 1970, midnight.
     * Returns -1 if the date time information if not available.
     * @hide
     */
    CARAPI GetDateTime(
        /* [out] */ Int64* result);

    /**
     * Returns number of milliseconds since Jan. 1, 1970, midnight UTC.
     * Returns -1 if the date time information if not available.
     * @hide
     */
    CARAPI GetGpsDateTime(
        /* [out] */ Int64* result);

private:
    /**
     * Initialize mAttributes with the attributes from the file mFilename.
     *
     * mAttributes is a HashMap which stores the Exif attributes of the file.
     * The key is the standard tag name and the value is the tag's value: e.g.
     * Model -> Nikon. Numeric values are stored as strings.
     *
     * This function also initialize mHasThumbnail to indicate whether the
     * file has a thumbnail inside.
     */
    CARAPI_(void) LoadAttributes();

    static CARAPI ConvertRationalLatLonToFloat(
        /* [in] */ const String& rationalString,
        /* [in] */ const String& ref,
        /* [out] */ Float* result);

    CARAPI_(Boolean) AppendThumbnailNative(
        /* [in] */ const String& fileName,
        /* [in] */ const String& thumbnailFileName);

    CARAPI_(void) SaveAttributesNative(
        /* [in] */ const String& fileName,
        /* [in] */ const String& compressedAttributes);

    CARAPI_(String) GetAttributesNative(
        /* [in] */ const String& fileName);

    CARAPI_(void) CommitChangesNative(
        /* [in] */ const String& fileName);

    CARAPI_(AutoPtr< ArrayOf<Byte> >) GetThumbnailNative(
        /* [in] */ const String& fileName);

    CARAPI_(AutoPtr< ArrayOf<Int64> >) GetThumbnailRangeNative(
        /* [in] */ const String& fileName);

    static AutoPtr<ISimpleDateFormat> GetFormatter();

private:
    static AutoPtr<ISimpleDateFormat> sFormatter;

    String mFilename;

    HashMap<String, String> mAttributes;

    Boolean mHasThumbnail;

    // Because the underlying implementation (jhead) uses static variables,
    // there can only be one user at a time for the native functions (and
    // they cannot keep state in the native code across function calls). We
    // use sLock to serialize the accesses.
    static Object sLock;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CEXIFINTERFACE_H__
