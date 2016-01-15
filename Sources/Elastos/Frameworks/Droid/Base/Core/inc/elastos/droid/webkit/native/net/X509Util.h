// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_NET_X509UTIL_H_
#define _ELASTOS_DROID_WEBKIT_NET_X509UTIL_H_

#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/net/ElastosCertVerifyResult.h"
#include "elastos/utility/etl/List.h"
#include "elastos/utility/etl/Pair.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::Http::IX509TrustManagerExtensions;
using Elastos::Droid::Webkit::Net::ElastosCertVerifyResult;
using Elastos::IO::IFile;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::IKeyStore;
using Elastos::Security::IPublicKey;
using Elastos::Utility::Etl::List;
using Elastos::Utility::ISet;
using Elastosx::Net::Ssl::IX509TrustManager;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

/**
  * Utility functions for verifying X.509 certificates.
  */
// @JNINamespace("net")
class X509Util
    : public Object
{
private:
    /**
      * Interface that wraps one of X509TrustManager or
      * X509TrustManagerExtensions to support platforms before the latter was
      * added.
      */
    class X509TrustManagerImplementation
        : public Object
    {
    public:
        virtual CARAPI_(AutoPtr<IList>) CheckServerTrusted(
            /* [in] */ ArrayOf<IX509Certificate*>* chain,
            /* [in] */ const String& authType,
            /* [in] */ const String& host) = 0;
    };

public:
    class TrustStorageListener
        : public BroadcastReceiver
    {
    public:
        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    };

    class X509TrustManagerIceCreamSandwich
        : public X509TrustManagerImplementation
    {
    public:
        X509TrustManagerIceCreamSandwich(
            /* [in] */ IX509TrustManager* trustManager);

        // @Override
        CARAPI_(AutoPtr<IList>) CheckServerTrusted(
            /* [in] */ ArrayOf<IX509Certificate*>* chain,
            /* [in] */ const String& authType,
            /* [in] */ const String& host);

    private:
        const AutoPtr<IX509TrustManager> mTrustManager;
    };

    class X509TrustManagerJellyBean
        : public X509TrustManagerImplementation
    {
    public:
        // @SuppressLint("NewApi")
        X509TrustManagerJellyBean(
            /* [in] */ IX509TrustManager* trustManager);

        // @Override
        CARAPI_(AutoPtr<IList>) CheckServerTrusted(
            /* [in] */ ArrayOf<IX509Certificate*>* chain,
            /* [in] */ const String& authType,
            /* [in] */ const String& host);

    private:
        /*const*/ AutoPtr<IX509TrustManagerExtensions> mTrustManagerExtensions;
    };
    /**
      * Convert a DER encoded certificate to an X509Certificate.
      */
    static CARAPI_(AutoPtr<IX509Certificate>) CreateCertificateFromBytes(
        /* [in] */ ArrayOf<Byte>* derBytes);

    static CARAPI AddTestRootCertificate(
        /* [in] */ ArrayOf<Byte>* rootCertBytes);

    static CARAPI ClearTestRootCertificates();

    /**
      * If an EKU extension is present in the end-entity certificate, it MUST contain either the
      * anyEKU or serverAuth or netscapeSGC or Microsoft SGC EKUs.
      *
      * @return true if there is no EKU extension or if any of the EKU extensions is one of the valid
      * OIDs for web server certificates.
      *
      * TODO(palmer): This can be removed after the equivalent change is made to the Android default
      * TrustManager and that change is shipped to a large majority of Android users.
      */
    static CARAPI_(Boolean) VerifyKeyUsage(
        /* [in] */ IX509Certificate* certificate);

    static CARAPI_(AutoPtr<ElastosCertVerifyResult>) VerifyServerCertificates(
        /* [in] */ ArrayOf< AutoPtr< ArrayOf<Byte> > >* certChain,
        /* [in] */ const String& authType,
        /* [in] */ const String& host);

    static CARAPI SetDisableNativeCodeForTest(
        /* [in] */ Boolean disabled);

private:
    /**
      * Ensures that the trust managers and certificate factory are initialized.
      */
    static CARAPI EnsureInitialized();

    /**
      * Creates a X509TrustManagerImplementation backed up by the given key
      * store. When null is passed as a key store, system default trust store is
      * used. Returns null if no created TrustManager was suitable.
      * @throws KeyStoreException, NoSuchAlgorithmException on error initializing the TrustManager.
      */
    static CARAPI_(AutoPtr<X509Util::X509TrustManagerImplementation>) CreateTrustManager(
        /* [in] */ IKeyStore* keyStore);

    /**
      * After each modification of test key store, trust manager has to be generated again.
      */
    static CARAPI ReloadTestTrustManager();

    /**
      * After each modification by the system of the key store, trust manager has to be regenerated.
      */
    static CARAPI ReloadDefaultTrustManager();

    static CARAPI_(AutoPtr< ArrayOf<Byte> >) MiddleInitHexDigits();

    static CARAPI_(String) HashPrincipal(
        /* [in] */ IX500Principal* principal);

    static CARAPI_(Boolean) IsKnownRoot(
        /* [in] */ IX509Certificate* root);

    /**
      * Notify the native net::CertDatabase instance that the system database has been updated.
      */
    static CARAPI NativeNotifyKeyChainChanged();

    /**
      * Record histograms on the platform's certificate verification capabilities.
      */
    static CARAPI NativeRecordCertVerifyCapabilitiesHistogram(
        /* [in] */ Boolean foundSystemTrustRoots);

    /**
      * Returns the application context.
      */
    static CARAPI_(AutoPtr<IContext>) NativeGetApplicationContext();

private:
    static const String TAG;
    static AutoPtr<ICertificateFactory> sCertificateFactory;
    static const String OID_TLS_SERVER_AUTH;
    static const String OID_ANY_EKU;
    // Server-Gated Cryptography (necessary to support a few legacy issuers):
    //    Netscape:
    static const String OID_SERVER_GATED_NETSCAPE;
    //    Microsoft:
    static const String OID_SERVER_GATED_MICROSOFT;
    /**
      * Trust manager backed up by the read-only system certificate store.
      */
    static AutoPtr<X509Util::X509TrustManagerImplementation> sDefaultTrustManager;
    /**
      * BroadcastReceiver that listens to change in the system keystore to invalidate certificate
      * caches.
      */
    static AutoPtr<TrustStorageListener> sTrustStorageListener;
    /**
      * Trust manager backed up by a custom certificate store. We need such manager to plant test
      * root CA to the trust store in testing.
      */
    static AutoPtr<X509Util::X509TrustManagerImplementation> sTestTrustManager;
    static AutoPtr<IKeyStore> sTestKeyStore;
    /**
      * The system key store. This is used to determine whether a trust anchor is a system trust
      * anchor or user-installed.
      */
    static AutoPtr<IKeyStore> sSystemKeyStore;
    /**
      * The directory where system certificates are stored. This is used to determine whether a
      * trust anchor is a system trust anchor or user-installed. The KeyStore API alone is not
      * sufficient to efficiently query whether a given X500Principal, PublicKey pair is a trust
      * anchor.
      */
    static AutoPtr<IFile> sSystemCertificateDirectory;
    /**
      * An in-memory cache of which trust anchors are system trust roots. This avoids reading and
      * decoding the root from disk on every verification. Mirrors a similar in-memory cache in
      * Conscrypt's X509TrustManager implementation.
      */
    static AutoPtr<ISet> sSystemTrustAnchorCache;
    /**
      * True if the system key store has been loaded. If the "AndroidCAStore" KeyStore instance
      * was not found, sSystemKeyStore may be null while sLoadedSystemKeyStore is true.
      */
    static Boolean sLoadedSystemKeyStore;
    /**
      * Lock object used to synchronize all calls that modify or depend on the trust managers.
      */
    static const AutoPtr<Object> sLock;
    /**
      * Allow disabling registering the observer and recording histograms for the certificate
      * changes. Net unit tests do not load native libraries which prevent this to succeed. Moreover,
      * the system does not allow to interact with the certificate store without user interaction.
      */
    static Boolean sDisableNativeCodeForTest;
    static AutoPtr< ArrayOf<Byte> > HEX_DIGITS;
};

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_NET_X509UTIL_H_

