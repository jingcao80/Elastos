
#ifndef __ELASTOS_DROID_INTERNAL_APP_CALERTCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_APP_CALERTCONTROLLER_H__

#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Internal_App_CAlertController.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/widget/ListView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnApplyWindowInsetsListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IScrollView;
using Elastos::Droid::Widget::ListView;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CAlertController)
    , public Object
    , public IAlertController
{
    friend class CAlertControllerAlertParams;
public:
    class RecycleListView
        : public ListView
        , public IAlertControllerRecycleListView
    {
    public:
        RecycleListView();

        CAR_INTERFACE_DECL()

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

        CARAPI SetRecycleOnMeasure(
            /* [in] */ Boolean recycleOnMeasure);

    protected:
        CARAPI_(Boolean) RecycleOnMeasure();

    private:
        Boolean mRecycleOnMeasure;

    };

private:
    class ButtonViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
        friend class CAlertController;
    public:
        CAR_INTERFACE_DECL()

        ButtonViewOnClickListener(
            /* [in] */ CAlertController* host);

        ~ButtonViewOnClickListener();

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CAlertController* mHost;
        Boolean mTag;
    };

    class ButtonHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CAlertController::ButtonHandler")

        ButtonHandler(
            /* [in] */ IDialogInterface* dialog);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        // Button clicks have Message.what as the BUTTON{1,2,3} constant
        static const Int32 MSG_DISMISS_DIALOG;

    private:
        AutoPtr<IWeakReference> mDialog;
    };

    class OnApplyWindowInsetsListener
        : public Object
        , public IViewOnApplyWindowInsetsListener
    {
    public:
        OnApplyWindowInsetsListener(
            /* [in] */ CAlertController* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnApplyWindowInsets(
            /* [in] */ IView* view,
            /* [in] */ IWindowInsets* insets,
            /* [out] */ IWindowInsets** outsets);

    private:
        CAlertController* mHost;
    };

public:
    CAlertController();

    ~CAlertController();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IDialogInterface* di,
        /* [in] */ IWindow* window);

    CARAPI InstallContent();

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    /**
     * @see AlertDialog.Builder#setCustomTitle(View)
     */
    CARAPI SetCustomTitle(
        /* [in] */ IView* customTitleView);

    CARAPI SetMessage(
        /* [in] */ ICharSequence* message);

    /**
     * Set the view resource to display in the dialog.
     */
    CARAPI SetView(
        /* [in] */ Int32 layoutResId);

    /**
     * Set the view to display in the dialog.
     */
    CARAPI SetView(
        /* [in] */ IView* view);

    /**
     * Set the view to display in the dialog along with the spacing around that view
     */
    CARAPI SetView(
        /* [in] */ IView* view,
        /* [in] */ Int32 viewSpacingLeft,
        /* [in] */ Int32 viewSpacingTop,
        /* [in] */ Int32 viewSpacingRight,
        /* [in] */ Int32 viewSpacingBottom);

    /**
     * Sets a hint for the best button panel layout.
     */
    CARAPI SetButtonPanelLayoutHint(
        /* [in] */ Int32 layoutHint);

    /**
     * Sets a click listener or a message to be sent when the button is clicked.
     * You only need to pass one of {@code listener} or {@code msg}.
     *
     * @param whichButton Which button, can be one of
     *            {@link DialogInterface#BUTTON_POSITIVE},
     *            {@link DialogInterface#BUTTON_NEGATIVE}, or
     *            {@link DialogInterface#BUTTON_NEUTRAL}
     * @param text The text to display in positive button.
     * @param listener The {@link DialogInterface.OnClickListener} to use.
     * @param msg The {@link Message} to be sent when clicked.
     */
    CARAPI SetButton(
        /* [in] */ Int32 whichButton,
        /* [in] */ ICharSequence* text,
        /* [in] */ IDialogInterfaceOnClickListener* listener,
        /* [in] */ IMessage* msg);

    /**
     * Specifies the icon to display next to the alert title.
     *
     * @param resId the resource identifier of the drawable to use as the icon,
     *            or 0 for no icon
     */
    CARAPI SetIcon(
        /* [in] */ Int32 resId);

    /**
     * Specifies the icon to display next to the alert title.
     *
     * @param icon the drawable to use as the icon or null for no icon
     */
    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    /**
     * @param attrId the attributeId of the theme-specific drawable
     * to resolve the resourceId for.
     *
     * @return resId the resourceId of the theme-specific drawable
     */
    CARAPI GetIconAttributeResId(
        /* [in] */ Int32 attrId,
        /* [out] */ Int32* resId);

    CARAPI SetInverseBackgroundForced(
        /* [in] */ Boolean forceInverseBackground);

    CARAPI SetListView(
        /* [in] */ IListView* listview);

    CARAPI GetListView(
        /* [out] */ IListView** listview);

    CARAPI GetButton(
        /* [in] */ Int32 whichButton,
        /* [out] */ IButton** button);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

