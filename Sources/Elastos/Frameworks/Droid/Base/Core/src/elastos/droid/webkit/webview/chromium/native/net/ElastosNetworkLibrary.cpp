
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Content.h>
#include "Elastos.Droid.KeyStore.h"
#include "_Elastos.Droid.Core.h"
#include "_Elastos.Droid.Telephony.h"
//TODO: #include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/webkit/webview/chromium/native/net/ElastosNetworkLibrary.h"
#include "elastos/droid/webkit/webview/chromium/native/net/CertificateMimeType.h"
#include "elastos/droid/webkit/webview/chromium/native/net/api/ElastosNetworkLibrary_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/net/X509Util.h"
#include "elastos/core/IntegralToString.h"
#include "elastos/core/StringBuilder.h"
//TODO: #include "elastos/net/URLConnection.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::KeyStore::Security::IKeyChain;
//TODO: using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Webkit::Webview::Chromium::Net::CertificateMimeType;
using Elastos::Droid::Webkit::Webview::Chromium::Net::X509Util;
using Elastos::Core::IntegralToString;
using Elastos::Core::StringBuilder;
using Elastos::Net::CNetworkInterfaceHelper;
using Elastos::Net::EIID_IInet6Address;
using Elastos::Net::IInet6Address;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInterfaceAddress;
using Elastos::Net::INetworkInterface;
using Elastos::Net::INetworkInterfaceHelper;
//using Elastos::Net::URLConnection;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

//=====================================================================
//                        ElastosNetworkLibrary
//=====================================================================
const String ElastosNetworkLibrary::TAG("ElastosNetworkLibrary");

Boolean ElastosNetworkLibrary::StoreKeyPair(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<Byte>* publicKey,
    /* [in] */ ArrayOf<Byte>* privateKey)
{
    // ==================before translated======================
    // // TODO(digit): Use KeyChain official extra values to pass the public and private
    // // keys when they're available. The "KEY" and "PKEY" hard-coded constants were taken
    // // from the platform sources, since there are no official KeyChain.EXTRA_XXX definitions
    // // for them. b/5859651
    // try {
    //     Intent intent = KeyChain.createInstallIntent();
    //     intent.putExtra("PKEY", privateKey);
    //     intent.putExtra("KEY", publicKey);
    //     intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
    //     context.startActivity(intent);
    //     return true;
    // } catch (ActivityNotFoundException e) {
    //     Log.w(TAG, "could not store key pair: " + e);
    // }
    // return false;

    //try {
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        String privateKeyTmp(*publicKey);
        String publicKeyTmp(*publicKey);
        intent->PutExtra(String("PKEY"), privateKeyTmp);
        intent->PutExtra(String("KEY"), publicKeyTmp);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        context->StartActivity(intent);
        return TRUE;
    //} catch (ActivityNotFoundException e) {
    //    Log.w(TAG, "could not store key pair: " + e);
    //}
    return FALSE;
}

Boolean ElastosNetworkLibrary::StoreCertificate(
    /* [in] */ IContext* context,
    /* [in] */ Int32 certType,
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // try {
    //     Intent intent = KeyChain.createInstallIntent();
    //     intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
    //
    //     switch (certType) {
    //         case CertificateMimeType.X509_USER_CERT:
    //         case CertificateMimeType.X509_CA_CERT:
    //             intent.putExtra(KeyChain.EXTRA_CERTIFICATE, data);
    //             break;
    //
    //         case CertificateMimeType.PKCS12_ARCHIVE:
    //             intent.putExtra(KeyChain.EXTRA_PKCS12, data);
    //             break;
    //
    //         default:
    //             Log.w(TAG, "invalid certificate type: " + certType);
    //             return false;
    //     }
    //     context.startActivity(intent);
    //     return true;
    // } catch (ActivityNotFoundException e) {
    //     Log.w(TAG, "could not store crypto file: " + e);
    // }
    // return false;

    //try {
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

        switch (certType) {
            case CertificateMimeType::X509_USER_CERT:
            case CertificateMimeType::X509_CA_CERT:
                {
                    String dataTmp(*data);
                    intent->PutExtra(IKeyChain::EXTRA_CERTIFICATE, dataTmp);
                }
                break;
            case CertificateMimeType::PKCS12_ARCHIVE:
                {
                    String dataTmp(*data);
                    intent->PutExtra(IKeyChain::EXTRA_PKCS12, dataTmp);
                }
                break;
            default:
                {
                    Logger::W(TAG, "invalid certificate type: %d", certType);
                    return FALSE;
                }
        }
        context->StartActivity(intent);
        return TRUE;
    //} catch (ActivityNotFoundException e) {
    //    Log.w(TAG, "could not store crypto file: " + e);
    //}
    return FALSE;
}

