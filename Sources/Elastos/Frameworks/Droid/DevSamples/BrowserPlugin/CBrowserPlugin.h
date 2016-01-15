
#ifndef __CBROWSERPLUGIN_H__
#define __CBROWSERPLUGIN_H__

#include "_CBrowserPlugin.h"
#include "elastos/droid/app/Service.h"

//using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BrowserPlugin {

CarClass(CBrowserPlugin), public Elastos::Droid::App::Service
{
public:
    CAR_INTERFACE_DECL();
    ECode OnBind(IIntent* intent, IBinder** binder);
private:
};

}
}
}
}

#endif // __CBROWSERPLUGIN_H__
