
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_CDESSERTCASEDREAM_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_CDESSERTCASEDREAM_H__

#include "_Elastos_Droid_Packages_SystemUI_CDessertCaseDream.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/service/dreams/DreamService.h>
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Service::Dreams::DreamService;
using Elastos::Droid::Widget::IFrameLayout;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

CarClass(CDessertCaseDream)
    , public DreamService
    , public IDessertCaseDream
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
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI OnAttachedToWindow();

    CARAPI OnDreamingStarted();

    CARAPI OnDreamingStopped();

private:
    AutoPtr<IDessertCaseView> mView;
    AutoPtr</*IDessertCaseViewRescalingContainer*/IFrameLayout> mContainer;
};

} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_CDESSERTCASEDREAM_H__
