#include "elastos/droid/media/CMediaExtractor.h"
#include "elastos/droid/media/CMediaFormat.h"
#include <elastos/utility/logging/Logger.h>
#include <media/Media_Utils.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaSource.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::ByteOrder;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::IBuffer;
using Elastos::Utility::CUUID;
using Elastos::Utility::IUUID;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaExtractor, Object, IMediaExtractor)

CAR_OBJECT_IMPL(CMediaExtractor)

//==========================================================================
//                  JavaDataSourceBridge
//==========================================================================
JavaDataSourceBridge::JavaDataSourceBridge(
    /* [in] */ IDataSource* dataSource)
    : mDataSource(dataSource)
{
}

JavaDataSourceBridge::~JavaDataSourceBridge()
{
    if (mDataSource != NULL) {
        mDataSource->Close();
        mDataSource = NULL;
    }
}

android::status_t JavaDataSourceBridge::initCheck() const
{
    return android::OK;
}

ssize_t JavaDataSourceBridge::readAt(
    off64_t offset, void* buffer, size_t size)
{
    AutoPtr<ArrayOf<Byte> > byteArrayObj = ArrayOf<Byte>::Alloc(size);
    Int32 numread;
    assert(mDataSource != NULL);
    mDataSource->ReadAt(offset, byteArrayObj, size, &numread);
    memcpy(buffer, byteArrayObj->GetPayload(), size);
    return numread;
}

android::status_t JavaDataSourceBridge::getSize(
    off64_t *size)
{
    int64_t len;
    mDataSource->GetSize(&len);
    if (len < 0) {
        *size = android::ERROR_UNSUPPORTED;
    }
    else {
        *size = len;
    }
    return android::OK;
}

//==========================================================================
//                  CMediaExtractor
//==========================================================================
const String CMediaExtractor::TAG("CMediaExtractor");

Boolean NativeInit()
{
    android::DataSource::RegisterDefaultSniffers();
    return TRUE;
}

const Boolean CMediaExtractor::INIT = NativeInit();

android::sp<android::NuMediaExtractor> CMediaExtractor::mImpl;

android::sp<android::NuMediaExtractor> CMediaExtractor::SetMediaExtractor(
    const android::sp<android::NuMediaExtractor>& newME)
{
    android::sp<android::NuMediaExtractor>& old = mImpl;
    mImpl = newME;
    return old;
}

android::sp<android::NuMediaExtractor> CMediaExtractor::GetMediaExtractor()
{
    return mImpl;
}

ECode CMediaExtractor::constructor()
{
    return NativeSetup();
}

