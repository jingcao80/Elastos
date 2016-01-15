
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CFILECLIENTSESSIONCACHEHELPER_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CFILECLIENTSESSIONCACHEHELPER_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CFileClientSessionCacheHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CFileClientSessionCacheHelper)
{
public:
    CARAPI UsingDirectory(
        /* [in] */ Elastos::IO::IFile * pDirectory,
        /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLClientSessionCache ** ppCache);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CFILECLIENTSESSIONCACHEHELPER_H__
