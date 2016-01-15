
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.KeyStore.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/net/http/CX509TrustManagerExtensions.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/utility/CPairHelper.h"
#include "elastos/droid/webkit/native/net/X509Util.h"
#include "elastos/droid/webkit/native/net/CertVerifyStatusElastos.h"
#include "elastos/droid/webkit/native/net/api/X509Util_dec.h"
#include "elastos/core/AutoLock.h"
#include "elastos/core/IntegralToString.h"
#include <elastos/utility/logging/Logger.h>
//#include "elastos/security/CKeyStoreHelper.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::KeyStore::Security::IKeyChain;
using Elastos::Droid::Net::Http::CX509TrustManagerExtensions;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::IPairHelper;
using Elastos::Droid::Webkit::Net::CertVerifyStatusElastos;
using Elastos::Core::AutoLock;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IntegralToString;
using Elastos::Core::ISystem;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CFile;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Logger;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;
//using Elastos::Security::CKeyStoreHelper;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Security::IKeyStoreHelper;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::IPrincipal;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastosx::Net::Ssl::CTrustManagerFactoryHelper;
using Elastosx::Net::Ssl::ITrustManager;
using Elastosx::Net::Ssl::ITrustManagerFactory;
using Elastosx::Net::Ssl::ITrustManagerFactoryHelper;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

//=====================================================================
//              X509Util::TrustStorageListener
//=====================================================================
ECode X509Util::TrustStorageListener::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // ==================before translated======================
    // if (intent.getAction().equals(KeyChain.ACTION_STORAGE_CHANGED)) {
    //     try {
    //         reloadDefaultTrustManager();
    //     }
    //     catch (CertificateException e) {
    //         Log.e(TAG, "Unable to reload the default TrustManager", e);
    //     }
    //     catch (KeyStoreException e) {
    //         Log.e(TAG, "Unable to reload the default TrustManager", e);
    //     }
    //     catch (NoSuchAlgorithmException e) {
    //         Log.e(TAG, "Unable to reload the default TrustManager", e);
    //     }
    // }

    String action;
    intent->GetAction(&action);
    if (action.Equals(IKeyChain::ACTION_STORAGE_CHANGED)) {
        //try {
            ReloadDefaultTrustManager();
        //}
        //catch (CertificateException e) {
        //    Log.e(TAG, "Unable to reload the default TrustManager", e);
        //}
        //catch (KeyStoreException e) {
        //    Log.e(TAG, "Unable to reload the default TrustManager", e);
        //}
        //catch (NoSuchAlgorithmException e) {
        //    Log.e(TAG, "Unable to reload the default TrustManager", e);
        //}
    }
    return NOERROR;
}

//=====================================================================
//              X509Util::X509TrustManagerIceCreamSandwich
//=====================================================================
X509Util::X509TrustManagerIceCreamSandwich::X509TrustManagerIceCreamSandwich(
    /* [in] */ IX509TrustManager* trustManager)
    : mTrustManager(trustManager)
{
    // ==================before translated======================
    // mTrustManager = trustManager;
}

AutoPtr<IList> X509Util::X509TrustManagerIceCreamSandwich::CheckServerTrusted(
    /* [in] */ /*ArrayOf<ICertificate*>* chain, //*/ ArrayOf<IX509Certificate*>* chain,
    /* [in] */ const String& authType,
    /* [in] */ const String& host)
{
    // ==================before translated======================
    // mTrustManager.checkServerTrusted(chain, authType);
    // return Collections.<X509Certificate>emptyList();

    assert(0);
    AutoPtr< ArrayOf<ICertificate*> > chainTmp = ArrayOf<ICertificate*>::Alloc(chain->GetLength());
    for (Int32 idx=0; idx<chain->GetLength(); ++idx) {
        AutoPtr<IX509Certificate> itemTmp = (*chain)[idx];
        ICertificate* item = ICertificate::Probe(itemTmp.Get());
        chainTmp->Set(idx, item);
    }

    mTrustManager->CheckServerTrusted(chainTmp, authType);
    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);
    return result;
}

//=====================================================================
//                 X509Util::X509TrustManagerJellyBean
//=====================================================================
X509Util::X509TrustManagerJellyBean::X509TrustManagerJellyBean(
    /* [in] */ IX509TrustManager* trustManager)
{
    // ==================before translated======================
    // mTrustManagerExtensions = new X509TrustManagerExtensions(trustManager);

    assert(0);
    CX509TrustManagerExtensions::New(trustManager, (IX509TrustManagerExtensions**)&mTrustManagerExtensions);
}