String ElastosNetworkLibrary::GetMimeTypeFromExtension(
    /* [in] */ const String& extension)
{
    // ==================before translated======================
    // return URLConnection.guessContentTypeFromName("foo." + extension);

    assert(0);
    String result("");//TODO = URLConnection::GuessContentTypeFromName(String("foo.") + extension);
    return result;
}

Boolean ElastosNetworkLibrary::HaveOnlyLoopbackAddresses()
{
    // ==================before translated======================
    // Enumeration<NetworkInterface> list = null;
    // try {
    //     list = NetworkInterface.getNetworkInterfaces();
    //     if (list == null) return false;
    // } catch (Exception e) {
    //     Log.w(TAG, "could not get network interfaces: " + e);
    //     return false;
    // }
    //
    // while (list.hasMoreElements()) {
    //     NetworkInterface netIf = list.nextElement();
    //     try {
    //         if (netIf.isUp() && !netIf.isLoopback()) return false;
    //     } catch (SocketException e) {
    //         continue;
    //     }
    // }
    // return true;

    AutoPtr<IEnumeration> list(NULL);
    //try {
        AutoPtr<INetworkInterfaceHelper> helper;
        CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&helper);
        helper->GetNetworkInterfaces((IEnumeration**)&list);
        if (NULL == list)
            return FALSE;
    //} catch (Exception e) {
    //    Log.w(TAG, "could not get network interfaces: " + e);
    //    return false;
    //}

    Boolean hasMoreElements = FALSE;
    list->HasMoreElements(&hasMoreElements);
    while (hasMoreElements) {
        AutoPtr<IInterface> tmp;
        list->GetNextElement((IInterface**)&tmp);
        INetworkInterface* netIf = INetworkInterface::Probe(tmp);
        //try {
            Boolean isUp = FALSE;
            Boolean isLoopback = FALSE;
            netIf->IsUp(&isUp);
            netIf->IsLoopback(&isLoopback);
            if (isUp && !isLoopback)
                return FALSE;
        //} catch (SocketException e) {
        //    continue;
        //}
    }
    return TRUE;
}

