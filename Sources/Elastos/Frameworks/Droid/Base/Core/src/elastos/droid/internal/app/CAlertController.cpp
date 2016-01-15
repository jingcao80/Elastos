
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/app/CAlertController.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/widget/CLinearLayoutLayoutParams.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::EIID_IDialogInterface;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::View::EIID_IViewOnApplyWindowInsetsListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

//==============================================================================
//                  CAlertController::ButtonViewOnClickListener
//==============================================================================
CAR_INTERFACE_IMPL(CAlertController::ButtonViewOnClickListener, Object, IViewOnClickListener);

CAlertController::ButtonViewOnClickListener::ButtonViewOnClickListener(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{
}

ECode CAlertController::ButtonViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IAlertController> ac;
    mWeakHost->Resolve(EIID_IAlertController, (IInterface**)&ac);
    if (ac == NULL) {
        Slogger::E("CAlertController", "ButtonViewOnClickListener::OnClick, CAlertController has been destoryed!");
        return NOERROR;
    }

    CAlertController* mHost = (CAlertController*)ac.Get();

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> m;
    if (v == IView::Probe(mHost->mButtonPositive) && mHost->mButtonPositiveMessage != NULL) {
        helper->Obtain(mHost->mButtonPositiveMessage, (IMessage**)&m);
    }
    else if (v == IView::Probe(mHost->mButtonNegative) && mHost->mButtonNegativeMessage != NULL) {
        helper->Obtain(mHost->mButtonNegativeMessage, (IMessage**)&m);
    }
    else if (v == IView::Probe(mHost->mButtonNeutral) && mHost->mButtonNeutralMessage != NULL) {
        helper->Obtain(mHost->mButtonNeutralMessage, (IMessage**)&m);
    }
    if (m != NULL) {
        m->SendToTarget();
    }

    // Post a message so we dismiss after the above handlers are executed
    AutoPtr<IMessage> msg;
    helper->Obtain(mHost->mHandler, ButtonHandler::MSG_DISMISS_DIALOG, ac, (IMessage**)&msg);
    msg->SendToTarget();

    return NOERROR;
}

//==============================================================================
//                  CAlertController::ButtonHandler
//==============================================================================
const Int32 CAlertController::ButtonHandler::MSG_DISMISS_DIALOG = 1;

CAlertController::ButtonHandler::ButtonHandler(
    /* [in] */ IDialogInterface* dialog)
{
    AutoPtr<IWeakReferenceSource> wr = IWeakReferenceSource::Probe(dialog);
    if (wr) {
        wr->GetWeakReference((IWeakReference**)&mDialog);
    }
}

ECode CAlertController::ButtonHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case IDialogInterface::BUTTON_POSITIVE:
        case IDialogInterface::BUTTON_NEGATIVE:
        case IDialogInterface::BUTTON_NEUTRAL: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IDialogInterfaceOnClickListener* listener = IDialogInterfaceOnClickListener::Probe(obj);
            if (listener) {
                AutoPtr<IDialogInterface> dialog;
                mDialog->Resolve(EIID_IDialogInterface, (IInterface**)&dialog);
                listener->OnClick(dialog, what);
            }
            break;
        }

        case MSG_DISMISS_DIALOG: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IDialogInterface::Probe(obj)->Dismiss();
            break;
        }
    }
    return NOERROR;
}

//==============================================================================
//                  CAlertController::OnApplyWindowInsetsListener
//==============================================================================
CAR_INTERFACE_IMPL(CAlertController::OnApplyWindowInsetsListener, Object, IViewOnApplyWindowInsetsListener);

CAlertController::OnApplyWindowInsetsListener::OnApplyWindowInsetsListener(
    /* [in] */ CAlertController* host)
    : mHost(host)
{}

// @Override
ECode CAlertController::OnApplyWindowInsetsListener::OnApplyWindowInsets(
    /* [in] */ IView* view,
    /* [in] */ IWindowInsets* insets,
    /* [out] */ IWindowInsets** outsets)
{
    AutoPtr<IView> parent;
    mHost->mWindow->FindViewById(R::id::parentPanel, (IView**)&parent);

    Boolean isRound;
    if (insets->IsRound(&isRound), isRound) {
        // TODO: Get the padding as a function of the window size.
        AutoPtr<IResources> res;
        mHost->mContext->GetResources((IResources**)&res);
        Int32 roundOffset;
        res->GetDimensionPixelOffset(
            R::dimen::alert_dialog_round_padding, &roundOffset);
        parent->SetPadding(roundOffset, roundOffset, roundOffset, roundOffset);
    }
    return insets->ConsumeSystemWindowInsets(outsets);
}

