
#ifndef __ELASTOS_DROID_INTERNAL_APP_ALERTACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_ALERTACTIVITY_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IDialogInterface;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class AlertActivity
    : public Activity
    , public IAlertActivity
    , public IDialogInterface
{
public:
    CAR_INTERFACE_DECL()

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI Cancel();

    CARAPI Dismiss();

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* keyDown);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* keyUp);

protected:
    /**
     * Sets up the alert, including applying the parameters to the alert model,
     * and installing the alert's content.
     *
     * @see #mAlert
     * @see #mAlertParams
     */
    virtual CARAPI_(void) SetupAlert();

protected:
    /**
     * The model for the alert.
     *
     * @see #mAlertParams
     */
    AutoPtr<IAlertController> mAlert;

    /**
     * The parameters for the alert.
     */
    AutoPtr<IAlertControllerAlertParams> mAlertParams;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_ALERTACTIVITY_H__
