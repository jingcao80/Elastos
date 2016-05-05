
#ifndef __ELASTOS_DROID_WIDGET_ACTIVITYCHOOSERVIEW_H__
#define __ELASTOS_DROID_WIDGET_ACTIVITYCHOOSERVIEW_H__

#include "elastos/droid/database/DataSetObserver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/widget/ImageView.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/widget/ListPopupWindow.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::IAccessibilityDelegate;
using Elastos::Droid::View::IActionProvider;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IActivityChooserModel;
using Elastos::Droid::Widget::IActivityChooserModelClient;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IListPopupWindowForwardingListener;
using Elastos::Droid::Widget::ImageView;
using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::ListPopupWindow;

namespace Elastos {
namespace Droid {
namespace Widget {

class ActivityChooserView
    : public ViewGroup
    , public IActivityChooserView
    , public IActivityChooserModelClient
{
private:
    class InnerNotifyDataSetObserver
        : public DataSetObserver
    {
    public:
        CAR_INTERFACE_DECL()

        InnerNotifyDataSetObserver(
            /* [in] */ ActivityChooserView* owner);

        // @Override
        CARAPI OnChanged();

        // @Override
        CARAPI OnInvalidated();

    private:
        ActivityChooserView* mOwner;
    };

    class InnerOnGlobalLayoutListener
        : public Object
        , public IOnGlobalLayoutListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnGlobalLayoutListener(
            /* [in] */ ActivityChooserView* owner);

        // @Override
        CARAPI OnGlobalLayout();

    private:
        ActivityChooserView* mOwner;
    };

    class InnerAccessibilityDelegate
        : public Elastos::Droid::View::View::AccessibilityDelegate
    {
    public:
        InnerAccessibilityDelegate(
            /* [in] */ ActivityChooserView* owner);

        // @Override
        CARAPI OnInitializeAccessibilityNodeInfo(
            /* [in] */ IView* host,
            /* [in] */ IAccessibilityNodeInfo* info);

    private:
        ActivityChooserView* mOwner;
    };

    class InnerForwardingListener
        : public Object
        , public IListPopupWindowForwardingListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerForwardingListener(
            /* [in] */ ActivityChooserView* owner);

        // @Override
        CARAPI GetPopup(
            /* [out] */ IListPopupWindow** result);

    protected:
        // @Override
        CARAPI_(Boolean) OnForwardingStarted();

        // @Override
        CARAPI_(Boolean) OnForwardingStopped();

    private:
        ActivityChooserView* mOwner;
    };

    class InnerUpdateDataSetObserver
        : public DataSetObserver
    {
    public:
        InnerUpdateDataSetObserver(
            /* [in] */ ActivityChooserView* owner);

        // @Override
        CARAPI OnChanged();

        // just override pure function for compiling
        CARAPI OnInvalidated();

    private:
        ActivityChooserView* mOwner;
    };

    /**
      * Interface implementation to avoid publishing them in the APIs.
      */
    class Callbacks
        : public Object
        , public IAdapterViewOnItemClickListener
        , public IViewOnClickListener
        , public IViewOnLongClickListener
        , public IPopupWindowOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        Callbacks(
            /* [in] */ ActivityChooserView* owner);

        // AdapterView#OnItemClickListener
        virtual CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        // View.OnClickListener
        virtual CARAPI OnClick(
            /* [in] */ IView* view);

        // OnLongClickListener#onLongClick
        // @Override
        CARAPI OnLongClick(
            /* [in] */ IView* view,
            /* [out] */ Boolean* result);

        // PopUpWindow.OnDismissListener#onDismiss
        virtual CARAPI OnDismiss();

    private:
        CARAPI_(void) NotifyOnDismissListener();

        CARAPI_(void) StartActivity(
            /* [in] */ IIntent* intent,
            /* [in] */ IResolveInfo* resolveInfo);

    private:
        ActivityChooserView* mOwner;
    };

    /**
      * Adapter for backing the list of activities shown in the popup.
      */
    class ActivityChooserViewAdapter
        : public BaseAdapter
    {
    public:
        ActivityChooserViewAdapter(
            /* [in] */ ActivityChooserView* owner);

        virtual CARAPI SetDataModel(
            /* [in] */ IActivityChooserModel* dataModel);

        // @Override
        CARAPI GetItemViewType(
            /* [in] */ Int32 position,
            /* [out] */ Int32* result);

        // @Override
        CARAPI GetViewTypeCount(
            /* [out] */ Int32* result);

        virtual CARAPI GetCount(
            /* [out] */ Int32* result);

        virtual CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** result);

        virtual CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* result);

        virtual CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

        virtual CARAPI MeasureContentWidth(
            /* [out] */ Int32* result);

        virtual CARAPI SetMaxActivityCount(
            /* [in] */ Int32 maxActivityCount);

        virtual CARAPI GetDefaultActivity(
            /* [out] */ IResolveInfo** result);

        virtual CARAPI SetShowFooterView(
            /* [in] */ Boolean showFooterView);

        virtual CARAPI GetActivityCount(
            /* [out] */ Int32* result);

        virtual CARAPI GetHistorySize(
            /* [out] */ Int32* result);

        virtual CARAPI GetDataModel(
            /* [out] */ IActivityChooserModel** result);

        virtual CARAPI SetShowDefaultActivity(
            /* [in] */ Boolean showDefaultActivity,
            /* [in] */ Boolean highlightDefaultActivity);

        virtual CARAPI GetShowDefaultActivity(
            /* [out] */ Boolean* result);

    public:
        static const Int32 MAX_ACTIVITY_COUNT_UNLIMITED;
        static const Int32 MAX_ACTIVITY_COUNT_DEFAULT = 4;
        static const Int32 ITEM_VIEW_TYPE_ACTIVITY = 0;
        static const Int32 ITEM_VIEW_TYPE_FOOTER = 1;
        static const Int32 ITEM_VIEW_TYPE_COUNT = 3;

    private:
        ActivityChooserView* mOwner;
        AutoPtr<IActivityChooserModel> mDataModel;
        Int32 mMaxActivityCount;
        Boolean mShowDefaultActivity;
        Boolean mHighlightDefaultActivity;
        Boolean mShowFooterView;
    };