AutoPtr<IList> X509Util::X509TrustManagerJellyBean::CheckServerTrusted(
    /* [in] */ ArrayOf<IX509Certificate*>* chain,
    /* [in] */ const String& authType,
    /* [in] */ const String& host)
{
    // ==================before translated======================
    // return mTrustManagerExtensions.checkServerTrusted(chain, authType, host);

    AutoPtr<IList> result;
    mTrustManagerExtensions->CheckServerTrusted(chain, authType, host, (IList**)&result);
    return result;
}

//=====================================================================
//                               X509Util
//=====================================================================
const String X509Util::TAG("X509Util");
AutoPtr<ICertificateFactory> X509Util::sCertificateFactory;
const String X509Util::OID_TLS_SERVER_AUTH("1.3.6.1.5.5.7.3.1");
const String X509Util::OID_ANY_EKU("2.5.29.37.0");
const String X509Util::OID_SERVER_GATED_NETSCAPE("2.16.840.1.113730.4.1");
const String X509Util::OID_SERVER_GATED_MICROSOFT("1.3.6.1.4.1.311.10.3.3");
AutoPtr<X509Util::X509TrustManagerImplementation> X509Util::sDefaultTrustManager;
AutoPtr<X509Util::TrustStorageListener> X509Util::sTrustStorageListener;
AutoPtr<X509Util::X509TrustManagerImplementation> X509Util::sTestTrustManager;
AutoPtr<IKeyStore> X509Util::sTestKeyStore;
AutoPtr<IKeyStore> X509Util::sSystemKeyStore;
AutoPtr<IFile> X509Util::sSystemCertificateDirectory;
AutoPtr<ISet> X509Util::sSystemTrustAnchorCache;
Boolean X509Util::sLoadedSystemKeyStore;
const AutoPtr<Object> X509Util::sLock = new Object();
Boolean X509Util::sDisableNativeCodeForTest = false;
AutoPtr< ArrayOf<Byte> > X509Util::HEX_DIGITS = X509Util::MiddleInitHexDigits();

AutoPtr<IX509Certificate> X509Util::CreateCertificateFromBytes(
    /* [in] */ ArrayOf<Byte>* derBytes)
{
    // ==================before translated======================
    // ensureInitialized();
    // return (X509Certificate) sCertificateFactory.generateCertificate(
    //         new ByteArrayInputStream(derBytes));

    EnsureInitialized();
    AutoPtr<IInputStream> inputStream;
    CByteArrayInputStream::New(derBytes, (IInputStream**)&inputStream);

    AutoPtr<ICertificate> cert;
    sCertificateFactory->GenerateCertificate(inputStream, (ICertificate**)&cert);
    AutoPtr<IX509Certificate> result = IX509Certificate::Probe(cert.Get());
    return result;
}

ECode X509Util::AddTestRootCertificate(
    /* [in] */ ArrayOf<Byte>* rootCertBytes)
{
    VALIDATE_NOT_NULL(rootCertBytes);
    // ==================before translated======================
    // ensureInitialized();
    // X509Certificate rootCert = createCertificateFromBytes(rootCertBytes);
    // synchronized(sLock) {
    //     sTestKeyStore.setCertificateEntry(
    //             "root_cert_" + Integer.toString(sTestKeyStore.size()), rootCert);
    //     reloadTestTrustManager();
    // }

    EnsureInitialized();
    AutoPtr<IX509Certificate> rootCert = CreateCertificateFromBytes(rootCertBytes);
    {
        AutoLock lock(sLock);
        Int32 size = 0;
        sTestKeyStore->GetSize(&size);
        String sSize = IntegralToString::ToString(size);

        ICertificate* certificate = ICertificate::Probe(rootCert);
        sTestKeyStore->SetCertificateEntry(String("root_cert_") + sSize, (ICertificate*)&certificate);
        ReloadTestTrustManager();
    }
    return NOERROR;
}

ECode X509Util::ClearTestRootCertificates()
{
    // ==================before translated======================
    // ensureInitialized();
    // synchronized(sLock) {
    //     try {
    //         sTestKeyStore.load(null);
    //         reloadTestTrustManager();
    //     } catch (IOException e) {
    //         // No IO operation is attempted.
    //     }
    // }

    assert(0);
    EnsureInitialized();
    AutoLock lock(sLock);
    //try {
        sTestKeyStore->Load(NULL, NULL);
        ReloadTestTrustManager();
    //} catch (IOException e) {
        // No IO operation is attempted.
    //}
    return NOERROR;
}