//==============================================================================
//                  CAlertController::RecycleListView
//==============================================================================

// CAR_INTERFACE_IMPL(CAlertController::RecycleListView, ListView, IRecycleListView);
CAR_INTERFACE_IMPL(CAlertController::RecycleListView, Object, IRecycleListView);

CAlertController::RecycleListView::RecycleListView()
    : mRecycleOnMeasure(TRUE)
{
}

ECode CAlertController::RecycleListView::constructor(
    /* [in] */ IContext* context)
{
    assert(0);
    return NOERROR;
    // return ListView::constructor(context);
}

ECode CAlertController::RecycleListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    assert(0);
    return NOERROR;
    // return ListView::constructor(context, attrs);
}

ECode CAlertController::RecycleListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    assert(0);
    return NOERROR;
    // return ListView::constructor(context, attrs, defStyleAttr,);
}

ECode CAlertController::RecycleListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    assert(0);
    return NOERROR;
    // return ListView::constructor(context, attrs, defStyleAttr, defStyleRes);
}

Boolean CAlertController::RecycleListView::RecycleOnMeasure()
{
    return mRecycleOnMeasure;
}

ECode CAlertController::RecycleListView::SetRecycleOnMeasure(
    /* [in] */ Boolean recycleOnMeasure)
{
    mRecycleOnMeasure = recycleOnMeasure;
    return NOERROR;
}

//==============================================================================
//                  CAlertController
//==============================================================================

CAR_INTERFACE_IMPL(CAlertController, Object, IAlertController)

CAR_OBJECT_IMPL(CAlertController)

CAlertController::CAlertController()
    : mViewLayoutResId(0)
    , mViewSpacingLeft(0)
    , mViewSpacingTop(0)
    , mViewSpacingRight(0)
    , mViewSpacingBottom(0)
    , mViewSpacingSpecified(FALSE)
    , mIconId(0)
    , mForceInverseBackground(FALSE)
    , mCheckedItem(-1)
    , mAlertDialogLayout(0)
    , mButtonPanelSideLayout(0)
    , mListLayout(0)
    , mMultiChoiceItemLayout(0)
    , mSingleChoiceItemLayout(0)
    , mListItemLayout(0)
    , mButtonPanelLayoutHint(IAlertDialog::LAYOUT_HINT_NONE)
{
    // Slogger::V("CAlertController", " >> create CAlertController(): %p", this);
}

CAlertController::~CAlertController()
{
    // Slogger::V("CAlertController", " >> destory ~CAlertController(): %p", this);
}

Boolean CAlertController::ShouldCenterSingleButton(
    /* [in] */ IContext* context)
{
    AutoPtr<ITypedValue> outValue;
    CTypedValue::New((ITypedValue**)&outValue);
    AutoPtr<IResourcesTheme> rTheme;
    context->GetTheme((IResourcesTheme**)&rTheme);
    Boolean result;
    rTheme->ResolveAttribute(R::attr::alertDialogCenterButtons, outValue, TRUE, &result);
    Int32 data;
    outValue->GetData(&data);
    return data != 0;
}

ECode CAlertController::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDialogInterface* di,
    /* [in] */ IWindow* window)
{
    mContext = context;
    mDialogInterface = di;
    mWindow = window;
    mHandler = new ButtonHandler(di);
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    mButtonHandler = new ButtonViewOnClickListener(wr);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::AlertDialog),
        ArraySize(R::styleable::AlertDialog));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
        NULL, attrIds, R::attr::alertDialogStyle, 0, (ITypedArray**)&a);
    assert(a != NULL);
    a->GetResourceId(R::styleable::AlertDialog_layout,
            R::layout::alert_dialog, &mAlertDialogLayout);
    a->GetResourceId(R::styleable::AlertDialog_buttonPanelSideLayout,
            R::layout::alert_dialog, &mButtonPanelSideLayout);
    a->GetResourceId(
            R::styleable::AlertDialog_listLayout,
            R::layout::select_dialog, &mListLayout);
    a->GetResourceId(
            R::styleable::AlertDialog_multiChoiceItemLayout,
            R::layout::select_dialog_multichoice, &mMultiChoiceItemLayout);
    a->GetResourceId(
            R::styleable::AlertDialog_singleChoiceItemLayout,
            R::layout::select_dialog_singlechoice, &mSingleChoiceItemLayout);
    a->GetResourceId(
            R::styleable::AlertDialog_listItemLayout,
            R::layout::select_dialog_item, &mListItemLayout);

    a->Recycle();
    return NOERROR;
}

