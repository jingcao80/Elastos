
#ifndef __ELASTOS_DROID_PHONE_COMMON_UTIL_VIEWUTIL_H__
#define __ELASTOS_DROID_PHONE_COMMON_UTIL_VIEWUTIL_H__

#include <elastos/droid/view/ViewOutlineProvider.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewOutlineProvider;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Util {

/**
 * Provides static functions to work with views
 */
class ViewUtil
{
private:
    class MyViewOutlineProvider
        : public ViewOutlineProvider
    {
    public:
        // @Override
        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);
    };

public:
    /**
     * Returns the width as specified in the LayoutParams
     * @throws IllegalStateException Thrown if the view's width is unknown before a layout pass
     * s
     */
    static CARAPI GetConstantPreLayoutWidth(
        /* [in] */ IView* view,
        /* [out] */ Int32* width);

    /**
     * Returns a boolean indicating whether or not the view's layout direction is RTL
     *
     * @param view - A valid view
     * @return True if the view's layout direction is RTL
     */
    static CARAPI_(Boolean) IsViewLayoutRtl(
        /* [in] */ IView* view);

    /**
     * Configures the floating action button, clipping it to a circle and setting its translation z.
     * @param view The float action button's view.
     * @param res The resources file.
     */
    static CARAPI_(void) SetupFloatingActionButton(
        /* [in] */ IView* view,
        /* [in] */ IResources* res);

    /**
     * Adds padding to the bottom of the given {@link ListView} so that the floating action button
     * does not obscure any content.
     *
     * @param listView to add the padding to
     * @param res valid resources object
     */
    static CARAPI_(void) AddBottomPaddingToListViewForFab(
        /* [in] */ IListView* listView,
        /* [in] */ IResources* res);

    static CARAPI_(void) ResizeText(
        /* [in] */ ITextView* textView,
        /* [in] */ Int32 originalTextSize,
        /* [in] */ Int32 minTextSize);

private:
    ViewUtil() {}

private:
    static const AutoPtr<ViewOutlineProvider> OVAL_OUTLINE_PROVIDER;
};

} // namespace Util
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_UTIL_VIEWUTIL_H__