Boolean X509Util::VerifyKeyUsage(
    /* [in] */ IX509Certificate* certificate)
{
    // ==================before translated======================
    // List<String> ekuOids;
    // try {
    //     ekuOids = certificate.getExtendedKeyUsage();
    // } catch (NullPointerException e) {
    //     // getExtendedKeyUsage() can crash due to an Android platform bug. This probably
    //     // happens when the EKU extension data is malformed so return false here.
    //     // See http://crbug.com/233610
    //     return false;
    // }
    // if (ekuOids == null)
    //     return true;
    //
    // for (String ekuOid : ekuOids) {
    //     if (ekuOid.equals(OID_TLS_SERVER_AUTH) ||
    //         ekuOid.equals(OID_ANY_EKU) ||
    //         ekuOid.equals(OID_SERVER_GATED_NETSCAPE) ||
    //         ekuOid.equals(OID_SERVER_GATED_MICROSOFT)) {
    //         return true;
    //     }
    // }
    //
    // return false;

    assert(0);
    AutoPtr<IList> ekuOids;
    //try {
        certificate->GetExtendedKeyUsage((IList**)&ekuOids);
    //} catch (NullPointerException e) {
        // getExtendedKeyUsage() can crash due to an Android platform bug. This probably
        // happens when the EKU extension data is malformed so return false here.
        // See http://crbug.com/233610
    //    return false;
    //}
    if (NULL == ekuOids)
        return TRUE;

    String ekuOid;
    Int32 ekuSize = 0;
    ekuOids->GetSize(&ekuSize);
    for (Int32 i=0; i<ekuSize; ++i) {
        AutoPtr<IInterface> item;
        ekuOids->Get(i, (IInterface**)&item);

        // need say IX509Certificate's func GetExtendedKeyUsage how to set data to interface
        // think setting ICharSequence as String temp
        ICharSequence* charSequence = ICharSequence::Probe(item);
        charSequence->ToString(&ekuOid);
        if (ekuOid.Equals(OID_TLS_SERVER_AUTH) ||
            ekuOid.Equals(OID_ANY_EKU) ||
            ekuOid.Equals(OID_SERVER_GATED_NETSCAPE) ||
            ekuOid.Equals(OID_SERVER_GATED_MICROSOFT)) {
            return TRUE;
        }
    }

    return FALSE;
}

