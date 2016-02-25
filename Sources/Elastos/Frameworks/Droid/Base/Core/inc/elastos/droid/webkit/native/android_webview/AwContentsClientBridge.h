#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWCONTENTSCLIENTBRIDGE_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWCONTENTSCLIENTBRIDGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Webkit.h>
#include "elastos/droid/webkit/native/net/ElastosPrivateKey.h"
#include "elastos/droid/webkit/native/net/DefaultElastosKeyStore.h"
#include "elastos/droid/webkit/native/android_webview/ClientCertLookupTable.h"

using Elastos::Droid::Webkit::IValueCallback;
using Elastos::Droid::Webkit::Net::ElastosPrivateKey;
using Elastos::Droid::Webkit::Net::DefaultElastosKeyStore;
using Elastos::Core::IRunnable;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
using Elastos::Security::IPrincipal;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Cert::IX509Certificate;
// import java.security.cert.CertificateEncodingException;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {
class AwContentsClient;

/**
 * This class handles the JNI communication logic for the the AwContentsClient class.
 * Both the Java and the native peers of AwContentsClientBridge are owned by the
 * corresponding AwContents instances. This class and its native peer are connected
 * via weak references. The native AwContentsClientBridge sets up and clear these weak
 * references.
 */
//@JNINamespace("android_webview")
class AwContentsClientBridge
    : public Object
{
public:
    /**
     * Callback to communicate clientcertificaterequest back to the AwContentsClientBridge.
     * The public methods should be called on UI thread.
     * A request can not be proceeded, ignored  or canceled more than once. Doing this
     * is a programming error and causes an exception.
     */
    class ClientCertificateRequestCallback
        : public Object
    {
    public:
        class ProceedRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL();
            ProceedRunnable(
                /* [in] */ ClientCertificateRequestCallback* owner,
                /* [in] */ IPrivateKey* privateKey,
                /* [in] */ ArrayOf<IX509Certificate*>* chain);

            CARAPI Run();

        private:
            ClientCertificateRequestCallback* mOwner;
            IPrivateKey* mPrivateKey;
            ArrayOf<IX509Certificate*>* mChain;
        };

        class IgnoreRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL();
            IgnoreRunnable(
                /* [in] */ ClientCertificateRequestCallback* owner);

            CARAPI Run();

        private:
            ClientCertificateRequestCallback* mOwner;
        };

        class CancelRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL();
            CancelRunnable(
                /* [in] */ ClientCertificateRequestCallback* owner);

            CARAPI Run();

        private:
            ClientCertificateRequestCallback* mOwner;
        };

    public:
        ClientCertificateRequestCallback(
            /* [in] */ AwContentsClientBridge* owner,
            /* [in] */ Int32 id,
            /* [in] */ const String& host,
            /* [in] */ Int32 port);

        virtual CARAPI_(void) Proceed(
            /* [in] */ IPrivateKey* privateKey,
            /* [in] */ ArrayOf<IX509Certificate*>* chain);

        virtual CARAPI_(void) Ignore();

        virtual CARAPI_(void) Cancel();

    private:
        CARAPI_(void) ProceedOnUiThread(
            /* [in] */ IPrivateKey* privateKey,
            /* [in] */ ArrayOf<IX509Certificate*>* chain);

        CARAPI_(void) IgnoreOnUiThread();

        CARAPI_(void) CancelOnUiThread();

        CARAPI CheckIfCalled();

        CARAPI_(void) ProvideResponse(
            /* [in] */ ElastosPrivateKey* androidKey,
            /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* certChain);

    private:
        AwContentsClientBridge* mOwner;

        Int32 mId;
        String mHost;
        Int32 mPort;
        Boolean mIsCalled;
    };

private:
    class InnerValueCallback
        : public Object
        , public IValueCallback
    {
    public:
        CAR_INTERFACE_DECL();

    private:
        class InnerRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL();
            InnerRunnable(
                /* [in] */ InnerValueCallback* owner,
                /* [in] */ IInterface* value,
                /* [in] */ Int32 id);

            CARAPI Run();

        private:
            InnerValueCallback* mOwner;
            IInterface* mValue;
            Int32 mId;
        };

    public:
        InnerValueCallback(
            /* [in] */ AwContentsClientBridge* owner,
            /* [in] */ Int32 id);

        CARAPI OnReceiveValue(
            /* [in] */ IInterface* value);

    private:
        AwContentsClientBridge* mOwner;
        Int32 mId;
    };

