
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPINFAILURELOGGERHELPER_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPINFAILURELOGGERHELPER_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CPinFailureLoggerHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CPinFailureLoggerHelper)
{
public:
    CARAPI Log(
        /* [in] */ const String& cn,
        /* [in] */ Boolean chainContainsUserCert,
        /* [in] */ Boolean pinIsEnforcing,
        /* [in] */ Elastos::Utility::IList * pChain);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CPINFAILURELOGGERHELPER_H__
