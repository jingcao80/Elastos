
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CFILECLIENTSESSIONCACHE_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CFILECLIENTSESSIONCACHE_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CFileClientSessionCache.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CFileClientSessionCache)
    , public Object
    , public IFileClientSessionCache
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CFILECLIENTSESSIONCACHE_H__
