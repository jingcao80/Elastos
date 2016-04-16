// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_ANDROIDNETWORKLIBRARY_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_ANDROIDNETWORKLIBRARY_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/net/ElastosCertVerifyResult.h"

// package org.chromium.net;
// import android.content.ActivityNotFoundException;
// import android.content.Context;
// import android.content.Intent;
// import android.security.KeyChain;
// import android.telephony.TelephonyManager;
// import android.util.Log;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.CalledByNativeUnchecked;
// import java.net.Inet6Address;
// import java.net.InetAddress;
// import java.net.InterfaceAddress;
// import java.net.NetworkInterface;
// import java.net.SocketException;
// import java.net.URLConnection;
// import java.security.KeyStoreException;
// import java.security.NoSuchAlgorithmException;
// import java.security.cert.CertificateException;
// import java.util.Enumeration;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Webkit::Webview::Chromium::Net::ElastosCertVerifyResult;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

/**
  * This class implements net utilities required by the net component.
  */
class ElastosNetworkLibrary
    : public Object
{
public:
    /**
      * Stores the key pair through the CertInstaller activity.
      * @param context current application context.
      * @param publicKey The public key bytes as DER-encoded SubjectPublicKeyInfo (X.509)
      * @param privateKey The private key as DER-encoded PrivateKeyInfo (PKCS#8).
      * @return: true on success, false on failure.
      *
      * Note that failure means that the function could not launch the CertInstaller
      * activity. Whether the keys are valid or properly installed will be indicated
      * by the CertInstaller UI itself.
      */
    // @CalledByNative
    static CARAPI_(Boolean) StoreKeyPair(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<Byte>* publicKey,
        /* [in] */ ArrayOf<Byte>* privateKey);

    /**
       * Adds a cryptographic file (User certificate, a CA certificate or
       * PKCS#12 keychain) through the system's CertInstaller activity.
       *
       * @param context current application context.
       * @param certType cryptographic file type. E.g. CertificateMimeType.X509_USER_CERT
       * @param data certificate/keychain data bytes.
       * @return true on success, false on failure.
       *
       * Note that failure only indicates that the function couldn't launch the
       * CertInstaller activity, not that the certificate/keychain was properly
       * installed to the keystore.
       */
    // @CalledByNative
    static CARAPI_(Boolean) StoreCertificate(
        /* [in] */ IContext* context,
        /* [in] */ Int32 certType,
        /* [in] */ ArrayOf<Byte>* data);

    /**
      * @return the mime type (if any) that is associated with the file
      *         extension. Returns null if no corresponding mime type exists.
      */
    // @CalledByNative
    static CARAPI_(String) GetMimeTypeFromExtension(
        /* [in] */ const String& extension);

    /**
      * @return true if it can determine that only loopback addresses are
      *         configured. i.e. if only 127.0.0.1 and ::1 are routable. Also
      *         returns false if it cannot determine this.
      */
    // @CalledByNative
    static CARAPI_(Boolean) HaveOnlyLoopbackAddresses();

    /**
      * @return the network interfaces list (if any) string. The items in
      *         the list string are delimited by a new line, each item
      *         is tab separated network interface name, address with network
      *         prefix length and network interface index.
      *         as "name\taddress/prefix\tindex". e.g.
      *           eth0\t10.0.0.2/8\t5\neth0\tfe80::5054:ff:fe12:3456/16\t5
      *         represents a network list string with two items.
      */
    // @CalledByNative
    static CARAPI_(String) GetNetworkList();

    /**
      * Validate the server's certificate chain is trusted. Note that the caller
      * must still verify the name matches that of the leaf certificate.
      *
      * @param certChain The ASN.1 DER encoded bytes for certificates.
      * @param authType The key exchange algorithm name (e.g. RSA).
      * @param host The hostname of the server.
      * @return Android certificate verification result code.
      */
    // @CalledByNative return AndroidCertVerifyResult
    static CARAPI_(AutoPtr<IInterface>) VerifyServerCertificates(
        /* [in] */ ArrayOf< AutoPtr< ArrayOf<Byte> > >* certChain,
        /* [in] */ const String& authType,
        /* [in] */ const String& host);

    /**
      * Adds a test root certificate to the local trust store.
      * @param rootCert DER encoded bytes of the certificate.
      */
    // @CalledByNativeUnchecked
    static CARAPI_(void) AddTestRootCertificate(
        /* [in] */ ArrayOf<Byte>* rootCert);

    /**
      * Removes all test root certificates added by |addTestRootCertificate| calls from the local
      * trust store.
      */
    // @CalledByNativeUnchecked
    static CARAPI_(void) ClearTestRootCertificates();

    static CARAPI_(void*) ElaElastosNetworkLibraryCallback_Init();

private:
    static CARAPI_(Boolean) StoreKeyPair(
        /* [in] */ IInterface* context,
        /* [in] */ ArrayOf<Byte>* publicKey,
        /* [in] */ ArrayOf<Byte>* privateKey);

    static CARAPI_(Boolean) StoreCertificate(
        /* [in] */ IInterface* context,
        /* [in] */ Int32 certType,
        /* [in] */ ArrayOf<Byte>* data);

    static CARAPI_(String) GetNetworkCountryIso(
        /* [in] */ IInterface* context);

    static CARAPI_(String) GetNetworkOperator(
        /* [in] */ IInterface* context);

    /**
      * Returns the ISO country code equivalent of the current MCC.
      */
    // @CalledByNative
    static CARAPI_(String) GetNetworkCountryIso(
        /* [in] */ IContext* context);

    /**
      * Returns the MCC+MNC (mobile country code + mobile network code) as
      * the numeric name of the current registered operator.
      */
    // @CalledByNative
    static CARAPI_(String) GetNetworkOperator(
        /* [in] */ IContext* context);

private:
    static const String TAG;
};

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NET_ANDROIDNETWORKLIBRARY_H_