public:
    static CARAPI_(Boolean) CanTextInput(
        /* [in] */ IView* v);

protected:
    CARAPI_(void) OnLastStrongRef(const void* id);

private:
    static CARAPI_(Boolean) ShouldCenterSingleButton(
        /* [in] */ IContext* context);

    CARAPI_(Int32) SelectContentView();

    CARAPI_(void) SetupDecor();

    CARAPI_(void) SetupView();

    CARAPI_(Boolean) SetupTitle(
        /* [in] */ ILinearLayout* topPanel);

    CARAPI_(void) SetupContent(
        /* [in] */ ILinearLayout* contentPanel);

    CARAPI_(Boolean) SetupButtons();

    CARAPI_(void) CenterButton(
        /* [in] */ IButton* button);

    CARAPI_(void) SetBackground(
        /* [in] */ ITypedArray* a,
        /* [in] */ IView* topPanel,
        /* [in] */ IView* contentPanel,
        /* [in] */ IView* customPanel,
        /* [in] */ IView* buttonPanel,
        /* [in] */ Boolean hasTitle,
        /* [in] */ Boolean hasCustomView,
        /* [in] */ Boolean hasButtons);

private:
    IContext* mContext; // mContext is Activity, that usually hold this's reference
    // TODO: check memory leak
    AutoPtr<IDialogInterface> mDialogInterface;// mDialogInterface usually is XXXActivity too
    AutoPtr<IWindow> mWindow;

    AutoPtr<ICharSequence> mTitle;

    AutoPtr<ICharSequence> mMessage;

    AutoPtr<IListView> mListView;

    AutoPtr<IView> mView;

    Int32 mViewLayoutResId;

    Int32 mViewSpacingLeft;
    Int32 mViewSpacingTop;
    Int32 mViewSpacingRight;
    Int32 mViewSpacingBottom;
    Boolean mViewSpacingSpecified;

    AutoPtr<IButton> mButtonPositive;
    AutoPtr<ICharSequence> mButtonPositiveText;
    AutoPtr<IMessage> mButtonPositiveMessage;

    AutoPtr<IButton> mButtonNegative;
    AutoPtr<ICharSequence> mButtonNegativeText;
    AutoPtr<IMessage> mButtonNegativeMessage;

    AutoPtr<IButton> mButtonNeutral;
    AutoPtr<ICharSequence> mButtonNeutralText;
    AutoPtr<IMessage> mButtonNeutralMessage;

    AutoPtr<IScrollView> mScrollView;

    Int32 mIconId;
    AutoPtr<IDrawable> mIcon;

    AutoPtr<IImageView> mIconView;
    AutoPtr<ITextView> mTitleView;
    AutoPtr<ITextView> mMessageView;
    AutoPtr<IView> mCustomTitleView;

    Boolean mForceInverseBackground;

    AutoPtr<IListAdapter> mAdapter;

    Int32 mCheckedItem;

    Int32 mAlertDialogLayout;
    Int32 mButtonPanelSideLayout;
    Int32 mListLayout;
    Int32 mMultiChoiceItemLayout;
    Int32 mSingleChoiceItemLayout;
    Int32 mListItemLayout;

    Int32 mButtonPanelLayoutHint;

    AutoPtr<ButtonHandler> mHandler;

    AutoPtr<ButtonViewOnClickListener> mButtonHandler;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CALERTCONTROLLER_H__
