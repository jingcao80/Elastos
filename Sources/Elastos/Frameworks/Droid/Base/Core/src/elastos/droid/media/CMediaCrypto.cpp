#include "elastos/droid/media/CMediaCrypto.h"
#include "elastos/droid/media/Media_Utils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <media/stagefright/MediaCodecList.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaCrypto, Object, IMediaCrypto)

CAR_OBJECT_IMPL(CMediaCrypto)

const String CMediaCrypto::TAG("CMediaCrypto");

struct fields_t {
    //jfieldID context;
    Handle32 context;
};

static fields_t gFields;

static android::sp<JCrypto> getCrypto()
{
    return (JCrypto *)gFields.context;
}

//===========================================================
//              JCrypto
//===========================================================

JCrypto::JCrypto(IWeakReference* wr, const uint8_t uuid[16], const void *initData, size_t initSize)
{
    mObject = wr;
    mCrypto = MakeCrypto(uuid, initData, initSize);
}

JCrypto::~JCrypto()
{
    mCrypto.clear();
    mObject = NULL;
}

android::sp<android::ICrypto> JCrypto::MakeCrypto()
{
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();

    android::sp<android::IBinder> binder =
        sm->getService(android::String16("media.player"));

    android::sp<android::IMediaPlayerService> service =
        android::interface_cast<android::IMediaPlayerService>(binder);

    if (service == NULL) {
        return NULL;
    }

    android::sp<android::ICrypto> crypto = service->makeCrypto();

    if (crypto == NULL || crypto->initCheck() != android::OK) {
        return NULL;
    }

    return crypto;
}

// static
android::sp<android::ICrypto> JCrypto::MakeCrypto(
    const uint8_t uuid[16], const void *initData, size_t initSize)
{
    android::sp<android::ICrypto> crypto = MakeCrypto();

    if (crypto == NULL) {
        return NULL;
    }

    android::status_t err = crypto->createPlugin(uuid, initData, initSize);

    if (err != android::OK) {
        return NULL;
    }

    return crypto;
}

Boolean JCrypto::requiresSecureDecoderComponent(const char *mime) const
{
    if (mCrypto == NULL) {
        return FALSE;
    }

    return mCrypto->requiresSecureDecoderComponent(mime);
}

// static
Boolean JCrypto::IsCryptoSchemeSupported(const uint8_t uuid[16])
{
    android::sp<android::ICrypto> crypto = MakeCrypto();

    if (crypto == NULL) {
        return FALSE;
    }

    return crypto->isCryptoSchemeSupported(uuid);
}

android::status_t JCrypto::initCheck() const
{
    return mCrypto == NULL ? android::NO_INIT : android::OK;
}

// static

android::sp<android::ICrypto> JCrypto::GetCrypto()
{
    android::sp<JCrypto> jcrypto = getCrypto();
    if (jcrypto == NULL) {
        return NULL;
    }

    return jcrypto->mCrypto;
}

//===========================================================
//              CMediaCrypto
//===========================================================

static void NativeInit()
{
}

static Boolean InitStatic()
{
    NativeInit();
    return TRUE;
}

Boolean CMediaCrypto::mInitStatic = InitStatic();

android::sp<JCrypto> CMediaCrypto::SetCrypto(
    const android::sp<JCrypto> &crypto)
{
    android::sp<JCrypto> old = (JCrypto *)gFields.context;
    if (crypto != NULL) {
        crypto->incStrong(this);
    }
    if (old != NULL) {
        old->decStrong(this);
    }
    gFields.context = (Handle32)crypto.get();

    return old;
}

CMediaCrypto::CMediaCrypto()
{
}

CMediaCrypto::~CMediaCrypto()
{
    Finalize();
}

ECode CMediaCrypto::constructor()
{
    return NOERROR;
}

ECode CMediaCrypto::constructor(
    /* [in] */ IUUID* uuid,
    /* [in] */ ArrayOf<Byte>* initData)
{
    AutoPtr<ArrayOf<Byte> > bytes = GetByteArrayFromUUID(uuid);
    return NativeSetup(bytes, initData);
}

ECode CMediaCrypto::IsCryptoSchemeSupported(
    /* [in] */ IUUID* uuid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > bytes = GetByteArrayFromUUID(uuid);
    *result = IsCryptoSchemeSupportedNative(bytes);
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > CMediaCrypto::GetByteArrayFromUUID(
    /* [in] */ IUUID* uuid)
{
    Int64 msb, lsb;
    uuid->GetMostSignificantBits(&msb);
    uuid->GetLeastSignificantBits(&lsb);

    AutoPtr<ArrayOf<Byte> > uuidBytes = ArrayOf<Byte>::Alloc(16);
    for (int i = 0; i < 8; ++i) {
        uuidBytes->Set(i, (byte)(msb >> (8 * (7 - i))));
        uuidBytes->Set(i, (byte)(lsb >> (8 * (7 - i))));
    }

    return uuidBytes;
}

Boolean CMediaCrypto::IsCryptoSchemeSupportedNative(
    /* [in] */ ArrayOf<Byte>* uuid)
{
    if (uuid == NULL || uuid->GetLength() != 16) {
        Slogger::E(TAG, "IsCryptoSchemeSupportedNative uuidLength != 16");
        return FALSE;
    }

    return JCrypto::IsCryptoSchemeSupported(
        (uint8_t *)uuid->GetPayload());
}

ECode CMediaCrypto::RequiresSecureDecoderComponent(
    /* [in] */ const String& mime,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mime.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::sp<JCrypto> crypto = getCrypto();
    if (crypto == NULL) {
        Slogger::E(TAG, "crypto is NULL.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = crypto->requiresSecureDecoderComponent(mime.string());
    return NOERROR;
}

void CMediaCrypto::Finalize()
{
    NativeFinalize();
}

ECode CMediaCrypto::ReleaseResources()
{
     SetCrypto(NULL);
     return NOERROR;
}

ECode CMediaCrypto::NativeSetup(
    /* [in] */ ArrayOf<Byte>* uuidObj,
    /* [in] */ ArrayOf<Byte>* initDataObj)
{
    VALIDATE_NOT_NULL(uuidObj);

    Int32 uuidLength = uuidObj->GetLength();
    if (uuidLength != 16) {
        Slogger::E(TAG, "NativeSetup uuidLength != 16");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Byte *uuid = uuidObj->GetPayload();

    Int32 initDataLength = 0;
    Byte *initData = NULL;

    if (initDataObj != NULL) {
        initDataLength = initDataObj->GetLength();
        initData = initDataObj->GetPayload();
    }

    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    android::sp<JCrypto> crypto = new JCrypto(wr, (const uint8_t *)uuid, initData, initDataLength);

    android::status_t err = crypto->initCheck();

    if (err != android::OK) {
        Slogger::E(TAG, "Failed to instantiate crypto object.");
        return E_CRYPTO_EXCEPTION;
    }

    SetCrypto(crypto);
    return NOERROR;
}

ECode CMediaCrypto::NativeFinalize()
{
    SetCrypto(NULL);
    return NOERROR;
}


} // namespace Media
} // namepsace Droid
} // namespace Elastos