ECode CMediaExtractor::SetDataSource(
    /* [in] */ IDataSource* source)
{
    if (source == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<JavaDataSourceBridge> bridge = new JavaDataSourceBridge(source);
    android::status_t err = mImpl->setDataSource(bridge);

    if (err != android::OK) {
        Logger::E(TAG, "Failed to instantiate extractor.");
       return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaExtractor::SetDataSource(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [in] */ IMap* headers)
{
    VALIDATE_NOT_NULL(uri);

    String scheme;
    uri->GetScheme(&scheme);
    if(scheme.IsNull() || scheme.Equals("file")) {
        String path;
        uri->GetPath(&path);
        return SetDataSource(path);
    }

    AutoPtr<IAssetFileDescriptor> fd;
    //try {
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    ECode ec = resolver->OpenAssetFileDescriptor(uri, String("r"), (IAssetFileDescriptor**)&fd);
    if (fd == NULL) {
        return NOERROR;
    }

    // Note: using getDeclaredLength so that our behavior is the same
    // as previous versions when the content provider is returning
    // a full file.
    Int64 intVal, intVal2;
    String tempString;
    AutoPtr <IFileDescriptor> tmpFd;

    FAIL_GOTO(fd->GetDeclaredLength(&intVal), _EXIT_);
    if (intVal < 0) {
        FAIL_GOTO(fd->GetFileDescriptor((IFileDescriptor**)&tmpFd), _EXIT_);
        ec = SetDataSource(tmpFd);
    }
    else {
        FAIL_GOTO(fd->GetFileDescriptor((IFileDescriptor**)&tmpFd), _EXIT_);
        FAIL_GOTO(fd->GetStartOffset(&intVal), _EXIT_);
        FAIL_GOTO(fd->GetDeclaredLength(&intVal2), _EXIT_);

        ec = SetDataSource(tmpFd, intVal, intVal2);
    }

_EXIT_:
    if (fd != 0) {
        fd->Close();
        fd = NULL;
    }

    if (SUCCEEDED(ec)) {
        return ec;
    }

    uri->ToString(&tempString);
    return SetDataSource(tempString, headers);
}

ECode CMediaExtractor::SetDataSource(
    /* [in] */ const String& path,
    /* [in] */ IMap* headers)
{
    AutoPtr<ArrayOf<String> > keys;
    AutoPtr<ArrayOf<String> > values;

    if (headers != NULL) {
        headers->GetKeys( (ArrayOf<String>**)&keys);
        if (keys != NULL) {
            Int32 keylen = keys->GetLength();
            values = ArrayOf<String>::Alloc(keylen);

            ICharSequence* seq;
            String str;
            for (Int32 i = 0; i < keylen; i++) {
                AutoPtr<IInterface> tmpObj;
                headers->Get((*keys)[i], (IInterface**)&tmpObj);
                seq = ICharSequence::Probe(tmpObj.Get());
                if (seq) {
                    seq->ToString(&str);
                    values->Set(i, str);
                }
            }
        }
    }

    AutoPtr<IBinder> binder;
    MediaHTTPService.createHttpServiceBinderIfNecessary(
            path, (IBinder**)&binder);
    return NativeSetDataSource(
        binder, path, keys, values);
}

ECode CMediaExtractor::NativeSetDataSource(
    /* [in] */ const String& path,
    /* [in] */ android::KeyedVector<android::String8,android::String8>* headers)
{
    if (path.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::status_t err = mImpl->setDataSource(path.string(), headers);
    if (err != android::OK) {
        Logger::E(TAG, "Failed to instantiate extractor.");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode  CMediaExtractor::NativeSetDataSource(
    /* [in] */ Int32 fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 size)
{
    android::status_t err = mImpl->setDataSource(fd, offset, size);
    if (err != android::OK) {
        Logger::E(TAG, "Failed to instantiate extractor.");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode  CMediaExtractor::NativeSetDataSource(
    /* [in] */ IBinder* httpServiceBinder,
    /* [in] */ const String& path,
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<String>* values)
{
    android::KeyedVector<android::String8, android::String8> headers;
    FAIL_RETURN(Media_Utils::ConvertKeyValueArraysToKeyedVector(keys, values, &headers));

    return NativeSetDataSource(path, &headers);
}

ECode CMediaExtractor::SetDataSource(
    /* [in] */ const String& path)
{
    AutoPtr<IBinder> binder;
    MediaHTTPService::CreateHttpServiceBinderIfNecessary(
                path, (IBinder**)&binder);
    return nativeSetDataSource(binder, path, NULL, NULL);
}

ECode CMediaExtractor::SetDataSource(
    /* [in] */ IFileDescriptor* fd)
{
    return SetDataSource(fd, 0, 0x7ffffffffffffffL);
}

//native code
ECode CMediaExtractor::SetDataSource(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    VALIDATE_NOT_NULL(fd);
    Int32 tempfd;
    fd->GetDescriptor(&tempfd);
    return NativeSetDataSource(tempfd, offset, length);
}

ECode  CMediaExtractor::Finalize()
{
    return NativeFinalize();
}

ECode CMediaExtractor::ReleaseResources()
{
    SetMediaExtractor(NULL);
    return NOERROR;
}

ECode CMediaExtractor::GetTrackCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mImpl->countTracks();
    return NOERROR;
}

ECode CMediaExtractor::GetTrackCount(
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IHashMap> psshMap;
    AutoPtr<IMap> formatMap = GetFileFormatNative();

    AutoPtr<ICharSequence> cs;
    CString::New("pssh", (ICharSequence**)&cs);
    Boolean b;
    if (formatMap != NULL && (formatMap->ContainsKey(cs, &b), b)) {
        AutoPtr<IInterface> obj;
        formatMap->Get(cs, (IInterface**)&obj);
        AutoPtr<IByteBuffer> rawpssh = IByteBuffer::Probe(obj);

        AutoPtr<IByteOrderHelper> orderHelper;
        CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
        ByteOrder nativeOrder;
        orderHelper->GetNativeOrder(&nativeOrder);

        rawpssh->SetOrder(nativeOrder);
        rawpssh->Rewind();
        formatMap->Remove(cs);

        // parse the flat pssh bytebuffer into something more manageable
        CHashMap::New((IHashMap**)&psshMap);
        Int32 remaining;
        rawpssh->GetRemaining(&remaining);
        while (remaining > 0) {
            rawpssh->SetOrder(Elastos::IO::ByteOrder_BIG_ENDIAN);
            Int64 msb;
            rawpssh->GetInt64(&msb);
            Int64 lsb;
            rawpssh->GetInt64(&lsb);
            AutoPtr<IUUID> uuid;
            CUUID::New(msb, lsb, (IUUID**)&uuid);
            rawpssh->SetOrder(nativeOrder);
            Int32 datalen;
            rawpssh->GetInt32(&datalen);
            AutoPtr<ArrayOf<Byte> > psshdata = ArrayOf<Byte>::Alloc(datalen);
            rawpssh->GetArray((ArrayOf<Byte>**)&psshdata);
            psshMap->Put(uuid, psshdata);
        }
    }
    return psshMap;
}

ECode CMediaExtractor::GetTrackFormat(
    /* [in] */ Int32 index,
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IMap> tiMap;
    tiMap = GetTrackFormatNative(index);

    return CMediaFormat::New(tiMap, result);
}

AutoPtr<IMap> CMediaExtractor::GetFileFormatNative()
{
    android::sp<android::AMessage> msg;
    mImpl->getFileFormat(&msg);

    //convert message to map
    AutoPtr<IMap> format;
    Media_Utils::ConvertMessageToMap(msg, (IMap**)&format);

    return msgMap;
}

AutoPtr<IMap> CMediaExtractor::GetTrackFormatNative(
    /* [in] */ Int32 index)
{
    android::sp<android::AMessage> msg;
    mImpl->getTrackFormat((ssize_t)index, &msg);

    //convert message to map
    AutoPtr<IMap> msgMap;
    Media_Utils::ConvertMessageToMap(msg, (IMap**)&msgMap);

    return msgMap;
}

ECode CMediaExtractor::SelectTrack(
    /* [in] */ Int32 index)
{
     mImpl->selectTrack(index);
     return NOERROR;
}

ECode CMediaExtractor::UnselectTrack(
    /* [in] */ Int32 index)
{
    mImpl->unselectTrack(index);
    return NOERROR;
}

ECode CMediaExtractor::SeekTo(
    /* [in] */ Int64 timeUs,
    /* [in] */ Int32 mode)
{
    if (mode< android::MediaSource::ReadOptions::SEEK_PREVIOUS_SYNC
        || mode >= android::MediaSource::ReadOptions::SEEK_CLOSEST) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

     mImpl->seekTo(timeUs, (android::MediaSource::ReadOptions::SeekMode)mode);
     return NOERROR;
}

ECode CMediaExtractor::Advance(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

     android::status_t err = mImpl->advance();
     *result = TRUE;
     if (err == android::ERROR_END_OF_STREAM)
         *result = FALSE;
     else if (err != android::OK)
         *result = FALSE;

    return NOERROR;
}

ECode CMediaExtractor::ReadSampleData(
    /* [in] */ IByteBuffer* byteBuf,
    /* [in] */ Int32 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    VALIDATE_NOT_NULL(byteBuf);

    AutoPtr<ArrayOf<Byte> > byteArray;
    byteBuf->GetArray((ArrayOf<Byte>**)&byteArray);
    if (byteArray == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int64 dstSize = byteArray->GetLength();
    if (dstSize < offset) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::sp<android::ABuffer> buffer = new android::ABuffer(
        (char *)(byteArray->GetPayload()) + offset, dstSize - offset);
    android::status_t err = mImpl->readSampleData(buffer);

    if (err == android::ERROR_END_OF_STREAM) {
        *result = -1;
        return NOERROR;
    }
    else if (err != android::OK) {
        *result = FALSE;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = buffer->size();
    return NOERROR;
}

ECode CMediaExtractor::GetSampleTrackIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    size_t aresult;
    android::status_t err = mImpl->getSampleTrackIndex(&aresult);

    if (err == android::ERROR_END_OF_STREAM) {
        *result = -1;
        return NOERROR;
    }
    else if (err != android::OK) {
        Logger::E(TAG, "CMediaExtractor::GetSampleTrackIndex error");
        *result = FALSE;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = aresult;
    return NOERROR;
}

ECode CMediaExtractor::GetSampleTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    android::status_t err = mImpl->getSampleTime(result);
    if (err == android::ERROR_END_OF_STREAM) {
        *result = -1ll;
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else if (err != android::OK){
        Logger::E(TAG, "CMediaExtractor::GetSampleTime error");
        *result = FALSE;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode CMediaExtractor::GetSampleFlags(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    android::status_t err;
    android::sp<android::MetaData> meta;

    UInt32 type;
    const void *data;
    size_t size;
    Int32 val;
    UInt32 sampleFlags = 0;

    err = mImpl->getSampleMeta(&meta);
    if (err != android::OK) {
        goto _EXIT_;
    }

    if ( meta->findInt32(android::kKeyIsSyncFrame, &val) && val != 0){
        (sampleFlags) |= android::NuMediaExtractor::SAMPLE_FLAG_SYNC;
    }

    if (meta->findData(android::kKeyEncryptedSizes, &type, &data, &size)) {
        (sampleFlags) |= android::NuMediaExtractor::SAMPLE_FLAG_ENCRYPTED;
    }

    *result = sampleFlags;
    return NOERROR;

_EXIT_:
    if (err == android::ERROR_END_OF_STREAM) {
        *result = -1ll;
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else if (err != android::OK){
        Logger::E(TAG, "CMediaExtractor::GetSampleFlags error");
        *result = FALSE;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode CMediaExtractor::GetSampleCryptoInfo(
    /* [in] */ IMediaCodecCryptoInfo* info,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(info);

    android::sp<android::MetaData> meta;
    android::status_t err = mImpl->getSampleMeta(&meta);
    if (err != android::OK) {
        return NOERROR;
    }

    UInt32 type;
    const void *data;
    size_t size;
    if (!meta->findData(android::kKeyEncryptedSizes, &type, &data, &size)) {
        return NOERROR;
    }

    size_t numSubSamples = size / sizeof(size_t);
    if (numSubSamples == 0) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > numBytesOfEncryptedDataObj = ArrayOf<Int32>::Alloc(numSubSamples);
    for (size_t i = 0; i < numSubSamples; ++i) {
        numBytesOfEncryptedDataObj->Set(i, ((const size_t *)data)[i]);
    }

    size_t encSize = size;
    AutoPtr<ArrayOf<Int32> > numBytesOfPlainDataObj;
    if (meta->findData(android::kKeyPlainSizes, &type, &data, &size)) {
        if (size != encSize) {
            // The two must be of the same length.
            return NOERROR;
        }

        numBytesOfPlainDataObj = ArrayOf<Int32>::Alloc(numSubSamples);
        for (size_t i = 0; i < numSubSamples; ++i) {
            numBytesOfPlainDataObj->Set(i, ((const size_t *)data)[i]);
        }
    }

    AutoPtr<ArrayOf<Byte> > keyObj;
    if (meta->findData(android::kKeyCryptoKey, &type, &data, &size)) {
        if (size != 16) {
            // Keys must be 16 bytes in length.
           return NOERROR;
        }

        keyObj = ArrayOf<Byte>::Alloc(size);
        memcpy(keyObj->GetPayload(), data, size);
    }

    AutoPtr<ArrayOf<Byte> > ivObj;
    if (meta->findData(android::kKeyCryptoIV, &type, &data, &size)) {
        if (size != 16) {
            // IVs must be 16 bytes in length.
           return NOERROR;
        }

        ivObj = ArrayOf<Byte>::Alloc(size);
        memcpy(ivObj->GetPayload(), data, size);
    }

    Int32 mode;
    if (!meta->findInt32(android::kKeyCryptoMode, &mode)) {
        mode = android::CryptoPlugin::kMode_AES_CTR;
    }

    info->Set(numSubSamples, numBytesOfPlainDataObj,
        numBytesOfEncryptedDataObj, keyObj, ivObj,mode);
    *result = TRUE;

    return NOERROR;
}

ECode CMediaExtractor::GetCachedDuration(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    Int64 cachedDurationUs;
    bool eos;
    android::status_t err = mImpl->getCachedDuration(&cachedDurationUs, &eos);
    if (!err){
        *result = -111;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = cachedDurationUs;
    return NOERROR;
}

ECode CMediaExtractor::HasCacheReachedEndOfStream(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    int64_t cachedDurationUs;
    bool eos;
    if (!mImpl->getCachedDuration(&cachedDurationUs, &eos)){
        *result = TRUE;
        return NOERROR;
    }

    *result = eos;
    return NOERROR;
}

ECode CMediaExtractor::NativeSetup()
{
    android::sp<android::NuMediaExtractor> extractor = new struct android::NuMediaExtractor;
    SetMediaExtractor(extractor);
    return NOERROR;
}

ECode CMediaExtractor::NativeFinalize()
{
    SetMediaExtractor(NULL);
    return NOERROR;
}


} // namespace Media
} // namepsace Droid
} // namespace Elastos
