
#include "CServiceConnection.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ServiceDemo {

ECode CServiceConnection::OnServiceConnected(
    /* [in] */ Elastos::Droid::Content::IComponentName * pName,
    /* [in] */ Elastos::Droid::Os::IBinder * pService)
{
    // TODO: Add your code here
PRINT_ENTER_LEAVE("CServiceConnection::OnServiceConnected")
    return NOERROR;
}

ECode CServiceConnection::OnServiceDisconnected(
    /* [in] */ Elastos::Droid::Content::IComponentName * pName)
{
    // TODO: Add your code here
PRINT_ENTER_LEAVE("CServiceConnection::OnServiceDisconnected")
    return NOERROR;
}

}
}
}
}

