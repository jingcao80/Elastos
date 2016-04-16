#include "elastos/droid/webkit/webview/chromium/native/android_webview/ClientCertLookupTable.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//===============================================================
//                 ClientCertLookupTable::Cert
//===============================================================

/**
 * A container for the certificate data.
 */
ClientCertLookupTable::Cert::Cert(
    /* [in] */ ElastosPrivateKey* privateKey,
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* certChain)
{
    this->privateKey = privateKey;
    Int32 length = certChain->GetLength();
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Byte> > > > newChain = ArrayOf<AutoPtr<ArrayOf<Byte> > >::Alloc(length);
    for (Int32 i = 0; i < length; ++i) {
        (*newChain)[i] = ((*certChain)[i])->Clone();//Arrays.copyOf(certChain[i], certChain[i].length);
    }
    this->certChain = newChain;
}

//===============================================================
//                     ClientCertLookupTable
//===============================================================

// Clear client certificate preferences
void ClientCertLookupTable::Clear()
{
    mCerts.Clear();
    mDenieds.Clear();
}

ClientCertLookupTable::ClientCertLookupTable()
{
    //mCerts = new HashMap<String, Cert>();
    //mDenieds = new HashSet<String>();
}

void ClientCertLookupTable::Allow(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ ElastosPrivateKey* privateKey,
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* chain)
{
    String host_and_port = HostAndPort(host, port);
    //mCerts.put(host_and_port, new Cert(privateKey, chain));
    mCerts[host_and_port] = new Cert(privateKey, chain);
    mDenieds.Erase(host_and_port);
}

void ClientCertLookupTable::Deny(
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    String host_and_port = HostAndPort(host, port);
    mCerts.Erase(host_and_port);
    mDenieds.Insert(host_and_port);
}

AutoPtr<ClientCertLookupTable::Cert> ClientCertLookupTable::GetCertData(
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    //return mCerts->Get(HostAndPort(host, port));
    return mCerts[HostAndPort(host, port)];
}

Boolean ClientCertLookupTable::IsDenied(
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    //return mDenieds.Contains(HostAndPort(host, port));
    return mDenieds.Find(HostAndPort(host, port)) != mDenieds.End();
}

// TODO(sgurun) add a test for this. Not separating host and pair properly will be
// a security issue.
String ClientCertLookupTable::HostAndPort(
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    String sport = StringUtils::ToString(port);
    String retStr(host);
    retStr += ":";
    retStr += sport;
    return retStr;
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
