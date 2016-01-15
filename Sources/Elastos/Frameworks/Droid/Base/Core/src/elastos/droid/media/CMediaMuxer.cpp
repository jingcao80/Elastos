#include "elastos/droid/media/CMediaMuxer.h"

using Elastos::Core::CCloseGuardHelper;
using Elastos::Core::ICharSequence;
using Elastos::Core::ICloseGuardHelper;
using Elastos::IO::CRandomAccessFile;
using Elastos::IO::IBuffer;
using Elastos::IO::ICloseable;
using Elastos::IO::IRandomAccessFile;
using Elastos::Utility::IMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Media {

const Int32 CMediaMuxer::MUXER_STATE_UNINITIALIZED  = -1;
const Int32 CMediaMuxer::MUXER_STATE_INITIALIZED    = 0;
const Int32 CMediaMuxer::MUXER_STATE_STARTED        = 1;
const Int32 CMediaMuxer::MUXER_STATE_STOPPED        = 2;

CAR_INTERFACE_IMPL(CMediaMuxer, Object, IMediaMuxer)

CAR_OBJECT_IMPL(CMediaMuxer)

CMediaMuxer::CMediaMuxer()
    : mState(MUXER_STATE_UNINITIALIZED)
    , mLastTrackIndex(-1)
    , mNativeObject(0)
{
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
}

CMediaMuxer::~CMediaMuxer()
{
    // try {
    if (mCloseGuard != NULL) {
        mCloseGuard->WarnIfOpen();
    }
    if (mNativeObject != 0) {
        NativeRelease(mNativeObject);
        mNativeObject = 0;
    }
    // } finally {
    //     super.finalize();
    // }
}

ECode CMediaMuxer::constructor()
{
    return NOERROR;
}

ECode CMediaMuxer::constructor(
    /* [in] */ const String& path,
    /* [in] */ Int32 format)
{
    if (path == NULL) {
        // throw new IllegalArgumentException("path must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (format != IMediaMuxerOutputFormat::MUXER_OUTPUT_MPEG_4 &&
            format != IMediaMuxerOutputFormat::MUXER_OUTPUT_WEBM) {
        // throw new IllegalArgumentException("format is invalid");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Use RandomAccessFile so we can open the file with RW access;
    // RW access allows the native writer to memory map the output file.
    AutoPtr<IRandomAccessFile> file;
    // try {
    CRandomAccessFile::New(path, String("rws"), (IRandomAccessFile**)&file);
    AutoPtr<IFileDescriptor> fd;
    file->GetFD((IFileDescriptor**)&fd);
    mNativeObject = NativeSetup(fd, format);
    mState = MUXER_STATE_INITIALIZED;
    mCloseGuard->Open(String("release"));
    // } finally {
    if (file != NULL) {
        ICloseable::Probe(file)->Close();
    }
    // }
    return NOERROR;
}

ECode CMediaMuxer::SetOrientationHint(
    /* [in] */ Int32 degrees)
{
    if (degrees != 0 && degrees != 90  && degrees != 180 && degrees != 270) {
        // throw new IllegalArgumentException("Unsupported angle: " + degrees);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mState == MUXER_STATE_INITIALIZED) {
        NativeSetOrientationHint(mNativeObject, degrees);
    }
    else {
        // throw new IllegalStateException("Can't set rotation degrees due" +
        //         " to wrong state.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaMuxer::SetLocation(
    /* [in] */ Float latitude,
    /* [in] */ Float longitude)
{
    Int32 latitudex10000  = (Int32) (latitude * 10000 + 0.5);
    Int32 longitudex10000 = (Int32) (longitude * 10000 + 0.5);

    if (latitudex10000 > 900000 || latitudex10000 < -900000) {
        String msg = String("Latitude: ") + latitude + " out of range.";
        // throw new IllegalArgumentException(msg);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (longitudex10000 > 1800000 || longitudex10000 < -1800000) {
        String msg = String("Longitude: ") + longitude + " out of range";
        // throw new IllegalArgumentException(msg);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mState == MUXER_STATE_INITIALIZED && mNativeObject != 0) {
        NativeSetLocation(mNativeObject, latitudex10000, longitudex10000);
    }
    else {
        // throw new IllegalStateException("Can't set location due to wrong state.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaMuxer::Start()
{
    if (mNativeObject == 0) {
        // throw new IllegalStateException("Muxer has been released!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mState == MUXER_STATE_INITIALIZED) {
        NativeStart(mNativeObject);
        mState = MUXER_STATE_STARTED;
    }
    else {
        // throw new IllegalStateException("Can't start due to wrong state.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaMuxer::Stop()
{
    if (mState == MUXER_STATE_STARTED) {
        NativeStop(mNativeObject);
        mState = MUXER_STATE_STOPPED;
    }
    else {
        // throw new IllegalStateException("Can't stop due to wrong state.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaMuxer::AddTrack(
    /* [in] */ IMediaFormat* format,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (format == NULL) {
        // throw new IllegalArgumentException("format must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mState != MUXER_STATE_INITIALIZED) {
        // throw new IllegalStateException("Muxer is not initialized.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mNativeObject == 0) {
        // throw new IllegalStateException("Muxer has been released!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 trackIndex = -1;
    // Convert the MediaFormat into key-value pairs and send to the native.
    AutoPtr<IMap> formatMap;
    format->GetMap((IMap**)&formatMap);

    AutoPtr<ArrayOf<String> > keys;
    AutoPtr<ArrayOf<IInterface*> > values;
    Int32 mapSize;
    formatMap->GetSize(&mapSize);
    if (mapSize > 0) {
        keys = ArrayOf<String>::Alloc(mapSize);
        values = ArrayOf<IInterface*>::Alloc(mapSize);
        Int32 i = 0;

        AutoPtr<ISet> entrySet;
        formatMap->GetEntrySet((ISet**)&entrySet);
        AutoPtr<ArrayOf<IInterface*> > array;
        entrySet->ToArray((ArrayOf<IInterface*>**)&array);

        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<IMapEntry> entry;
            entry = IMapEntry::Probe((*array)[i]);
            AutoPtr<IInterface> iKey;
            entry->GetKey((IInterface**)&iKey);
            String key;
            ICharSequence::Probe(iKey)->ToString(&key);
            keys->Set(i, key);
            AutoPtr<IInterface> iValue;
            entry->GetValue((IInterface**)&iValue);
            values->Set(i, iValue);
        }
        trackIndex = NativeAddTrack(mNativeObject, keys, values);
    }
    else {
        // throw new IllegalArgumentException("format must not be empty.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Track index number is expected to incremented as addTrack succeed.
    // However, if format is invalid, it will get a negative trackIndex.
    if (mLastTrackIndex >= trackIndex) {
        // throw new IllegalArgumentException("Invalid format.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mLastTrackIndex = trackIndex;
    *result = trackIndex;
    return NOERROR;
}

ECode CMediaMuxer::WriteSampleData(
    /* [in] */ Int32 trackIndex,
    /* [in] */ IByteBuffer* byteBuf,
    /* [in] */ IMediaCodecBufferInfo* bufferInfo)
{
    if (trackIndex < 0 || trackIndex > mLastTrackIndex) {
        // throw new IllegalArgumentException("trackIndex is invalid");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (byteBuf == NULL) {
        // throw new IllegalArgumentException("byteBuffer must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (bufferInfo == NULL) {
        // throw new IllegalArgumentException("bufferInfo must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 size;
    bufferInfo->GetSize(&size);
    Int32 offset;
    bufferInfo->GetOffset(&offset);
    Int32 flags;
    bufferInfo->GetFlags(&flags);
    Int64 presentationTimeUs;
    bufferInfo->GetPresentationTimeUs(&presentationTimeUs);
    Int32 capacity;
    IBuffer::Probe(byteBuf)->GetCapacity(&capacity);

    if (size < 0 || offset < 0
            || (offset + size) > capacity
            || presentationTimeUs < 0) {
        // throw new IllegalArgumentException("bufferInfo must specify a" +
        //         " valid buffer offset, size and presentation time");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mNativeObject == 0) {
        // throw new IllegalStateException("Muxer has been released!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mState != MUXER_STATE_STARTED) {
        // throw new IllegalStateException("Can't write, muxer is not started");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    NativeWriteSampleData(mNativeObject, trackIndex, byteBuf,
            offset, size,
            presentationTimeUs, flags);
}

ECode CMediaMuxer::ReleaseResources()
{
    if (mState == MUXER_STATE_STARTED) {
        Stop();
    }
    if (mNativeObject != 0) {
        NativeRelease(mNativeObject);
        mNativeObject = 0;
        mCloseGuard->Close();
    }
    mState = MUXER_STATE_UNINITIALIZED;
    return NOERROR;
}

// TODO: Need jni code
Int64 CMediaMuxer::NativeSetup(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 format)
{
    // int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
    // ALOGV("native_setup: fd %d", fd);

    // MediaMuxer::OutputFormat fileFormat =
    //     static_cast<MediaMuxer::OutputFormat>(format);
    // sp<MediaMuxer> muxer = new MediaMuxer(fd, fileFormat);
    // muxer->incStrong(clazz);
    // return reinterpret_cast<jlong>(muxer.get());
}

void CMediaMuxer::NativeRelease(
    /* [in] */ Int64 nativeObject)
{
    // sp<MediaMuxer> muxer(reinterpret_cast<MediaMuxer *>(nativeObject));
    // if (muxer != NULL) {
    //     muxer->decStrong(clazz);
    // }
}

void CMediaMuxer::NativeStart(
    /* [in] */ Int64 nativeObject)
{
    // sp<MediaMuxer> muxer(reinterpret_cast<MediaMuxer *>(nativeObject));
    // if (muxer == NULL) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Muxer was not set up correctly");
    //     return;
    // }
    // status_t err = muxer->start();

    // if (err != OK) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Failed to start the muxer");
    //     return;
    // }
}

void CMediaMuxer::NativeStop(
    /* [in] */ Int64 nativeObject)
{
    // sp<MediaMuxer> muxer(reinterpret_cast<MediaMuxer *>(nativeObject));
    // if (muxer == NULL) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Muxer was not set up correctly");
    //     return;
    // }

    // status_t err = muxer->stop();

    // if (err != OK) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Failed to stop the muxer");
    //     return;
    // }
}

Int32 CMediaMuxer::NativeAddTrack(
    /* [in] */ Int64 nativeObject,
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    // sp<MediaMuxer> muxer(reinterpret_cast<MediaMuxer *>(nativeObject));
    // if (muxer == NULL) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Muxer was not set up correctly");
    //     return -1;
    // }

    // sp<AMessage> trackformat;
    // status_t err = ConvertKeyValueArraysToMessage(env, keys, values,
    //                                               &trackformat);
    // if (err != OK) {
    //     jniThrowException(env, "java/lang/IllegalArgumentException",
    //                       "ConvertKeyValueArraysToMessage got an error");
    //     return err;
    // }

    // // Return negative value when errors happen in addTrack.
    // jint trackIndex = muxer->addTrack(trackformat);

    // if (trackIndex < 0) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Failed to add the track to the muxer");
    //     return -1;
    // }
    // return trackIndex;
}

void CMediaMuxer::NativeSetOrientationHint(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 degrees)
{
    // sp<MediaMuxer> muxer(reinterpret_cast<MediaMuxer *>(nativeObject));
    // if (muxer == NULL) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Muxer was not set up correctly");
    //     return;
    // }
    // status_t err = muxer->setOrientationHint(degrees);

    // if (err != OK) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Failed to set orientation hint");
    //     return;
    // }
}

void CMediaMuxer::NativeSetLocation(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 latitude,
    /* [in] */ Int32 longitude)
{
    // MediaMuxer* muxer = reinterpret_cast<MediaMuxer *>(nativeObject);

    // status_t res = muxer->setLocation(latitude, longitude);
    // if (res != OK) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Failed to set location");
    //     return;
    // }
}

void CMediaMuxer::NativeWriteSampleData(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 trackIndex,
    /* [in] */ IByteBuffer* byteBuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ Int64 presentationTimeUs,
    /* [in] */ Int32 flags)
{
    // sp<MediaMuxer> muxer(reinterpret_cast<MediaMuxer *>(nativeObject));
    // if (muxer == NULL) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "Muxer was not set up correctly");
    //     return;
    // }

    // // Try to convert the incoming byteBuffer into ABuffer
    // void *dst = env->GetDirectBufferAddress(byteBuf);

    // jlong dstSize;
    // jbyteArray byteArray = NULL;

    // if (dst == NULL) {

    //     byteArray =
    //         (jbyteArray)env->CallObjectMethod(byteBuf, gFields.arrayID);

    //     if (byteArray == NULL) {
    //         jniThrowException(env, "java/lang/IllegalArgumentException",
    //                           "byteArray is null");
    //         return;
    //     }

    //     jboolean isCopy;
    //     dst = env->GetByteArrayElements(byteArray, &isCopy);

    //     dstSize = env->GetArrayLength(byteArray);
    // } else {
    //     dstSize = env->GetDirectBufferCapacity(byteBuf);
    // }

    // if (dstSize < (offset + size)) {
    //     ALOGE("writeSampleData saw wrong dstSize %lld, size  %d, offset %d",
    //           dstSize, size, offset);
    //     if (byteArray != NULL) {
    //         env->ReleaseByteArrayElements(byteArray, (jbyte *)dst, 0);
    //     }
    //     jniThrowException(env, "java/lang/IllegalArgumentException",
    //                       "sample has a wrong size");
    //     return;
    // }

    // sp<ABuffer> buffer = new ABuffer((char *)dst + offset, size);

    // status_t err = muxer->writeSampleData(buffer, trackIndex, timeUs, flags);

    // if (byteArray != NULL) {
    //     env->ReleaseByteArrayElements(byteArray, (jbyte *)dst, 0);
    // }

    // if (err != OK) {
    //     jniThrowException(env, "java/lang/IllegalStateException",
    //                       "writeSampleData returned an error");
    // }
    // return;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