String ElastosNetworkLibrary::GetNetworkList()
{
    // ==================before translated======================
    // Enumeration<NetworkInterface> list = null;
    // try {
    //     list = NetworkInterface.getNetworkInterfaces();
    //     if (list == null) return "";
    // } catch (SocketException e) {
    //     Log.w(TAG, "Unable to get network interfaces: " + e);
    //     return "";
    // }
    //
    // StringBuilder result = new StringBuilder();
    // while (list.hasMoreElements()) {
    //     NetworkInterface netIf = list.nextElement();
    //     try {
    //         // Skip loopback interfaces, and ones which are down.
    //         if (!netIf.isUp() || netIf.isLoopback())
    //             continue;
    //         for (InterfaceAddress interfaceAddress : netIf.getInterfaceAddresses()) {
    //             InetAddress address = interfaceAddress.getAddress();
    //             // Skip loopback addresses configured on non-loopback interfaces.
    //             if (address.isLoopbackAddress())
    //                 continue;
    //             StringBuilder addressString = new StringBuilder();
    //             addressString.append(netIf.getName());
    //             addressString.append("\t");
    //
    //             String ipAddress = address.getHostAddress();
    //             if (address instanceof Inet6Address && ipAddress.contains("%")) {
    //                 ipAddress = ipAddress.substring(0, ipAddress.lastIndexOf("%"));
    //             }
    //             addressString.append(ipAddress);
    //             addressString.append("/");
    //             addressString.append(interfaceAddress.getNetworkPrefixLength());
    //             addressString.append("\t");
    //
    //             // TODO(vitalybuka): use netIf.getIndex() when API level 19 is availible.
    //             addressString.append("0");
    //
    //             if (result.length() != 0)
    //                 result.append("\n");
    //             result.append(addressString.toString());
    //         }
    //     } catch (SocketException e) {
    //         continue;
    //     }
    // }
    // return result.toString();

    AutoPtr<IEnumeration> lists(NULL);
    //try {
        AutoPtr<INetworkInterfaceHelper> helper;
        CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&helper);
        helper->GetNetworkInterfaces((IEnumeration**)&lists);
        if (NULL == lists)
            return String("");
    //} catch (SocketException e) {
    //    Log.w(TAG, "Unable to get network interfaces: " + e);
    //    return "";
    //}

    StringBuilder result;
    Int32 resultLength = 0;

    Boolean hasMoreElements = FALSE;
    lists->HasMoreElements(&hasMoreElements);
    while (hasMoreElements) {
        AutoPtr<IInterface> tmp;
        lists->GetNextElement((IInterface**)&tmp);
        INetworkInterface* netIf = INetworkInterface::Probe(tmp);
        //try {
            // Skip loopback interfaces, and ones which are down.
            Boolean isUp = FALSE;
            Boolean isLoopback = FALSE;
            netIf->IsUp(&isUp);
            netIf->IsLoopback(&isLoopback);
            if (!isUp || isLoopback)
                continue;

            AutoPtr<IList> interfaceAddressTmp;
            netIf->GetInterfaceAddresses((IList**)&interfaceAddressTmp);
            Int32 interfaceAddressSize = 0;
            interfaceAddressTmp->GetSize(&interfaceAddressSize);

            for (Int32 i=0; i<interfaceAddressSize; ++i) {
                AutoPtr<IInterface> interfaceTmp;
                interfaceAddressTmp->Get(i, (IInterface**)&interfaceTmp);

                IInterfaceAddress* interfaceAddress = IInterfaceAddress::Probe(interfaceTmp);
                AutoPtr<IInetAddress> address;
                interfaceAddress->GetAddress((IInetAddress**)&address);

                // Skip loopback addresses configured on non-loopback interfaces.
                Boolean isLoopbackAddress = FALSE;
                address->IsLoopbackAddress(&isLoopbackAddress);
                if (isLoopbackAddress)
                    continue;

                StringBuilder addressString;
                String netName;
                netIf->GetName(&netName);
                addressString.Append(netName);
                addressString.Append(String("\t"));

                String ipAddress;
                address->GetHostAddress(&ipAddress);
                IInet6Address* inet6Addr = IInet6Address::Probe(address);
                if (inet6Addr != NULL && ipAddress.Contains(String("%"))) {
                    ipAddress = ipAddress.Substring(0, ipAddress.LastIndexOf(String("%")));
                }
                addressString.Append(ipAddress);
                addressString.Append(String("/"));

                Int16 prefixLength = 0;
                interfaceAddress->GetNetworkPrefixLength(&prefixLength);

                String tmpStr = IntegralToString::ToString(prefixLength);
                addressString.Append(tmpStr);
                addressString.Append(String("\t"));

                // TODO(vitalybuka): use netIf.getIndex() when API level 19 is availible.
                addressString.Append(String("0"));

                result.GetLength(&resultLength);
                if (resultLength != 0)
                    result.Append(String("\n"));
                String addressStringStr;
                addressString.ToString(&addressStringStr);
                result.Append(addressStringStr);
            }
        //} catch (SocketException e) {
        //    continue;
        //}
    }

    String resultStr;
    result.ToString(&resultStr);
    return resultStr;
}

