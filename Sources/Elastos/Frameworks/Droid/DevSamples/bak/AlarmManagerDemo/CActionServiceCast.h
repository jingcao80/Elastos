
#ifndef __ACTIONSERVICECAST_H__
#define __ACTIONSERVICECAST_H__

#include "Elastos.Droid.Core.h"
#include "Elastos.Core.h"
#include "elastos/droid/app/Service.h"
#include "_CActionServiceCast.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::App::Service;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AlarmManagerDemo {

class CActionServiceCast
    : public Elastos::Droid::App::Service
{
public:
    CActionServiceCast();

    ~CActionServiceCast();

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    CARAPI OnCreate();

    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    CARAPI OnDestroy();

private:
    static Int32 mNum;
};

}  //AlarmManagerDemo
}  //DevSamples
}  //Droid
}  //Elastos

#endif //__ACTIONSERVICECAST_H__



