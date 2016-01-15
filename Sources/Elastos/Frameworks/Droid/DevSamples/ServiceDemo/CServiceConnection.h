
#ifndef __CSERVICECONNECTION_H__
#define __CSERVICECONNECTION_H__

#include "_CServiceConnection.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ServiceDemo {

CarClass(CServiceConnection)
{
public:
    CARAPI OnServiceConnected(
        /* [in] */ Elastos::Droid::Content::IComponentName * pName,
        /* [in] */ Elastos::Droid::Os::IBinder * pService);

    CARAPI OnServiceDisconnected(
        /* [in] */ Elastos::Droid::Content::IComponentName * pName);

private:
};

}
}
}
}

#endif // __CSERVICECONNECTION_H__
