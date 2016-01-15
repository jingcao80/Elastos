
#ifndef __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTEDIALOGPRESENTER_H__
#define __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTEDIALOGPRESENTER_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class MediaRouteDialogPresenter
{
public:
    static CARAPI_(AutoPtr<IDialogFragment>) ShowDialogFragment(
        /* [in] */ IActivity* activity,
        /* [in] */ Int32 routeTypes,
        /* [in] */ IViewOnClickListener* extendedSettingsClickListener);

    static CARAPI_(AutoPtr<IDialog>) CreateDialog(
        /* [in] */ IContext* context,
        /* [in] */ Int32 routeTypes,
        /* [in] */ IViewOnClickListener* extendedSettingsClickListener);

private:
    MediaRouteDialogPresenter();

private:
    static const String TAG;
    static const String CHOOSER_FRAGMENT_TAG ;
    static const String CONTROLLER_FRAGMENT_TAG;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_MEDIAROUTEDIALOGPRESENTER_H__
