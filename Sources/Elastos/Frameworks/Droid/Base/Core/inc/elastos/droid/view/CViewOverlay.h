
#ifndef __ELASTOS_DROID_VIEW_CVIEWOVERLAY_H__
#define __ELASTOS_DROID_VIEW_CVIEWOVERLAY_H__

#include "_Elastos_Droid_View_CViewOverlay.h"
#include "elastos/droid/view/ViewOverlay.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * An overlay is an extra layer that sits on top of a View (the "host view")
 * which is drawn after all other content in that view (including children,
 * if the view is a ViewGroup). Interaction with the overlay layer is done
 * by adding and removing drawables.
 *
 * <p>An overlay requested from a ViewGroup is of type {@link ViewGroupOverlay},
 * which also supports adding and removing views.</p>
 *
 * @see View#getOverlay() View.getOverlay()
 * @see ViewGroup#getOverlay() ViewGroup.getOverlay()
 * @see ViewGroupOverlay
 */
CarClass(CViewOverlay)
    , public ViewOverlay
{
public:
    CAR_OBJECT_DECL();
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CVIEWOVERLAY_H__
