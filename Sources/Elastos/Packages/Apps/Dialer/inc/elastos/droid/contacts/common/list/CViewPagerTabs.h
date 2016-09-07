
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CVIEWPAGERTABS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CVIEWPAGERTABS_H__

#include "_Elastos_Droid_Contacts_Common_List_CViewPagerTabs.h"
#include "elastos/droid/contacts/common/list/ViewPagerTabStrip.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/widget/HorizontalScrollView.h"

using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Graphics::IOutline;
using Elastos::Droid::Support::V4::View::IViewPagerOnPageChangeListener;
using Elastos::Droid::Support::V4::View::IViewPager;
using Elastos::Droid::Support::V4::View::IPagerAdapter;
using Elastos::Droid::View::IViewOutlineProvider;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::HorizontalScrollView;
using Elastos::Core::ICharSequence;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * Lightweight implementation of ViewPager tabs. This looks similar to traditional actionBar tabs,
 * but allows for the view containing the tabs to be placed anywhere on screen. Text-related
 * attributes can also be assigned in XML - these will get propogated to the child TextViews
 * automatically.
 */
CarClass(CViewPagerTabs)
    , public HorizontalScrollView
    , public IViewPagerTabs
    , public IViewPagerOnPageChangeListener
{
private:
    class ViewBoundsOutlineProvider
        : public Object
        , public IViewOutlineProvider
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);
    };

    /**
     * Simulates actionbar tab behavior by showing a toast with the tab title when long clicked.
     */
    class OnTabLongClickListener
        : public Object
        , public IViewOnLongClickListener
    {
    public:
        OnTabLongClickListener(
            /* [in] */ Int32 position,
            /* [in] */ CViewPagerTabs* host)
            : mPosition(position)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        Int32 mPosition;
        CViewPagerTabs* mHost;
    };

    class TextViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TextViewOnClickListener(
            /* [in] */ Int32 position,
            /* [in] */ CViewPagerTabs* host)
            : mPosition(position)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        Int32 mPosition;
        CViewPagerTabs* mHost;
    };

public:
    CViewPagerTabs();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetViewPager(
        /* [in] */ IViewPager* viewPager);

    // @Override
    CARAPI OnPageScrolled(
        /* [in] */ Int32 position,
        /* [in] */ Float offset,
        /* [in] */ Int32 offsetPixels);

    // @Override
    CARAPI OnPageSelected(
        /* [in] */ Int32 position);

    // @Override
    CARAPI OnPageScrollStateChanged(
        /* [in] */ Int32 state);

private:
    static CARAPI_(AutoPtr<IViewOutlineProvider>) InitViewOutlineProvider();

    CARAPI_(void) AddTabs(
        /* [in] */ IPagerAdapter* adapter);

    CARAPI_(void) AddTab(
        /* [in] */ ICharSequence* tabTitle,
        /* [in] */ Int32 position);

    CARAPI_(Int32) GetRtlPosition(
        /* [in] */ Int32 position);

private:
    static const AutoPtr<IViewOutlineProvider> VIEW_BOUNDS_OUTLINE_PROVIDER;

    static const Int32 TAB_SIDE_PADDING_IN_DPS = 10;

    // TODO: This should use <declare-styleable> in the future
    static const AutoPtr<ArrayOf<Int32> > ATTRS;

    AutoPtr<IViewPager> mPager;
    AutoPtr<ViewPagerTabStrip> mTabStrip;

    /**
     * Linearlayout that will contain the TextViews serving as tabs. This is the only child
     * of the parent HorizontalScrollView.
     */
    Int32 mTextStyle;
    AutoPtr<IColorStateList> mTextColor;
    Int32 mTextSize;
    Boolean mTextAllCaps;
    Int32 mPrevSelected;
    Int32 mSidePadding;

    friend class OnTabLongClickListener;
    friend class TextViewOnClickListener;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CVIEWPAGERTABS_H__
