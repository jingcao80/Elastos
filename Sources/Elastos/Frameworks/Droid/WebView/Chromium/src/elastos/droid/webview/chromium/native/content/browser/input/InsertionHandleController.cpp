
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/InsertionHandleController.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/HandleView.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::View;
using Elastos::Droid::View::CView;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::R;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

//==================================================================
//            InsertionHandleController::PastePopupMenu
//==================================================================
CAR_INTERFACE_IMPL(InsertionHandleController::PastePopupMenu, Object, IViewOnClickListener);

InsertionHandleController::PastePopupMenu::PastePopupMenu(
    /* [in] */ InsertionHandleController* owner)
    : mOwner(owner)
    , mPositionX(0)
    , mPositionY(0)
{
    CPopupWindow::New(mOwner->mContext, NULL,
             R::attr::textSelectHandleWindowStyle,
             (IPopupWindow**)&mContainer);
    mContainer->SetSplitTouchEnabled(TRUE);
    mContainer->SetClippingEnabled(FALSE);

    mContainer->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
    mContainer->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);

    AutoPtr< ArrayOf<Int32> > POPUP_LAYOUT_ATTRS = ArrayOf<Int32>::Alloc(4);
    (*POPUP_LAYOUT_ATTRS)[0] = R::attr::textEditPasteWindowLayout;
    (*POPUP_LAYOUT_ATTRS)[1] = R::attr::textEditNoPasteWindowLayout;
    (*POPUP_LAYOUT_ATTRS)[2] = R::attr::textEditSidePasteWindowLayout;
    (*POPUP_LAYOUT_ATTRS)[3] = R::attr::textEditSideNoPasteWindowLayout;

    Int32 length = POPUP_LAYOUT_ATTRS->GetLength();
    mPasteViews = ArrayOf<IView*>::Alloc(length);
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IView> view = new View::View();
        //CView::New((IView**)&view);
        (*mPasteViews)[i] = view;
    }

    mPasteViewLayouts = ArrayOf<Int32>::Alloc(length);

    AutoPtr<ITypedArray> attrs;
    mOwner->mContext->ObtainStyledAttributes(POPUP_LAYOUT_ATTRS, (ITypedArray**)&attrs);
    Int32 attrsLength;
    attrs->GetLength(&attrsLength);
    for (Int32 i = 0; i < attrsLength; ++i) {
        Int32 index;
        attrs->GetIndex(i, &index);
        attrs->GetResourceId(index, 0, &((*mPasteViewLayouts)[i]));
    }
    attrs->Recycle();
}

Int32 InsertionHandleController::PastePopupMenu::ViewIndex(
    /* [in] */ Boolean onTop)
{
    return (onTop ? 0 : 1 << 1) + (mOwner->CanPaste() ? 0 : 1 << 0);
}

void InsertionHandleController::PastePopupMenu::UpdateContent(
    /* [in] */ Boolean onTop)
{
    const Int32 viewIndex = ViewIndex(onTop);
    AutoPtr<IView> view = (*mPasteViews)[viewIndex];

    if (view == NULL) {
        const Int32 layout = (*mPasteViewLayouts)[viewIndex];
        AutoPtr<ILayoutInflater> inflater;
        AutoPtr<IInterface> obj;
        mOwner->mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
        inflater = ILayoutInflater::Probe(obj);
        if (inflater != NULL) {
            inflater->Inflate(layout, NULL, (IView**)&view);
        }

        if (view == NULL) {
//            throw new IllegalArgumentException("Unable to inflate TextEdit paste window");
            Logger::E("InsertionHandleController", "PastePopupMenu::UpdateContent");
            assert(0);
        }

        const Int32 size = View::View::MeasureSpec::MakeMeasureSpec(0, View::View::MeasureSpec::UNSPECIFIED);
        AutoPtr<IViewGroupLayoutParams> params;
        CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
                 IViewGroupLayoutParams::WRAP_CONTENT,
                 (IViewGroupLayoutParams**)&params);
        view->SetLayoutParams(params);
        view->Measure(size, size);

        view->SetOnClickListener(this);

        (*mPasteViews)[viewIndex] = view;
    }

    mContainer->SetContentView(view);
}

