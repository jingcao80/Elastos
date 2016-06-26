#ifndef  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICELOCALBINDER_H__
#define  __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICELOCALBINDER_H__

#include "_Elastos_Droid_Phone_CEmergencyCallbackModeServiceLocalBinder.h"
#include "elastos/droid/phone/CEmergencyCallbackModeService.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Phone::CEmergencyCallbackModeService;

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CEmergencyCallbackModeServiceLocalBinder) 
    , public CEmergencyCallbackModeService::LocalBinder
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CEMERGENCYCALLBACKMODESERVICELOCALBINDER_H__