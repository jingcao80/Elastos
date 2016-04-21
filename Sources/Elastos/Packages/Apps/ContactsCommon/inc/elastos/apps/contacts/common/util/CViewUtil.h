#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CVIEWUTIL_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CVIEWUTIL_H__

#include "_Elastos_Apps_Contacts_Common_Util_CViewUtil.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CarClass(CViewUtil)
    , public singleton
    , public IViewUtil
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the width as specified in the LayoutParams
     * @throws IllegalStateException Thrown if the view's width is unknown before a layout pass
     * s
     */
    CARAPI GetConstantPreLayoutWidth(
        /* [in] */ IView* view,
        /* [out] */ Int32* width);

    /**
     * Returns a boolean indicating whether or not the view's layout direction is RTL
     *
     * @param view - A valid view
     * @return True if the view's layout direction is RTL
     */
    CARAPI IsViewLayoutRtl(
        /* [in] */ IView* view,
        /* [out] */ Boolean* result);

    /**
     * Adds a rectangular outline to a view. This can be useful when you want to add a shadow
     * to a transparent view. See b/16856049.
     * @param view view that the outline is added to
     * @param res The resources file.
     */
    CARAPI AddRectangularOutlineProvider(
        /* [in] */ IView* view,
        /* [in] */ IResources* res);

    /**
     * Configures the floating action button, clipping it to a circle and setting its translation z.
     * @param view The float action button's view.
     * @param res The resources file.
     */
    CARAPI SetupFloatingActionButton(
        /* [in] */ IView* view,
        /* [in] */ IResources* res);

    /**
     * Adds padding to the bottom of the given {@link ListView} so that the floating action button
     * does not obscure any content.
     *
     * @param listView to add the padding to
     * @param res valid resources object
     */
    CARAPI AddBottomPaddingToListViewForFab(
        /* [in] */ IListView* listView,
        /* [in] */ IResources* res);
};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_CVIEWUTIL_H__
