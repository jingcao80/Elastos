
#ifndef __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECONTROLLERDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECONTROLLERDIALOGFRAGMENT_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/app/DialogFragment.h"

using Elastos::Droid::App::DialogFragment;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class MediaRouteControllerDialogFragment
    : public DialogFragment
    , public IMediaRouteControllerDialogFragment
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a media route controller dialog fragment.
     * <p>
     * All subclasses of this class must also possess a default constructor.
     * </p>
     */
    CARAPI constructor();

    /**
     * Called when the controller dialog is being created.
     * <p>
     * Subclasses may override this method to customize the dialog.
     * </p>
     */
    CARAPI OnCreateControllerDialog(
        /* [in] */ IContext* context,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IMediaRouteControllerDialog** dialog);

    // @Override
    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_MEDIAROUTECONTROLLERDIALOGFRAGMENT_H__
