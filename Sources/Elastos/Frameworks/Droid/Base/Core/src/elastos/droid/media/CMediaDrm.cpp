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

#include "elastos/droid/Manifest.h"
#include "elastos/droid/media/CMediaDrm.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/utility/logging/Logger.h>

#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <media/IDrm.h>
#include <media/IDrmClient.h>
#include <media/IMediaDrmService.h>
#include <media/stagefright/foundation/ABase.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/MediaErrors.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

using android::sp;
using android::status_t;
using android::String8;
using android::Vector;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaDrm::TAG("MediaDrm");
const Int32 CMediaDrm::DRM_EVENT = 200;
const String CMediaDrm::PERMISSION = Elastos::Droid::Manifest::permission::ACCESS_DRM_CERTIFICATES;

//==============================================================================
//  JNIDrmListener
//==============================================================================

class DrmListener: virtual public android::RefBase
{
public:
    virtual void notify(android::DrmPlugin::EventType eventType, int extra,
                        const android::Parcel *obj) = 0;
};

class JNIDrmListener: public DrmListener
{
public:
    JNIDrmListener(IWeakReference* obj);
    ~JNIDrmListener();
    virtual void notify(android::DrmPlugin::EventType eventType,
            int extra, const android::Parcel *obj = NULL);
private:
    JNIDrmListener();
    AutoPtr<IWeakReference>    mObject;    // Weak ref to MediaDrm Java object to call on
};

JNIDrmListener::JNIDrmListener(IWeakReference* obj)
    : mObject(obj)
{
    // We use a weak reference so the MediaDrm object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
}

JNIDrmListener::~JNIDrmListener()
{
}

void JNIDrmListener::notify(android::DrmPlugin::EventType eventType, int extra,
                            const android::Parcel *obj)
{
    Int32 jeventType;

    // translate DrmPlugin event types into their java equivalents
    switch(eventType) {
        case android::DrmPlugin::kDrmPluginEventProvisionRequired:
            jeventType = IMediaDrm::EVENT_PROVISION_REQUIRED;
            break;
        case android::DrmPlugin::kDrmPluginEventKeyNeeded:
            jeventType = IMediaDrm::EVENT_KEY_REQUIRED;
            break;
        case android::DrmPlugin::kDrmPluginEventKeyExpired:
            jeventType = IMediaDrm::EVENT_KEY_EXPIRED;
            break;
        case android::DrmPlugin::kDrmPluginEventVendorDefined:
            jeventType = IMediaDrm::EVENT_VENDOR_DEFINED;
            break;
        default:
            ALOGE("Invalid event DrmPlugin::EventType %d, ignored", (int)eventType);
            return;
    }

    if (obj && obj->dataSize() > 0) {
// TODO: JNI Parcel
        // jobject jParcel = createJavaParcelObject(env);
        // if (jParcel != NULL) {
        //     Parcel* nativeParcel = parcelForJavaObject(env, jParcel);
        //     nativeParcel->setData(obj->data(), obj->dataSize());
        //     env->CallStaticVoidMethod(mClass, gFields.post_event, mObject,
        //             jeventType, extra, jParcel);
        //     env->DeleteLocalRef(jParcel);
        // }
    }
}

//==============================================================================
//  JDrm
//==============================================================================

struct JDrm : public android::BnDrmClient {
    static bool IsCryptoSchemeSupported(const uint8_t uuid[16], const String8 &mimeType);

    JDrm(CMediaDrm* obj, const uint8_t uuid[16]);

    status_t initCheck() const;
    sp<android::IDrm> getDrm() { return mDrm; }

    void notify(android::DrmPlugin::EventType, int extra, const android::Parcel *obj);
    status_t setListener(const sp<DrmListener>& listener);

    void disconnect();

protected:
    virtual ~JDrm();

private:
    CMediaDrm* mObject;
    sp<android::IDrm> mDrm;

    sp<DrmListener> mListener;
    android::Mutex mNotifyLock;
    android::Mutex mLock;

    static sp<android::IDrm> MakeDrm();
    static sp<android::IDrm> MakeDrm(const uint8_t uuid[16]);

    DISALLOW_EVIL_CONSTRUCTORS(JDrm);
};

JDrm::JDrm(
    CMediaDrm* obj, const uint8_t uuid[16])
    : mObject(obj)
{
    mDrm = MakeDrm(uuid);
    if (mDrm != NULL) {
        mDrm->setListener(this);
    }
}

JDrm::~JDrm()
{
    mObject = NULL;
}

// static
sp<android::IDrm> JDrm::MakeDrm()
{
    sp<android::IServiceManager> sm = android::defaultServiceManager();

    sp<android::IBinder> binder =
        sm->getService(android::String16("media.player"));

    sp<android::IMediaDrmService> service =
        android::interface_cast<android::IMediaDrmService>(binder);

    if (service == NULL) {
        return NULL;
    }

    sp<android::IDrm> drm = service->makeDrm();

    if (drm == NULL || (drm->initCheck() != android::OK &&
            drm->initCheck() != android::NO_INIT))
    {
        return NULL;
    }

    return drm;
}

