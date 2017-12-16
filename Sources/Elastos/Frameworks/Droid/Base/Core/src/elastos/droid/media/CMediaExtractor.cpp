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

#include "elastos/droid/media/CMediaExtractor.h"
#include "elastos/droid/media/CMediaFormat.h"
#include "elastos/droid/media/CMediaHTTPService.h"
#include "elastos/droid/media/Media_Utils.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/NativeBinder.h"
#include <elastos/utility/logging/Logger.h>

#include <media/IMediaHTTPService.h>
#include <media/hardware/CryptoAPI.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/MetaData.h>
#include <utils/KeyedVector.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Os::IBinderForDroidObject;
using Elastos::Droid::Net::Uri;
using Elastos::Core::CArrayOf;
using Elastos::Core::CByte;
using Elastos::Core::CString;
using Elastos::Core::EIID_IByte;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::ICharSequence;
using Elastos::IO::ByteOrder;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::IBuffer;
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CUUID;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
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
        ICloseable::Probe(mDataSource)->Close();
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

CMediaExtractor::CMediaExtractor()
    : mNativeContext(0)
{
}

CMediaExtractor::~CMediaExtractor()
{
    NativeFinalize();
}

ECode CMediaExtractor::constructor()
{
    return NativeSetup();
}

ECode CMediaExtractor::SetDataSource(
    /* [in] */ IDataSource* source)
{
    if (mImpl == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (source == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
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

    ((Uri*)uri)->ToString(&tempString);
    return SetDataSource(tempString, headers);
}

ECode CMediaExtractor::SetDataSource(
    /* [in] */ const String& path,
    /* [in] */ IMap* headers)
{
    AutoPtr<ArrayOf<String> > keys;
    AutoPtr<ArrayOf<String> > values;

    if (headers != NULL) {
        Int32 size;
        headers->GetSize(&size);
        keys = ArrayOf<String>::Alloc(size);
        values = ArrayOf<String>::Alloc(size);

        AutoPtr<ISet> set;
        headers->GetEntrySet((ISet**)&set);
        AutoPtr<IIterator> it;
        set->GetIterator((IIterator**)&it);
        Int32 i = 0;
        Boolean b;
        while (it->HasNext(&b), b) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
            obj = NULL;
            entry->GetKey((IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            keys->Set(i, str);
            obj = NULL;
            entry->GetValue((IInterface**)&obj);
            ICharSequence::Probe(obj)->ToString(&str);
            values->Set(i, str);
            ++i;
        }
    }

    AutoPtr<IBinder> binder;
    CMediaHTTPService::CreateHttpServiceBinderIfNecessary(
            path, (IBinder**)&binder);
    return NativeSetDataSource(
        binder, path, keys, values);
}

// ECode CMediaExtractor::NativeSetDataSource(
//     /* [in] */ const android::sp<android::IMediaHTTPService> &httpService,
//     /* [in] */ const String& path,
//     /* [in] */ android::KeyedVector<android::String8,android::String8>* headers)
// {
//     if (path.IsNull()) {
//         return E_ILLEGAL_ARGUMENT_EXCEPTION;
//     }

//     android::status_t err = mImpl->setDataSource(httpService, path.string(), headers);
//     if (err != android::OK) {
//         Logger::E(TAG, "Failed to instantiate extractor.");
//         return E_IO_EXCEPTION;
//     }
//     return NOERROR;
// }

// ECode  CMediaExtractor::NativeSetDataSource(
//     /* [in] */ Int32 fd,
//     /* [in] */ Int64 offset,
//     /* [in] */ Int64 size)
// {
//     android::status_t err = mImpl->setDataSource(fd, offset, size);
//     if (err != android::OK) {
//         Logger::E(TAG, "Failed to instantiate extractor.");
//         return E_IO_EXCEPTION;
//     }
//     return NOERROR;
// }

ECode CMediaExtractor::NativeSetDataSource(
    /* [in] */ IBinder* httpServiceBinder,
    /* [in] */ const String& pathStr,
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<String>* values)
{
    if (mImpl == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (pathStr.IsNull()) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::KeyedVector<android::String8, android::String8> headers;
    if (!Media_Utils::ConvertKeyValueArraysToKeyedVector(
                keys, values, &headers)) {
        return NOERROR;
    }

    const char *path = pathStr.string();

    if (path == NULL) {
        return NOERROR;
    }

    android::sp<android::IMediaHTTPService> httpService;
    if (httpServiceBinder != NULL) {
        android::sp<android::IBinder> binder = IBinderForDroidObject(httpServiceBinder);
        httpService = android::interface_cast<android::IMediaHTTPService>(binder);
    }

    android::status_t err = mImpl->setDataSource(httpService, path, &headers);

    if (err != android::OK) {
        // jniThrowException(
        //         env,
        //         "java/io/IOException",
        //         "Failed to instantiate extractor.");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaExtractor::SetDataSource(
    /* [in] */ const String& path)
{
    AutoPtr<IBinder> binder;
    CMediaHTTPService::CreateHttpServiceBinderIfNecessary(
                path, (IBinder**)&binder);
    return NativeSetDataSource(binder, path, NULL, NULL);
}

ECode CMediaExtractor::SetDataSource(
    /* [in] */ IFileDescriptor* fd)
{
    return SetDataSource(fd, 0, 0x7ffffffffffffffL);
}

ECode CMediaExtractor::SetDataSource(
    /* [in] */ IFileDescriptor* fileDesc,
    /* [in] */ Int64 offset,
    /* [in] */ Int64 length)
{
    if (mImpl == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (fileDesc == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 fd;
    fileDesc->GetDescriptor(&fd);

    android::status_t err = mImpl->setDataSource(fd, offset, length);

    if (err != android::OK) {
        // jniThrowException(
        //         env,
        //         "java/io/IOException",
        //         "Failed to instantiate extractor.");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
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

ECode CMediaExtractor::GetPsshInfo(
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IHashMap> psshMap;
    AutoPtr<IMap> formatMap;
    GetFileFormatNative((IMap**)&formatMap);

    AutoPtr<ICharSequence> cs;
    CString::New(String("pssh"), (ICharSequence**)&cs);
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
        IBuffer::Probe(rawpssh)->Rewind();
        formatMap->Remove(cs);

        // parse the flat pssh bytebuffer into something more manageable
        CHashMap::New((IHashMap**)&psshMap);
        Int32 remaining;
        IBuffer::Probe(rawpssh)->GetRemaining(&remaining);
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

            AutoPtr<IArrayOf> array;
            CArrayOf::New(EIID_IByte, psshdata->GetLength(), (IArrayOf**)&array);
            for (Int32 i = 0; i < psshdata->GetLength(); ++i) {
                AutoPtr<IByte> byte;
                CByte::New((*psshdata)[i], (IByte**)&byte);
                array->Set(i, byte);
            }
            psshMap->Put(uuid, array);
        }
    }
    *result = IMap::Probe(psshMap);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaExtractor::GetTrackFormat(
    /* [in] */ Int32 index,
    /* [out] */ IMediaFormat** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IMap> tiMap;
    GetTrackFormatNative(index, (IMap**)&tiMap);

    return CMediaFormat::New(tiMap, result);
}

ECode CMediaExtractor::GetFileFormatNative(
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mImpl == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::sp<android::AMessage> msg;
    android::status_t err = mImpl->getFileFormat(&msg);
    if (err != android::OK) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IMap> format;
    err = Media_Utils::ConvertMessageToMap(msg, (IMap**)&format);

    if (err != android::OK) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = format;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaExtractor::GetTrackFormatNative(
    /* [in] */ Int32 index,
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mImpl == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::sp<android::AMessage> msg;
    android::status_t err = mImpl->getTrackFormat((ssize_t)index, &msg);
    if (err != android::OK) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IMap> format;
    err = Media_Utils::ConvertMessageToMap(msg, (IMap**)&format);

    if (err != android::OK) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = format;
    REFCOUNT_ADD(*result)
    return NOERROR;
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
    android::sp<android::NuMediaExtractor> extractor = new android::NuMediaExtractor;
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