Boolean CAlertController::CanTextInput(
    /* [in] */ IView* v)
{
    Boolean result;
    v->OnCheckIsTextEditor(&result);
    if (result) {
        return TRUE;
    }

    AutoPtr<IViewGroup> vg = IViewGroup::Probe(v);
    if (NULL == vg) {
        return FALSE;
    }

    Int32 i;
    vg->GetChildCount(&i);
    AutoPtr<IView> tempView;
    while (i > 0) {
        i--;
        tempView = NULL;
        vg->GetChildAt(i, (IView**)&tempView);
        if (CanTextInput(tempView)) {
            return TRUE;
        }
    }

    return FALSE;
}

ECode CAlertController::InstallContent()
{
    /* We use a custom title so never request a window title */
    Boolean result;
    mWindow->RequestFeature(IWindow::FEATURE_NO_TITLE, &result);
    Int32 contentView = SelectContentView();
    mWindow->SetContentView(contentView);
    SetupView();
    SetupDecor();
    return NOERROR;
}

Int32 CAlertController::SelectContentView()
{
    if (mButtonPanelSideLayout == 0) {
        return mAlertDialogLayout;
    }
    if (mButtonPanelLayoutHint == IAlertDialog::LAYOUT_HINT_SIDE) {
        return mButtonPanelSideLayout;
    }
    // TODO: use layout hint side for long messages/lists
    return mAlertDialogLayout;
}

ECode CAlertController::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;
    if (mTitleView != NULL) {
        mTitleView->SetText(title);
    }
    return NOERROR;
}

ECode CAlertController::SetCustomTitle(
    /* [in] */ IView* customTitleView)
{
    mCustomTitleView = customTitleView;
    return NOERROR;
}

ECode CAlertController::SetMessage(
       /* [in] */ ICharSequence* message)
{
    mMessage = message;
    if (mMessageView != NULL) {
        mMessageView->SetText(message);
    }
    return NOERROR;
}

ECode CAlertController::SetView(
    /* [in] */ Int32 layoutResId)
{
    mView = NULL;
    mViewLayoutResId = layoutResId;
    mViewSpacingSpecified = FALSE;
    return NOERROR;
}

ECode CAlertController::SetView(
       /* [in] */ IView* view)
{
    mView = view;
    mViewLayoutResId = 0;
    mViewSpacingSpecified = FALSE;
    return NOERROR;
}

ECode CAlertController::SetView(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSpacingLeft,
    /* [in] */ Int32 viewSpacingTop,
    /* [in] */ Int32 viewSpacingRight,
    /* [in] */ Int32 viewSpacingBottom)
{
    mView = view;
    mViewLayoutResId = 0;
    mViewSpacingSpecified = TRUE;
    mViewSpacingLeft = viewSpacingLeft;
    mViewSpacingTop = viewSpacingTop;
    mViewSpacingRight = viewSpacingRight;
    mViewSpacingBottom = viewSpacingBottom;
    return NOERROR;
}

ECode CAlertController::SetButtonPanelLayoutHint(
    /* [in] */ Int32 layoutHint)
{
    mButtonPanelLayoutHint = layoutHint;
    return NOERROR;
}