// static
sp<android::IDrm> JDrm::MakeDrm(const uint8_t uuid[16])
{
    sp<android::IDrm> drm = MakeDrm();

    if (drm == NULL) {
        return NULL;
    }

    status_t err = drm->createPlugin(uuid, android::String8(""));

    if (err != android::OK) {
        return NULL;
    }

    return drm;
}

status_t JDrm::setListener(const sp<DrmListener>& listener) {
    android::Mutex::Autolock lock(mLock);
    mListener = listener;
    return android::OK;
}

void JDrm::notify(
    android::DrmPlugin::EventType eventType,
    int extra,
    const android::Parcel *obj)
{
    sp<DrmListener> listener;
    mLock.lock();
    listener = mListener;
    mLock.unlock();

    if (listener != NULL) {
        android::Mutex::Autolock lock(mNotifyLock);
        listener->notify(eventType, extra, obj);
    }
}

void JDrm::disconnect() {
    if (mDrm != NULL) {
        mDrm->destroyPlugin();
        mDrm.clear();
    }
}

// static
bool JDrm::IsCryptoSchemeSupported(
    const uint8_t uuid[16], const String8 &mimeType)
{
    sp<android::IDrm> drm = MakeDrm();

    if (drm == NULL) {
        return false;
    }

    return drm->isCryptoSchemeSupported(uuid, mimeType);
}

status_t JDrm::initCheck() const {
    return mDrm == NULL ? android::NO_INIT : android::OK;
}

//==============================================================================
//  JNI conversion utilities
//==============================================================================

static Vector<uint8_t> JByteArrayToVector(ArrayOf<Byte>* byteArray) {
    Vector<uint8_t> vector;
    size_t length = byteArray->GetLength();
    vector.insertAt((size_t)0, length);
//TODO: GetByteArrayRegion
    // env->GetByteArrayRegion(byteArray, 0, length, (Byte *)vector.editArray());
    return vector;
}

static AutoPtr<ArrayOf<Byte> > VectorToJByteArray(Vector<uint8_t> const &vector) {
    size_t length = vector.size();
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(length);
    if (result != NULL) {
//TODO: SetByteArrayRegion
        // env->SetByteArrayRegion(result, 0, length, (Byte *)vector.array());
    }
    return result;
}

static String8 JStringToString8(const String& jstr) {
    String8 result;

    const char *s = jstr.string();
    if (s) {
        result = s;
    }
    return result;
}

static android::KeyedVector<String8, String8> HashMapToKeyedVector(
    IHashMap* hashMap)
{
    android::KeyedVector<String8, String8> keyedVector;

    AutoPtr<ISet> entrySet;
    hashMap->GetEntrySet((ISet**)&entrySet);
    if (entrySet) {
        AutoPtr<IIterator> iterator;
        entrySet->GetIterator((IIterator**)&iterator);
        if (iterator) {
            Boolean hasNext;
            iterator->HasNext(&hasNext);
            while (hasNext) {
                AutoPtr<IInterface> obj;
                iterator->GetNext((IInterface**)&obj);
                if (obj) {
                    AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
                    obj = NULL;
                    entry->GetKey((IInterface**)&obj);
                    String keyStr;
                    ICharSequence::Probe(obj)->ToString(&keyStr);

                    entry->GetValue((IInterface**)&obj);
                    String valueStr;
                    ICharSequence::Probe(obj)->ToString(&valueStr);

                    String8 key = JStringToString8(keyStr);
                    String8 value = JStringToString8(valueStr);
                    keyedVector.add(key, value);

                    iterator->HasNext(&hasNext);
                }
            }
        }
    }
    return keyedVector;
}

static AutoPtr<IHashMap> KeyedVectorToHashMap (
    android::KeyedVector<String8, String8> const &map)
{
    AutoPtr<IHashMap> hashMap;
    CHashMap::New((IHashMap**)&hashMap);
    for (size_t i = 0; i < map.size(); ++i) {
        String jkey = String(map.keyAt(i).string());
        String jvalue = String(map.valueAt(i).string());
        AutoPtr<ICharSequence> csKey;
        CString::New(jkey, (ICharSequence**)&csKey);
        AutoPtr<ICharSequence> csValue;
        CString::New(jvalue, (ICharSequence**)&csValue);
        hashMap->Put(csKey, csValue);
    }
    return hashMap;
}

