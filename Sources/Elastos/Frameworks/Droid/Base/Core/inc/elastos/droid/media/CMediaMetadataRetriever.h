
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAMETADATARETRIEVER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAMETADATARETRIEVER_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Media_CMediaMetadataRetriever.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <media/mediametadataretriever.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Media {
/**
 * CMediaMetadataRetriever class provides a unified interface for retrieving
 * frame and meta data from an input media file.
 */
CarClass(CMediaMetadataRetriever)
    , public Object
    , public IMediaMetadataRetriever
{
public:
    CMediaMetadataRetriever();

    virtual ~CMediaMetadataRetriever();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Sets the data source (file pathname) to use. Call this
     * method before the rest of the methods in this class. This method may be
     * time-consuming.
     *
     * @param path The path of the input media file.
     * @throws IllegalArgumentException If the path is invalid.
     */
    CARAPI SetDataSource(
        /* [in] */ const String& path);

    /**
     * Sets the data source (URI) to use. Call this
     * method before the rest of the methods in this class. This method may be
     * time-consuming.
     *
     * @param uri The URI of the input media.
     * @param headers the headers to be sent together with the request for the data
     * @throws IllegalArgumentException If the URI is invalid.
     */
    CARAPI SetDataSource(
        /* [in] */ const String& uri,
        /* [in] */ IMap* headers);

    /**
     * Sets the data source (FileDescriptor) to use.  It is the caller's
     * responsibility to close the file descriptor. It is safe to do so as soon
     * as this call returns. Call this method before the rest of the methods in
     * this class. This method may be time-consuming.
     *
     * @param fd the FileDescriptor for the file you want to play
     * @param offset the offset into the file where the data to be played starts,
     * in bytes. It must be non-negative
     * @param length the length in bytes of the data to be played. It must be
     * non-negative.
     * @throws IllegalArgumentException if the arguments are invalid
     */
    CARAPI SetDataSource(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    /**
     * Sets the data source (FileDescriptor) to use. It is the caller's
     * responsibility to close the file descriptor. It is safe to do so as soon
     * as this call returns. Call this method before the rest of the methods in
     * this class. This method may be time-consuming.
     *
     * @param fd the FileDescriptor for the file you want to play
     * @throws IllegalArgumentException if the FileDescriptor is invalid
     */
    CARAPI SetDataSource(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Sets the data source as a content Uri. Call this method before
     * the rest of the methods in this class. This method may be time-consuming.
     *
     * @param context the Context to use when resolving the Uri
     * @param uri the Content URI of the data you want to play
     * @throws IllegalArgumentException if the Uri is invalid
     * @throws SecurityException if the Uri cannot be used due to lack of
     * permission.
     */
    CARAPI SetDataSource(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri);

    /**
     * Call this method after setDataSource(). This method retrieves the
     * meta data value associated with the keyCode.
     *
     * The keyCode currently supported is listed below as METADATA_XXX
     * constants. With any other value, it returns a NULL pointer.
     *
     * @param keyCode One of the constants listed below at the end of the class.
     * @return The meta data value associate with the given keyCode on success;
     * NULL on failure.
     */
    CARAPI ExtractMetadata(
        /* [in] */ Int32 keyCode,
        /* [out] */ String* data);

    /**
     * Call this method after setDataSource(). This method finds a
     * representative frame close to the given time position by considering
     * the given option if possible, and returns it as a bitmap. This is
     * useful for generating a thumbnail for an input data source or just
     * obtain and display a frame at the given time position.
     *
     * @param timeUs The time position where the frame will be retrieved.
     * When retrieving the frame at the given time position, there is no
     * guarantee that the data source has a frame located at the position.
     * When this happens, a frame nearby will be returned. If timeUs is
     * negative, time position and option will ignored, and any frame
     * that the implementation considers as representative may be returned.
     *
     * @param option a hint on how the frame is found. Use
     * {@link #OPTION_PREVIOUS_SYNC} if one wants to retrieve a sync frame
     * that has a timestamp earlier than or the same as timeUs. Use
     * {@link #OPTION_NEXT_SYNC} if one wants to retrieve a sync frame
     * that has a timestamp later than or the same as timeUs. Use
     * {@link #OPTION_CLOSEST_SYNC} if one wants to retrieve a sync frame
     * that has a timestamp closest to or the same as timeUs. Use
     * {@link #OPTION_CLOSEST} if one wants to retrieve a frame that may
     * or may not be a sync frame but is closest to or the same as timeUs.
     * {@link #OPTION_CLOSEST} often has larger performance overhead compared
     * to the other options if there is no sync frame located at timeUs.
     *
     * @return A Bitmap containing a representative video frame, which
     *         can be NULL, if such a frame cannot be retrieved.
     */
    CARAPI GetFrameAtTime(
        /* [in] */ Int64 timeUs,
        /* [in] */ Int32 option,
        /* [out] */ IBitmap** bitmap);

    /**
     * Call this method after setDataSource(). This method finds a
     * representative frame close to the given time position if possible,
     * and returns it as a bitmap. This is useful for generating a thumbnail
     * for an input data source. Call this method if one does not care
     * how the frame is found as Int64 as it is close to the given time;
     * otherwise, please call {@link #getFrameAtTime(Int64, Int32)}.
     *
     * @param timeUs The time position where the frame will be retrieved.
     * When retrieving the frame at the given time position, there is no
     * guarentee that the data source has a frame located at the position.
     * When this happens, a frame nearby will be returned. If timeUs is
     * negative, time position and option will ignored, and any frame
     * that the implementation considers as representative may be returned.
     *
     * @return A Bitmap containing a representative video frame, which
     *         can be NULL, if such a frame cannot be retrieved.
     *
     * @see #getFrameAtTime(Int64, Int32)
     */
    CARAPI GetFrameAtTime(
        /* [in] */ Int64 timeUs,
        /* [out] */ IBitmap** bitmap);

    /**
     * Call this method after setDataSource(). This method finds a
     * representative frame at any time position if possible,
     * and returns it as a bitmap. This is useful for generating a thumbnail
     * for an input data source. Call this method if one does not
     * care about where the frame is located; otherwise, please call
     * {@link #getFrameAtTime(Int64)} or {@link #getFrameAtTime(Int64, Int32)}
     *
     * @return A Bitmap containing a representative video frame, which
     *         can be NULL, if such a frame cannot be retrieved.
     *
     * @see #getFrameAtTime(Int64)
     * @see #getFrameAtTime(Int64, Int32)
     */
    CARAPI GetFrameAtTime(
        /* [out] */ IBitmap** bitmap);

    /**
     * Call this method after setDataSource(). This method finds the optional
     * graphic or album art associated associated with the data source. If
     * there are more than one pictures, (any) one of them is returned.
     *
     * @return NULL if no such graphic is found.
     */
    CARAPI GetEmbeddedPicture(
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Call it when one is done with the object. This method releases the memory
     * allocated internally.
     */
    CARAPI ReleaseResources();

protected:
    CARAPI Finalize();

private:
    CARAPI NativeSetDataSource(
        /* [in] */ IBinder* httpServiceBinder,
        /* [in] */ const String& uri,
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<String>* values);

    CARAPI NativeGetFrameAtTime(
        /* [in] */ Int64 timeUs,
        /* [in] */ Int32 option,
        /* [out] */ IBitmap** bitmap);

    CARAPI GetEmbeddedPicture(
        /* [in] */ Int32 pictureType,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI NativeSetup();

    CARAPI_(void) NativeFinalize();

    CARAPI_(android::MediaMetadataRetriever*) GetRetriever();

    CARAPI_(void) SetRetriever(android::MediaMetadataRetriever* retriever);
private:
    static const Int32 EMBEDDED_PICTURE_TYPE_ANY;
    static const String TAG;

    // The field below is accessed by native methods
    android::MediaMetadataRetriever* mContext;
    Object sLock;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif
