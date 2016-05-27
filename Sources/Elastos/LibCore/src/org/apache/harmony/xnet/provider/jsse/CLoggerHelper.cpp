
#include "CLoggerHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_SINGLETON_IMPL(CLoggerHelper)

CAR_INTERFACE_IMPL(CLoggerHelper, Singleton, ILoggerHelper)

ECode CLoggerHelper::GetStream(
    /* [in] */ const String& name,
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ILoggerStream ** ppStream)
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