static AutoPtr<IList> ListOfVectorsToArrayListOfByteArray(
    android::List<Vector<uint8_t> > list)
{
    AutoPtr<IArrayList> arrayList;
    CArrayList::New((IArrayList**)&arrayList);

    android::List<Vector<uint8_t> >::iterator iter = list.begin();
    while (iter != list.end()) {
        AutoPtr<ArrayOf<Byte> > byteArray = VectorToJByteArray(*iter);

        AutoPtr<IByteBufferHelper> helper;
        CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
        AutoPtr<IByteBuffer> byteBuffer;
        helper->Wrap(byteArray, (IByteBuffer**)&byteBuffer);

        arrayList->Add(byteBuffer);
        iter++;
    }

    return IList::Probe(arrayList);
}

static bool CheckSession(
    const sp<android::IDrm> &drm, const ArrayOf<Byte>* jsessionId)
{
    if (drm == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException", "MediaDrm obj is null");
        return false;
    }

    if (jsessionId == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", "sessionId is null");
        return false;
    }
    return true;
}

//==============================================================================
//  CMediaDrm::KeyRequest
//==============================================================================

CAR_INTERFACE_IMPL(CMediaDrm::KeyRequest, Object, IMediaDrmKeyRequest)

ECode CMediaDrm::KeyRequest::GetData(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mData;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::KeyRequest::GetDefaultUrl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDefaultUrl;
    return NOERROR;
}

//==============================================================================
//  CMediaDrm::ProvisionRequest
//==============================================================================

CAR_INTERFACE_IMPL(CMediaDrm::ProvisionRequest, Object, IMediaDrmProvisionRequest)

ECode CMediaDrm::ProvisionRequest::GetData(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mData;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::ProvisionRequest::GetDefaultUrl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDefaultUrl;
    return NOERROR;
}

//==============================================================================
//  CMediaDrm::CryptoSession
//==============================================================================

CAR_INTERFACE_IMPL(CMediaDrm::CryptoSession, Object, IMediaDrmCryptoSession)

CMediaDrm::CryptoSession::CryptoSession(
    /* [in] */ CMediaDrm* drm,
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ const String& cipherAlgorithm,
    /* [in] */ const String& macAlgorithm)
{
    mSessionId = sessionId;
    mDrm = drm;
    SetCipherAlgorithmNative(drm, sessionId, cipherAlgorithm);
    SetMacAlgorithmNative(drm, sessionId, macAlgorithm);
}