AutoPtr<IInterface> ElastosNetworkLibrary::VerifyServerCertificates(
    /* [in] */ ArrayOf< AutoPtr< ArrayOf<Byte> > >* certChain,
    /* [in] */ const String& authType,
    /* [in] */ const String& host)
{
    // ==================before translated======================
    // try {
    //     return X509Util.verifyServerCertificates(certChain, authType, host);
    // } catch (KeyStoreException e) {
    //     return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_FAILED);
    // } catch (NoSuchAlgorithmException e) {
    //     return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_FAILED);
    // }

    //try {
        return TO_IINTERFACE(X509Util::VerifyServerCertificates(certChain, authType, host));
    //} catch (KeyStoreException e) {
    //    return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_FAILED);
    //} catch (NoSuchAlgorithmException e) {
    //    return new AndroidCertVerifyResult(CertVerifyStatusAndroid.VERIFY_FAILED);
    //}
}

void ElastosNetworkLibrary::AddTestRootCertificate(
    /* [in] */ ArrayOf<Byte>* rootCert)
{
    //VALIDATE_NOT_NULL(rootCert);
    // ==================before translated======================
    // X509Util.addTestRootCertificate(rootCert);

    X509Util::AddTestRootCertificate(rootCert);
}

void ElastosNetworkLibrary::ClearTestRootCertificates()
{
    // ==================before translated======================
    // X509Util.clearTestRootCertificates();

    X509Util::ClearTestRootCertificates();
}

String ElastosNetworkLibrary::GetNetworkCountryIso(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // TelephonyManager telephonyManager =
    //     (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
    // if (telephonyManager != null) {
    //   return telephonyManager.getNetworkCountryIso();
    // }
    // return "";

    AutoPtr<IInterface> telephonyManagerTmp;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&telephonyManagerTmp);
    ITelephonyManager* telephonyManager = ITelephonyManager::Probe(telephonyManagerTmp);
    String result("");
    if (telephonyManager != NULL) {
        telephonyManager->GetNetworkCountryIso(&result);
        return result;
    }
    return result;
}

String ElastosNetworkLibrary::GetNetworkOperator(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // TelephonyManager telephonyManager =
    //     (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
    // if (telephonyManager != null) {
    //   return telephonyManager.getNetworkOperator();
    // }
    // return "";

    AutoPtr<IInterface> telephonyManagerTmp;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&telephonyManagerTmp);
    ITelephonyManager* telephonyManager = ITelephonyManager::Probe(telephonyManagerTmp);
    String result("");
    if (telephonyManager != NULL) {
        telephonyManager->GetNetworkOperator(&result);
        return result;
    }
    return result;
}

Boolean ElastosNetworkLibrary::StoreKeyPair(
    /* [in] */ IInterface* context,
    /* [in] */ ArrayOf<Byte>* publicKey,
    /* [in] */ ArrayOf<Byte>* privateKey)
{
    IContext* c = IContext::Probe(context);
    return StoreKeyPair(c, publicKey, privateKey);
}

Boolean ElastosNetworkLibrary::StoreCertificate(
    /* [in] */ IInterface* context,
    /* [in] */ Int32 certType,
    /* [in] */ ArrayOf<Byte>* data)
{
    IContext* c = IContext::Probe(context);
    return StoreCertificate(c, certType, data);
}

String ElastosNetworkLibrary::GetNetworkCountryIso(
    /* [in] */ IInterface* context)
{
    IContext* c = IContext::Probe(context);
    return GetNetworkCountryIso(c);
}

String ElastosNetworkLibrary::GetNetworkOperator(
    /* [in] */ IInterface* context)
{
    IContext* c = IContext::Probe(context);
    return GetNetworkOperator(c);
}

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

