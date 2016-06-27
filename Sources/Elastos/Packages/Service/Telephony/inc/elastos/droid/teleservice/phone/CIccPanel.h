#ifndef  __ELASTOS_DROID_PHONE_CICCPANEL_H__
#define  __ELASTOS_DROID_PHONE_CICCPANEL_H__

#include "_Elastos_Droid_TeleService_Phone_CIccPanel.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Dialog.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::App::Dialog;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Base class for ICC-related panels in the Phone UI.
 */
CarClass(CIccPanel)
    , public Dialog
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnStop();

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnStart();

protected:
    static const String TAG;

private:
    AutoPtr<IStatusBarManager> mStatusBarManager;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CICCPANEL_H__