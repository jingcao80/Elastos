
#ifndef __ELASTOS_DROID_GALAXY4_CGALAXY4_H__
#define __ELASTOS_DROID_GALAXY4_CGALAXY4_H__

#include "_Elastos_Droid_Galaxy4_CGalaxy4.h"
#include "CGalaxyView.h"
#include <Elastos.Droid.Os.h>
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

CarClass(CGalaxy4)
    , public Activity
{
public:
    CAR_OBJECT_DECL()

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    AutoPtr<CGalaxyView> mView;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_CGALAXY4_H__