void InsertionHandleController::PastePopupMenu::Show()
{
    UpdateContent(TRUE);
    PositionAtCursor();
}

void InsertionHandleController::PastePopupMenu::Hide()
{
    mContainer->Dismiss();
}

Boolean InsertionHandleController::PastePopupMenu::IsShowing()
{
    Boolean bFlag = FALSE;
    mContainer->IsShowing(&bFlag);
    return bFlag;
}

//@Override
ECode InsertionHandleController::PastePopupMenu::OnClick(
    /* [in] */ IView* v)
{
    if (mOwner->CanPaste()) {
        mOwner->Paste();
    }
    Hide();

    return NOERROR;
}

void InsertionHandleController::PastePopupMenu::PositionAtCursor()
{
    AutoPtr<IView> contentView;
    mContainer->GetContentView((IView**)&contentView);
    Int32 width;
    contentView->GetMeasuredWidth(&width);
    Int32 height;
    contentView->GetMeasuredHeight(&height);

    Int32 lineHeight = mOwner->GetLineHeight();

    Int32 x = mOwner->mHandle->GetAdjustedPositionX();
    mPositionX = (Int32) (x - width / 2.0f);
    Int32 y = mOwner->mHandle->GetAdjustedPositionY();
    mPositionY = y - height - lineHeight;

    const AutoPtr< ArrayOf<Int32> > coords = ArrayOf<Int32>::Alloc(2);
    mOwner->mParent->GetLocationInWindow(coords);
    (*coords)[0] += mPositionX;
    (*coords)[1] += mPositionY;

    Int32 screenWidth;
    AutoPtr<IResources> res;
    mOwner->mContext->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    metrics->GetWidthPixels(&screenWidth);
    if ((*coords)[1] < 0) {
        UpdateContent(FALSE);
        // Update dimensions from new view
        mContainer->GetContentView((IView**)&contentView);
        contentView->GetMeasuredWidth(&width);
        contentView->GetMeasuredHeight(&height);

        // Vertical clipping, move under edited line and to the side of insertion cursor
        // TODO bottom clipping in case there is no system bar
        (*coords)[1] += height;
        (*coords)[1] += lineHeight;

        // Move to right hand side of insertion cursor by default. TODO RTL text.
        AutoPtr<IDrawable> handle = mOwner->mHandle->GetDrawable();
        Int32 intrinsicWidth;
        handle->GetIntrinsicWidth(&intrinsicWidth);
        const Int32 handleHalfWidth = intrinsicWidth / 2;

        Int32 positionX = mOwner->mHandle->GetAdjustedPositionX();
        if (positionX + width < screenWidth) {
            (*coords)[0] += handleHalfWidth + width / 2;
        }
        else {
            (*coords)[0] -= handleHalfWidth + width / 2;
        }
    }
    else {
        // Horizontal clipping
        (*coords)[0] = Elastos::Core::Math::Max(0, (*coords)[0]);
        (*coords)[0] = Elastos::Core::Math::Min(screenWidth - width, (*coords)[0]);
    }

    mContainer->ShowAtLocation(mOwner->mParent, IGravity::NO_GRAVITY, (*coords)[0], (*coords)[1]);
}

//==================================================================
//                 InsertionHandleController
//==================================================================

InsertionHandleController::InsertionHandleController(
    /* [in] */ IView* parent,
    /* [in] */ PositionObserver* positionObserver)
    : mParent(parent)
    , mIsShowing(FALSE)
    , mAllowAutomaticShowing(FALSE)
    , mPositionObserver(positionObserver)
{
    parent->GetContext((IContext**)&mContext);
}

/** Allows the handle to be shown automatically when cursor position changes */
void InsertionHandleController::AllowAutomaticShowing()
{
    mAllowAutomaticShowing = TRUE;
}