AutoPtr<ElastosCertVerifyResult> X509Util::VerifyServerCertificates(
    /* [in] */ ArrayOf< AutoPtr< ArrayOf<Byte> > >* certChain,
    /* [in] */ const String& authType,
    /* [in] */ const String& host)
{
    // ==================before translated======================
    // if (certChain == null || certChain.length == 0 || certChain[0] == null) {
    //     throw new IllegalArgumentException("Expected non-null and non-empty certificate " +
    //             "chain passed as |certChain|. |certChain|=" + Arrays.deepToString(certChain));
    // }
    //
    //
    // try {
    //     ensureInitialized();
    // } catch (CertificateException e) {
    //     return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_FAILED);
    // }
    //
    // X509Certificate[] serverCertificates = new X509Certificate[certChain.length];
    // try {
    //     for (int i = 0; i < certChain.length; ++i) {
    //         serverCertificates[i] = createCertificateFromBytes(certChain[i]);
    //     }
    // } catch (CertificateException e) {
    //     return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_UNABLE_TO_PARSE);
    // }
    //
    // // Expired and not yet valid certificates would be rejected by the trust managers, but the
    // // trust managers report all certificate errors using the general CertificateException. In
    // // order to get more granular error information, cert validity time range is being checked
    // // separately.
    // try {
    //     serverCertificates[0].checkValidity();
    //     if (!verifyKeyUsage(serverCertificates[0])) {
    //         return new AndroidCertVerifyResult(
    //                 CertVerifyStatusAndroid.VERIFY_INCORRECT_KEY_USAGE);
    //     }
    // } catch (CertificateExpiredException e) {
    //     return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_EXPIRED);
    // } catch (CertificateNotYetValidException e) {
    //     return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_NOT_YET_VALID);
    // } catch (CertificateException e) {
    //     return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_FAILED);
    // }
    //
    // synchronized(sLock) {
    //     // If no trust manager was found, fail without crashing on the null pointer.
    //     if (sDefaultTrustManager == null)
    //         return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_FAILED);
    //
    //     List<X509Certificate> verifiedChain;
    //     try {
    //         verifiedChain = sDefaultTrustManager.checkServerTrusted(serverCertificates,
    //                                                                 authType, host);
    //     } catch (CertificateException eDefaultManager) {
    //         try {
    //             verifiedChain = sTestTrustManager.checkServerTrusted(serverCertificates,
    //                                                                  authType, host);
    //         } catch (CertificateException eTestManager) {
    //             // Neither of the trust managers confirms the validity of the certificate chain,
    //             // log the error message returned by the system trust manager.
    //             Log.i(TAG, "Failed to validate the certificate chain, error: " +
    //                       eDefaultManager.getMessage());
    //             return new AndroidCertVerifyResult(
    //                     CertVerifyStatusAndroid.VERIFY_NO_TRUSTED_ROOT);
    //         }
    //     }
    //
    //     boolean isIssuedByKnownRoot = false;
    //     if (verifiedChain.size() > 0) {
    //         X509Certificate root = verifiedChain.get(verifiedChain.size() - 1);
    //         isIssuedByKnownRoot = isKnownRoot(root);
    //     }
    //
    //     return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_OK,
    //                                        isIssuedByKnownRoot, verifiedChain);
    // }

    assert(0);

    // old if expression has throw, use assert instead temporary
    assert(NULL != certChain);
    assert(0 != certChain->GetLength());
    AutoPtr< ArrayOf<Byte> > firstItem = (*certChain)[0];
    assert(NULL != firstItem);

    //try {
        EnsureInitialized();
    //} catch (CertificateException e) {
    //    return new ElastosCertVerifyResult(CertVerifyStatusElastos.VERIFY_FAILED);
    //}

    AutoPtr< ArrayOf<IX509Certificate*> > serverCertificates = ArrayOf<IX509Certificate*>::Alloc(certChain->GetLength());
    //try {
        for (Int32 i = 0; i < certChain->GetLength(); ++i) {
            AutoPtr<IX509Certificate> item = CreateCertificateFromBytes((*certChain)[i]);
            serverCertificates->Set(i, item);
        }
    //} catch (CertificateException e) {
    //    return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_UNABLE_TO_PARSE);
    //}

    // Expired and not yet valid certificates would be rejected by the trust managers, but the
    // trust managers report all certificate errors using the general CertificateException. In
    // order to get more granular error information, cert validity time range is being checked
    // separately.
    //try {
        (*serverCertificates)[0]->CheckValidity();
        if (!VerifyKeyUsage((*serverCertificates)[0])) {
            AutoPtr<ElastosCertVerifyResult> ret = new ElastosCertVerifyResult(CertVerifyStatusElastos::VERIFY_INCORRECT_KEY_USAGE);
            return ret;
        }
    //} catch (CertificateExpiredException e) {
    //    return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_EXPIRED);
    //} catch (CertificateNotYetValidException e) {
    //    return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_NOT_YET_VALID);
    //} catch (CertificateException e) {
    //    return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_FAILED);
    //}

    AutoLock lock(sLock);
    // If no trust manager was found, fail without crashing on the null pointer.
    if (sDefaultTrustManager == NULL) {
        AutoPtr<ElastosCertVerifyResult> ret = new ElastosCertVerifyResult(CertVerifyStatusElastos::VERIFY_FAILED);
        return ret;
    }

    AutoPtr<IList> verifiedChain;
    //try {
        verifiedChain = sDefaultTrustManager->CheckServerTrusted(serverCertificates, authType, host);
    //} catch (CertificateException eDefaultManager) {
    //    try {
    //        verifiedChain = sTestTrustManager->CheckServerTrusted(serverCertificates, authType, host);
    //    } catch (CertificateException eTestManager) {
            // Neither of the trust managers confirms the validity of the certificate chain,
            // log the error message returned by the system trust manager.
    //        Log.i(TAG, "Failed to validate the certificate chain, error: " +
    //                  eDefaultManager.getMessage());
    //        return new AndroidCertVerifyResult(
    //                CertVerifyStatusAndroid.VERIFY_NO_TRUSTED_ROOT);
    //    }
    //}

    Boolean isIssuedByKnownRoot = FALSE;
    Int32 verifiedSize = 0;
    verifiedChain->GetSize(&verifiedSize);
    if (verifiedSize > 0) {
        AutoPtr<IInterface> tmp;
        verifiedChain->Get(verifiedSize - 1, (IInterface**)&tmp);
        AutoPtr<IX509Certificate> root = IX509Certificate::Probe(tmp.Get());
        isIssuedByKnownRoot = IsKnownRoot((IX509Certificate*)&root);
    }

    AutoPtr<ElastosCertVerifyResult> ret = new ElastosCertVerifyResult(CertVerifyStatusElastos::VERIFY_OK,
        isIssuedByKnownRoot, verifiedChain);
    return ret;
}

