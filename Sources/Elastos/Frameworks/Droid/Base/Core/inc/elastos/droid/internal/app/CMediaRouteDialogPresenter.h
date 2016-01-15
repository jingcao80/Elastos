
#ifndef __ELASTOS_DROID_INTERNAL_APP_CMEDIAROUTEDIALOGPRESENTER_H__
#define __ELASTOS_DROID_INTERNAL_APP_CMEDIAROUTEDIALOGPRESENTER_H__

#include "_Elastos_Droid_Internal_App_CMediaRouteDialogPresenter.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CMediaRouteDialogPresenter)
    , public Singleton
    , public IMediaRouteDialogPresenter
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ShowDialogFragment(
        /* [in] */ IActivity* activity,
        /* [in] */ Int32 routeTypes,
        /* [in] */ IViewOnClickListener* extendedSettingsClickListener,
        /* [out] */ IDialogFragment** dialogFragment);

    CARAPI CreateDialog(
        /* [in] */ IContext* context,
        /* [in] */ Int32 routeTypes,
        /* [in] */ IViewOnClickListener* extendedSettingsClickListener,
        /* [out] */ IDialog** dialog);
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_APP_CMEDIAROUTEDIALOGPRESENTER_H__
