
#ifndef __ELASTOS_DROID_SYSTEMUI_EGG_LLANDACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_EGG_LLANDACTIVITY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Egg {

class LLandActivity
    : public Activity
{
public:
    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);
};

} // namespace Egg
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_EGG_LLANDACTIVITY_H__