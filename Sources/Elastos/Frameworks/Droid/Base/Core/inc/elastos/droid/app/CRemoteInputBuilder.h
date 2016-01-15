
#ifndef __ELASTOS_DROID_APP_CREMOTEINPUT_BUILDER_H__
#define __ELASTOS_DROID_APP_CREMOTEINPUT_BUILDER_H__

#include "_Elastos_Droid_App_CRemoteInputBuilder.h"
#include "elastos/droid/app/CRemoteInput.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CRemoteInputBuilder)
    , public CRemoteInput::Builder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CREMOTEINPUT_BUILDER_H__