ECode CAlertController::SetButton(
    /* [in] */ Int32 whichButton,
    /* [in] */ ICharSequence* text,
    /* [in] */ IDialogInterfaceOnClickListener* listener,
    /* [in] */ IMessage* _msg)
{
    AutoPtr<IMessage> msg = _msg;
    if (msg == NULL && listener != NULL) {
        mHandler->ObtainMessage(whichButton, listener, (IMessage**)&msg);
    }

    switch (whichButton) {
        case IDialogInterface::BUTTON_POSITIVE:
            mButtonPositiveText = text;
            mButtonPositiveMessage = msg;
            break;

        case IDialogInterface::BUTTON_NEGATIVE:
            mButtonNegativeText = text;
            mButtonNegativeMessage = msg;
            break;

        case IDialogInterface::BUTTON_NEUTRAL:
            mButtonNeutralText = text;
            mButtonNeutralMessage = msg;
            break;

        default:
            Slogger::E("CAlertController", "SetButton: Button does not exist");
//             throw new IllegalArgumentException("Button does not exist");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CAlertController::SetIcon(
   /* [in] */ Int32 resId)
{
    mIcon = NULL;
    mIconId = resId;

    if (mIconView != NULL) {
        if (resId != 0) {
            mIconView->SetImageResource(mIconId);
        }
        else {
            IView::Probe(mIconView)->SetVisibility(IView::GONE);
        }
    }
    return NOERROR;
}

ECode CAlertController::SetIcon(
   /* [in] */ IDrawable* icon)
{
    mIcon = icon;
    mIconId = 0;

    if (mIconView != NULL) {
        if (mIcon != NULL) {
            mIconView->SetImageDrawable(icon);
        }
        else {
            IView::Probe(mIconView)->SetVisibility(IView::GONE);
        }
    }
    return NOERROR;
}

ECode CAlertController::GetIconAttributeResId(
    /* [in] */ Int32 attrId,
    /* [out] */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId);
    AutoPtr<ITypedValue> out;
    CTypedValue::New((ITypedValue**)&out);
    AutoPtr<IResourcesTheme> rTheme;
    mContext->GetTheme((IResourcesTheme**)&rTheme);
    Boolean result;
    rTheme->ResolveAttribute(attrId, out, TRUE, &result);
    out->GetResourceId(resId);
    return NOERROR;
}

ECode CAlertController::SetInverseBackgroundForced(
       /* [in] */ Boolean forceInverseBackground)
{
    mForceInverseBackground = forceInverseBackground;
    return NOERROR;
}

ECode CAlertController::SetListView(
    /* [in] */ IListView* listview)
{
    mListView = listview;
    return NOERROR;
}

ECode CAlertController::GetListView(
       /* [out] */ IListView** listview)
{
    VALIDATE_NOT_NULL(listview);
    *listview = mListView;
    REFCOUNT_ADD(*listview);
    return NOERROR;
}

ECode CAlertController::GetButton(
    /* [in] */ Int32 whichButton,
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button);
    *button = NULL;
    switch (whichButton) {
        case IDialogInterface::BUTTON_POSITIVE:
            *button = mButtonPositive;
            break;
        case IDialogInterface::BUTTON_NEGATIVE:
            *button = mButtonNegative;
            break;
        case IDialogInterface::BUTTON_NEUTRAL:
            *button = mButtonNeutral;
            break;
        default:
            break;
    }

    REFCOUNT_ADD(*button);
    return NOERROR;
}

ECode CAlertController::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean handled;
    *result = mScrollView != NULL && (mScrollView->ExecuteKeyEvent(event, &handled), handled);
    return NOERROR;
}

ECode CAlertController::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean handled;
    *result = mScrollView != NULL && (mScrollView->ExecuteKeyEvent(event, &handled), handled);
    return NOERROR;
}

void CAlertController::SetupDecor()
{
    AutoPtr<IView> decor;
    mWindow->GetDecorView((IView**)&decor);
    AutoPtr<IView> parent;
    mWindow->FindViewById(R::id::parentPanel, (IView**)&parent);
    if (parent != NULL && decor != NULL) {
        AutoPtr<OnApplyWindowInsetsListener> listener = new OnApplyWindowInsetsListener(this);
        decor->SetOnApplyWindowInsetsListener(listener);
        decor->SetFitsSystemWindows(TRUE);
        decor->RequestApplyInsets();
    }
}

