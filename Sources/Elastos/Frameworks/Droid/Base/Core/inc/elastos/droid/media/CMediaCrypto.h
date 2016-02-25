
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACRYPTO_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACRYPTO_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_CMediaCrypto.h"
#include "elastos/droid/ext/frameworkext.h"
#include <binder/IServiceManager.h>
#include <elastos/core/Object.h>
#include <media/ICrypto.h>
#include <media/IMediaPlayerService.h>
#include <utils/RefBase.h>

using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Media {

struct ICrypto;

struct JCrypto : public android::RefBase {
    static Boolean IsCryptoSchemeSupported(const uint8_t uuid[16]);

    JCrypto(IWeakReference* obj, const uint8_t uuid[16], const void *initData, size_t initSize);

    android::status_t initCheck() const;

    Boolean requiresSecureDecoderComponent(const char *mime) const;

    static android::sp<android::ICrypto> GetCrypto(IMediaCrypto* obj);

protected:
    virtual ~JCrypto();

private:
    AutoPtr<IWeakReference> mObject;
    android::sp<android::ICrypto> mCrypto;

    static android::sp<android::ICrypto> MakeCrypto();

    static android::sp<android::ICrypto> MakeCrypto(
            const uint8_t uuid[16], const void *initData, size_t initSize);

    DISALLOW_EVIL_CONSTRUCTORS(JCrypto);
};


CarClass(CMediaCrypto)
    , public Object
    , public IMediaCrypto
{
public:
    CMediaCrypto();

    virtual ~CMediaCrypto();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Instantiate a MediaCrypto object using opaque, crypto scheme specific
     * data.
     * @param uuid The UUID of the crypto scheme.
     * @param initData Opaque initialization data specific to the crypto scheme.
     */
    CARAPI constructor(
        /* [in] */ IUUID* uuid,
        /* [in] */ ArrayOf<Byte>* initData);

    /**
     * Query if the given scheme identified by its UUID is supported on
     * this device.
     * @param uuid The UUID of the crypto scheme.
     */
    static CARAPI IsCryptoSchemeSupported(
        /* [in] */ IUUID* uuid,
        /* [out] */ Boolean* result);

    /**
     * Query if the crypto scheme requires the use of a secure decoder
     * to decode data of the given mime type.
     * @param mime The mime type of the media data
     */
    CARAPI RequiresSecureDecoderComponent(
        /* [in] */ const String& mime,
        /* [out] */ Boolean* result);

    CARAPI ReleaseResources();

protected:
    CARAPI_(void) Finalize();

private:
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) GetByteArrayFromUUID(
        /* [in] */ IUUID* uuid);

    static CARAPI_(Boolean) IsCryptoSchemeSupportedNative(
        /* [in] */ ArrayOf<Byte>* uuid);

    CARAPI NativeSetup(
        /* [in] */ ArrayOf<Byte>* uuid,
        /* [in] */ ArrayOf<Byte>* initData);

    CARAPI NativeFinalize();

    android::sp<JCrypto> SetCrypto(
       const android::sp<JCrypto> &crypto);

public:
    Int64 mNativeContext;

private:
    static Boolean mInitStatic;
    static const String TAG;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIACRYPTO_H__
