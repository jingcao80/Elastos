
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_VIEWUTIL_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_VIEWUTIL_H__

#include <elastos/droid/view/ViewOutlineProvider.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IOutline;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewOutlineProvider;
using Elastos::Droid::Widget::IListView;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Provides static functions to work with views
 */
class ViewUtil
{
private:
    class OVALViewOutlineProvide
        : public ViewOutlineProvider
    {
    public:
        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);
    };

    class RECTViewOutlineProvide
        : public ViewOutlineProvider
    {
    public:
        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);
    };

public:
    static CARAPI GetConstantPreLayoutWidth(
        /* [in] */ IView* view,
        /* [out] */ Int32* result);

    static CARAPI_(Boolean) IsViewLayoutRtl(
        /* [in] */ IView* view);

    static CARAPI_(void) AddRectangularOutlineProvider(
        /* [in] */ IView* view,
        /* [in] */ IResources* res);

    static CARAPI_(void) SetupFloatingActionButton(
        /* [in] */ IView* view,
        /* [in] */ IResources* res);

    static CARAPI_(void) AddBottomPaddingToListViewForFab(
        /* [in] */ IListView* listView,
        /* [in] */ IResources* res);

private:
    static const AutoPtr<ViewOutlineProvider> OVAL_OUTLINE_PROVIDER;
    static const AutoPtr<ViewOutlineProvider> RECT_OUTLINE_PROVIDER;
};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_VIEWUTIL_H__
