#include "elastos/droid/media/CMediaMuxer.h"
#include "elastos/droid/media/Media_Utils.h"
#include <elastos/utility/logging/Logger.h>

#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaMuxer.h>

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
using Elastos::Utility::Logging::Logger;

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
    mCloseGuard->Open(String("CMediaMuxer::Release"));
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
        Logger::E("CMediaMuxer", "latitude: %.2f out of range.", latitude);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (longitudex10000 > 1800000 || longitudex10000 < -1800000) {
        Logger::E("CMediaMuxer", "Longitude: %.2f out of range.", longitude);
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
        NativeAddTrack(mNativeObject, keys, values, &trackIndex);
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
    return NOERROR;
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

Int64 CMediaMuxer::NativeSetup(
    /* [in] */ IFileDescriptor* fileDes,
    /* [in] */ Int32 format)
{
    Int32 fd;
    fileDes->GetDescriptor(&fd);
    ALOGV("native_setup: fd %d", fd);

    android::MediaMuxer::OutputFormat fileFormat =
        static_cast<android::MediaMuxer::OutputFormat>(format);
    android::sp<android::MediaMuxer> muxer = new android::MediaMuxer(fd, fileFormat);
    return reinterpret_cast<Int64>(muxer.get());
}

void CMediaMuxer::NativeRelease(
    /* [in] */ Int64 nativeObject)
{
}

ECode CMediaMuxer::NativeStart(
    /* [in] */ Int64 nativeObject)
{
    android::sp<android::MediaMuxer> muxer(
            reinterpret_cast<android::MediaMuxer *>(nativeObject));
    if (muxer == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "Muxer was not set up correctly");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    android::status_t err = muxer->start();

    if (err != android::OK) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "Failed to start the muxer");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaMuxer::NativeStop(
    /* [in] */ Int64 nativeObject)
{
    android::sp<android::MediaMuxer> muxer(
            reinterpret_cast<android::MediaMuxer *>(nativeObject));
    if (muxer == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "Muxer was not set up correctly");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::status_t err = muxer->stop();

    if (err != android::OK) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "Failed to start the muxer");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaMuxer::NativeAddTrack(
    /* [in] */ Int64 nativeObject,
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<IInterface*>* values,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    android::sp<android::MediaMuxer> muxer(
            reinterpret_cast<android::MediaMuxer *>(nativeObject));
    if (muxer == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "Muxer was not set up correctly");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::sp<android::AMessage> trackformat;
    android::status_t err = Media_Utils::ConvertKeyValueArraysToMessage(keys, values, &trackformat);
    if (err != android::OK) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "ConvertKeyValueArraysToMessage got an error");
        *result = err;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Return negative value when errors happen in addTrack.
    Int32 trackIndex = muxer->addTrack(trackformat);

    if (trackIndex < 0) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "Failed to add the track to the muxer");
        *result = -1;
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = trackIndex;
    return NOERROR;
}

ECode CMediaMuxer::NativeSetOrientationHint(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 degrees)
{
    android::sp<android::MediaMuxer> muxer(
            reinterpret_cast<android::MediaMuxer *>(nativeObject));
    if (muxer == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "Muxer was not set up correctly");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    android::status_t err = muxer->setOrientationHint(degrees);

    if (err != android::OK) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "Failed to set orientation hint");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaMuxer::NativeSetLocation(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 latitude,
    /* [in] */ Int32 longitude)
{
    android::sp<android::MediaMuxer> muxer(
            reinterpret_cast<android::MediaMuxer *>(nativeObject));

    android::status_t res = muxer->setLocation(latitude, longitude);
    if (res != android::OK) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "Failed to set location");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaMuxer::NativeWriteSampleData(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 trackIndex,
    /* [in] */ IByteBuffer* byteBuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [in] */ Int64 timeUs,
    /* [in] */ Int32 flags)
{
    android::sp<android::MediaMuxer> muxer(
            reinterpret_cast<android::MediaMuxer *>(nativeObject));
    if (muxer == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "Muxer was not set up correctly");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // Try to convert the incoming byteBuffer into ABuffer
    Handle64 addr;
    IBuffer::Probe(byteBuf)->GetEffectiveDirectAddress(&addr);
    void *dst = (void*)addr;

    Int64 dstSize;
    AutoPtr<ArrayOf<Byte> > byteArray;

    if (dst == NULL) {

        byteBuf->GetArray((ArrayOf<Byte>**)&byteArray);

        if (byteArray == NULL) {
            // jniThrowException(env, "java/lang/IllegalArgumentException",
            //                   "byteArray is null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        dst = byteArray->GetPayload();

        dstSize = byteArray->GetLength();
    }
    else {
        Int32 size;
        IBuffer::Probe(byteBuf)->GetCapacity(&size);
        dstSize = size;
    }

    if (dstSize < (offset + size)) {
        ALOGE("writeSampleData saw wrong dstSize %lld, size  %d, offset %d",
              dstSize, size, offset);
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "sample has a wrong size");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<android::ABuffer> buffer;
    buffer = new android::ABuffer((char *)dst + offset, size);

    android::status_t err = muxer->writeSampleData(buffer, trackIndex, timeUs, flags);

    if (err != android::OK) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "writeSampleData returned an error");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