void CAlertController::SetupView()
{
    AutoPtr<IView> contentPanel;
    mWindow->FindViewById(R::id::contentPanel, (IView**)&contentPanel);
    SetupContent(ILinearLayout::Probe(contentPanel));

    Boolean hasButtons = SetupButtons();

    AutoPtr<IView> topPanel;
    mWindow->FindViewById(R::id::topPanel, (IView**)&topPanel);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::AlertDialog),
        ArraySize(R::styleable::AlertDialog));
    AutoPtr<ITypedArray> a;
    mContext->ObtainStyledAttributes(
        NULL, attrIds, R::attr::alertDialogStyle, 0, (ITypedArray**)&a);

    Boolean hasTitle = SetupTitle(ILinearLayout::Probe(topPanel));

    AutoPtr<IView> buttonPanel;
    mWindow->FindViewById(R::id::buttonPanel, (IView**)&buttonPanel);
    if (!hasButtons) {
        buttonPanel->SetVisibility(IView::GONE);
        AutoPtr<IView> spacer;
        mWindow->FindViewById(R::id::textSpacerNoButtons, (IView**)&spacer);
        if (spacer != NULL) {
            spacer->SetVisibility(IView::VISIBLE);
        }
        mWindow->SetCloseOnTouchOutsideIfNotSet(TRUE);
    }

    AutoPtr<IView> customPanel;
    mWindow->FindViewById(R::id::customPanel, (IView**)&customPanel);
    AutoPtr<IView> customView;
    if (mView != NULL) {
         customView = mView;
    }
    else if (mViewLayoutResId != 0) {
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
        inflater->Inflate(mViewLayoutResId, IViewGroup::Probe(customPanel), FALSE, (IView**)&customView);
    }
    else {
        customView = NULL;
    }

    Boolean hasCustomView = customView != NULL;
    if (!hasCustomView || !CanTextInput(customView)) {
        mWindow->SetFlags(IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM,
            IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM);
    }

    if (hasCustomView) {
        AutoPtr<IView> tempView;
        mWindow->FindViewById(R::id::custom, (IView**)&tempView);
        AutoPtr<IFrameLayout> custom = IFrameLayout::Probe(tempView);

        AutoPtr<IViewGroupLayoutParams> lParams;
        CViewGroupLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::MATCH_PARENT,
                (IViewGroupLayoutParams**)&lParams);
        IViewGroup::Probe(custom)->AddView(customView, lParams);
        if (mViewSpacingSpecified) {
            IView::Probe(custom)->SetPadding(mViewSpacingLeft, mViewSpacingTop, mViewSpacingRight,
                    mViewSpacingBottom);
        }
        if (mListView != NULL) {
            AutoPtr<IViewGroupLayoutParams> vlp;
            customPanel->GetLayoutParams((IViewGroupLayoutParams**)&vlp);
            AutoPtr<ILinearLayoutLayoutParams> vglParams = ILinearLayoutLayoutParams::Probe(vlp);
            vglParams->SetWeight(0);
        }
    }
    else {
        customPanel->SetVisibility(IView::GONE);
    }


    /* Only display the divider if we have a title and a
     * custom view or a message.
     */
    if (hasTitle) {
        AutoPtr<IView> divider;
        if (mMessage != NULL || customView != NULL || mListView != NULL) {
            mWindow->FindViewById(R::id::titleDivider, (IView**)&divider);
        }
        else {
            mWindow->FindViewById(R::id::titleDividerTop, (IView**)&divider);
        }

        if (divider != NULL) {
            divider->SetVisibility(IView::VISIBLE);
        }
    }

    SetBackground(a, topPanel, contentPanel, customPanel,
        buttonPanel, hasTitle, hasCustomView, hasButtons);
    a->Recycle();
}