public:
    CAR_INTERFACE_DECL()

    ActivityChooserView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
      * {@inheritDoc}
      */
    virtual CARAPI SetActivityChooserModel(
        /* [in] */ IActivityChooserModel* dataModel);

    /**
      * Sets the background for the button that expands the activity
      * overflow list.
      *
      * <strong>Note:</strong> Clients would like to set this drawable
      * as a clue about the action the chosen activity will perform. For
      * example, if a share activity is to be chosen the drawable should
      * give a clue that sharing is to be performed.
      *
      * @param drawable The drawable.
      */
    virtual CARAPI SetExpandActivityOverflowButtonDrawable(
        /* [in] */ IDrawable* drawable);

    /**
      * Sets the content description for the button that expands the activity
      * overflow list.
      *
      * description as a clue about the action performed by the button.
      * For example, if a share activity is to be chosen the content
      * description should be something like "Share with".
      *
      * @param resourceId The content description resource id.
      */
    virtual CARAPI SetExpandActivityOverflowButtonContentDescription(
        /* [in] */ Int32 resourceId);

    /**
      * Set the provider hosting this view, if applicable.
      * @hide Internal use only
      */
    virtual CARAPI SetProvider(
        /* [in] */ IActionProvider* provider);

    /**
      * Shows the popup window with activities.
      *
      * @return True if the popup was shown, false if already showing.
      */
    virtual CARAPI ShowPopup(
        /* [out] */ Boolean* result);

    /**
      * Dismisses the popup window with activities.
      *
      * @return True if dismissed, false if already dismissed.
      */
    virtual CARAPI DismissPopup(
        /* [out] */ Boolean* result);

    /**
      * Gets whether the popup window with activities is shown.
      *
      * @return True if the popup is shown.
      */
    virtual CARAPI IsShowingPopup(
        /* [out] */ Boolean* result);

    virtual CARAPI GetDataModel(
        /* [out] */ IActivityChooserModel** result);

    /**
      * Sets a listener to receive a callback when the popup is dismissed.
      *
      * @param listener The listener to be notified.
      */
    virtual CARAPI SetOnDismissListener(
        /* [in] */ IPopupWindowOnDismissListener* listener);

    /**
      * Sets the initial count of items shown in the activities popup
      * i.e. the items before the popup is expanded. This is an upper
      * bound since it is not guaranteed that such number of intent
      * handlers exist.
      *
      * @param itemCount The initial popup item count.
      */
    virtual CARAPI SetInitialActivityCount(
        /* [in] */ Int32 itemCount);

    /**
      * Sets a content description of the default action button. This
      * resource should be a string taking one formatting argument and
      * will be used for formatting the content description of the button
      * dynamically as the default target changes. For example, a resource
      * pointing to the string "share with %1$s" will result in a content
      * description "share with Bluetooth" for the Bluetooth activity.
      *
      * @param resourceId The resource id.
      */
    virtual CARAPI SetDefaultActionButtonContentDescription(
        /* [in] */ Int32 resourceId);

protected:
    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    /**
      * Shows the popup no matter if it was already showing.
      *
      * @param maxActivityCount The max number of activities to display.
      */
    CARAPI ShowPopupUnchecked(
        /* [in] */ Int32 maxActivityCount);

    /**
      * Gets the list popup window which is lazily initialized.
      *
      * @return The popup.
      */
    CARAPI_(AutoPtr<ListPopupWindow>) GetListPopupWindow();

    /**
      * Updates the buttons state.
      */
    CARAPI_(void) UpdateAppearance();

public:
    AutoPtr<IActionProvider> mProvider;

private:
    static const String TAG;
    /*const*/ AutoPtr<ActivityChooserViewAdapter> mAdapter;
    /*const*/ AutoPtr<Callbacks> mCallbacks;
    /*const*/ AutoPtr<LinearLayout> mActivityChooserContent;
    /*const*/ AutoPtr<IDrawable> mActivityChooserContentBackground;
    /*const*/ AutoPtr<FrameLayout> mExpandActivityOverflowButton;
    /*const*/ AutoPtr<IImageView> mExpandActivityOverflowButtonImage;
    /*const*/ AutoPtr<FrameLayout> mDefaultActivityButton;
    /*const*/ AutoPtr<IImageView> mDefaultActivityButtonImage;
    /*const*/ Int32 mListPopupMaxWidth;
    /*const*/ AutoPtr<IDataSetObserver> mModelDataSetOberver;
    /*const*/ AutoPtr<IOnGlobalLayoutListener> mOnGlobalLayoutListener;
    AutoPtr<ListPopupWindow> mListPopupWindow;
    AutoPtr<IPopupWindowOnDismissListener> mOnDismissListener;
    Boolean mIsSelectingDefaultActivity;
    Int32 mInitialActivityCount;
    Boolean mIsAttachedToWindow;
    Int32 mDefaultActionButtonContentDescription;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_ACTIVITYCHOOSERVIEW_H__

