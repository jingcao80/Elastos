#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_VIEWUTIL_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_VIEWUTIL_H__


namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Provides static functions to work with views
 */
class ECO_PUBLIC ViewUtil
    : public Object
{
private:
    class OVALViewOutlineProvide
        : public Object
        , public IViewOutlineProvider
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);
    };

    class RECTViewOutlineProvide
        : public Object
        , public IViewOutlineProvider
    {
    public:
        CAR_INTERFACE_DECL()

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
    static const AutoPtr<IViewOutlineProvider> OVAL_OUTLINE_PROVIDER;
    static const AutoPtr<IViewOutlineProvider> RECT_OUTLINE_PROVIDER;
};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_VIEWUTIL_H__