Boolean CAlertController::SetupTitle(
    /* [in] */ ILinearLayout* topPanel)
{
    Boolean hasTitle = TRUE;

    if (mCustomTitleView != NULL) {
        // Add the custom title view directly to the topPanel layout
        AutoPtr<IViewGroupLayoutParams> lp;
        CLinearLayoutLayoutParams::New(
            IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::WRAP_CONTENT,
            (IViewGroupLayoutParams**)&lp);

        IViewGroup::Probe(topPanel)->AddView(mCustomTitleView, 0, lp);

        // Hide the title template
        AutoPtr<IView> titleTemplate;
        mWindow->FindViewById(R::id::title_template, (IView**)&titleTemplate);
        titleTemplate->SetVisibility(IView::GONE);
    }
    else {
        AutoPtr<IView> iconView;
        mWindow->FindViewById(R::id::icon, (IView**)&iconView);
        mIconView = IImageView::Probe(iconView.Get());
        assert(mIconView != NULL);

        Boolean hasTextTitle = !TextUtils::IsEmpty(mTitle);
        if (hasTextTitle) {
            /* Display the title if a title is supplied, else hide it */
            AutoPtr<IView> titleView;
            mWindow->FindViewById(R::id::alertTitle, (IView**)&titleView);
            mTitleView = ITextView::Probe(titleView.Get());
            assert(mTitleView != NULL);
            mTitleView->SetText(mTitle);

            /* Do this last so that if the user has supplied any
             * icons we use them instead of the default ones. If the
             * user has specified 0 then make it disappear.
             */
            if (mIconId != 0) {
                mIconView->SetImageResource(mIconId);
            }
            else if (mIcon != NULL) {
                mIconView->SetImageDrawable(mIcon);
            }
            else {
                /* Apply the padding from the icon to ensure the
                 * title is aligned correctly.
                 */
                Int32 left, top, right, bottom;
                iconView->GetPaddingLeft(&left);
                iconView->GetPaddingTop(&top);
                iconView->GetPaddingRight(&right);
                iconView->GetPaddingBottom(&bottom);
                titleView->SetPadding(left, top, right, bottom);
                iconView->SetVisibility(IView::GONE);
            }
        }
        else {
            // Hide the title template
            AutoPtr<IView> titleTemplate;
            mWindow->FindViewById(R::id::title_template, (IView**)&titleTemplate);
            titleTemplate->SetVisibility(IView::GONE);
            iconView->SetVisibility(IView::GONE);
            IView::Probe(topPanel)->SetVisibility(IView::GONE);
            hasTitle = FALSE;
        }
    }
    return hasTitle;
}

void CAlertController::SetupContent(
    /* [in] */ ILinearLayout* contentPanel)
{
    AutoPtr<IView> scrollView;
    mWindow->FindViewById(R::id::scrollView, (IView**)&scrollView);
    mScrollView = IScrollView::Probe(scrollView.Get());
    assert(mScrollView != NULL);
    scrollView->SetFocusable(FALSE);

    // Special case for users that only want to display a String
    AutoPtr<IView> messageView;
    mWindow->FindViewById(R::id::message, (IView**)&messageView);
    mMessageView = ITextView::Probe(messageView.Get());
    if (mMessageView == NULL) {
        return;
    }

    if (mMessage != NULL) {
        mMessageView->SetText(mMessage);
    }
    else {
        messageView->SetVisibility(IView::GONE);
        IViewGroup::Probe(mScrollView)->RemoveViewInLayout(messageView);

        if (mListView != NULL) {
            IViewGroup::Probe(contentPanel)->RemoveViewInLayout(scrollView);

            Int32 lp = IViewGroupLayoutParams::MATCH_PARENT;
            AutoPtr<IViewGroupLayoutParams> linearParams;
            CLinearLayoutLayoutParams::New(lp, lp, (IViewGroupLayoutParams**)&linearParams);
            IViewGroup::Probe(contentPanel)->AddView(IView::Probe(mListView), linearParams);

            AutoPtr<IViewGroupLayoutParams> lParams;
            CLinearLayoutLayoutParams::New(lp, lp, 1.0f, (IViewGroupLayoutParams**)&lParams);
            IView::Probe(contentPanel)->SetLayoutParams(lParams);
        }
        else {
            IView::Probe(contentPanel)->SetVisibility(IView::GONE);
        }
    }
}

