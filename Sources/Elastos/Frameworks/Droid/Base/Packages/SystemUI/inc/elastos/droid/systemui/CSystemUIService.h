
#ifndef  __ELASTOS_DROID_SYSTEMUI_CSYSTEMUISERVICE_H__
#define  __ELASTOS_DROID_SYSTEMUI_CSYSTEMUISERVICE_H__

#include "_Elastos_Droid_SystemUI_CSystemUIService.h"
#include <elastos/droid/app/Service.h>

using Elastos::Droid::App::Service;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CSystemUIService)
    , public Elastos::Droid::App::Service
    , public ISystemUIService
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI OnCreate();

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

protected:
    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);
};

} // SystemUI
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CSYSTEMUISERVICE_H__
