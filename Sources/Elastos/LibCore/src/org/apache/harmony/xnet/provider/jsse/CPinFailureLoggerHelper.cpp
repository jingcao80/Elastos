
#include "CPinFailureLoggerHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_SINGLETON_IMPL(CPinFailureLoggerHelper)

CAR_INTERFACE_IMPL(CPinFailureLoggerHelper, Singleton, IPinFailureLoggerHelper)

ECode CPinFailureLoggerHelper::Log(
    /* [in] */ const String& cn,
    /* [in] */ Boolean chainContainsUserCert,
    /* [in] */ Boolean pinIsEnforcing,
    /* [in] */ Elastos::Utility::IList * pChain)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

