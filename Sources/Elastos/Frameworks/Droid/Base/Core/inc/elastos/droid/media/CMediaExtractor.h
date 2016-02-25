
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAEXTRACTOR_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAEXTRACTOR_H__

#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Media_CMediaExtractor.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

#include <media/stagefright/DataSource.h>
#include <media/stagefright/NuMediaExtractor.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Media::IDataSource;
using Elastos::Droid::Media::IMediaCodecCryptoInfo;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Media {

class JavaDataSourceBridge : public android::DataSource {
public:
    AutoPtr<IDataSource> mDataSource;

    JavaDataSourceBridge(
        /* [in] */ IDataSource* dataSource);

    ~JavaDataSourceBridge();

    virtual android::status_t initCheck() const;

    virtual ssize_t readAt(off64_t offset, void* buffer, size_t size);

    virtual android::status_t getSize(off64_t *size);
};

/**
 * MediaExtractor facilitates extraction of demuxed, typically encoded,  media data
 * from a data source.
 * <p>It is generally used like this:
 * <pre>
 * MediaExtractor extractor = new MediaExtractor();
 * extractor.setDataSource(...);
 * int numTracks = extractor.getTrackCount();
 * for (int i = 0; i &lt; numTracks; ++i) {
 *   MediaFormat format = extractor.getTrackFormat(i);
 *   String mime = format.getString(MediaFormat.KEY_MIME);
 *   if (weAreInterestedInThisTrack) {
 *     extractor.selectTrack(i);
 *   }
 * }
 * ByteBuffer inputBuffer = ByteBuffer.allocate(...)
 * while (extractor.readSampleData(inputBuffer, ...) &gt;= 0) {
 *   int trackIndex = extractor.getSampleTrackIndex();
 *   long presentationTimeUs = extractor.getSampleTime();
 *   ...
 *   extractor.advance();
 * }
 *
 * extractor.release();
 * extractor = null;
 * </pre>
 */
CarClass(CMediaExtractor)
    , public Object
    , public IMediaExtractor
{
public:
    CMediaExtractor();

    virtual ~CMediaExtractor();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Sets the DataSource object to be used as the data source for this extractor
     * {@hide}
     */
    CARAPI SetDataSource(
        /* [in] */ IDataSource* source);

    /**
     * Sets the data source as a content Uri.
     *
     * @param context the Context to use when resolving the Uri
     * @param uri the Content URI of the data you want to extract from.
     * @param headers the headers to be sent together with the request for the data
     */
    CARAPI SetDataSource(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ IMap* headers);

    /**
     * Sets the data source (file-path or http URL) to use.
     *
     * @param path the path of the file, or the http URL
     * @param headers the headers associated with the http request for the stream you want to play
     */
    CARAPI SetDataSource(
        /* [in] */ const String& path,
        /* [in] */ IMap* headers);

    /**
     * Sets the data source (file-path or http URL) to use.
     *
     * @param path the path of the file, or the http URL of the stream
     *
     * <p>When <code>path</code> refers to a local file, the file may actually be opened by a
     * process other than the calling application.  This implies that the pathname
     * should be an absolute path (as any other process runs with unspecified current working
     * directory), and that the pathname should reference a world-readable file.
     * As an alternative, the application could first open the file for reading,
     * and then use the file descriptor form {@link #setDataSource(FileDescriptor)}.
     */
    CARAPI SetDataSource(
        /* [in] */ const String& path);

    /**
     * Sets the data source (FileDescriptor) to use. It is the caller's responsibility
     * to close the file descriptor. It is safe to do so as soon as this call returns.
     *
     * @param fd the FileDescriptor for the file you want to extract from.
     */
    CARAPI SetDataSource(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Sets the data source (FileDescriptor) to use.  The FileDescriptor must be
     * seekable (N.B. a LocalSocket is not seekable). It is the caller's responsibility
     * to close the file descriptor. It is safe to do so as soon as this call returns.
     *
     * @param fd the FileDescriptor for the file you want to extract from.
     * @param offset the offset into the file where the data to be extracted starts, in bytes
     * @param length the length in bytes of the data to be extracted
     */
    CARAPI SetDataSource(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    /**
     * Make sure you call this when you're done to free up any resources
     * instead of relying on the garbage collector to do this for you at
     * some point in the future.
     */
    CARAPI ReleaseResources();

    /**
     * Count the number of tracks found in the data source.
     */
    CARAPI GetTrackCount(
        /* [out] */ Int32* result);

    /**
     * Get the PSSH info if present.
     * @return a map of uuid-to-bytes, with the uuid specifying
     * the crypto scheme, and the bytes being the data specific to that scheme.
     */
    CARAPI GetPsshInfo(
        /* [out] */ IMap** result);

    /**
     * Get the track format at the specified index.
     * More detail on the representation can be found at {@link android.media.MediaCodec}
     */
    CARAPI GetTrackFormat(
        /* [in] */ Int32 index,
        /* [out] */ IMediaFormat** result);

    /**
     * Subsequent calls to {@link #readSampleData}, {@link #getSampleTrackIndex} and
     * {@link #getSampleTime} only retrieve information for the subset of tracks
     * selected.
     * Selecting the same track multiple times has no effect, the track is
     * only selected once.
     */
    CARAPI SelectTrack(
        /* [in] */ Int32 index);

    /**
     * Subsequent calls to {@link #readSampleData}, {@link #getSampleTrackIndex} and
     * {@link #getSampleTime} only retrieve information for the subset of tracks
     * selected.
     */
    CARAPI UnselectTrack(
        /* [in] */ Int32 index);

    /**
     * All selected tracks seek near the requested time according to the
     * specified mode.
     */
    CARAPI SeekTo(
        /* [in] */ Int64 timeUs,
        /* [in] */ Int32 mode);

    /**
     * Advance to the next sample. Returns false if no more sample data
     * is available (end of stream).
     */
    CARAPI Advance(
        /* [out] */ Boolean* result);

    /**
     * Retrieve the current encoded sample and store it in the byte buffer
     * starting at the given offset. Returns the sample size (or -1 if
     * no more samples are available).
     */
    CARAPI ReadSampleData(
        /* [in] */ IByteBuffer* byteBuf,
        /* [in] */ Int32 offset,
        /* [out] */ Int32* result);

    /**
     * Returns the track index the current sample originates from (or -1
     * if no more samples are available)
     */
    CARAPI GetSampleTrackIndex(
        /* [out] */ Int32* result);

    /**
     * Returns the current sample's presentation time in microseconds.
     * or -1 if no more samples are available.
     */
    CARAPI GetSampleTime(
        /* [out] */ Int64* result);

    /**
     * Returns the current sample's flags.
     */
    CARAPI GetSampleFlags(
        /* [out] */ Int32* result);

    /**
     * If the sample flags indicate that the current sample is at least
     * partially encrypted, this call returns relevant information about
     * the structure of the sample data required for decryption.
     * @param info The android.media.MediaCodec.CryptoInfo structure
     *             to be filled in.
     * @return true iff the sample flags contain {@link #SAMPLE_FLAG_ENCRYPTED}
     */
    CARAPI GetSampleCryptoInfo(
        /* [in] */ IMediaCodecCryptoInfo* info,
        /* [out] */ Boolean* result);

    /**
     * Returns an estimate of how much data is presently cached in memory
     * expressed in microseconds. Returns -1 if that information is unavailable
     * or not applicable (no cache).
     */
    CARAPI GetCachedDuration(
        /* [out] */ Int64* result);

    /**
     * Returns true iff we are caching data and the cache has reached the
     * end of the data stream (for now, a future seek may of course restart
     * the fetching of data).
     * This API only returns a meaningful result if {@link #getCachedDuration}
     * indicates the presence of a cache, i.e. does NOT return -1.
     */
    CARAPI HasCacheReachedEndOfStream(
        /* [out] */ Boolean* result);

private:
    CARAPI NativeSetDataSource(
        /* [in] */ IBinder* httpServiceBinder,
        /* [in] */ const String& path,
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<String>* values);

    // CARAPI NativeSetDataSource(
    //     /* [in] */ const android::sp<android::IMediaHTTPService> &httpService,
    //     /* [in] */ const String& path,
    //     /* [in] */ android::KeyedVector<android::String8,android::String8>* headers);

    // CARAPI NativeSetDataSource(
    //     /* [in] */ Int32 fd,
    //     /* [in] */ Int64 offset,
    //     /* [in] */ Int64 size);

    CARAPI GetFileFormatNative(
        /* [out] */ IMap** result);

    CARAPI GetTrackFormatNative(
        /* [in] */ Int32 index,
        /* [out] */ IMap** result);

    CARAPI NativeSetup();

    CARAPI NativeFinalize();

    static android::sp<android::NuMediaExtractor> SetMediaExtractor(
        const android::sp<android::NuMediaExtractor>& newME);

    static android::sp<android::NuMediaExtractor> GetMediaExtractor();

private:
    static android::sp<android::NuMediaExtractor> mImpl;
    static const String TAG;
    static const Boolean INIT;
    Int64 mNativeContext;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIAEXTRACTOR_H__