Boolean CAlertController::SetupButtons()
{
    Int32 BIT_BUTTON_POSITIVE = 1;
    Int32 BIT_BUTTON_NEGATIVE = 2;
    Int32 BIT_BUTTON_NEUTRAL = 4;
    Int32 whichButtons = 0;

    AutoPtr<IView> buttonPositive;
    mWindow->FindViewById(R::id::button1, (IView**)&buttonPositive);
    mButtonPositive = IButton::Probe(buttonPositive);
    assert(mButtonPositive != NULL);
    buttonPositive->SetOnClickListener(mButtonHandler);

    if (TextUtils::IsEmpty(mButtonPositiveText)) {
        buttonPositive->SetVisibility(IView::GONE);
    }
    else {
        ITextView::Probe(mButtonPositive)->SetText(mButtonPositiveText);
        buttonPositive->SetVisibility(IView::VISIBLE);
        whichButtons = whichButtons | BIT_BUTTON_POSITIVE;
    }

    AutoPtr<IView> buttonNegative;
    mWindow->FindViewById(R::id::button2, (IView**)&buttonNegative);
    mButtonNegative = IButton::Probe(buttonNegative);
    buttonNegative->SetOnClickListener(mButtonHandler);

    if (TextUtils::IsEmpty(mButtonNegativeText)) {
        buttonNegative->SetVisibility(IView::GONE);
    }
    else {
        ITextView::Probe(mButtonNegative)->SetText(mButtonNegativeText);
        buttonNegative->SetVisibility(IView::VISIBLE);

        whichButtons = whichButtons | BIT_BUTTON_NEGATIVE;
    }

    AutoPtr<IView> buttonNeutral;
    mWindow->FindViewById(R::id::button3, (IView**)&buttonNeutral);
    mButtonNeutral = IButton::Probe(buttonNeutral);
    buttonNeutral->SetOnClickListener(mButtonHandler);

    if (TextUtils::IsEmpty(mButtonNeutralText)) {
        buttonNeutral->SetVisibility(IView::GONE);
    }
    else {
        ITextView::Probe(mButtonNeutral)->SetText(mButtonNeutralText);
        buttonNeutral->SetVisibility(IView::VISIBLE);

        whichButtons = whichButtons | BIT_BUTTON_NEUTRAL;
    }

    if (ShouldCenterSingleButton(mContext)) {
        /*
         * If we only have 1 button it should be centered on the layout and
         * expand to fill 50% of the available space.
         */
        if (whichButtons == BIT_BUTTON_POSITIVE) {
            CenterButton(mButtonPositive);
        }
        else if (whichButtons == BIT_BUTTON_NEGATIVE) {
            CenterButton(mButtonNegative);
        }
        else if (whichButtons == BIT_BUTTON_NEUTRAL) {
            CenterButton(mButtonNeutral);
        }
    }

    return whichButtons != 0;
}

void CAlertController::CenterButton(
    /* [in] */ IButton* button)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    IView::Probe(button)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    AutoPtr<ILinearLayoutLayoutParams> params = ILinearLayoutLayoutParams::Probe(lp);
    params->SetGravity(IGravity::CENTER_HORIZONTAL);
    params->SetWeight(0.5f);
    IView::Probe(button)->SetLayoutParams(lp);
    AutoPtr<IView> leftSpacer;
    mWindow->FindViewById(R::id::leftSpacer, (IView**)&leftSpacer);
    if (leftSpacer != NULL) {
        leftSpacer->SetVisibility(IView::VISIBLE);
    }
    AutoPtr<IView> rightSpacer;
    mWindow->FindViewById(R::id::rightSpacer, (IView**)&rightSpacer);
    if (rightSpacer != NULL) {
        rightSpacer->SetVisibility(IView::VISIBLE);
    }
}

