#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_CLIENTCERTLOOKUPTABLE_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_CLIENTCERTLOOKUPTABLE_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/net/ElastosPrivateKey.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/Map.h>
#include <elastos/utility/etl/Set.h>

using Elastos::Droid::Webkit::Net::ElastosPrivateKey;
using Elastos::Utility::Etl::Map;
using Elastos::Utility::Etl::Set;

// import java.util.Arrays;
// import java.util.HashMap;
// import java.util.HashSet;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

/**
 * Store user's client certificate decision for a host and port pair. Not
 * thread-safe. All accesses are done on UI thread.
 */
class ClientCertLookupTable
:public Object
{
public:
    /**
     * A container for the certificate data.
     */
    class Cert
    :public Object
    {
    public:
        Cert(
            /* [in] */ ElastosPrivateKey* privateKey,
            /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* certChain);

    public:
        AutoPtr<ElastosPrivateKey> privateKey;
        AutoPtr<ArrayOf<AutoPtr<ArrayOf<Byte> > > > certChain;
    };

public:
    ClientCertLookupTable();

    // Clear client certificate preferences
    CARAPI_(void) Clear();

    CARAPI_(void) Allow(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ ElastosPrivateKey* privateKey,
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* chain);

    CARAPI_(void) Deny(
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    CARAPI_(AutoPtr<Cert>) GetCertData(
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    CARAPI_(Boolean) IsDenied(
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

private:
    // TODO(sgurun) add a test for this. Not separating host and pair properly will be
    // a security issue.
    static CARAPI_(String) HostAndPort(
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

private:
    Map<String, AutoPtr<Cert> > mCerts;
    Set<String> mDenieds;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_CLIENTCERTLOOKUPTABLE_H__
