
#ifndef __ELASTOS_DROID_MEDIA_CMEDIADRM_H__
#define __ELASTOS_DROID_MEDIA_CMEDIADRM_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_CMediaDrm.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * MediaDrm can be used to obtain keys for decrypting protected media streams, in
 * conjunction with {@link android.media.MediaCrypto}.  The MediaDrm APIs
 * are designed to support the ISO/IEC 23001-7: Common Encryption standard, but
 * may also be used to implement other encryption schemes.
 * <p>
 * Encrypted content is prepared using an encryption server and stored in a content
 * library. The encrypted content is streamed or downloaded from the content library to
 * client devices via content servers.  Licenses to view the content are obtained from
 * a License Server.
 * <p>
 * <p><img src="../../../images/mediadrm_overview.png"
 *      alt="MediaDrm Overview diagram"
 *      border="0" /></p>
 * <p>
 * Keys are requested from the license server using a key request. The key
 * response is delivered to the client app, which provides the response to the
 * MediaDrm API.
 * <p>
 * A Provisioning server may be required to distribute device-unique credentials to
 * the devices.
 * <p>
 * Enforcing requirements related to the number of devices that may play content
 * simultaneously can be performed either through key renewal or using the secure
 * stop methods.
 * <p>
 * The following sequence diagram shows the interactions between the objects
 * involved while playing back encrypted content:
 * <p>
 * <p><img src="../../../images/mediadrm_decryption_sequence.png"
 *         alt="MediaDrm Overview diagram"
 *         border="0" /></p>
 * <p>
 * The app first constructs {@link android.media.MediaExtractor} and
 * {@link android.media.MediaCodec} objects. It accesses the DRM-scheme-identifying UUID,
 * typically from metadata in the content, and uses this UUID to construct an instance
 * of a MediaDrm object that is able to support the DRM scheme required by the content.
 * Crypto schemes are assigned 16 byte UUIDs.  The method {@link #isCryptoSchemeSupported}
 * can be used to query if a given scheme is supported on the device.
 * <p>
 * The app calls {@link #openSession} to generate a sessionId that will uniquely identify
 * the session in subsequent interactions. The app next uses the MediaDrm object to
 * obtain a key request message and send it to the license server, then provide
 * the server's response to the MediaDrm object.
 * <p>
 * Once the app has a sessionId, it can construct a MediaCrypto object from the UUID and
 * sessionId.  The MediaCrypto object is registered with the MediaCodec in the
 * {@link MediaCodec.#configure} method to enable the codec to decrypt content.
 * <p>
 * When the app has constructed {@link android.media.MediaExtractor},
 * {@link android.media.MediaCodec} and {@link android.media.MediaCrypto} objects,
 * it proceeds to pull samples from the extractor and queue them into the decoder.  For
 * encrypted content, the samples returned from the extractor remain encrypted, they
 * are only decrypted when the samples are delivered to the decoder.
 * <p>
 * <a name="Callbacks"></a>
 * <h3>Callbacks</h3>
 * <p>Applications should register for informational events in order
 * to be informed of key state updates during playback or streaming.
 * Registration for these events is done via a call to
 * {@link #setOnEventListener}. In order to receive the respective
 * callback associated with this listener, applications are required to create
 * MediaDrm objects on a thread with its own Looper running (main UI
 * thread by default has a Looper running).
 */
CarClass(CMediaDrm)
    , public Object
    , public IMediaDrm
{
public:
    /**
     * Contains the opaque data an app uses to request keys from a license server
     */
    class KeyRequest
        : public Object
        , public IMediaDrmKeyRequest
    {
    public:
        KeyRequest()
        {}

        virtual ~KeyRequest()
        {}

        CAR_INTERFACE_DECL()

        /**
         * Get the opaque message data
         */
        CARAPI GetData(
            /* [out, callee] */ ArrayOf<Byte>** result);

        /**
         * Get the default URL to use when sending the key request message to a
         * server, if known.  The app may prefer to use a different license
         * server URL from other sources.
         */
        CARAPI GetDefaultUrl(
            /* [out] */ String* result);

    private:
        AutoPtr<ArrayOf<Byte> > mData;
        String mDefaultUrl;
    };

    /**
     * Contains the opaque data an app uses to request a certificate from a provisioning
     * server
     */
    class ProvisionRequest
        : public Object
        , public IMediaDrmProvisionRequest
    {
    public:
        ProvisionRequest()
        {}

        virtual ~ProvisionRequest()
        {}

        CAR_INTERFACE_DECL()

        /**
         * Get the opaque message data
         */
        CARAPI GetData(
            /* [out, callee] */ ArrayOf<Byte>** result);

        /**
         * Get the default URL to use when sending the key request message to a
         * server, if known.  The app may prefer to use a different license
         * server URL from other sources.
         */
        CARAPI GetDefaultUrl(
            /* [out] */ String* result);

    private:
        AutoPtr<ArrayOf<Byte> > mData;
        String mDefaultUrl;
    };

    /**
     * In addition to supporting decryption of DASH Common Encrypted Media, the
     * MediaDrm APIs provide the ability to securely deliver session keys from
     * an operator's session key server to a client device, based on the factory-installed
     * root of trust, and then perform encrypt, decrypt, sign and verify operations
     * with the session key on arbitrary user data.
     * <p>
     * The CryptoSession class implements generic encrypt/decrypt/sign/verify methods
     * based on the established session keys.  These keys are exchanged using the
     * getKeyRequest/provideKeyResponse methods.
     * <p>
     * Applications of this capability could include securing various types of
     * purchased or private content, such as applications, books and other media,
     * photos or media delivery protocols.
     * <p>
     * Operators can create session key servers that are functionally similar to a
     * license key server, except that instead of receiving license key requests and
     * providing encrypted content keys which are used specifically to decrypt A/V media
     * content, the session key server receives session key requests and provides
     * encrypted session keys which can be used for general purpose crypto operations.
     * <p>
     * A CryptoSession is obtained using {@link #getCryptoSession}
     */
    class CryptoSession
        : public Object
        , public IMediaDrmCryptoSession
    {
    public:
        CryptoSession(
            /* [in] */ CMediaDrm* drm,
            /* [in] */ ArrayOf<Byte>* sessionId,
            /* [in] */ const String& cipherAlgorithm,
            /* [in] */ const String& macAlgorithm);

        virtual ~CryptoSession()
        {}

        CAR_INTERFACE_DECL()

        /**
         * Encrypt data using the CryptoSession's cipher algorithm
         *
         * @param keyid specifies which key to use
         * @param input the data to encrypt
         * @param iv the initialization vector to use for the cipher
         */
        CARAPI Encrypt(
            /* [in] */ ArrayOf<Byte>* keyid,
            /* [in] */ ArrayOf<Byte>* input,
            /* [in] */ ArrayOf<Byte>* iv,
            /* [out, callee] */ ArrayOf<Byte>** result);

        /**
         * Decrypt data using the CryptoSessions's cipher algorithm
         *
         * @param keyid specifies which key to use
         * @param input the data to encrypt
         * @param iv the initialization vector to use for the cipher
         */
        CARAPI Decrypt(
            /* [in] */ ArrayOf<Byte>* keyid,
            /* [in] */ ArrayOf<Byte>* input,
            /* [in] */ ArrayOf<Byte>* iv,
            /* [out, callee] */ ArrayOf<Byte>** result);

        /**
         * Sign data using the CryptoSessions's mac algorithm.
         *
         * @param keyid specifies which key to use
         * @param message the data for which a signature is to be computed
         */
        CARAPI Sign(
            /* [in] */ ArrayOf<Byte>* keyid,
            /* [in] */ ArrayOf<Byte>* message,
            /* [out, callee] */ ArrayOf<Byte>** result);

        /**
         * Verify a signature using the CryptoSessions's mac algorithm. Return true
         * if the signatures match, false if they do no.
         *
         * @param keyid specifies which key to use
         * @param message the data to verify
         * @param signature the reference signature which will be compared with the
         *        computed signature
         */
        CARAPI Verify(
            /* [in] */ ArrayOf<Byte>* keyid,
            /* [in] */ ArrayOf<Byte>* message,
            /* [in] */ ArrayOf<Byte>* signature,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<ArrayOf<Byte> > mSessionId;
        CMediaDrm* mDrm;
    };

    /**
     * Contains the opaque data an app uses to request a certificate from a provisioning
     * server
     *
     * @hide - not part of the public API at this time
     */
    class CertificateRequest
        : public Object
        , public IMediaDrmCertificateRequest
    {
    public:
        CertificateRequest(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ const String& defaultUrl);


        virtual ~CertificateRequest()
        {}

        CAR_INTERFACE_DECL()

        /**
         * Get the opaque message data
         */
        CARAPI GetData(
            /* [out, callee] */ ArrayOf<Byte>** result);

        /**
         * Get the default URL to use when sending the key request message to a
         * server, if known.  The app may prefer to use a different license
         * server URL from other sources.
         */
        CARAPI GetDefaultUrl(
            /* [out] */ String* result);

    private:
        AutoPtr<ArrayOf<Byte> > mData;
        String mDefaultUrl;
    };

    /**
     * Contains the wrapped private key and public certificate data associated
     * with a certificate.
     *
     * @hide - not part of the public API at this time
     */
    class Certificate
        : public Object
        , public IMediaDrmCertificate
    {
    public:
        Certificate()
        {}

        virtual ~Certificate()
        {}

        CAR_INTERFACE_DECL()

        /**
         * Get the wrapped private key data
         */
        CARAPI GetWrappedPrivateKey(
            /* [out, callee] */ ArrayOf<Byte>** result);

        /**
         * Get the PEM-encoded certificate chain
         */
        CARAPI GetContent(
            /* [out, callee] */ ArrayOf<Byte>** result);

    private:
        AutoPtr<ArrayOf<Byte> > mWrappedKey;
        AutoPtr<ArrayOf<Byte> > mCertificateData;
    };

private:
    class EventHandler
        : public Handler
    {
    public:
        EventHandler(
            /* [in] */ CMediaDrm* host,
            /* [in] */ ILooper* looper)
                : Handler(looper)
                , mHost(host)
            {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CMediaDrm* mHost;
    };

public:
    CMediaDrm();

    virtual ~CMediaDrm();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Instantiate a MediaDrm object
     *
     * @param uuid The UUID of the crypto scheme.
     *
     * @throws UnsupportedSchemeException if the device does not support the
     * specified scheme UUID
     */
    CARAPI constructor(
        /* [in] */ IUUID* uuid);

    /**
     * Register a callback to be invoked when an event occurs
     *
     * @param listener the callback that will be run
     */
    CARAPI SetOnEventListener(
        /* [in] */ IMediaDrmOnEventListener* listener);

    /**
     * Open a new session with the MediaDrm object.  A session ID is returned.
     *
     * @throws NotProvisionedException if provisioning is needed
     * @throws ResourceBusyException if required resources are in use
     */
    CARAPI OpenSession(
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Close a session on the MediaDrm object that was previously opened
     * with {@link #openSession}.
     */
    CARAPI CloseSession(
        /* [in] */ ArrayOf<Byte>* sessionId);

    /**
     * A key request/response exchange occurs between the app and a license server
     * to obtain or release keys used to decrypt encrypted content.
     * <p>
     * getKeyRequest() is used to obtain an opaque key request byte array that is
     * delivered to the license server.  The opaque key request byte array is returned
     * in KeyRequest.data.  The recommended URL to deliver the key request to is
     * returned in KeyRequest.defaultUrl.
     * <p>
     * After the app has received the key request response from the server,
     * it should deliver to the response to the DRM engine plugin using the method
     * {@link #provideKeyResponse}.
     *
     * @param scope may be a sessionId or a keySetId, depending on the specified keyType.
     * When the keyType is KEY_TYPE_STREAMING or KEY_TYPE_OFFLINE,
     * scope should be set to the sessionId the keys will be provided to.  When the keyType
     * is KEY_TYPE_RELEASE, scope should be set to the keySetId of the keys
     * being released. Releasing keys from a device invalidates them for all sessions.
     * @param init container-specific data, its meaning is interpreted based on the
     * mime type provided in the mimeType parameter.  It could contain, for example,
     * the content ID, key ID or other data obtained from the content metadata that is
     * required in generating the key request. init may be null when keyType is
     * KEY_TYPE_RELEASE.
     * @param mimeType identifies the mime type of the content
     * @param keyType specifes the type of the request. The request may be to acquire
     * keys for streaming or offline content, or to release previously acquired
     * keys, which are identified by a keySetId.
     * @param optionalParameters are included in the key request message to
     * allow a client application to provide additional message parameters to the server.
     *
     * @throws NotProvisionedException if reprovisioning is needed, due to a
     * problem with the certifcate
     */
    CARAPI GetKeyRequest(
        /* [in] */ ArrayOf<Byte>* scope,
        /* [in] */ ArrayOf<Byte>* init,
        /* [in] */ const String& mimeType,
        /* [in] */ Int32 keyType,
        /* [in] */ IHashMap* optionalParameters,
        /* [out] */ IMediaDrmKeyRequest** result);

    /**
     * A key response is received from the license server by the app, then it is
     * provided to the DRM engine plugin using provideKeyResponse.  When the
     * response is for an offline key request, a keySetId is returned that can be
     * used to later restore the keys to a new session with the method
     * {@link #restoreKeys}.
     * When the response is for a streaming or release request, null is returned.
     *
     * @param scope may be a sessionId or keySetId depending on the type of the
     * response.  Scope should be set to the sessionId when the response is for either
     * streaming or offline key requests.  Scope should be set to the keySetId when
     * the response is for a release request.
     * @param response the byte array response from the server
     *
     * @throws NotProvisionedException if the response indicates that
     * reprovisioning is required
     * @throws DeniedByServerException if the response indicates that the
     * server rejected the request
     * @throws ResourceBusyException if required resources are in use
     */
    CARAPI ProvideKeyResponse(
        /* [in] */ ArrayOf<Byte>* scope,
        /* [in] */ ArrayOf<Byte>* response,
        /* [out, callee] */ ArrayOf<Byte>** result);


    /**
     * Restore persisted offline keys into a new session.  keySetId identifies the
     * keys to load, obtained from a prior call to {@link #provideKeyResponse}.
     *
     * @param sessionId the session ID for the DRM session
     * @param keySetId identifies the saved key set to restore
     */
    CARAPI RestoreKeys(
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [in] */ ArrayOf<Byte>* keySetId);

    /**
     * Remove the current keys from a session.
     *
     * @param sessionId the session ID for the DRM session
     */
    CARAPI RemoveKeys(
        /* [in] */ ArrayOf<Byte>* sessionId);

    /**
     * Request an informative description of the key status for the session.  The status is
     * in the form of {name, value} pairs.  Since DRM license policies vary by vendor,
     * the specific status field names are determined by each DRM vendor.  Refer to your
     * DRM provider documentation for definitions of the field names for a particular
     * DRM engine plugin.
     *
     * @param sessionId the session ID for the DRM session
     */
    CARAPI QueryKeyStatus(
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [out] */ IHashMap** result);

    /**
     * A provision request/response exchange occurs between the app and a provisioning
     * server to retrieve a device certificate.  If provisionining is required, the
     * EVENT_PROVISION_REQUIRED event will be sent to the event handler.
     * getProvisionRequest is used to obtain the opaque provision request byte array that
     * should be delivered to the provisioning server. The provision request byte array
     * is returned in ProvisionRequest.data. The recommended URL to deliver the provision
     * request to is returned in ProvisionRequest.defaultUrl.
     */
    CARAPI GetProvisionRequest(
        /* [out] */ IMediaDrmProvisionRequest** result);

    /**
     * After a provision response is received by the app, it is provided to the DRM
     * engine plugin using this method.
     *
     * @param response the opaque provisioning response byte array to provide to the
     * DRM engine plugin.
     *
     * @throws DeniedByServerException if the response indicates that the
     * server rejected the request
     */
    CARAPI ProvideProvisionResponse(
        /* [in] */ ArrayOf<Byte>* response);

    /**
     * Remove provisioning from a device.  Only system apps may unprovision a
     * device.  Note that removing provisioning will invalidate any keys saved
     * for offline use (KEY_TYPE_OFFLINE), which may render downloaded content
     * unplayable until new licenses are acquired.  Since provisioning is global
     * to the device, license invalidation will apply to all content downloaded
     * by any app, so appropriate warnings should be given to the user.
     * @hide
     */
    CARAPI UnprovisionDevice();

    /**
     * A means of enforcing limits on the number of concurrent streams per subscriber
     * across devices is provided via SecureStop. This is achieved by securely
     * monitoring the lifetime of sessions.
     * <p>
     * Information from the server related to the current playback session is written
     * to persistent storage on the device when each MediaCrypto object is created.
     * <p>
     * In the normal case, playback will be completed, the session destroyed and the
     * Secure Stops will be queried. The app queries secure stops and forwards the
     * secure stop message to the server which verifies the signature and notifies the
     * server side database that the session destruction has been confirmed. The persisted
     * record on the client is only removed after positive confirmation that the server
     * received the message using releaseSecureStops().
     */
    CARAPI GetSecureStops(
        /* [out] */ IList** result);


    /**
     * Process the SecureStop server response message ssRelease.  After authenticating
     * the message, remove the SecureStops identified in the response.
     *
     * @param ssRelease the server response indicating which secure stops to release
     */
    CARAPI ReleaseSecureStops(
        /* [in] */ ArrayOf<Byte>* ssRelease);

    /**
     * Read a DRM engine plugin String property value, given the property name string.
     * <p>
     * Standard fields names are:
     * {@link #PROPERTY_VENDOR}, {@link #PROPERTY_VERSION},
     * {@link #PROPERTY_DESCRIPTION}, {@link #PROPERTY_ALGORITHMS}
     */
    CARAPI GetPropertyString(
        /* [in] */ const String& propertyName,
        /* [out] */ String* result);

    /**
     * Read a DRM engine plugin byte array property value, given the property name string.
     * <p>
     * Standard fields names are {@link #PROPERTY_DEVICE_UNIQUE_ID}
     */
    CARAPI GetPropertyByteArray(
        /* [in] */ const String& propertyName,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * Set a DRM engine plugin String property value.
     */
    CARAPI SetPropertyString(
        /* [in] */ const String& propertyName,
        /* [in] */ const String& value);

    /**
     * Set a DRM engine plugin byte array property value.
     */
    CARAPI SetPropertyByteArray(
        /* [in] */ const String& propertyName,
        /* [in] */ ArrayOf<Byte>* value);

    /**
     * Obtain a CryptoSession object which can be used to encrypt, decrypt,
     * sign and verify messages or data using the session keys established
     * for the session using methods {@link #getKeyRequest} and
     * {@link #provideKeyResponse} using a session key server.
     *
     * @param sessionId the session ID for the session containing keys
     * to be used for encrypt, decrypt, sign and/or verify
     * @param cipherAlgorithm the algorithm to use for encryption and
     * decryption ciphers. The algorithm string conforms to JCA Standard
     * Names for Cipher Transforms and is case insensitive.  For example
     * "AES/CBC/NoPadding".
     * @param macAlgorithm the algorithm to use for sign and verify
     * The algorithm string conforms to JCA Standard Names for Mac
     * Algorithms and is case insensitive.  For example "HmacSHA256".
     * <p>
     * The list of supported algorithms for a DRM engine plugin can be obtained
     * using the method {@link #getPropertyString} with the property name
     * "algorithms".
     */
    CARAPI GetCryptoSession(
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [in] */ const String& cipherAlgorithm,
        /* [in] */ const String& macAlgorithm,
        /* [out] */ IMediaDrmCryptoSession** result);

    /**
     * Generate a certificate request, specifying the certificate type
     * and authority. The response received should be passed to
     * provideCertificateResponse.
     *
     * @param certType Specifies the certificate type.
     *
     * @param certAuthority is passed to the certificate server to specify
     * the chain of authority.
     *
     * @hide - not part of the public API at this time
     */
    CARAPI GetCertificateRequest(
        /* [in] */ Int32 certType,
        /* [in] */ const String& certAuthority,
        /* [out] */ IMediaDrmCertificateRequest** result);

    /**
     * Process a response from the certificate server.  The response
     * is obtained from an HTTP Post to the url provided by getCertificateRequest.
     * <p>
     * The public X509 certificate chain and wrapped private key are returned
     * in the returned Certificate objec.  The certificate chain is in PEM format.
     * The wrapped private key should be stored in application private
     * storage, and used when invoking the signRSA method.
     *
     * @param response the opaque certificate response byte array to provide to the
     * DRM engine plugin.
     *
     * @throws DeniedByServerException if the response indicates that the
     * server rejected the request
     *
     * @hide - not part of the public API at this time
     */
    CARAPI ProvideCertificateResponse(
        /* [in] */ ArrayOf<Byte>* response,
        /* [out] */ IMediaDrmCertificate** result);

    /**
     * Sign data using an RSA key
     *
     * @param sessionId a sessionId obtained from openSession on the MediaDrm object
     * @param algorithm the signing algorithm to use, e.g. "PKCS1-BlockType1"
     * @param wrappedKey - the wrapped (encrypted) RSA private key obtained
     * from provideCertificateResponse
     * @param message the data for which a signature is to be computed
     *
     * @hide - not part of the public API at this time
     */
    CARAPI SignRSA(
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [in] */ const String& algorithm,
        /* [in] */ ArrayOf<Byte>* wrappedKey,
        /* [in] */ ArrayOf<Byte>* message,
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI ReleaseResources();

    /**
     * Query if the given scheme identified by its UUID is supported on
     * this device.
     * @param uuid The UUID of the crypto scheme.
     */
    CARAPI IsCryptoSchemeSupported(
        /* [in] */ IUUID* uuid,
        /* [out] */ Boolean* result);

    /**
     * Query if the given scheme identified by its UUID is supported on
     * this device, and whether the drm plugin is able to handle the
     * media container format specified by mimeType.
     * @param uuid The UUID of the crypto scheme.
     * @param mimeType The MIME type of the media container, e.g. "video/mp4"
     *   or "video/webm"
     */
    CARAPI IsCryptoSchemeSupported(
        /* [in] */ IUUID* uuid,
        /* [in] */ const String& mimeType,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) GetByteArrayFromUUID(
        /* [in] */ IUUID* uuid);

    static CARAPI_(Boolean) IsCryptoSchemeSupportedNative(
        /* [in] */ ArrayOf<Byte>* uuid,
        /* [in] */ const String& mimeType);

    /*
     * This method is called from native code when an event occurs.  This method
     * just uses the EventHandler system to post the event back to the main app thread.
     * We use a weak reference to the original MediaPlayer object so that the native
     * code is safe from the object disappearing from underneath it.  (This is
     * the cookie passed to native_setup().)
     */
    static CARAPI_(void) PostEventFromNative(
        /* [in] */ IInterface* mediadrm_ref,
        /* [in] */ Int32 eventType,
        /* [in] */ Int32 extra,
        /* [in] */ IInterface* obj);

    CARAPI_(AutoPtr<ProvisionRequest>) GetProvisionRequestNative(
        /* [in] */ Int32 certType,
        /* [in] */ const String& certAuthority);

    CARAPI_(AutoPtr<Certificate>) ProvideProvisionResponseNative(
        /* [in] */ ArrayOf<Byte>* response);

    static CARAPI_(void) SetCipherAlgorithmNative(
        /* [in] */ IMediaDrm* drm,
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [in] */ const String& algorithm);

    static CARAPI_(void) SetMacAlgorithmNative(
        /* [in] */ IMediaDrm* drm,
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [in] */ const String& algorithm);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) EncryptNative(
        /* [in] */ IMediaDrm* drm,
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [in] */ ArrayOf<Byte>* keyId,
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ ArrayOf<Byte>* iv);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) DecryptNative(
        /* [in] */ IMediaDrm* drm,
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [in] */ ArrayOf<Byte>* keyId,
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ ArrayOf<Byte>* iv);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) SignNative(
        /* [in] */ IMediaDrm* drm,
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [in] */ ArrayOf<Byte>* keyId,
        /* [in] */ ArrayOf<Byte>* message);

    static CARAPI_(Boolean) VerifyNative(
        /* [in] */ IMediaDrm* drm,
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [in] */ ArrayOf<Byte>* keyId,
        /* [in] */ ArrayOf<Byte>* message,
        /* [in] */ ArrayOf<Byte>* signature);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) SignRSANative(
        /* [in] */ IMediaDrm* drm,
        /* [in] */ ArrayOf<Byte>* sessionId,
        /* [in] */ const String& algorithm,
        /* [in] */ ArrayOf<Byte>* wrappedKey,
        /* [in] */ ArrayOf<Byte>* message);

    static CARAPI_(void) NativeInit();

    CARAPI_(void) NativeSetup(
        /* [in] */ IInterface* mediadrm_this,
        /* [in] */ ArrayOf<Byte>* uuid);

    CARAPI_(void) NativeFinalize();

private:
    static const String TAG;

    static const Int32 DRM_EVENT;

    static const String PERMISSION;

    AutoPtr<EventHandler> mEventHandler;
    AutoPtr<IMediaDrmOnEventListener> mOnEventListener;

    Int64 mNativeContext;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIADRM_H__