ECode X509Util::SetDisableNativeCodeForTest(
    /* [in] */ Boolean disabled)
{
    // ==================before translated======================
    // sDisableNativeCodeForTest = disabled;

    sDisableNativeCodeForTest = disabled;
    return NOERROR;
}

ECode X509Util::EnsureInitialized()
{
    // ==================before translated======================
    // synchronized(sLock) {
    //     if (sCertificateFactory == null) {
    //         sCertificateFactory = CertificateFactory.getInstance("X.509");
    //     }
    //     if (sDefaultTrustManager == null) {
    //         sDefaultTrustManager = X509Util.createTrustManager(null);
    //     }
    //     if (!sLoadedSystemKeyStore) {
    //         try {
    //             sSystemKeyStore = KeyStore.getInstance("AndroidCAStore");
    //             try {
    //                 sSystemKeyStore.load(null);
    //             } catch (IOException e) {
    //                 // No IO operation is attempted.
    //             }
    //             sSystemCertificateDirectory =
    //                     new File(System.getenv("ANDROID_ROOT") + "/etc/security/cacerts");
    //         } catch (KeyStoreException e) {
    //             // Could not load AndroidCAStore. Continue anyway; isKnownRoot will always
    //             // return false.
    //         }
    //         if (!sDisableNativeCodeForTest)
    //             nativeRecordCertVerifyCapabilitiesHistogram(sSystemKeyStore != null);
    //         sLoadedSystemKeyStore = true;
    //     }
    //     if (sSystemTrustAnchorCache == null) {
    //         sSystemTrustAnchorCache = new HashSet<Pair<X500Principal, PublicKey>>();
    //     }
    //     if (sTestKeyStore == null) {
    //         sTestKeyStore = KeyStore.getInstance(KeyStore.getDefaultType());
    //         try {
    //             sTestKeyStore.load(null);
    //         } catch (IOException e) {
    //             // No IO operation is attempted.
    //         }
    //     }
    //     if (sTestTrustManager == null) {
    //         sTestTrustManager = X509Util.createTrustManager(sTestKeyStore);
    //     }
    //     if (!sDisableNativeCodeForTest && sTrustStorageListener == null) {
    //         sTrustStorageListener = new TrustStorageListener();
    //         nativeGetApplicationContext().registerReceiver(sTrustStorageListener,
    //                 new IntentFilter(KeyChain.ACTION_STORAGE_CHANGED));
    //     }
    // }

    assert(0);
    AutoLock lock(sLock);
    if (sCertificateFactory == NULL) {
        AutoPtr<ICertificateFactoryHelper> helper;
        CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
        helper->GetInstance(String("X.509"), (ICertificateFactory**)&sCertificateFactory);
    }
    if (sDefaultTrustManager == NULL) {
        sDefaultTrustManager = X509Util::CreateTrustManager(NULL);
    }
    if (!sLoadedSystemKeyStore) {
        //try {
            AutoPtr<IKeyStoreHelper> helper;
            //--: CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&helper);
            helper->GetInstance(String("AndroidCAStore"), (IKeyStore**)&sSystemKeyStore);
            //try {
                sSystemKeyStore->Load(NULL, NULL);
            //} catch (IOException e) {
                // No IO operation is attempted.
            //}
            String sysEnv;
            AutoPtr<ISystem> sys;
            CSystem::AcquireSingleton((ISystem**)&sys);
            sys->GetEnv(String("ANDROID_ROOT") + String("/etc/security/cacerts"), &sysEnv);
            CFile::New(sysEnv, (IFile**)&sSystemCertificateDirectory);
        //} catch (KeyStoreException e) {
            // Could not load AndroidCAStore. Continue anyway; isKnownRoot will always
            // return false.
        //}
        if (FALSE != sDisableNativeCodeForTest)
            NativeRecordCertVerifyCapabilitiesHistogram(sSystemKeyStore != NULL);
        sLoadedSystemKeyStore = TRUE;
    }
    if (NULL == sSystemTrustAnchorCache) {
        CHashSet::New((ISet**)&sSystemTrustAnchorCache);
    }

    if (NULL == sTestKeyStore) {
        AutoPtr<IKeyStoreHelper> keyStore;
        //--: CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&keyStore);
        String defaultType;
        keyStore->GetDefaultType(&defaultType);
        keyStore->GetInstance(defaultType, (IKeyStore**)&sTestKeyStore);
        //try {
            sTestKeyStore->Load(NULL, NULL);
        //} catch (IOException e) {
            // No IO operation is attempted.
        //}
    }
    if (NULL == sTestTrustManager) {
        sTestTrustManager = X509Util::CreateTrustManager(sTestKeyStore);
    }
    if (!sDisableNativeCodeForTest && NULL == sTrustStorageListener) {
        sTrustStorageListener = new TrustStorageListener();

        AutoPtr<IIntentFilter> intentFilter;
        CIntentFilter::New(IKeyChain::ACTION_STORAGE_CHANGED, (IIntentFilter**)&intentFilter);

        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);

        NativeGetApplicationContext()->RegisterReceiver((IBroadcastReceiver*)&sTrustStorageListener,
            (IIntentFilter*)&intentFilter, (IIntent**)&intent);
    }

    return NOERROR;
}