ECode CMediaDrm::CryptoSession::Encrypt(
    /* [in] */ ArrayOf<Byte>* keyid,
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ ArrayOf<Byte>* iv,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = EncryptNative(mDrm, mSessionId, keyid, input, iv);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::CryptoSession::Decrypt(
    /* [in] */ ArrayOf<Byte>* keyid,
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ ArrayOf<Byte>* iv,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = DecryptNative(mDrm, mSessionId, keyid, input, iv);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::CryptoSession::Sign(
    /* [in] */ ArrayOf<Byte>* keyid,
    /* [in] */ ArrayOf<Byte>* message,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = SignNative(mDrm, mSessionId, keyid, message);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::CryptoSession::Verify(
    /* [in] */ ArrayOf<Byte>* keyid,
    /* [in] */ ArrayOf<Byte>* message,
    /* [in] */ ArrayOf<Byte>* signature,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = VerifyNative(mDrm, mSessionId, keyid, message, signature);
    return NOERROR;
}

//==============================================================================
//  CMediaDrm::CertificateRequest
//==============================================================================

CAR_INTERFACE_IMPL(CMediaDrm::CertificateRequest, Object, IMediaDrmCertificateRequest)

CMediaDrm::CertificateRequest::CertificateRequest(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ const String& defaultUrl)
{
    mData = data;
    mDefaultUrl = defaultUrl;
}

ECode CMediaDrm::CertificateRequest::GetData(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mData;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::CertificateRequest::GetDefaultUrl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDefaultUrl;
    return NOERROR;
}

//==============================================================================
//  CMediaDrm::Certificate
//==============================================================================

CAR_INTERFACE_IMPL(CMediaDrm::Certificate, Object, IMediaDrmCertificate)

ECode CMediaDrm::Certificate::GetWrappedPrivateKey(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWrappedKey;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::Certificate::GetContent(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCertificateData;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//==============================================================================
//  CMediaDrm::EventHandler
//==============================================================================

ECode CMediaDrm::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (mHost->mNativeContext == 0) {
        Logger::W(TAG, "MediaDrm went away with unhandled events");
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
    case CMediaDrm::DRM_EVENT: {
        Int32 arg1, arg2;
        msg->GetArg1(&arg1);
        msg->GetArg2(&arg2);
        Logger::I(TAG, "Drm event (%d,%d)", arg1, arg2);

        if (mHost->mOnEventListener != NULL) {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IParcel> parcel = IParcel::Probe(obj);
            if(parcel != NULL) {
                AutoPtr<ArrayOf<Byte> > sessionId;
// TODO: Need IParcel::CreateByteArray
                // parcel->CreateByteArray((ArrayOf<Byte>**)&sessionId);
                if (sessionId->GetLength() == 0) {
                    sessionId = NULL;
                }
                AutoPtr<ArrayOf<Byte> > data;
// TODO: Need IParcel::CreateByteArray
                // parcel->CreateByteArray((ArrayOf<Byte>**)&data);
                if (data->GetLength() == 0) {
                    data = NULL;
                }
                mHost->mOnEventListener->OnEvent(mHost, sessionId, arg1, arg2, data);
            }
        }
        return NOERROR;
    }
    default:
        Logger::E(TAG, "Unknown message type %d", what);
        return NOERROR;
    }
}

//==============================================================================
//  CMediaDrm
//==============================================================================

CAR_INTERFACE_IMPL(CMediaDrm, Object, IMediaDrm)

CAR_OBJECT_IMPL(CMediaDrm)

CMediaDrm::CMediaDrm()
    : mNativeContext(0)
{
}

CMediaDrm::~CMediaDrm()
{
    NativeFinalize();
}

ECode CMediaDrm::constructor()
{
    return NOERROR;
}

ECode CMediaDrm::constructor(
    /* [in] */ IUUID* uuid)
{
    AutoPtr<ILooper> looper;
    if ((looper = Looper::GetMyLooper()) != NULL) {
        mEventHandler = new EventHandler(this, looper);
    }
    else if ((looper = Looper::GetMainLooper()) != NULL) {
        mEventHandler = new EventHandler(this, looper);
    }
    else {
        mEventHandler = NULL;
    }

    /* Native setup requires a weak reference to our object.
     * It's easier to create it here than in C++.
     */
    AutoPtr<IWeakReference> weakHost;
    GetWeakReference((IWeakReference**)&weakHost);
    NativeSetup(weakHost, GetByteArrayFromUUID(uuid));
    return NOERROR;
}

ECode CMediaDrm::SetOnEventListener(
    /* [in] */ IMediaDrmOnEventListener* listener)
{
    mOnEventListener = listener;
    return NOERROR;
}

ECode CMediaDrm::OpenSession(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (drm == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "MediaDrm obj is null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Vector<uint8_t> sessionId;
    status_t err = drm->openSession(sessionId);

    // if (throwExceptionAsNecessary(env, err, "Failed to open session")) {
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = VectorToJByteArray(sessionId);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::CloseSession(
    /* [in] */ ArrayOf<Byte>* jsessionId)
{
    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));

    status_t err = drm->closeSession(sessionId);

    // throwExceptionAsNecessary(env, err, "Failed to close session");
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaDrm::GetKeyRequest(
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [in] */ ArrayOf<Byte>* jinitData,
    /* [in] */ const String& jmimeType,
    /* [in] */ Int32 jkeyType,
    /* [in] */ IHashMap* joptParams,
    /* [out] */ IMediaDrmKeyRequest** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));

    Vector<uint8_t> initData;
    if (jinitData != NULL) {
        initData = JByteArrayToVector(jinitData);
    }

    String8 mimeType;
    if (jmimeType != NULL) {
        mimeType = JStringToString8(jmimeType);
    }

    android::DrmPlugin::KeyType keyType;
    if (jkeyType == IMediaDrm::KEY_TYPE_STREAMING) {
        keyType = android::DrmPlugin::kKeyType_Streaming;
    }
    else if (jkeyType == IMediaDrm::KEY_TYPE_OFFLINE) {
        keyType = android::DrmPlugin::kKeyType_Offline;
    }
    else if (jkeyType == IMediaDrm::KEY_TYPE_RELEASE) {
        keyType = android::DrmPlugin::kKeyType_Release;
    }
    else {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "invalid keyType");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::KeyedVector<String8, String8> optParams;
    if (joptParams != NULL) {
        optParams = HashMapToKeyedVector(joptParams);
    }

    Vector<uint8_t> request;
    String8 defaultUrl;
    android::DrmPlugin::KeyRequestType keyRequestType;

    status_t err = drm->getKeyRequest(sessionId, initData, mimeType,
            keyType, optParams, request, defaultUrl, &keyRequestType);

    // if (throwExceptionAsNecessary(env, err, "Failed to get key request")) {
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // Fill out return obj
    AutoPtr<KeyRequest> keyObj = new KeyRequest();

    AutoPtr<ArrayOf<Byte> > jrequest = VectorToJByteArray(request);
    keyObj->mData = jrequest;

    String jdefaultUrl = String(defaultUrl.string());
    keyObj->mDefaultUrl = jdefaultUrl;

    *result = keyObj;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::ProvideKeyResponse(
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [in] */ ArrayOf<Byte>* jresponse,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));

    if (jresponse == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "key response is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Vector<uint8_t> response(JByteArrayToVector(jresponse));
    Vector<uint8_t> keySetId;

    status_t err = drm->provideKeyResponse(sessionId, response, keySetId);

    // if (throwExceptionAsNecessary(env, err, "Failed to handle key response")) {
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *result = VectorToJByteArray(keySetId);
    REFCOUNT_ADD(*result)
    return NOERROR;
}


ECode CMediaDrm::RestoreKeys(
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [in] */ ArrayOf<Byte>* jkeysetId)
{
    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (jkeysetId == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));
    Vector<uint8_t> keySetId(JByteArrayToVector(jkeysetId));

    status_t err = drm->restoreKeys(sessionId, keySetId);

    // throwExceptionAsNecessary(env, err, "Failed to restore keys");
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaDrm::RemoveKeys(
    /* [in] */ ArrayOf<Byte>* jkeysetId)
{
    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (jkeysetId == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "keySetId is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Vector<uint8_t> keySetId(JByteArrayToVector(jkeysetId));

    status_t err = drm->removeKeys(keySetId);

    // throwExceptionAsNecessary(env, err, "Failed to remove keys");
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaDrm::QueryKeyStatus(
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [out] */ IHashMap** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return NOERROR;
    }
    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));

    android::KeyedVector<String8, String8> infoMap;

    status_t err = drm->queryKeyStatus(sessionId, infoMap);

    // if (throwExceptionAsNecessary(env, err, "Failed to query key status")) {
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IHashMap> temp = KeyedVectorToHashMap(infoMap);
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::GetProvisionRequest(
    /* [out] */ IMediaDrmProvisionRequest** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMediaDrmProvisionRequest> temp = GetProvisionRequestNative(CERTIFICATE_TYPE_NONE, String(""));
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::ProvideProvisionResponse(
    /* [in] */ ArrayOf<Byte>* response)
{
    ProvideProvisionResponseNative(response);
    return NOERROR;
}

ECode CMediaDrm::UnprovisionDevice()
{
    return NOERROR;
}

ECode CMediaDrm::GetSecureStops(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (drm == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "MediaDrm obj is null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    android::List<Vector<uint8_t> > secureStops;

    status_t err = drm->getSecureStops(secureStops);

    // if (throwExceptionAsNecessary(env, err, "Failed to get secure stops")) {
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IList> temp = ListOfVectorsToArrayListOfByteArray(secureStops);
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}


ECode CMediaDrm::ReleaseSecureStops(
    /* [in] */ ArrayOf<Byte>* jssRelease)
{
    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (drm == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "MediaDrm obj is null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Vector<uint8_t> ssRelease(JByteArrayToVector(jssRelease));

    status_t err = drm->releaseSecureStops(ssRelease);

    // throwExceptionAsNecessary(env, err, "Failed to release secure stops");
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaDrm::GetPropertyString(
    /* [in] */ const String& jname,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (drm == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "MediaDrm obj is null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (jname == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "property name String is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String8 name = JStringToString8(jname);
    String8 value;

    status_t err = drm->getPropertyString(name, value);

    // if (throwExceptionAsNecessary(env, err, "Failed to get property")) {
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = String(value.string());
    return NOERROR;
}

ECode CMediaDrm::GetPropertyByteArray(
    /* [in] */ const String& jname,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (drm == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "MediaDrm obj is null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (jname == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "property name String is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String8 name = JStringToString8(jname);
    Vector<uint8_t> value;

    status_t err = drm->getPropertyByteArray(name, value);

    // if (throwExceptionAsNecessary(env, err, "Failed to get property")) {
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *result = VectorToJByteArray(value);
    return NOERROR;
}

ECode CMediaDrm::SetPropertyString(
    /* [in] */ const String& jname,
    /* [in] */ const String& jvalue)
{
    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (drm == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "MediaDrm obj is null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (jname == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "property name String is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (jvalue == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "property value String is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String8 name = JStringToString8(jname);
    String8 value = JStringToString8(jvalue);

    status_t err = drm->setPropertyString(name, value);

    // throwExceptionAsNecessary(env, err, "Failed to set property");
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaDrm::SetPropertyByteArray(
    /* [in] */ const String& jname,
    /* [in] */ ArrayOf<Byte>* jvalue)
{
    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (drm == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "MediaDrm obj is null");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (jname == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "property name String is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (jvalue == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "property value byte array is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String8 name = JStringToString8(jname);
    Vector<uint8_t> value = JByteArrayToVector(jvalue);

    status_t err = drm->setPropertyByteArray(name, value);

    // throwExceptionAsNecessary(env, err, "Failed to set property");
    if (err != android::OK) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CMediaDrm::GetCryptoSession(
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ const String& cipherAlgorithm,
    /* [in] */ const String& macAlgorithm,
    /* [out] */ IMediaDrmCryptoSession** result)
{
    VALIDATE_NOT_NULL(result)
    *result = new CryptoSession(this, sessionId, cipherAlgorithm, macAlgorithm);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::GetCertificateRequest(
    /* [in] */ Int32 certType,
    /* [in] */ const String& certAuthority,
    /* [out] */ IMediaDrmCertificateRequest** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ProvisionRequest> provisionRequest =
            GetProvisionRequestNative(certType, certAuthority);
    AutoPtr<ArrayOf<Byte> > data;
    provisionRequest->GetData((ArrayOf<Byte>**)&data);
    String defaultUrl;
    provisionRequest->GetDefaultUrl(&defaultUrl);
    *result = new CertificateRequest(data, defaultUrl);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::ProvideCertificateResponse(
    /* [in] */ ArrayOf<Byte>* response,
    /* [out] */ IMediaDrmCertificate** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMediaDrmCertificate> temp = ProvideProvisionResponseNative(response);
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::SignRSA(
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ const String& algorithm,
    /* [in] */ ArrayOf<Byte>* wrappedKey,
    /* [in] */ ArrayOf<Byte>* message,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = SignRSANative(this, sessionId, algorithm, wrappedKey, message);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::ReleaseResources()
{
    sp<JDrm> drm = (JDrm *)mNativeContext;
    if (drm != NULL) {
        drm->setListener(NULL);
        drm->disconnect();
    }
    mNativeContext = 0;
    return NOERROR;
}

ECode CMediaDrm::IsCryptoSchemeSupported(
    /* [in] */ IUUID* uuid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return IsCryptoSchemeSupportedNative(GetByteArrayFromUUID(uuid), String(NULL), result);
}

ECode CMediaDrm::IsCryptoSchemeSupported(
    /* [in] */ IUUID* uuid,
    /* [in] */ const String& mimeType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return IsCryptoSchemeSupportedNative(GetByteArrayFromUUID(uuid), mimeType, result);
}

AutoPtr<ArrayOf<Byte> > CMediaDrm::GetByteArrayFromUUID(
    /* [in] */ IUUID* uuid)
{
    Int64 msb;
    uuid->GetMostSignificantBits(&msb);
    Int64 lsb;
    uuid->GetLeastSignificantBits(&lsb);

    AutoPtr<ArrayOf<Byte> > uuidBytes = ArrayOf<Byte>::Alloc(16);
    for (Int32 i = 0; i < 8; ++i) {
        uuidBytes->Set(i, (byte)(msb >> (8 * (7 - i))));
        uuidBytes->Set(8 + i, (byte)(lsb >> (8 * (7 - i))));
    }

    return uuidBytes;
}

ECode CMediaDrm::IsCryptoSchemeSupportedNative(
    /* [in] */ ArrayOf<Byte>* uuidObj,
    /* [in] */ const String& jmimeType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (uuidObj == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Vector<uint8_t> uuid = JByteArrayToVector(uuidObj);

    if (uuid.size() != 16) {
        // jniThrowException(
        //         env,
        //         "java/lang/IllegalArgumentException",
        //         "invalid UUID size, expected 16 bytes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String8 mimeType;
    if (jmimeType != NULL) {
        mimeType = JStringToString8(jmimeType);
    }

    *result = JDrm::IsCryptoSchemeSupported(uuid.array(), mimeType);
    return NOERROR;
}

void CMediaDrm::PostEventFromNative(
    /* [in] */ IInterface* ref,
    /* [in] */ Int32 eventType,
    /* [in] */ Int32 extra,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IWeakReference> wr = IWeakReference::Probe(ref);
    AutoPtr<IMediaDrm> imd;
    wr->Resolve(EIID_IMediaDrm, (IInterface**)&imd);
    if (imd == NULL) {
        return;
    }

    AutoPtr<CMediaDrm> md = (CMediaDrm*)imd.Get();
    if (md->mEventHandler != NULL) {
        AutoPtr<IMessage> m;
        md->mEventHandler->ObtainMessage(DRM_EVENT, eventType, extra, obj, (IMessage**)&m);
        Boolean b;
        md->mEventHandler->SendMessage(m, &b);
    }
}

AutoPtr<CMediaDrm::ProvisionRequest> CMediaDrm::GetProvisionRequestNative(
    /* [in] */ Int32 jcertType,
    /* [in] */ const String& jcertAuthority)
{
    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (drm == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "MediaDrm obj is null");
        return NULL;
    }

    Vector<uint8_t> request;
    String8 defaultUrl;

    String8 certType;
    if (jcertType == IMediaDrm::CERTIFICATE_TYPE_X509) {
        certType = "X.509";
    }
    else if (jcertType == IMediaDrm::CERTIFICATE_TYPE_NONE) {
        certType = "none";
    }
    else {
        certType = "invalid";
    }

    String8 certAuthority = JStringToString8(jcertAuthority);
    status_t err = drm->getProvisionRequest(certType, certAuthority, request, defaultUrl);

    // if (throwExceptionAsNecessary(env, err, "Failed to get provision request")) {
    if (err != android::OK) {
        return NULL;
    }

    // Fill out return obj
    AutoPtr<ProvisionRequest> provisionObj = new ProvisionRequest();

    AutoPtr<ArrayOf<Byte> > jrequest = VectorToJByteArray(request);
    provisionObj->mData = jrequest;

    String jdefaultUrl = String(defaultUrl.string());
    provisionObj->mDefaultUrl = jdefaultUrl;

    return provisionObj;
}

AutoPtr<CMediaDrm::Certificate> CMediaDrm::ProvideProvisionResponseNative(
    /* [in] */ ArrayOf<Byte>* jresponse)
{
    sp<android::IDrm> drm = ((JDrm *)mNativeContext)->getDrm();

    if (drm == NULL) {
        // jniThrowException(env, "java/lang/IllegalStateException",
        //                   "MediaDrm obj is null");
        return NULL;
    }

    if (jresponse == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "provision response is null");
        return NULL;
    }

    Vector<uint8_t> response(JByteArrayToVector(jresponse));
    Vector<uint8_t> certificate, wrappedKey;

    status_t err = drm->provideProvisionResponse(response, certificate, wrappedKey);

    // Fill out return obj
    AutoPtr<Certificate> certificateObj;

    if (certificate.size() && wrappedKey.size()) {
        certificateObj = new Certificate();

        AutoPtr<ArrayOf<Byte> > jcertificate = VectorToJByteArray(certificate);
        certificateObj->mCertificateData = jcertificate;

        AutoPtr<ArrayOf<Byte> > jwrappedKey = VectorToJByteArray(wrappedKey);
        certificateObj->mWrappedKey = jwrappedKey;
    }

    // throwExceptionAsNecessary(env, err, "Failed to handle provision response");
    if (err != android::OK) {
        return NULL;
    }
    return certificateObj;
}

void CMediaDrm::SetCipherAlgorithmNative(
    /* [in] */ IMediaDrm* jdrm,
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [in] */ const String& jalgorithm)
{
    sp<android::IDrm> drm = ((JDrm *)((CMediaDrm*)jdrm)->mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return;
    }

    if (jalgorithm == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "algorithm String is null");
        return;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));
    String8 algorithm = JStringToString8(jalgorithm);

    status_t err = drm->setCipherAlgorithm(sessionId, algorithm);

    // throwExceptionAsNecessary(env, err, "Failed to set cipher algorithm");
}

void CMediaDrm::SetMacAlgorithmNative(
    /* [in] */ IMediaDrm* jdrm,
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [in] */ const String& jalgorithm)
{
    sp<android::IDrm> drm = ((JDrm *)((CMediaDrm*)jdrm)->mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return;
    }

    if (jalgorithm == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "algorithm String is null");
        return;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));
    String8 algorithm = JStringToString8(jalgorithm);

    status_t err = drm->setMacAlgorithm(sessionId, algorithm);

    // throwExceptionAsNecessary(env, err, "Failed to set mac algorithm");
}

AutoPtr<ArrayOf<Byte> > CMediaDrm::EncryptNative(
    /* [in] */ IMediaDrm* jdrm,
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [in] */ ArrayOf<Byte>* jkeyId,
    /* [in] */ ArrayOf<Byte>* jinput,
    /* [in] */ ArrayOf<Byte>* jiv)
{
    sp<android::IDrm> drm = ((JDrm *)((CMediaDrm*)jdrm)->mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return NULL;
    }

    if (jkeyId == NULL || jinput == NULL || jiv == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "required argument is null");
        return NULL;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));
    Vector<uint8_t> keyId(JByteArrayToVector(jkeyId));
    Vector<uint8_t> input(JByteArrayToVector(jinput));
    Vector<uint8_t> iv(JByteArrayToVector(jiv));
    Vector<uint8_t> output;

    status_t err = drm->encrypt(sessionId, keyId, input, iv, output);

    // if (throwExceptionAsNecessary(env, err, "Failed to encrypt")) {
    if (err != android::OK) {
        return NULL;
    }

    return VectorToJByteArray(output);
}

AutoPtr<ArrayOf<Byte> > CMediaDrm::DecryptNative(
    /* [in] */ IMediaDrm* jdrm,
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [in] */ ArrayOf<Byte>* jkeyId,
    /* [in] */ ArrayOf<Byte>* jinput,
    /* [in] */ ArrayOf<Byte>* jiv)
{
    sp<android::IDrm> drm = ((JDrm *)((CMediaDrm*)jdrm)->mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return NULL;
    }

    if (jkeyId == NULL || jinput == NULL || jiv == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "required argument is null");
        return NULL;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));
    Vector<uint8_t> keyId(JByteArrayToVector(jkeyId));
    Vector<uint8_t> input(JByteArrayToVector(jinput));
    Vector<uint8_t> iv(JByteArrayToVector(jiv));
    Vector<uint8_t> output;

    status_t err = drm->decrypt(sessionId, keyId, input, iv, output);
    // if (throwExceptionAsNecessary(env, err, "Failed to decrypt")) {
    if (err != android::OK) {
        return NULL;
    }

    return VectorToJByteArray(output);
}

AutoPtr<ArrayOf<Byte> > CMediaDrm::SignNative(
    /* [in] */ IMediaDrm* jdrm,
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [in] */ ArrayOf<Byte>* jkeyId,
    /* [in] */ ArrayOf<Byte>* jmessage)
{
    sp<android::IDrm> drm = ((JDrm *)((CMediaDrm*)jdrm)->mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return NULL;
    }

    if (jkeyId == NULL || jmessage == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "required argument is null");
        return NULL;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));
    Vector<uint8_t> keyId(JByteArrayToVector(jkeyId));
    Vector<uint8_t> message(JByteArrayToVector(jmessage));
    Vector<uint8_t> signature;

    status_t err = drm->sign(sessionId, keyId, message, signature);

    // if (throwExceptionAsNecessary(env, err, "Failed to sign")) {
    if (err != android::OK) {
        return NULL;
    }

    return VectorToJByteArray(signature);
}

Boolean CMediaDrm::VerifyNative(
    /* [in] */ IMediaDrm* jdrm,
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [in] */ ArrayOf<Byte>* jkeyId,
    /* [in] */ ArrayOf<Byte>* jmessage,
    /* [in] */ ArrayOf<Byte>* jsignature)
{
    sp<android::IDrm> drm = ((JDrm *)((CMediaDrm*)jdrm)->mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return false;
    }

    if (jkeyId == NULL || jmessage == NULL || jsignature == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "required argument is null");
        return false;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));
    Vector<uint8_t> keyId(JByteArrayToVector(jkeyId));
    Vector<uint8_t> message(JByteArrayToVector(jmessage));
    Vector<uint8_t> signature(JByteArrayToVector(jsignature));
    bool match;

    status_t err = drm->verify(sessionId, keyId, message, signature, match);

    // throwExceptionAsNecessary(env, err, "Failed to verify");
    if (err != android::OK) {
        return FALSE;
    }
    return match;
}

AutoPtr<ArrayOf<Byte> > CMediaDrm::SignRSANative(
    /* [in] */ IMediaDrm* jdrm,
    /* [in] */ ArrayOf<Byte>* jsessionId,
    /* [in] */ const String& jalgorithm,
    /* [in] */ ArrayOf<Byte>* jwrappedKey,
    /* [in] */ ArrayOf<Byte>* jmessage)
{
    sp<android::IDrm> drm = ((JDrm *)((CMediaDrm*)jdrm)->mNativeContext)->getDrm();

    if (!CheckSession(drm, jsessionId)) {
        return NULL;
    }

    if (jalgorithm == NULL || jwrappedKey == NULL || jmessage == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "required argument is null");
        return NULL;
    }

    Vector<uint8_t> sessionId(JByteArrayToVector(jsessionId));
    String8 algorithm = JStringToString8(jalgorithm);
    Vector<uint8_t> wrappedKey(JByteArrayToVector(jwrappedKey));
    Vector<uint8_t> message(JByteArrayToVector(jmessage));
    Vector<uint8_t> signature;

    status_t err = drm->signRSA(sessionId, algorithm, message, wrappedKey, signature);

    // if (throwExceptionAsNecessary(env, err, "Failed to sign")) {
    if (err != android::OK) {
        return NULL;
    }

    return VectorToJByteArray(signature);
}

void CMediaDrm::NativeInit()
{
}

ECode CMediaDrm::NativeSetup(
    /* [in] */ IWeakReference* mediadrm_this,
    /* [in] */ ArrayOf<Byte>* uuidObj)
{
    if (uuidObj == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", "uuid is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Vector<uint8_t> uuid = JByteArrayToVector(uuidObj);

    if (uuid.size() != 16) {
        // jniThrowException(env, "java/lang/IllegalArgumentException",
        //                   "invalid UUID size, expected 16 bytes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    sp<JDrm> drm = new JDrm(this, uuid.array());

    status_t err = drm->initCheck();

    if (err != android::OK) {
        // jniThrowException(
        //         env,
        //         "android/media/UnsupportedSchemeException",
        //         "Failed to instantiate drm object.");
        return E_UNSUPPORTED_SCHEME_EXCEPTION;
    }

    sp<JNIDrmListener> listener = new JNIDrmListener(mediadrm_this);
    drm->setListener(listener);
    mNativeContext = (Int64)drm.get();
    return NOERROR;
}

void CMediaDrm::NativeFinalize()
{
    ReleaseResources();
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
