#ifndef __ELASTOS_DROID_APP_FRAGMENT_BREADCRUMBS_H__
#define __ELASTOS_DROID_APP_FRAGMENT_BREADCRUMBS_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::App::IFragmentManagerBackStackEntry;
using Elastos::Droid::App::IFragmentManagerOnBackStackChangedListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ILinearLayout;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Helper class for showing "bread crumbs" representing the fragment
 * stack in an activity.  This is intended to be used with
 * {@link ActionBar#setCustomView(View)
 * ActionBar.setCustomView(View)} to place the bread crumbs in
 * the action bar.
 *
 * <p>The default style for this view is
 * {@link android.R.style#Widget_FragmentBreadCrumbs}.
 *
 * @deprecated This widget is no longer supported.
 */
class FragmentBreadCrumbs
    : public ViewGroup
    , public IFragmentBreadCrumbs
    , public IFragmentManagerOnBackStackChangedListener
{
private:
    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ FragmentBreadCrumbs* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        FragmentBreadCrumbs* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    FragmentBreadCrumbs();

    virtual ~FragmentBreadCrumbs();

    CARAPI constructor(
        /*[in] */ IContext* ctx);

    CARAPI constructor(
        /*[in] */ IContext* ctx,
        /*[in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /*[in] */ IContext* ctx,
        /*[in] */ IAttributeSet* attrs,
        /*[in] */ Int32 defStyleAttr);

    /**
     * @hide
     */
    CARAPI constructor(
        /*[in] */ IContext* ctx,
        /*[in] */ IAttributeSet* attrs,
        /*[in] */ Int32 defStyleAttr,
        /*[in] */ Int32 defStyleRes);

    /**
     * Attach the bread crumbs to their activity.  This must be called once
     * when creating the bread crumbs.
     */
    CARAPI SetActivity(
        /* [in] */ IActivity* a);

    /**
     * The maximum number of breadcrumbs to show. Older fragment headers will be hidden from view.
     * @param visibleCrumbs the number of visible breadcrumbs. This should be greater than zero.
     */
    CARAPI SetMaxVisible(
        /* [in] */ Int32 visibleCrumbs);

    /**
     * Inserts an optional parent entry at the first position in the breadcrumbs. Selecting this
     * entry will result in a call to the specified listener's
     * {@link android.view.View.OnClickListener#onClick(View)}
     * method.
     *
     * @param title the title for the parent entry
     * @param shortTitle the short title for the parent entry
     * @param listener the {@link android.view.View.OnClickListener} to be called when clicked.
     * A NULL will result in no action being taken when the parent entry is clicked.
     */
    CARAPI SetParentTitle(
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* shortTitle,
        /* [in] */ IViewOnClickListener* listener);

    /**
     * Sets a listener for clicks on the bread crumbs.  This will be called before
     * the default click action is performed.
     *
     * @param listener The new listener to set.  Replaces any existing listener.
     */
    CARAPI SetOnBreadCrumbClickListener(
        /* [in] */ IOnBreadCrumbClickListener* listener);

    /**
     * Set a custom title for the bread crumbs.  This will be the first entry
     * shown at the left, representing the root of the bread crumbs.  If the
     * title is NULL, it will not be shown.
     */
    CARAPI SetTitle(
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* shortTitle) ;

    //@Override
    CARAPI OnBackStackChanged();

    CARAPI UpdateCrumbs();

protected:
    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@Override
    void OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
     AutoPtr<IBackStackRecord> CreateBackStackEntry(
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* shortTitle);

    /**
     * Returns the number of entries before the backstack, including the title of the current
     * fragment and any custom parent title that was set.
     */
    Int32 GetPreEntryCount();

    /**
     * Returns the pre-entry corresponding to the index. If there is a parent and a top entry
     * set, parent has an index of zero and top entry has an index of 1. Returns NULL if the
     * specified index doesn't exist or is NULL.
     * @param index should not be more than {@link #getPreEntryCount()} - 1
     */
    AutoPtr<IFragmentManagerBackStackEntry> GetPreEntry(
        /* [in] */ Int32 index);

private:
    static const String TAG;
    AutoPtr<IViewOnClickListener> mOnClickListener;

    AutoPtr<IActivity> mActivity;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<ILinearLayout> mContainer;
    Int32 mMaxVisible;

    // Hahah
    AutoPtr<IBackStackRecord> mTopEntry;
    AutoPtr<IBackStackRecord> mParentEntry;

    /** Listener to inform when a parent entry is clicked */
    AutoPtr<IViewOnClickListener> mParentClickListener;

    AutoPtr<IOnBreadCrumbClickListener> mOnBreadCrumbClickListener;

    Int32 mGravity;
    Int32 mLayoutResId;
    Int32 mTextColor;

    static const Int32 DEFAULT_GRAVITY;
};

} // namespace App
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_APP_FRAGMENT_BREADCRUMBS_H__