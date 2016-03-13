
#ifndef __ELASTOS_DROID_SERVER_AM_CPERMISSIONCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_AM_CPERMISSIONCONTROLLER_H__

#include "_Elastos_Droid_Server_Am_CPermissionController.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IPermissionController;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class CActivityManagerService;

CarClass(CPermissionController)
    , public Object
    , public IPermissionController
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIActivityManager* host);

    CARAPI CheckPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CActivityManagerService* mActivityManagerService;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CPERMISSIONCONTROLLER_H__