AutoPtr<X509Util::X509TrustManagerImplementation> X509Util::CreateTrustManager(
    /* [in] */ IKeyStore* keyStore)
{
    // ==================before translated======================
    // String algorithm = TrustManagerFactory.getDefaultAlgorithm();
    // TrustManagerFactory tmf = TrustManagerFactory.getInstance(algorithm);
    // tmf.init(keyStore);
    //
    // for (TrustManager tm : tmf.getTrustManagers()) {
    //     if (tm instanceof X509TrustManager) {
    //         try {
    //             if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
    //                 return new X509TrustManagerJellyBean((X509TrustManager) tm);
    //             } else {
    //                 return new X509TrustManagerIceCreamSandwich((X509TrustManager) tm);
    //             }
    //         } catch (IllegalArgumentException e) {
    //             String className = tm.getClass().getName();
    //             Log.e(TAG, "Error creating trust manager (" + className + "): " + e);
    //         }
    //     }
    // }
    // Log.e(TAG, "Could not find suitable trust manager");
    // return null;

    assert(0);
    AutoPtr<ITrustManagerFactoryHelper> helper;
    CTrustManagerFactoryHelper::AcquireSingleton((ITrustManagerFactoryHelper**)&helper);
    String algorithm;
    helper->GetDefaultAlgorithm(&algorithm);

    AutoPtr<ITrustManagerFactory> tmf;
    helper->GetInstance(algorithm, (ITrustManagerFactory**)&tmf);
    tmf->Init((IKeyStore*)&keyStore);

    AutoPtr< ArrayOf< AutoPtr<ITrustManager> > > managers;
    tmf->GetTrustManagers((ArrayOf<ITrustManager*>**)&managers);
    for (Int32 i=0; i<managers->GetLength(); ++i) {
        AutoPtr<ITrustManager> tm = (*managers)[i];
        IX509TrustManager* x509tm = IX509TrustManager::Probe(tm);
        if (NULL != x509tm) {
            //try {
                if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN_MR1) {
                    AutoPtr<X509TrustManagerImplementation> ret = new X509TrustManagerJellyBean((IX509TrustManager*)&x509tm);
                    return ret;
                }
                else {
                    AutoPtr<X509TrustManagerImplementation> ret = new X509TrustManagerIceCreamSandwich((IX509TrustManager*)&x509tm);
                    return ret;
                }
            //} catch (IllegalArgumentException e) {
            //    String className = tm.getClass().getName();
            //    Log.e(TAG, "Error creating trust manager (" + className + "): " + e);
            //}
        }
    }

    Logger::E(TAG, "Could not find suitable trust manager");
    return NULL;
}

ECode X509Util::ReloadTestTrustManager()
{
    // ==================before translated======================
    // sTestTrustManager = X509Util.createTrustManager(sTestKeyStore);

    sTestTrustManager = X509Util::CreateTrustManager(sTestKeyStore);
    return NOERROR;
}

ECode X509Util::ReloadDefaultTrustManager()
{
    // ==================before translated======================
    // sDefaultTrustManager = null;
    // sSystemTrustAnchorCache = null;
    // nativeNotifyKeyChainChanged();
    // ensureInitialized();

    sDefaultTrustManager = NULL;
    sSystemTrustAnchorCache = NULL;
    NativeNotifyKeyChainChanged();
    EnsureInitialized();
    return NOERROR;
}