void CAlertController::SetBackground(
    /* [in] */ ITypedArray* a,
    /* [in] */ IView* topPanel,
    /* [in] */ IView* contentPanel,
    /* [in] */ IView* customPanel,
    /* [in] */ IView* buttonPanel,
    /* [in] */ Boolean hasTitle,
    /* [in] */ Boolean hasCustomView,
    /* [in] */ Boolean hasButtons)
{
    Int32 fullDark = 0;
    Int32 topDark = 0;
    Int32 centerDark = 0;
    Int32 bottomDark = 0;
    Int32 fullBright = 0;
    Int32 topBright = 0;
    Int32 centerBright = 0;
    Int32 bottomBright = 0;
    Int32 bottomMedium = 0;

    // If the needsDefaultBackgrounds attribute is set, we know we're
    // inheriting from a framework style.
    Boolean needsDefaultBackgrounds;
    a->GetBoolean(
        R::styleable::AlertDialog_needsDefaultBackgrounds, TRUE, &needsDefaultBackgrounds);

    if (needsDefaultBackgrounds) {
        fullDark = R::drawable::popup_full_dark;
        topDark = R::drawable::popup_top_dark;
        centerDark = R::drawable::popup_center_dark;
        bottomDark = R::drawable::popup_bottom_dark;
        fullBright = R::drawable::popup_full_bright;
        topBright = R::drawable::popup_top_bright;
        centerBright = R::drawable::popup_center_bright;
        bottomBright = R::drawable::popup_bottom_bright;
        bottomMedium = R::drawable::popup_bottom_medium;
    }

    a->GetResourceId(R::styleable::AlertDialog_topBright, topBright, &topBright);
    a->GetResourceId(R::styleable::AlertDialog_topDark, topDark, &topDark);
    a->GetResourceId(R::styleable::AlertDialog_centerBright, centerBright, &centerBright);
    a->GetResourceId(R::styleable::AlertDialog_centerDark, centerDark, &centerDark);

    /*
     * We now set the background of all of the sections of the alert.
     * First collect together each section that is being displayed along
     * with whether it is on a light or dark background, then run through
     * them setting their backgrounds.  This is complicated because we need
     * to correctly use the full, top, middle, and bottom graphics depending
     * on how many views they are and where they appear.
     */

    AutoPtr<ArrayOf<IView*> > views = ArrayOf<IView*>::Alloc(4);
    AutoPtr<ArrayOf<Boolean> > light = ArrayOf<Boolean>::Alloc(4);
    AutoPtr<IView> lastView = NULL;
    Boolean lastLight = FALSE;

    Int32 pos = 0;
    if (hasTitle) {
        views->Set(pos, topPanel);
        (*light)[pos] = FALSE;
        pos++;
    }

    /* The contentPanel displays either a custom text message or
     * a ListView. If it's text we should use the dark background
     * for ListView we should use the light background. If neither
     * are there the contentPanel will be hidden so set it as null.
     */
    Int32 visibility;
    contentPanel->GetVisibility(&visibility);
    if (visibility == IView::GONE) {
        views->Set(pos, NULL);
    }
    else {
        views->Set(pos, contentPanel);
    }
    (*light)[pos] = mListView != NULL;
    pos++;

    if (hasCustomView) {
        views->Set(pos, customPanel);
        (*light)[pos] = mForceInverseBackground;
        pos++;
    }

    if (hasButtons) {
        views->Set(pos, buttonPanel);
        (*light)[pos] = TRUE;
    }

    Boolean setView = FALSE;
    for (pos = 0; pos < views->GetLength(); pos++) {
        AutoPtr<IView> v = (*views)[pos];
        if (v == NULL) {
            continue;
        }
        if (lastView != NULL) {
            if (!setView) {
                lastView->SetBackgroundResource(lastLight ? topBright : topDark);
            }
            else {
                lastView->SetBackgroundResource(lastLight ? centerBright : centerDark);
            }
            setView = TRUE;
        }
        lastView = v;
        lastLight = (*light)[pos];
    }

    if (lastView != NULL) {
        if (setView) {
            a->GetResourceId(R::styleable::AlertDialog_bottomBright,
                bottomBright, &bottomBright);
            a->GetResourceId(R::styleable::AlertDialog_bottomMedium,
                bottomMedium, &bottomMedium);
            a->GetResourceId(R::styleable::AlertDialog_bottomDark,
                bottomDark, &bottomDark);

            /* ListViews will use the Bright background but buttons use
             * the Medium background.
             */
            lastView->SetBackgroundResource(
                    lastLight ? (hasButtons ? bottomMedium : bottomBright) : bottomDark);
        }
        else {
            a->GetResourceId(R::styleable::AlertDialog_fullBright,
                fullBright, &fullBright);
            a->GetResourceId(R::styleable::AlertDialog_fullDark,
                fullDark, &fullDark);

            lastView->SetBackgroundResource(lastLight ? fullBright : fullDark);
        }
    }

    AutoPtr<IAdapterView> listView = IAdapterView::Probe(mListView);
    if (listView != NULL && mAdapter != NULL) {
        listView->SetAdapter(IAdapter::Probe(mAdapter));
        if (mCheckedItem > -1) {
            IAbsListView::Probe(listView)->SetItemChecked(mCheckedItem, TRUE);
            listView->SetSelection(mCheckedItem);
        }
    }
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
