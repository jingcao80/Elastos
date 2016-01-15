#include "elastos/droid/Manifest.h"
#include "elastos/droid/media/CMediaDrm.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaDrm::TAG("MediaDrm");
const Int32 CMediaDrm::DRM_EVENT = 200;
const String CMediaDrm::PERMISSION = Elastos::Droid::Manifest::permission::ACCESS_DRM_CERTIFICATES;

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
    NativeSetup(THIS_PROBE(IMediaDrm), GetByteArrayFromUUID(uuid));
    return NOERROR;
}

ECode CMediaDrm::SetOnEventListener(
    /* [in] */ IMediaDrmOnEventListener* listener)
{
    mOnEventListener = listener;
    return NOERROR;
}

// TODO: Need jni code
ECode CMediaDrm::OpenSession(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)

}

ECode CMediaDrm::CloseSession(
    /* [in] */ ArrayOf<Byte>* sessionId)
{

}

ECode CMediaDrm::GetKeyRequest(
    /* [in] */ ArrayOf<Byte>* scope,
    /* [in] */ ArrayOf<Byte>* init,
    /* [in] */ const String& mimeType,
    /* [in] */ Int32 keyType,
    /* [in] */ IHashMap* optionalParameters,
    /* [out] */ IMediaDrmKeyRequest** result)
{
    VALIDATE_NOT_NULL(result)

}

ECode CMediaDrm::ProvideKeyResponse(
    /* [in] */ ArrayOf<Byte>* scope,
    /* [in] */ ArrayOf<Byte>* response,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)

}


ECode CMediaDrm::RestoreKeys(
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ ArrayOf<Byte>* keySetId)
{

}

ECode CMediaDrm::RemoveKeys(
    /* [in] */ ArrayOf<Byte>* sessionId)
{

}

ECode CMediaDrm::QueryKeyStatus(
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [out] */ IHashMap** result)
{
    VALIDATE_NOT_NULL(result)

}

ECode CMediaDrm::GetProvisionRequest(
    /* [out] */ IMediaDrmProvisionRequest** result)
{
    VALIDATE_NOT_NULL(result)
    *result = GetProvisionRequestNative(CERTIFICATE_TYPE_NONE, String(""));
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDrm::ProvideProvisionResponse(
    /* [in] */ ArrayOf<Byte>* response)
{
    ProvideProvisionResponseNative(response);
    return NOERROR;
}

// TODO: Need jni code
ECode CMediaDrm::UnprovisionDevice()
{

}

ECode CMediaDrm::GetSecureStops(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

}


ECode CMediaDrm::ReleaseSecureStops(
    /* [in] */ ArrayOf<Byte>* ssRelease)
{

}

ECode CMediaDrm::GetPropertyString(
    /* [in] */ const String& propertyName,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

}

ECode CMediaDrm::GetPropertyByteArray(
    /* [in] */ const String& propertyName,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)

}

ECode CMediaDrm::SetPropertyString(
    /* [in] */ const String& propertyName,
    /* [in] */ const String& value)
{

}

ECode CMediaDrm::SetPropertyByteArray(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Byte>* value)
{

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
    *result = ProvideProvisionResponseNative(response);
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

}

ECode CMediaDrm::IsCryptoSchemeSupported(
    /* [in] */ IUUID* uuid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsCryptoSchemeSupportedNative(GetByteArrayFromUUID(uuid), String(NULL));
    return NOERROR;
}

ECode CMediaDrm::IsCryptoSchemeSupported(
    /* [in] */ IUUID* uuid,
    /* [in] */ const String& mimeType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsCryptoSchemeSupportedNative(GetByteArrayFromUUID(uuid), mimeType);
    return NOERROR;
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

Boolean CMediaDrm::IsCryptoSchemeSupportedNative(
    /* [in] */ ArrayOf<Byte>* uuid,
    /* [in] */ const String& mimeType)
{

}

void CMediaDrm::PostEventFromNative(
    /* [in] */ IInterface* mediadrm_ref,
    /* [in] */ Int32 eventType,
    /* [in] */ Int32 extra,
    /* [in] */ IInterface* obj)
{
    AutoPtr<CMediaDrm> md = (CMediaDrm*)IMediaDrm::Probe(mediadrm_ref);
    if (md == NULL) {
        return;
    }
    if (md->mEventHandler != NULL) {
        AutoPtr<IMessage> m;
        md->mEventHandler->ObtainMessage(DRM_EVENT, eventType, extra, obj, (IMessage**)&m);
        Boolean b;
        md->mEventHandler->SendMessage(m, &b);
    }
}

// TODO: Need jni code
AutoPtr<CMediaDrm::ProvisionRequest> CMediaDrm::GetProvisionRequestNative(
    /* [in] */ Int32 certType,
    /* [in] */ const String& certAuthority)
{

}

AutoPtr<CMediaDrm::Certificate> CMediaDrm::ProvideProvisionResponseNative(
    /* [in] */ ArrayOf<Byte>* response)
{

}

void CMediaDrm::SetCipherAlgorithmNative(
    /* [in] */ IMediaDrm* drm,
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ const String& algorithm)
{

}

void CMediaDrm::SetMacAlgorithmNative(
    /* [in] */ IMediaDrm* drm,
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ const String& algorithm)
{

}

AutoPtr<ArrayOf<Byte> > CMediaDrm::EncryptNative(
    /* [in] */ IMediaDrm* drm,
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ ArrayOf<Byte>* keyId,
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ ArrayOf<Byte>* iv)
{

}

AutoPtr<ArrayOf<Byte> > CMediaDrm::DecryptNative(
    /* [in] */ IMediaDrm* drm,
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ ArrayOf<Byte>* keyId,
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ ArrayOf<Byte>* iv)
{

}

AutoPtr<ArrayOf<Byte> > CMediaDrm::SignNative(
    /* [in] */ IMediaDrm* drm,
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ ArrayOf<Byte>* keyId,
    /* [in] */ ArrayOf<Byte>* message)
{

}

Boolean CMediaDrm::VerifyNative(
    /* [in] */ IMediaDrm* drm,
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ ArrayOf<Byte>* keyId,
    /* [in] */ ArrayOf<Byte>* message,
    /* [in] */ ArrayOf<Byte>* signature)
{

}

AutoPtr<ArrayOf<Byte> > CMediaDrm::SignRSANative(
    /* [in] */ IMediaDrm* drm,
    /* [in] */ ArrayOf<Byte>* sessionId,
    /* [in] */ const String& algorithm,
    /* [in] */ ArrayOf<Byte>* wrappedKey,
    /* [in] */ ArrayOf<Byte>* message)
{

}

void CMediaDrm::NativeInit()
{

}

void CMediaDrm::NativeSetup(
    /* [in] */ IInterface* mediadrm_this,
    /* [in] */ ArrayOf<Byte>* uuid)
{

}

void CMediaDrm::NativeFinalize()
{

}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
