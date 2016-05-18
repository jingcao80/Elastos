
#ifndef  __ELASTOS_DROID_SYSTEMUI_CDESSERTCASE_H__
#define  __ELASTOS_DROID_SYSTEMUI_CDESSERTCASE_H__

#include "_Elastos_Droid_SystemUI_CDessertCase.h"
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::SystemUI::IDessertCaseView;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CDessertCase)
    , public Activity
    , public IDessertCase
{
private:
    class Runnable_1
        : public Runnable
    {
    public:
        Runnable_1(
            /* [in] */ CDessertCase* owner);

        CARAPI Run();

    public:
        CDessertCase* mOwner;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

public:
    AutoPtr<IDessertCaseView> mView;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CDESSERTCASE_H__
