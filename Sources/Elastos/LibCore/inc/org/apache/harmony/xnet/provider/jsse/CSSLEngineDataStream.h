
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLENGINEDATASTREAM_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLENGINEDATASTREAM_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CSSLEngineDataStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CSSLEngineDataStream)
{
public:
    CARAPI HasData(
        /* [out] */ Boolean * pHasData);

    CARAPI GetData(
        /* [in] */ Int32 length,
        /* [out, callee] */ ArrayOf<Byte> ** ppData);

    CARAPI GetSessionData(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out, callee] */ ArrayOf<Byte> ** ppSessionData);

    CARAPI PutSessionData(
        /* [in] */ Elastosx::Net::Ssl::ISSLSession * pSession,
        /* [in] */ ArrayOf<Byte> * pSessionData);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLENGINEDATASTREAM_H__