AutoPtr< ArrayOf<Byte> > X509Util::MiddleInitHexDigits()
{
    // ==================before translated======================
    // ->WWZ_SIGN: ARRAY_INIT_START {
    // '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    //          'a', 'b', 'c', 'd', 'e', 'f',
    // ->WWZ_SIGN: ARRAY_INIT_END }

    Byte tmps[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f'};
    const Int32 length = sizeof(tmps) / sizeof(tmps[0]);
    AutoPtr< ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(length);
    for (Int32 i=0; i<length; ++i) {
        result->Set(i, tmps[i]);
    }
    return result;
}

String X509Util::HashPrincipal(
    /* [in] */ IX500Principal* principal)
{
    // ==================before translated======================
    // // Android hashes a principal as the first four bytes of its MD5 digest, encoded in
    // // lowercase hex and reversed. Verified in 4.2, 4.3, and 4.4.
    // byte[] digest = MessageDigest.getInstance("MD5").digest(principal.getEncoded());
    // char[] hexChars = new char[8];
    // for (int i = 0; i < 4; i++) {
    //     hexChars[2 * i] = HEX_DIGITS[(digest[3 - i] >> 4) & 0xf];
    //     hexChars[2 * i + 1] = HEX_DIGITS[digest[3 - i] & 0xf];
    // }
    // return new String(hexChars);

    // Android hashes a principal as the first four bytes of its MD5 digest, encoded in
    // lowercase hex and reversed. Verified in 4.2, 4.3, and 4.4.
    AutoPtr<IMessageDigestHelper> helper;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
    AutoPtr<IMessageDigest> msgDigest;
    helper->GetInstance(String("MD5"), (IMessageDigest**)&msgDigest);

    AutoPtr< ArrayOf<Byte> > encoded;
    principal->GetEncoded((ArrayOf<Byte>**)&encoded);

    AutoPtr< ArrayOf<Byte> > digest;
    msgDigest->Digest((ArrayOf<Byte>*)&encoded, (ArrayOf<Byte>**)&digest);

    AutoPtr< ArrayOf<Byte> > dig = ArrayOf<Byte>::Alloc(8);
    for (Int32 i = 0; i < 4; ++i) {
        (*dig)[2 * i] = (*HEX_DIGITS)[((*HEX_DIGITS)[3 - i] >> 4) & 0xf];
        (*dig)[2 * i + 1] = (*HEX_DIGITS)[(*HEX_DIGITS)[3 - i] & 0xf];
    }

    String result(*dig);
    return result;
}

Boolean X509Util::IsKnownRoot(
    /* [in] */ IX509Certificate* root)
{
    // ==================before translated======================
    // // Could not find the system key store. Conservatively report false.
    // if (sSystemKeyStore == null)
    //     return false;
    //
    // // Check the in-memory cache first; avoid decoding the anchor from disk
    // // if it has been seen before.
    // Pair<X500Principal, PublicKey> key =
    //     new Pair<X500Principal, PublicKey>(root.getSubjectX500Principal(), root.getPublicKey());
    // if (sSystemTrustAnchorCache.contains(key))
    //     return true;
    //
    // // Note: It is not sufficient to call sSystemKeyStore.getCertificiateAlias. If the server
    // // supplies a copy of a trust anchor, X509TrustManagerExtensions returns the server's
    // // version rather than the system one. getCertificiateAlias will then fail to find an anchor
    // // name. This is fixed upstream in https://android-review.googlesource.com/#/c/91605/
    // //
    // // TODO(davidben): When the change trickles into an Android release, query sSystemKeyStore
    // // directly.
    //
    // // System trust anchors are stored under a hash of the principal. In case of collisions,
    // // a number is appended.
    // String hash = hashPrincipal(root.getSubjectX500Principal());
    // for (int i = 0; true; i++) {
    //     String alias = hash + '.' + i;
    //     if (!new File(sSystemCertificateDirectory, alias).exists())
    //         break;
    //
    //     Certificate anchor = sSystemKeyStore.getCertificate("system:" + alias);
    //     // It is possible for this to return null if the user deleted a trust anchor. In
    //     // that case, the certificate remains in the system directory but is also added to
    //     // another file. Continue iterating as there may be further collisions after the
    //     // deleted anchor.
    //     if (anchor == null)
    //         continue;
    //
    //     if (!(anchor instanceof X509Certificate)) {
    //         // This should never happen.
    //         String className = anchor.getClass().getName();
    //         Log.e(TAG, "Anchor " + alias + " not an X509Certificate: " + className);
    //         continue;
    //     }
    //
    //     // If the subject and public key match, this is a system root.
    //     X509Certificate anchorX509 = (X509Certificate)anchor;
    //     if (root.getSubjectX500Principal().equals(anchorX509.getSubjectX500Principal()) &&
    //         root.getPublicKey().equals(anchorX509.getPublicKey())) {
    //         sSystemTrustAnchorCache.add(key);
    //         return true;
    //     }
    // }
    //
    // return false;

    assert(0);
    // Could not find the system key store. Conservatively report false.
    if (sSystemKeyStore == NULL)
        return FALSE;

    // Check the in-memory cache first; avoid decoding the anchor from disk
    // if it has been seen before.
    AutoPtr<IX500Principal> principal;
    root->GetSubjectX500Principal((IX500Principal**)&principal);

    ICertificate* certificate = ICertificate::Probe(root);
    AutoPtr<IPublicKey> publicKey;
    certificate->GetPublicKey((IPublicKey**)&publicKey);

    AutoPtr<IPairHelper> helper;
    CPairHelper::AcquireSingleton((IPairHelper**)&helper);
    AutoPtr<IPair> key;
    helper->Create(principal, publicKey, (IPair**)&key);
    Boolean contain = FALSE;
    sSystemTrustAnchorCache->Contains(key, &contain);
    if (contain)
        return TRUE;

    // Note: It is not sufficient to call sSystemKeyStore.getCertificiateAlias. If the server
    // supplies a copy of a trust anchor, X509TrustManagerExtensions returns the server's
    // version rather than the system one. getCertificiateAlias will then fail to find an anchor
    // name. This is fixed upstream in https://android-review.googlesource.com/#/c/91605/
    //
    // TODO(davidben): When the change trickles into an Android release, query sSystemKeyStore
    // directly.

    // System trust anchors are stored under a hash of the principal. In case of collisions,
    // a number is appended.
    String hash = HashPrincipal(principal);
    AutoPtr<IX509Certificate> x509Cert = NULL;
    for (Int32 i = 0; TRUE; ++i) {
        String alias = hash + String(".") + Char32(i);

        AutoPtr<IFile> tmpFile;
        CFile::New(sSystemCertificateDirectory, alias, (IFile**)&tmpFile);

        Boolean exists = FALSE;
        tmpFile->Exists(&exists);
        if (!exists)
            break;

        AutoPtr<ICertificate> anchor;
        sSystemKeyStore->GetCertificate(String("system:") + alias, (ICertificate**)&anchor);
        // It is possible for this to return null if the user deleted a trust anchor. In
        // that case, the certificate remains in the system directory but is also added to
        // another file. Continue iterating as there may be further collisions after the
        // deleted anchor.
        if (anchor == NULL)
            continue;

        x509Cert = IX509Certificate::Probe(anchor);
        if (NULL != x509Cert) {
            // This should never happen.
            //String className = anchor.getClass().getName();
            String className("X509Certificate");
            Logger::E(TAG, String("Anchor ") + alias + String(" not an X509Certificate: ") + className);
            continue;
        }

        // If the subject and public key match, this is a system root.
        AutoPtr<IX500Principal> anchorx509Principal;
        x509Cert->GetSubjectX500Principal((IX500Principal**)&anchorx509Principal);

        AutoPtr<IPublicKey> anchorx509PublicKey;
        anchor->GetPublicKey((IPublicKey**)&anchorx509PublicKey);

        IPrincipal* principalTmp = IPrincipal::Probe(principal);
        Boolean equalPrincipal = FALSE;
        principalTmp->Equals(anchorx509Principal, &equalPrincipal);

        if (equalPrincipal && TO_IINTERFACE(publicKey) == TO_IINTERFACE(anchorx509PublicKey)) {
            sSystemTrustAnchorCache->Add(key);
            return TRUE;
        }
    }

    return FALSE;
}

ECode X509Util::NativeNotifyKeyChainChanged()
{
    Elastos_X509Util_nativeNotifyKeyChainChanged();
    return NOERROR;
}

ECode X509Util::NativeRecordCertVerifyCapabilitiesHistogram(
    /* [in] */ Boolean foundSystemTrustRoots)
{
    Elastos_X509Util_nativeRecordCertVerifyCapabilitiesHistogram(foundSystemTrustRoots);
    return NOERROR;
}

AutoPtr<IContext> X509Util::NativeGetApplicationContext()
{
    AutoPtr<IInterface> c = Elastos_X509Util_nativeGetApplicationContext();
    IContext* context = IContext::Probe(c);
    return context;
}

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