/** Disallows the handle from being shown automatically when cursor position changes */
void InsertionHandleController::HideAndDisallowAutomaticShowing()
{
    Hide();
    mAllowAutomaticShowing = FALSE;
}

/**
 * Shows the handle.
 */
void InsertionHandleController::ShowHandle()
{
    CreateHandleIfNeeded();
    ShowHandleIfNeeded();
}

void InsertionHandleController::ShowPastePopup()
{
    if (mIsShowing) {
        mHandle->ShowPastePopupWindow();
    }
}

void InsertionHandleController::ShowHandleWithPastePopup()
{
    ShowHandle();
    ShowPastePopup();
}

/**
 * @return whether the handle is being dragged.
 */
Boolean InsertionHandleController::IsDragging()
{
    //Boolean bFlag = FALSE;
    return mHandle != NULL && (mHandle->IsDragging());
}

/** Shows the handle at the given coordinates, as long as automatic showing is allowed */
void InsertionHandleController::OnCursorPositionChanged()
{
    if (mAllowAutomaticShowing) {
        ShowHandle();
    }
}

/**
 * Moves the handle so that it points at the given coordinates.
 * @param x Handle x in physical pixels.
 * @param y Handle y in physical pixels.
 */
void InsertionHandleController::SetHandlePosition(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mHandle->PositionAt((Int32) x, (Int32) y);
}

/**
 * If the handle is not visible, sets its visibility to View.VISIBLE and begins fading it in.
 */
void InsertionHandleController::BeginHandleFadeIn()
{
    mHandle->BeginFadeIn();
}

/**
 * Sets the handle to the given visibility.
 */
void InsertionHandleController::SetHandleVisibility(
    /* [in] */ Int32 visibility)
{
    mHandle->SetVisibility(visibility);
}

Int32 InsertionHandleController::GetHandleX()
{
    return mHandle->GetAdjustedPositionX();
}

Int32 InsertionHandleController::GetHandleY()
{
    return mHandle->GetAdjustedPositionY();
}

//@VisibleForTesting
AutoPtr<HandleView> InsertionHandleController::GetHandleViewForTest()
{
    return mHandle;
}

//@Override
ECode InsertionHandleController::OnTouchModeChanged(
    /* [in] */ Boolean isInTouchMode)
{
    if (!isInTouchMode) {
        Hide();
    }

    return NOERROR;
}

//@Override
ECode InsertionHandleController::Hide()
{
    if (mIsShowing) {
        if (mHandle != NULL) {
            mHandle->Hide();
        }
        mIsShowing = FALSE;
    }

    return NOERROR;
}

//@Override
ECode InsertionHandleController::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsShowing;
    return NOERROR;
}

//@Override
ECode InsertionHandleController::BeforeStartUpdatingPosition(
    /* [in] */ HandleView* handle)
{
    return NOERROR;
}

//@Override
ECode InsertionHandleController::UpdatePosition(
    /* [in] */ HandleView* handle,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    SetCursorPosition(x, y);
    return NOERROR;
}

//@Override
ECode InsertionHandleController::OnDetached()
{
    return NOERROR;
}

Boolean InsertionHandleController::CanPaste()
{
    AutoPtr<IClipboardManager> cm;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(
            IContext::CLIPBOARD_SERVICE,
            (IInterface**)&obj);
    cm = IClipboardManager::Probe(obj);
    Boolean bFlag = FALSE;
    cm->HasPrimaryClip(&bFlag);

    return bFlag;
}

void InsertionHandleController::CreateHandleIfNeeded()
{
    if (mHandle == NULL) {
        mHandle = new HandleView(this, HandleView::CENTER, mParent, mPositionObserver);
    }
}

void InsertionHandleController::ShowHandleIfNeeded()
{
    if (!mIsShowing) {
        mIsShowing = TRUE;
        mHandle->Show();
        SetHandleVisibility(IView::VISIBLE);
    }
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