public:
    static const String TAG;

    AwContentsClientBridge(
        /* [in] */ AwContentsClient* client,
        /* [in] */ DefaultElastosKeyStore* keyStore,
        /* [in] */ ClientCertLookupTable* table);

    virtual CARAPI_(void) ConfirmJsResult(
        /* [in] */ Int32 id,
        /* [in] */ const String& prompt);

    virtual CARAPI_(void) CancelJsResult(
        /* [in] */ Int32 id);

protected:
    // Used for mocking this class in tests.
    AwContentsClientBridge(
        /* [in] */ DefaultElastosKeyStore* keyStore,
        /* [in] */ ClientCertLookupTable* table);

    // Intentionally not private for testing the native peer of this class.
    //@CalledByNative
    virtual CARAPI_(void) SelectClientCertificate(
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* encodedPrincipals,
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

private:
    // Used by the native peer to set/reset a weak ref to the native peer.
    //@CalledByNative
    CARAPI_(void) SetNativeContentsClientBridge(
        /* [in] */ Int64 nativeContentsClientBridge);

    // If returns false, the request is immediately canceled, and any call to proceedSslError
    // has no effect. If returns true, the request should be canceled or proceeded using
    // proceedSslError().
    // Unlike the webview classic, we do not keep keep a database of certificates that
    // are allowed by the user, because this functionality is already handled via
    // ssl_policy in native layers.
    //@CalledByNative
    CARAPI_(Boolean) AllowCertificateError(
        /* [in] */ Int32 certError,
        /* [in] */ ArrayOf<Byte>* derBytes,
        /* [in] */ const String& url,
        /* [in] */ Int32 id);

    CARAPI_(void) ProceedSslError(
        /* [in] */ Boolean proceed,
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleJsAlert(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleJsConfirm(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleJsPrompt(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ const String& defaultValue,
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleJsBeforeUnload(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(Boolean) ShouldOverrideUrlLoading(
        /* [in] */ const String& url);

    //--------------------------------------------------------------------------------------------
    //  Native methods
    //--------------------------------------------------------------------------------------------
    CARAPI_(void) NativeProceedSslError(
        /* [in] */ Int64 nativeAwContentsClientBridge,
        /* [in] */ Boolean proceed,
        /* [in] */ Int32 id);

    CARAPI_(void) NativeProvideClientCertificateResponse(
        /* [in] */ Int64 nativeAwContentsClientBridge,
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* certChain,
        /* [in] */ ElastosPrivateKey* androidKey);

    CARAPI_(void) NativeConfirmJsResult(
        /* [in] */ Int64 nativeAwContentsClientBridge,
        /* [in] */ Int32 id,
        /* [in] */ const String& prompt);

    CARAPI_(void) NativeCancelJsResult(
        /* [in] */ Int64 nativeAwContentsClientBridge,
        /* [in] */ Int32 id);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwContentsClientBridgeCallback_Init();

private:
    static CARAPI_(void) SetNativeContentsClientBridge(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 nativeContentsClientBridge);

    static CARAPI_(Boolean) AllowCertificateError(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 certError,
        /* [in] */ ArrayOf<Byte>* derBytes,
        /* [in] */ const String& url,
        /* [in] */ Int32 id);

    static CARAPI_(void) SelectClientCertificate(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* encodedPrincipals,
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    static CARAPI_(void) HandleJsAlert(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleJsConfirm(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleJsPrompt(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ const String& defaultValue,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleJsBeforeUnload(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ Int32 id);

    static CARAPI_(Boolean) ShouldOverrideUrlLoading(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url);

private:
    AwContentsClientBridge(const AwContentsClientBridge&);
    AwContentsClientBridge& operator= (const AwContentsClientBridge&);
    AwContentsClient* mClient;
    // The native peer of this object.
    Int64 mNativeContentsClientBridge;

    AutoPtr<DefaultElastosKeyStore> mLocalKeyStore;

    AutoPtr<ClientCertLookupTable> mLookupTable;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWCONTENTSCLIENTBRIDGE_H__
