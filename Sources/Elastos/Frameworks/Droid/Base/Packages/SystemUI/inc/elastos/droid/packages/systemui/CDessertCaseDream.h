
#ifndef __ELASTOS_DROID_SYSTEMUI_CDESSERTCASEDREAM_H__
#define __ELASTOS_DROID_SYSTEMUI_CDESSERTCASEDREAM_H__

#include "_Elastos_Droid_SystemUI_CDessertCaseDream.h"
#include "elastos/droid/os/Runnable.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Service::Dreams::IDreamService;


namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CDessertCaseDream)
//    , public DreamService
    , public Object
    , public IDessertCaseDream
//    , public IDreamService
{
private:
    class Runnable_1
        : public Runnable
    {
    public:
        Runnable_1(
            /* [in] */ CDessertCaseDream* owner);

        CARAPI Run();

    public:
        CDessertCaseDream* mOwner;
    };
public:
    CAR_INTERFACE_DECL()

    CARAPI OnAttachedToWindow();

    CARAPI OnDreamingStarted();

    CARAPI OnDreamingStopped();

private:
    AutoPtr<IDessertCaseView> mView;
    AutoPtr<IDessertCaseViewRescalingContainer> mContainer;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_CDESSERTCASEDREAM_H__
