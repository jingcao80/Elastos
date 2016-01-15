
#include "Elastos.Droid.App.h"
#include "elastos/droid/internal/app/CMediaRouteDialogPresenter.h"
#include "elastos/droid/internal/app/MediaRouteDialogPresenter.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(CMediaRouteDialogPresenter, Singleton, IMediaRouteDialogPresenter)
CAR_SINGLETON_IMPL(CMediaRouteDialogPresenter)

ECode CMediaRouteDialogPresenter::ShowDialogFragment(
    /* [in] */ IActivity* activity,
    /* [in] */ Int32 routeTypes,
    /* [in] */ IViewOnClickListener* extendedSettingsClickListener,
    /* [out] */ IDialogFragment** dialogFragment)
{
    VALIDATE_NOT_NULL(dialogFragment)
    AutoPtr<IDialogFragment> temp = MediaRouteDialogPresenter::ShowDialogFragment(
        activity, routeTypes, extendedSettingsClickListener);
    *dialogFragment = temp;
    REFCOUNT_ADD(*dialogFragment)
    return NOERROR;
}

ECode CMediaRouteDialogPresenter::CreateDialog(
    /* [in] */ IContext* context,
    /* [in] */ Int32 routeTypes,
    /* [in] */ IViewOnClickListener* extendedSettingsClickListener,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    AutoPtr<IDialog> temp = MediaRouteDialogPresenter::CreateDialog(
        context, routeTypes, extendedSettingsClickListener);
    *dialog = temp;
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
