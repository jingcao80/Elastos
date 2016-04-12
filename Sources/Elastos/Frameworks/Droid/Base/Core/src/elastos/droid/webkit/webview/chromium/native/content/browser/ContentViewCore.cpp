
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ApiCompatibilityUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/ContentViewCore_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/DeviceUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/GestureEventType.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ViewPositionObserver.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/GamepadList.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/HandleView.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/SelectPopupDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/SelectPopupDropdown.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/SelectPopupItem.h"
#include "elastos/droid/webkit/webview/chromium/native/content/common/ContentSwitches.h"
#include "elastos/droid/webkit/webview/chromium/native/base/CommandLine.h"
#include "elastos/droid/webkit/webview/chromium/native/base/TraceEvent.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/gfx/DeviceDisplayInfo.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"
#include "elastos/droid/Manifest.h"
//#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::EIID_IResultReceiver;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
//using Elastos::Droid::R;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::CSelection;
using Elastos::Droid::Text::ISelection;
using Elastos::Droid::Text::CEditableFactory;
using Elastos::Droid::Text::IEditableFactory;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::EIID_ISpannable;
using Elastos::Droid::Manifest;
using Elastos::Droid::View::CView;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IAbsoluteLayout;
using Elastos::Droid::Widget::CAbsoluteLayoutLayoutParams;
using Elastos::Droid::Widget::IAbsoluteLayoutLayoutParams;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::IPairHelper;

using Elastos::Droid::Webkit::Webview::Chromium::Base::ApiCompatibilityUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Common::ContentSwitches;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::Input::GamepadList;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::Input::HandleView;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::Input::SelectPopupDialog;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::Input::SelectPopupDropdown;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::Input::SelectPopupItem;
using Elastos::Droid::Webkit::Webview::Chromium::Base::CommandLine;
using Elastos::Droid::Webkit::Webview::Chromium::Base::TraceEvent;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Gfx::DeviceDisplayInfo;
using Elastos::Droid::Webkit::Webview::Chromium::Content::R;

using Elastos::Droid::View::Accessibility::EIID_IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityManagerAccessibilityStateChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IInputEvent;
using Elastos::Droid::View::EIID_IOnTouchModeChangeListener;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::InputMethod::EIID_IInputConnection;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//===============================================================
//         ContentViewCore::InnerViewAndroidDelegate
//===============================================================
CAR_INTERFACE_IMPL(ContentViewCore::InnerViewAndroidDelegate, Object, IObject);

ContentViewCore::InnerViewAndroidDelegate::InnerViewAndroidDelegate(
    /* [in] */ ContentViewCore* owner)
    : mOwner(owner)
    , mContainerViewAtCreation(owner->mContainerView)
{
}

//@Override
AutoPtr<IView> ContentViewCore::InnerViewAndroidDelegate::AcquireAnchorView()
{
    AutoPtr<IView> anchorView;
    CView::New(mOwner->mContext, (IView**)&anchorView);
    mContainerViewAtCreation->AddView(anchorView);
    return anchorView;
}

//@Override
//@SuppressWarnings("deprecation")  // AbsoluteLayout
ECode ContentViewCore::InnerViewAndroidDelegate::SetAnchorViewPosition(
    /* [in] */ IView* view,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float width,
    /* [in] */ Float height)
{
    AutoPtr<IViewParent> viewParent;
    view->GetParent((IViewParent**)&viewParent);
    if (viewParent == NULL) {
        // Ignore. setAnchorViewPosition has been called after the anchor view has
        // already been released.
        return NOERROR;
    }
    //assert(viewParent == mContainerViewAtCreation);

    Float scale = (Float) DeviceDisplayInfo::Create(mOwner->mContext)->GetDIPScale();

    // The anchor view should not go outside the bounds of the ContainerView.
    Int32 leftMargin = Elastos::Core::Math::Round(x * scale);
    Int32 topMargin = Elastos::Core::Math::Round(mOwner->mRenderCoordinates->GetContentOffsetYPix() + y * scale);
    Int32 scaledWidth = Elastos::Core::Math::Round(width * scale);
    // ContentViewCore currently only supports these two container view types.
    //if (mContainerViewAtCreation->Probe(EIID_IFrameLayout) != NULL) {
    if (IFrameLayout::Probe(mContainerViewAtCreation) != NULL) {
        Int32 startMargin;
        if (ApiCompatibilityUtils::IsLayoutRtl(IView::Probe(mContainerViewAtCreation))) {
            Int32 measuredWidth;
            IView::Probe(mContainerViewAtCreation)->GetMeasuredWidth(&measuredWidth);
            startMargin = measuredWidth - Elastos::Core::Math::Round((width + x) * scale);
        }
        else {
            startMargin = leftMargin;
        }

        Int32 width;
        IView::Probe(mContainerViewAtCreation)->GetWidth(&width);
        if (scaledWidth + startMargin > width) {
            scaledWidth = width - startMargin;
        }

        AutoPtr<IFrameLayoutLayoutParams> lp;
        CFrameLayoutLayoutParams::New(
            scaledWidth, Elastos::Core::Math::Round(height * scale),
            (IFrameLayoutLayoutParams**)&lp);
        ApiCompatibilityUtils::SetMarginStart(IViewGroupMarginLayoutParams::Probe(lp), startMargin);
        IViewGroupMarginLayoutParams::Probe(lp)->SetTopMargin(topMargin);
        view->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    }
    //else if (mContainerViewAtCreation->Probe(EIID_IAbsoluteLayout) != NULL) {
    else if (IAbsoluteLayout::Probe(mContainerViewAtCreation) != NULL) {
        // This fixes the offset due to a difference in
        // scrolling model of WebView vs. Chrome.
        // TODO(sgurun) fix this to use mContainerViewAtCreation.getScroll[X/Y]()
        // as it naturally accounts for scroll differences between
        // these models.
        leftMargin += mOwner->mRenderCoordinates->GetScrollXPixInt();
        topMargin += mOwner->mRenderCoordinates->GetScrollYPixInt();

        AutoPtr<IAbsoluteLayoutLayoutParams> lp;
        CAbsoluteLayoutLayoutParams::New(
                    scaledWidth, (Int32) (height * scale), leftMargin, topMargin,
                    (IAbsoluteLayoutLayoutParams**)&lp);
        view->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    }
    else {
        //Log.e(TAG, "Unknown layout " + mContainerViewAtCreation.getClass().getName());
        Logger::E(TAG, "Unknow layout");
    }
    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerViewAndroidDelegate::ReleaseAnchorView(
    /* [in] */ IView* anchorView)
{
    mContainerViewAtCreation->RemoveView(anchorView);
    return NOERROR;
}

//===============================================================
//           ContentViewCore::InnerImeAdapterDelegate
//===============================================================

ContentViewCore::InnerImeAdapterDelegate::InnerImeAdapterDelegate(
    /* [in] */ ContentViewCore* owner)
    : mOwner(owner)
{
}

//@Override
void ContentViewCore::InnerImeAdapterDelegate::OnImeEvent(
    /* [in] */ Boolean isFinish)
{
    mOwner->GetContentViewClient()->OnImeEvent();
    if (!isFinish) {
        mOwner->HideHandles();
    }
}

//@Override
void ContentViewCore::InnerImeAdapterDelegate::OnDismissInput()
{
    mOwner->GetContentViewClient()->OnImeStateChangeRequested(FALSE);
}

//@Override
AutoPtr<IView> ContentViewCore::InnerImeAdapterDelegate::GetAttachedView()
{
    return IView::Probe(mOwner->mContainerView);
}

//@Override
AutoPtr<IResultReceiver> ContentViewCore::InnerImeAdapterDelegate::GetNewShowKeyboardReceiver()
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    AutoPtr<IResultReceiver> receiver = new InnerResultReceiver(this, handler);
    return receiver;
}

//==========================================================================
//       ContentViewCore::InnerImeAdapterDelegate::InnerResultReceiver
//==========================================================================

ContentViewCore::InnerImeAdapterDelegate::InnerResultReceiver::InnerResultReceiver(
    /* [in] */ InnerImeAdapterDelegate* owner,
    /* [in] */ IHandler* handler)
    : mOwner(owner)
{
    ResultReceiver::constructor(handler);
}

void ContentViewCore::InnerImeAdapterDelegate::InnerResultReceiver::OnReceiveResult(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* resultData)
{
    mOwner->mOwner->GetContentViewClient()->OnImeStateChangeRequested(
            resultCode == IInputMethodManager::RESULT_SHOWN ||
            resultCode == IInputMethodManager::RESULT_UNCHANGED_SHOWN);
    if (resultCode == IInputMethodManager::RESULT_SHOWN) {
        // If OSK is newly shown, delay the form focus until
        // the onSizeChanged (in order to adjust relative to the
        // new size).
        // TODO(jdduke): We should not assume that onSizeChanged will
        // always be called, crbug.com/294908.
        IView::Probe(mOwner->mOwner->GetContainerView())->GetWindowVisibleDisplayFrame(
                mOwner->mOwner->mFocusPreOSKViewportRect);
    }
    else if (mOwner->mOwner->HasFocus() && resultCode ==
            IInputMethodManager::RESULT_UNCHANGED_SHOWN) {
        // If the OSK was already there, focus the form immediately.
        mOwner->mOwner->ScrollFocusedEditableNodeIntoView();
    }
}

//===============================================================
//               ContentViewCore::InnerListener
//===============================================================

ContentViewCore::InnerListener::InnerListener(
    /* [in] */ ContentViewCore* owner)
    : mOwner(owner)
{
}

ECode ContentViewCore::InnerListener::OnPositionChanged(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (mOwner->IsSelectionHandleShowing() || mOwner->IsInsertionHandleShowing()) {
        mOwner->TemporarilyHideTextHandles();
    }

    return NOERROR;
}

//===============================================================
//          ContentViewCore::InnerZoomControlsDelegate
//===============================================================

ContentViewCore::InnerZoomControlsDelegate::InnerZoomControlsDelegate(
    /* [in] */ ContentViewCore* owner)
    : mOwner(owner)
{
}

//@Override
void ContentViewCore::InnerZoomControlsDelegate::InvokeZoomPicker()
{
}

//@Override
void ContentViewCore::InnerZoomControlsDelegate::DismissZoomPicker()
{
}

//@Override
void ContentViewCore::InnerZoomControlsDelegate::UpdateZoomControls()
{
}

//===============================================================
//       ContentViewCore::InnerWebContentsObserverAndroid
//===============================================================

ContentViewCore::InnerWebContentsObserverAndroid::InnerWebContentsObserverAndroid(
    /* [in] */ ContentViewCore* owner)
    : WebContentsObserverElastos(owner)
    , mOwner(owner)
{
}

//@Override
ECode ContentViewCore::InnerWebContentsObserverAndroid::DidNavigateMainFrame(
    /* [in] */ const String& url,
    /* [in] */ const String& baseUrl,
    /* [in] */ Boolean isNavigationToDifferentPage,
    /* [in] */ Boolean isFragmentNavigation)
{
    if (!isNavigationToDifferentPage) {
        return NOERROR;
    }

    mOwner->HidePopups();
    mOwner->ResetScrollInProgress();
    mOwner->ResetGestureDetection();

    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerWebContentsObserverAndroid::RenderProcessGone(
    /* [in] */ Boolean wasOomProtected)
{
    mOwner->HidePopups();
    mOwner->ResetScrollInProgress();
    // No need to reset gesture detection as the detector will have
    // been destroyed in the RenderWidgetHostView.

    return NOERROR;
}

//===============================================================
//       ContentViewCore::InnerOnVisibilityChangedListener
//===============================================================

ContentViewCore::InnerOnVisibilityChangedListener::InnerOnVisibilityChangedListener(
    /* [in] */ ContentViewCore* owner)
    : mOwner(owner)
    , mContainerViewAtCreation(owner->mContainerView)
{
}

//@Override
ECode ContentViewCore::InnerOnVisibilityChangedListener::OnPopupZoomerShown(
    /* [in] */ PopupZoomer* zoomer)
{
    AutoPtr<IRunnable> runnable = new OnPopupZoomerShownRunnable(this, zoomer);
    Boolean res;
    IView::Probe(mContainerViewAtCreation)->Post(runnable, &res);
    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerOnVisibilityChangedListener::OnPopupZoomerHidden(
    /* [in] */ PopupZoomer* zoomer)
{
    AutoPtr<IRunnable> runnable = new OnPopupZoomerHiddenRunnable(this, zoomer);
    Boolean res;
    IView::Probe(mContainerViewAtCreation)->Post(runnable, &res);
    return NOERROR;
}

//===========================================================================================
//       ContentViewCore::InnerOnVisibilityChangedListener::OnPopupZoomerShownRunnable
//===========================================================================================

ContentViewCore::InnerOnVisibilityChangedListener::OnPopupZoomerShownRunnable::OnPopupZoomerShownRunnable(
    /* [in] */ InnerOnVisibilityChangedListener* owner,
    /* [in] */ PopupZoomer* zoomer)
    : mOwner(owner)
    , mZoomer(zoomer)
{
}

ECode ContentViewCore::InnerOnVisibilityChangedListener::OnPopupZoomerShownRunnable::Run()
{
    Int32 index;
    if ((mOwner->mContainerViewAtCreation->IndexOfChild(mZoomer, &index), index) == -1) {
        mOwner->mContainerViewAtCreation->AddView(IView::Probe(mZoomer));
    }
    else {
        assert(0);
//        assert false : "PopupZoomer should never be shown without being hidden";
    }

    return NOERROR;
}

//===========================================================================================
//       ContentViewCore::InnerOnVisibilityChangedListener::OnPopupZoomerHiddenRunnable
//===========================================================================================

ContentViewCore::InnerOnVisibilityChangedListener::OnPopupZoomerHiddenRunnable::OnPopupZoomerHiddenRunnable(
    /* [in] */ InnerOnVisibilityChangedListener* owner,
    /* [in] */ PopupZoomer* zoomer)
    : mOwner(owner)
    , mZoomer(zoomer)
{
}

ECode ContentViewCore::InnerOnVisibilityChangedListener::OnPopupZoomerHiddenRunnable::Run()
{
    Int32 index;
    if ((mOwner->mContainerViewAtCreation->IndexOfChild(mZoomer, &index), index) != -1) {
        mOwner->mContainerViewAtCreation->RemoveView(IView::Probe(mZoomer));
        IView::Probe(mOwner->mContainerViewAtCreation)->Invalidate();
    }
    else {
        assert(0);
//        assert false : "PopupZoomer should never be hidden without being shown";
    }

    return NOERROR;
}

//===============================================================
//              ContentViewCore::InnerOnTapListener
//===============================================================

ContentViewCore::InnerOnTapListener::InnerOnTapListener(
    /* [in] */ ContentViewCore* owner,
    /* [in] */ IViewGroup* vg)
    : mOwner(owner)
    , mContainerViewAtCreation(vg)
{
}

//@Override
Boolean ContentViewCore::InnerOnTapListener::OnSingleTap(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* e)
{
    Boolean res;
    IView::Probe(mContainerViewAtCreation)->RequestFocus(&res);
    if (mOwner->mNativeContentViewCore != 0) {
        Int64 time;
        Float x, y;
        IInputEvent::Probe(e)->GetEventTime(&time);
        e->GetX(&x);
        e->GetY(&y);
        mOwner->NativeSingleTap(mOwner->mNativeContentViewCore, time, x, y);
    }
    return TRUE;
}

//@Override
Boolean ContentViewCore::InnerOnTapListener::OnLongPress(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* e)
{
    if (mOwner->mNativeContentViewCore != 0) {
        Int64 time;
        Float x, y;
        IInputEvent::Probe(e)->GetEventTime(&time);
        e->GetX(&x);
        e->GetY(&y);
        mOwner->NativeLongPress(mOwner->mNativeContentViewCore, time, x, y);
    }
    return TRUE;
}

//===============================================================
//            ContentViewCore::FakeMouseMoveRunnable
//===============================================================

ContentViewCore::FakeMouseMoveRunnable::FakeMouseMoveRunnable(
    /* [in] */ ContentViewCore* owner,
    /* [in] */ IMotionEvent* eventFakeMouseMove)
    : mOwner(owner)
    , mEventFakeMouseMove(eventFakeMouseMove)
{
}

ECode ContentViewCore::FakeMouseMoveRunnable::Run()
{
    mOwner->OnHoverEvent(mEventFakeMouseMove);
    IInputEvent::Probe(mEventFakeMouseMove)->Recycle();
    return NOERROR;
}

//===============================================================
//        ContentViewCore::InnerSelectionHandleController
//===============================================================

ContentViewCore::InnerSelectionHandleController::InnerSelectionHandleController(
    /* [in] */ ContentViewCore* owner,
    /* [in] */ IView* parent,
    /* [in] */ PositionObserver* positionObserver)
    : SelectionHandleController(parent, positionObserver)
    , mOwner(owner)
{
}

CAR_INTERFACE_IMPL(ContentViewCore::InnerSelectionHandleController, Object, IOnTouchModeChangeListener);

//@Override
void ContentViewCore::InnerSelectionHandleController::SelectBetweenCoordinates(
    /* [in] */ Int32 x1,
    /* [in] */ Int32 y1,
    /* [in] */ Int32 x2,
    /* [in] */ Int32 y2)
{
    if (mOwner->mNativeContentViewCore != 0 && !(x1 == x2 && y1 == y2)) {
        mOwner->NativeSelectBetweenCoordinates(mOwner->mNativeContentViewCore,
                x1, y1 - mOwner->mRenderCoordinates->GetContentOffsetYPix(),
                x2, y2 - mOwner->mRenderCoordinates->GetContentOffsetYPix());
    }
}

//@Override
void ContentViewCore::InnerSelectionHandleController::ShowHandles(
    /* [in] */ Int32 startDir,
    /* [in] */ Int32 endDir)
{
    Boolean wasShowing;
    IsShowing(&wasShowing);
    SelectionHandleController::ShowHandles(startDir, endDir);
    if (!wasShowing || mOwner->mActionMode == NULL) {
        mOwner->ShowSelectActionBar();
    }
}

//===============================================================
//       ContentViewCore::InnerInsertionHandleController
//===============================================================

const Int32 ContentViewCore::InnerInsertionHandleController::AVERAGE_LINE_HEIGHT;

ContentViewCore::InnerInsertionHandleController::InnerInsertionHandleController(
    /* [in] */ ContentViewCore* owner,
    /* [in] */ IView* parent,
    /* [in] */ PositionObserver* positionObserver)
    : InsertionHandleController(parent, positionObserver)
    , mOwner(owner)
{
}

CAR_INTERFACE_IMPL(ContentViewCore::InnerInsertionHandleController, Object, IOnTouchModeChangeListener);

//@Override
void ContentViewCore::InnerInsertionHandleController::SetCursorPosition(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (mOwner->mNativeContentViewCore != 0) {
        mOwner->NativeMoveCaret(mOwner->mNativeContentViewCore,
                x, y - mOwner->mRenderCoordinates->GetContentOffsetYPix());
    }
}

//@Override
void ContentViewCore::InnerInsertionHandleController::Paste()
{
    mOwner->mImeAdapter->Paste();
    mOwner->HideHandles();
}

//@Override
Int32 ContentViewCore::InnerInsertionHandleController::GetLineHeight()
{
    return (Int32) Elastos::Core::Math::Ceil(
        mOwner->mRenderCoordinates->FromLocalCssToPix(AVERAGE_LINE_HEIGHT));
}

//@Override
void ContentViewCore::InnerInsertionHandleController::ShowHandle()
{
    InsertionHandleController::ShowHandle();
}

//===============================================================
//              ContentViewCore::InnerActionHandler
//===============================================================

ContentViewCore::InnerActionHandler::InnerActionHandler(
    /* [in] */ ContentViewCore* owner)
    : mOwner(owner)
{
}

//@Override
ECode ContentViewCore::InnerActionHandler::SelectAll()
{
    mOwner->mImeAdapter->SelectAll();
    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerActionHandler::Cut()
{
    mOwner->mImeAdapter->Cut();
    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerActionHandler::Copy()
{
    mOwner->mImeAdapter->Copy();
    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerActionHandler::Paste()
{
    mOwner->mImeAdapter->Paste();
    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerActionHandler::Share()
{
    const String query = mOwner->GetSelectedText();
    if (TextUtils::IsEmpty(query)) {
        return NOERROR;
    }

    AutoPtr<IIntent> send;
    CIntent::New(IIntent::ACTION_SEND, (IIntent**)&send);
    send->SetType(String("text/plain"));
    send->PutExtra(IIntent::EXTRA_TEXT, query);
    // try {
        AutoPtr<IIntentHelper> helper;
        CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
        AutoPtr<IIntent> i;
        String str;
        mOwner->GetContext()->GetString(R::string::actionbar_share, &str);
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        helper->CreateChooser(send, cs, (IIntent**)&i);
        i->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        mOwner->GetContext()->StartActivity(i);
    // } catch (android.content.ActivityNotFoundException ex) {
    //     // If no app handles it, do nothing.
    // }

    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerActionHandler::Search()
{
    const String query = mOwner->GetSelectedText();
    if (TextUtils::IsEmpty(query)) {
        return NOERROR;
    }

    // See if ContentViewClient wants to override
    if (mOwner->GetContentViewClient()->DoesPerformWebSearch()) {
        mOwner->GetContentViewClient()->PerformWebSearch(query);
        return NOERROR;
    }

    AutoPtr<IIntent> i;
    CIntent::New(IIntent::ACTION_WEB_SEARCH, (IIntent**)&i);
    i->PutExtra(ISearchManager::EXTRA_NEW_SEARCH, TRUE);
    i->PutExtra(ISearchManager::QUERY, query);
    String name;
    mOwner->GetContext()->GetPackageName(&name);
    i->PutExtra(IBrowser::EXTRA_APPLICATION_ID, name);
    if (IActivity::Probe(mOwner->GetContext()) == NULL) {
        i->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    }

    // try {
        mOwner->GetContext()->StartActivity(i);
    // } catch (android.content.ActivityNotFoundException ex) {
    //     // If no app handles it, do nothing.
    // }

    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerActionHandler::IsSelectionPassword(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwner->mImeAdapter->IsSelectionPassword();
    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerActionHandler::IsSelectionEditable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOwner->mSelectionEditable;
    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerActionHandler::OnDestroyActionMode()
{
    mOwner->mActionMode = NULL;
    if (mOwner->mUnselectAllOnActionModeDismiss) {
        mOwner->mImeAdapter->Unselect();
    }

    mOwner->GetContentViewClient()->OnContextualActionBarHidden();

    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerActionHandler::IsShareAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_SEND, (IIntent**)&intent);
    intent->SetType(String("text/plain"));
    AutoPtr<IPackageManager> pm;
    mOwner->GetContext()->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> resolves;
    pm->QueryIntentActivities(intent,
            IPackageManager::MATCH_DEFAULT_ONLY,
            (IList**)&resolves);
    Int32 size;
    resolves->GetSize(&size);

    *result = size > 0;

    return NOERROR;
}

//@Override
ECode ContentViewCore::InnerActionHandler::IsWebSearchAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mOwner->GetContentViewClient()->DoesPerformWebSearch()) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_WEB_SEARCH, (IIntent**)&intent);
    intent->PutExtra(ISearchManager::EXTRA_NEW_SEARCH, TRUE);
    AutoPtr<IPackageManager> pm;
    mOwner->GetContext()->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> resolves;
    pm->QueryIntentActivities(intent,
            IPackageManager::MATCH_DEFAULT_ONLY,
            (IList**)&resolves);
    Int32 size;
    resolves->GetSize(&size);

    *result = size > 0;

    return NOERROR;
}

//===============================================================
//         ContentViewCore::DeferredHandleFadeInRunnable
//===============================================================

ContentViewCore::DeferredHandleFadeInRunnable::DeferredHandleFadeInRunnable(
    /* [in] */ ContentViewCore* owner)
    : mOwner(owner)
{
}

ECode ContentViewCore::DeferredHandleFadeInRunnable::Run()
{
    if (!mOwner->AllowTextHandleFadeIn()) {
        // Delay fade in until it is allowed.
        mOwner->ScheduleTextHandleFadeIn();
    }
    else {
        if (mOwner->IsSelectionHandleShowing()) {
            mOwner->mSelectionHandleController->BeginHandleFadeIn();
        }

        if (mOwner->IsInsertionHandleShowing()) {
            mOwner->mInsertionHandleController->BeginHandleFadeIn();
        }
    }

    return NOERROR;
}

//===============================================================
//             ContentViewCore::InnerContentObserver
//===============================================================

ContentViewCore::InnerContentObserver::InnerContentObserver(
    /* [in] */ ContentViewCore* owner)
    : mOwner(owner)
{
}

ECode ContentViewCore::InnerContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    Boolean isEnabled = FALSE;
    mOwner->mAccessibilityManager->IsEnabled(&isEnabled);
    mOwner->SetAccessibilityState(isEnabled);
    return NOERROR;
}

//===============================================================
//                    ContentViewCore
//===============================================================

const String ContentViewCore::TAG("ContentViewCore");

// Used to avoid enabling zooming in / out if resulting zooming will
// produce little visible difference.
const Float ContentViewCore::ZOOM_CONTROLS_EPSILON = 0.007f;

// Used to represent gestures for long press and long tap.
const Int32 ContentViewCore::IS_LONG_PRESS;
const Int32 ContentViewCore::IS_LONG_TAP;

// Length of the delay (in ms) before fading in handles after the last page movement.
const Int32 ContentViewCore::TEXT_HANDLE_FADE_IN_DELAY;

// These values are obtained from Samsung.
// TODO(changwan): refactor SPen related code into a separate class. See
// http://crbug.com/398169.
const Int32 ContentViewCore::SPEN_ACTION_DOWN;;
const Int32 ContentViewCore::SPEN_ACTION_UP;
const Int32 ContentViewCore::SPEN_ACTION_MOVE;
const Int32 ContentViewCore::SPEN_ACTION_CANCEL;
Boolean ContentViewCore::sIsSPenSupported = FALSE;

/**
 * PID used to indicate an invalid render process.
 */
// Keep in sync with the value returned from ContentViewCoreImpl::GetCurrentRendererProcessId()
// if there is no render process.
const Int32 ContentViewCore::INVALID_RENDER_PROCESS_PID;

CAR_INTERFACE_IMPL(ContentViewCore, ScreenOrientationListener::ScreenOrientationObserver, IAccessibilityManagerAccessibilityStateChangeListener);

/**
 * Constructs a new ContentViewCore. Embedders must call initialize() after constructing
 * a ContentViewCore and before using it.
 *
 * @param context The context used to create this.
 */
ContentViewCore::ContentViewCore(
    /* [in] */ IContext* context)
    : mContext(context)
    , mNativeContentViewCore(0)
    , mViewportWidthPix(0)
    , mViewportHeightPix(0)
    , mPhysicalBackingWidthPix(0)
    , mPhysicalBackingHeightPix(0)
    , mOverdrawBottomHeightPix(0)
    , mViewportSizeOffsetWidthPix(0)
    , mViewportSizeOffsetHeightPix(0)
    , mHasSelection(FALSE)
    , mSelectionEditable(FALSE)
    , mUnselectAllOnActionModeDismiss(FALSE)
    , mNativeAccessibilityAllowed(FALSE)
    , mNativeAccessibilityEnabled(FALSE)
    , mTouchExplorationEnabled(FALSE)
    , mShouldSetAccessibilityFocusOnPageLoad(FALSE)
    , mLastTapX(0)
    , mLastTapY(0)
    , mTouchScrollInProgress(FALSE)
    , mPotentiallyActiveFlingCount(0)
    , mCurrentTouchOffsetX(0.0f)
    , mCurrentTouchOffsetY(0.0f)
    , mSmartClipOffsetX(0)
    , mSmartClipOffsetY(0)
{
    mAdapterInputConnectionFactory = new ImeAdapter::AdapterInputConnectionFactory();
    mInputMethodManagerWrapper = new InputMethodManagerWrapper(mContext);

    mRenderCoordinates = new RenderCoordinates();
    AutoPtr<IResources> res;
    GetContext()->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float deviceScaleFactor;
    dm->GetDensity(&deviceScaleFactor);
    String forceScaleFactor = CommandLine::GetInstance()->GetSwitchValue(
            ContentSwitches::FORCE_DEVICE_SCALE_FACTOR);
    if (forceScaleFactor != NULL) {
        deviceScaleFactor = StringUtils::ParseFloat(forceScaleFactor);
    }
    mRenderCoordinates->SetDeviceScaleFactor(deviceScaleFactor);
    mStartHandlePoint = mRenderCoordinates->CreateNormalizedPoint();
    mEndHandlePoint = mRenderCoordinates->CreateNormalizedPoint();
    mInsertionHandlePoint = mRenderCoordinates->CreateNormalizedPoint();
    AutoPtr<IInterface> obj;
    GetContext()->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    mAccessibilityManager = IAccessibilityManager::Probe(obj);
    mGestureStateListenersIterator = mGestureStateListeners.GetRewindableIterator();

    AutoPtr<IEditableFactory> factory;
    CEditableFactory::AcquireSingleton((IEditableFactory**)&factory);
    AutoPtr<ICharSequence> cs;
    CString::New(String(""), (ICharSequence**)&cs);
    factory->NewEditable(cs, (IEditable**)&mEditable);
    AutoPtr<ISelection> select;
    CSelection::AcquireSingleton((ISelection**)&select);
    AutoPtr<ISpannable> spannable = ISpannable::Probe(mEditable);
    select->SetSelection2(spannable, 0);
}

/**
 * @return The context used for creating this ContentViewCore.
 */
//@CalledByNative
AutoPtr<IContext> ContentViewCore::GetContext()
{
    return mContext;
}

/**
 * @return The ViewGroup that all view actions of this ContentViewCore should interact with.
 */
AutoPtr<IViewGroup> ContentViewCore::GetContainerView()
{
    return mContainerView;
}

/**
 * @return The WebContents currently being rendered.
 */
AutoPtr<WebContents> ContentViewCore::GetWebContents()
{
    return mWebContents;
}

/**
 * Specifies how much smaller the WebKit layout size should be relative to the size of this
 * view.
 * @param offsetXPix The X amount in pixels to shrink the viewport by.
 * @param offsetYPix The Y amount in pixels to shrink the viewport by.
 */
void ContentViewCore::SetViewportSizeOffset(
    /* [in] */ Int32 offsetXPix,
    /* [in] */ Int32 offsetYPix)
{
    if (offsetXPix != mViewportSizeOffsetWidthPix ||
            offsetYPix != mViewportSizeOffsetHeightPix) {
        mViewportSizeOffsetWidthPix = offsetXPix;
        mViewportSizeOffsetHeightPix = offsetYPix;
        if (mNativeContentViewCore != 0) NativeWasResized(mNativeContentViewCore);
    }
}

/**
 * Returns a delegate that can be used to add and remove views from the ContainerView.
 *
 * NOTE: Use with care, as not all ContentViewCore users setup their ContainerView in the same
 * way. In particular, the Android WebView has limitations on what implementation details can
 * be provided via a child view, as they are visible in the API and could introduce
 * compatibility breaks with existing applications. If in doubt, contact the
 * android_webview/OWNERS
 *
 * @return A ViewAndroidDelegate that can be used to add and remove views.
 */
//@VisibleForTesting
AutoPtr<ViewElastosDelegate> ContentViewCore::GetViewAndroidDelegate()
{
    return new InnerViewAndroidDelegate(this);
}

//@VisibleForTesting
void ContentViewCore::SetImeAdapterForTest(
    /* [in] */ ImeAdapter* imeAdapter)
{
    mImeAdapter = imeAdapter;
}

//@VisibleForTesting
AutoPtr<ImeAdapter> ContentViewCore::GetImeAdapterForTest()
{
    return mImeAdapter;
}

//@VisibleForTesting
void ContentViewCore::SetAdapterInputConnectionFactory(
    /* [in] */ ImeAdapter::AdapterInputConnectionFactory* factory)
{
    mAdapterInputConnectionFactory = factory;
}

//@VisibleForTesting
void ContentViewCore::SetInputMethodManagerWrapperForTest(
    /* [in] */ InputMethodManagerWrapper* immw)
{
    mInputMethodManagerWrapper = immw;
}

//@VisibleForTesting
AutoPtr<AdapterInputConnection> ContentViewCore::GetInputConnectionForTest()
{
    return mInputConnection;
}

AutoPtr<ImeAdapter> ContentViewCore::CreateImeAdapter(
    /* [in] */ IContext* context)
{
    AutoPtr<ImeAdapter::ImeAdapterDelegate> imeAdapterDelegate = new InnerImeAdapterDelegate(this);
    AutoPtr<ImeAdapter> imeAdapter = new ImeAdapter(mInputMethodManagerWrapper, imeAdapterDelegate);
    return imeAdapter;
}

void ContentViewCore::Initialize(
    /* [in] */ IViewGroup* containerView,
    /* [in] */ InternalAccessDelegate* internalDispatcher,
    /* [in] */ Handle64 nativeWebContents,
    /* [in] */ WindowElastos* windowElastos)
{
    SetContainerView(containerView);

    mPositionListener = new InnerListener(this);

    Handle64 windowNativePointer = windowElastos != NULL ? windowElastos->GetNativePointer() : 0;

    Handle64 viewAndroidNativePointer = 0;
    if (windowNativePointer != 0) {
        mViewElastos = new ViewElastos(windowElastos, GetViewAndroidDelegate());
        viewAndroidNativePointer = mViewElastos->GetNativePointer();
    }

    mZoomControlsDelegate = new InnerZoomControlsDelegate(this);

    mNativeContentViewCore = NativeInit(
            nativeWebContents, viewAndroidNativePointer, windowNativePointer,
            mRetainedJavaScriptObjects);
    mWebContents = NativeGetWebContentsAndroid(mNativeContentViewCore);
    mContentSettings = new ContentSettings(this, mNativeContentViewCore);

    SetContainerViewInternals(internalDispatcher);
    mRenderCoordinates->Reset();
    InitPopupZoomer(mContext);
    mImeAdapter = CreateImeAdapter(mContext);

    mAccessibilityInjector = AccessibilityInjector::NewInstance(this);

    mWebContentsObserver = new InnerWebContentsObserverAndroid(this);
}

/**
 * Sets a new container view for this {@link ContentViewCore}.
 *
 * <p>WARNING: This is not a general purpose method and has been designed with WebView
 * fullscreen in mind. Please be aware that it might not be appropriate for other use cases
 * and that it has a number of limitations. For example the PopupZoomer only works with the
 * container view with which this ContentViewCore has been initialized.
 *
 * <p>This method only performs a small part of replacing the container view and
 * embedders are responsible for:
 * <ul>
 *     <li>Disconnecting the old container view from this ContentViewCore</li>
 *     <li>Updating the InternalAccessDelegate</li>
 *     <li>Reconciling the state of this ContentViewCore with the new container view</li>
 *     <li>Tearing down and recreating the native GL rendering where appropriate</li>
 *     <li>etc.</li>
 * </ul>
 */
void ContentViewCore::SetContainerView(
    /* [in] */ IViewGroup* containerView)
{
    TraceEvent::Begin();
    if (mContainerView != NULL) {
        mPositionObserver->RemoveListener(mPositionListener);
        mSelectionHandleController = NULL;
        mInsertionHandleController = NULL;
        mInputConnection = NULL;
    }

    mContainerView = containerView;
    AutoPtr<IView> view = IView::Probe(mContainerView);
    mPositionObserver = new ViewPositionObserver(view);
    String contentDescription("Web View");

    if (R::string::accessibility_content_view == 0) {
        Logger::W(TAG, "Setting contentDescription to 'Web View' as no value was specified.");
    }
    else {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetString(
                R::string::accessibility_content_view,
                &contentDescription);
    }

    AutoPtr<ICharSequence> desCS;
    CString::New(contentDescription, (ICharSequence**)&desCS);
    view->SetContentDescription(desCS);
    view->SetWillNotDraw(FALSE);
    view->SetClickable(TRUE);
    TraceEvent::End();
}

//@CalledByNative
void ContentViewCore::OnNativeContentViewCoreDestroyed(
    /* [in] */ Handle64 nativeContentViewCore)
{
    assert(nativeContentViewCore == mNativeContentViewCore);
    mNativeContentViewCore = 0;
}

/**
 * Set the Container view Internals.
 * @param internalDispatcher Handles dispatching all hidden or super methods to the
 *                           containerView.
 */
void ContentViewCore::SetContainerViewInternals(
    /* [in] */ InternalAccessDelegate* internalDispatcher)
{
    mContainerViewInternals = internalDispatcher;
}

void ContentViewCore::InitPopupZoomer(
    /* [in] */ IContext* context)
{
    mPopupZoomer = new PopupZoomer(context);
    AutoPtr<PopupZoomer::OnVisibilityChangedListener> changedListener = new InnerOnVisibilityChangedListener(this);
    mPopupZoomer->SetOnVisibilityChangedListener(changedListener);

    // TODO(yongsheng): LONG_TAP is not enabled in PopupZoomer. So need to dispatch a LONG_TAP
    // gesture if a user completes a tap on PopupZoomer UI after a LONG_PRESS gesture.
    AutoPtr<PopupZoomer::OnTapListener> listener = new InnerOnTapListener(this, mContainerView);
    mPopupZoomer->SetOnTapListener(listener);
}

/**
 * Destroy the internal state of the ContentView. This method may only be
 * called after the ContentView has been removed from the view system. No
 * other methods may be called on this ContentView after this method has
 * been called.
 */
void ContentViewCore::Destroy()
{
    if (mNativeContentViewCore != 0) {
        NativeOnJavaContentViewCoreDestroyed(mNativeContentViewCore);
    }

    mWebContents = NULL;
    if (mViewElastos != NULL) {
         mViewElastos->Destroy();
    }
    mNativeContentViewCore = 0;
    mContentSettings = NULL;
    mJavaScriptInterfaces->Clear();
    ISet::Probe(mRetainedJavaScriptObjects)->Clear();
    UnregisterAccessibilityContentObserver();
    mGestureStateListeners.Clear();
    ScreenOrientationListener::GetInstance()->RemoveObserver(this);
}

void ContentViewCore::UnregisterAccessibilityContentObserver()
{
    if (mAccessibilityScriptInjectionObserver == NULL) {
        return;
    }

    AutoPtr<IContentResolver> resolver;
    GetContext()->GetContentResolver((IContentResolver**)&resolver);
    resolver->UnregisterContentObserver(
            mAccessibilityScriptInjectionObserver);
    mAccessibilityScriptInjectionObserver = NULL;
}

/**
 * Returns true initially, false after destroy() has been called.
 * It is illegal to call any other public method after destroy().
 */
Boolean ContentViewCore::IsAlive()
{
    return mNativeContentViewCore != 0;
}

/**
 * This is only useful for passing over JNI to native code that requires ContentViewCore*.
 * @return native ContentViewCore pointer.
 */
//@CalledByNative
Handle64 ContentViewCore::GetNativeContentViewCore()
{
    return mNativeContentViewCore;
}

void ContentViewCore::SetContentViewClient(
    /* [in] */ ContentViewClient* client)
{
    if (client == NULL) {
        //throw new IllegalArgumentException("The client can't be null.");
        Logger::E(TAG, "The client can't be null.");
        assert(0);
    }

    mContentViewClient = client;
}

//@VisibleForTesting
AutoPtr<ContentViewClient> ContentViewCore::GetContentViewClient()
{
    if (mContentViewClient == NULL) {
        // We use the Null Object pattern to avoid having to perform a null check in this class.
        // We create it lazily because most of the time a client will be set almost immediately
        // after ContentView is created.
        mContentViewClient = new ContentViewClient();
        // We don't set the native ContentViewClient pointer here on purpose. The native
        // implementation doesn't mind a null delegate and using one is better than passing a
        // Null Object, since we cut down on the number of JNI calls.
    }

    return mContentViewClient;
}

Int32 ContentViewCore::GetBackgroundColor()
{
    if (mNativeContentViewCore != 0) {
        return NativeGetBackgroundColor(mNativeContentViewCore);
    }

    return IColor::WHITE;
}

//@CalledByNative
void ContentViewCore::OnBackgroundColorChanged(
    /* [in] */ Int32 color)
{
    GetContentViewClient()->OnBackgroundColorChanged(color);
}

/**
 * Load url without fixing up the url string. Consumers of ContentView are responsible for
 * ensuring the URL passed in is properly formatted (i.e. the scheme has been added if left
 * off during user input).
 *
 * @param params Parameters for this load.
 */
void ContentViewCore::LoadUrl(
    /* [in] */ LoadUrlParams* params)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeLoadUrl(mNativeContentViewCore,
            params->mUrl,
            params->mLoadUrlType,
            params->mTransitionType,
            params->GetReferrer() != NULL ? params->GetReferrer()->GetUrl() : String(NULL),
            params->GetReferrer() != NULL ? params->GetReferrer()->GetPolicy() : 0,
            params->mUaOverrideOption,
            params->GetExtraHeadersString(),
            params->mPostData,
            params->mBaseUrlForDataUrl,
            params->mVirtualUrlForDataUrl,
            params->mCanLoadLocalResources,
            params->mIsRendererInitiated);
}

/**
 * Stops loading the current web contents.
 */
void ContentViewCore::StopLoading()
{
    if (mWebContents != NULL) {
        mWebContents->Stop();
    }
}

/**
 * Get the URL of the current page.
 *
 * @return The URL of the current page.
 */
String ContentViewCore::GetUrl()
{
    if (mNativeContentViewCore != 0) {
        return NativeGetURL(mNativeContentViewCore);
    }

    return String(NULL);
}

/**
 * Get the title of the current page.
 *
 * @return The title of the current page.
 */
String ContentViewCore::GetTitle()
{
    return mWebContents == NULL ? String(NULL) : mWebContents->GetTitle();
}

/**
 * Shows an interstitial page driven by the passed in delegate.
 *
 * @param url The URL being blocked by the interstitial.
 * @param delegate The delegate handling the interstitial.
 */
//@VisibleForTesting
void ContentViewCore::ShowInterstitialPage(
    /* [in] */ const String& url,
    /* [in] */ InterstitialPageDelegateElastos* delegate)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeShowInterstitialPage(mNativeContentViewCore, url, delegate->GetNative());
}

/**
 * @return Whether the page is currently showing an interstitial, such as a bad HTTPS page.
 */
Boolean ContentViewCore::IsShowingInterstitialPage()
{
    return mNativeContentViewCore == 0 ?
            FALSE : NativeIsShowingInterstitialPage(mNativeContentViewCore);
}

/**
 * @return Viewport width in physical pixels as set from onSizeChanged.
 */
//@CalledByNative
Int32 ContentViewCore::GetViewportWidthPix()
{
    return mViewportWidthPix;
}

/**
 * @return Viewport height in physical pixels as set from onSizeChanged.
 */
//@CalledByNative
Int32 ContentViewCore::GetViewportHeightPix()
{
    return mViewportHeightPix;
}

/**
 * @return Width of underlying physical surface.
 */
//@CalledByNative
Int32 ContentViewCore::GetPhysicalBackingWidthPix()
{
    return mPhysicalBackingWidthPix;
}

/**
 * @return Height of underlying physical surface.
 */
//@CalledByNative
Int32 ContentViewCore::GetPhysicalBackingHeightPix()
{
    return mPhysicalBackingHeightPix;
}

/**
 * @return Amount the output surface extends past the bottom of the window viewport.
 */
//@CalledByNative
Int32 ContentViewCore::GetOverdrawBottomHeightPix()
{
    return mOverdrawBottomHeightPix;
}

/**
 * @return The amount to shrink the viewport relative to {@link #getViewportWidthPix()}.
 */
//@CalledByNative
Int32 ContentViewCore::GetViewportSizeOffsetWidthPix()
{
    return mViewportSizeOffsetWidthPix;
}

/**
 * @return The amount to shrink the viewport relative to {@link #getViewportHeightPix()}.
 */
//@CalledByNative
Int32 ContentViewCore::GetViewportSizeOffsetHeightPix()
{
    return mViewportSizeOffsetHeightPix;
}

/**
 * @see android.webkit.WebView#getContentHeight()
 */
Float ContentViewCore::GetContentHeightCss()
{
    return mRenderCoordinates->GetContentHeightCss();
}

/**
 * @see android.webkit.WebView#getContentWidth()
 */
Float ContentViewCore::GetContentWidthCss()
{
    return mRenderCoordinates->GetContentWidthCss();
}

// TODO(teddchoc): Remove all these navigation controller methods from here and have the
//                 embedders manage it.
/**
 * @return Whether the current WebContents has a previous navigation entry.
 */
Boolean ContentViewCore::CanGoBack()
{
    return mWebContents != NULL && mWebContents->GetNavigationController()->CanGoBack();
}

/**
 * @return Whether the current WebContents has a navigation entry after the current one.
 */
Boolean ContentViewCore::CanGoForward()
{
    return mWebContents != NULL && mWebContents->GetNavigationController()->CanGoForward();
}

/**
 * @param offset The offset into the navigation history.
 * @return Whether we can move in history by given offset
 */
Boolean ContentViewCore::CanGoToOffset(
    /* [in] */ Int32 offset)
{
    return mWebContents != NULL &&
            mWebContents->GetNavigationController()->CanGoToOffset(offset);
}

/**
 * Navigates to the specified offset from the "current entry". Does nothing if the offset is out
 * of bounds.
 * @param offset The offset into the navigation history.
 */
void ContentViewCore::GoToOffset(
    /* [in] */ Int32 offset)
{
    if (mWebContents != NULL) {
        mWebContents->GetNavigationController()->GoToOffset(offset);
    }
}

//@Override
ECode ContentViewCore::GoToNavigationIndex(
    /* [in] */ Int32 index)
{
    if (mWebContents != NULL) {
        mWebContents->GetNavigationController()->GoToNavigationIndex(index);
    }

    return NOERROR;
}

/**
 * Goes to the navigation entry before the current one.
 */
void ContentViewCore::GoBack()
{
    if (mWebContents != NULL) {
        mWebContents->GetNavigationController()->GoBack();
    }
}

/**
 * Goes to the navigation entry following the current one.
 */
void ContentViewCore::GoForward()
{
    if (mWebContents != NULL) {
        mWebContents->GetNavigationController()->GoForward();
    }
}

/**
 * Loads the current navigation if there is a pending lazy load (after tab restore).
 */
void ContentViewCore::LoadIfNecessary()
{
    if (mNativeContentViewCore != 0) {
        NativeLoadIfNecessary(mNativeContentViewCore);
    }
}

/**
 * Requests the current navigation to be loaded upon the next call to loadIfNecessary().
 */
void ContentViewCore::RequestRestoreLoad()
{
    if (mNativeContentViewCore != 0) {
        NativeRequestRestoreLoad(mNativeContentViewCore);
    }
}

/**
 * Reload the current page.
 */
void ContentViewCore::Reload(
    /* [in] */ Boolean checkForRepost)
{
    mAccessibilityInjector->AddOrRemoveAccessibilityApisIfNecessary();
    if (mNativeContentViewCore != 0) {
        NativeReload(mNativeContentViewCore, checkForRepost);
    }
}

/**
 * Reload the current page, ignoring the contents of the cache.
 */
void ContentViewCore::ReloadIgnoringCache(
    /* [in] */ Boolean checkForRepost)
{
    mAccessibilityInjector->AddOrRemoveAccessibilityApisIfNecessary();
    if (mNativeContentViewCore != 0) {
        NativeReloadIgnoringCache(mNativeContentViewCore, checkForRepost);
    }
}

/**
 * Cancel the pending reload.
 */
void ContentViewCore::CancelPendingReload()
{
    if (mNativeContentViewCore != 0) {
        NativeCancelPendingReload(mNativeContentViewCore);
    }
}

/**
 * Continue the pending reload.
 */
void ContentViewCore::ContinuePendingReload()
{
    if (mNativeContentViewCore != 0) {
        NativeContinuePendingReload(mNativeContentViewCore);
    }
}

/**
 * Clears the ContentViewCore's page history in both the backwards and
 * forwards directions.
 */
void ContentViewCore::ClearHistory()
{
    if (mNativeContentViewCore != 0) {
        NativeClearHistory(mNativeContentViewCore);
    }
}

/**
 * @return The selected text (empty if no text selected).
 */
String ContentViewCore::GetSelectedText()
{
    return mHasSelection ? mLastSelectedText : String("");
}

/**
 * @return Whether the current selection is editable (false if no text selected).
 */
Boolean ContentViewCore::IsSelectionEditable()
{
    return mHasSelection ? mSelectionEditable : FALSE;
}

// End FrameLayout overrides.

/**
 * TODO(changwan): refactor SPen related code into a separate class. See
 * http://crbug.com/398169.
 * @return Whether SPen is supported on the device.
 */
Boolean ContentViewCore::IsSPenSupported(
    /* [in] */ IContext* context)
{
    if (sIsSPenSupported == FALSE/*NULL*/) {
        sIsSPenSupported = DetectSPenSupport(context);
    }

    return sIsSPenSupported/*.booleanValue()*/;
}

Boolean ContentViewCore::DetectSPenSupport(
    /* [in] */ IContext* context)
{
    if (!String("SAMSUNG").EqualsIgnoreCase(Build::MANUFACTURER)) {
        return FALSE;
    }

    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr< ArrayOf<IFeatureInfo*> > infos;
    pm->GetSystemAvailableFeatures((ArrayOf<IFeatureInfo*>**)&infos);
    Int32 length = infos->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        String name;
        (*infos)[i]->GetName(&name);
        if (String("com.sec.feature.spen_usp").EqualsIgnoreCase(name)) {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Convert SPen event action into normal event action.
 * TODO(changwan): refactor SPen related code into a separate class. See
 * http://crbug.com/398169.
 *
 * @param eventActionMasked Input event action. It is assumed that it is masked as the values
                            cannot be ORed.
 * @return Event action after the conversion
 */
Int32 ContentViewCore::ConvertSPenEventAction(
    /* [in] */ Int32 eventActionMasked)
{
    // S-Pen support: convert to normal stylus event handling
    switch (eventActionMasked) {
        case SPEN_ACTION_DOWN:
            return IMotionEvent::ACTION_DOWN;
        case SPEN_ACTION_UP:
            return IMotionEvent::ACTION_UP;
        case SPEN_ACTION_MOVE:
            return IMotionEvent::ACTION_MOVE;
        case SPEN_ACTION_CANCEL:
            return IMotionEvent::ACTION_CANCEL;
        default:
            return eventActionMasked;
    }
}

/**
 * @see View#onTouchEvent(MotionEvent)
 */
Boolean ContentViewCore::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    TraceEvent::Begin(String("onTouchEvent"));
    // try {
        CancelRequestToScrollFocusedEditableNodeIntoView();

        Int32 eventAction;
        event->GetActionMasked(&eventAction);

        if (IsSPenSupported(mContext)) {
            eventAction = ConvertSPenEventAction(eventAction);
        }

        // Only these actions have any effect on gesture detection.  Other
        // actions have no corresponding WebTouchEvent type and may confuse the
        // touch pipline, so we ignore them entirely.
        if (eventAction != IMotionEvent::ACTION_DOWN
                && eventAction != IMotionEvent::ACTION_UP
                && eventAction != IMotionEvent::ACTION_CANCEL
                && eventAction != IMotionEvent::ACTION_MOVE
                && eventAction != IMotionEvent::ACTION_POINTER_DOWN
                && eventAction != IMotionEvent::ACTION_POINTER_UP) {
            return FALSE;
        }

        if (mNativeContentViewCore == 0) {
            return FALSE;
        }

        // A zero offset is quite common, in which case the unnecessary copy should be avoided.
        AutoPtr<IMotionEvent> offset;
        if (mCurrentTouchOffsetX != 0 || mCurrentTouchOffsetY != 0) {
            offset = CreateOffsetMotionEvent(event);
            event = offset;
        }

        Int32 pointerCount;
        event->GetPointerCount(&pointerCount);
        Int64 eventTime;
        IInputEvent::Probe(event)->GetEventTime(&eventTime);
        Int32 historySize;
        event->GetHistorySize(&historySize);
        Int32 actionIndex;
        event->GetActionIndex(&actionIndex);
        Float x, y;
        event->GetX(&x);
        event->GetY(&y);
        Float x1, y1;
        event->GetX(1, &x1);
        event->GetY(1, &y1);
        Int32 id0, id1;
        event->GetPointerId(0, &id0);
        event->GetPointerId(0, &id1);
        Float major0, major1;
        event->GetTouchMajor(&major0);
        event->GetTouchMajor(1, &major1);
        Float rawX, rawY;
        event->GetRawX(&rawX);
        event->GetRawY(&rawY);
        Int32 type0, type1;
        event->GetToolType(0, &type0);
        event->GetToolType(1, &type1);
        Int32 state;
        event->GetButtonState(&state);
        const Boolean consumed = NativeOnTouchEvent(mNativeContentViewCore, event,
                eventTime, eventAction,
                pointerCount, historySize, actionIndex,
                x, y,
                pointerCount > 1 ? x1 : 0,
                pointerCount > 1 ? y1 : 0,
                id0, pointerCount > 1 ? id1 : -1,
                major0, pointerCount > 1 ? major1 : 0,
                rawX,rawY,
                type0,
                pointerCount > 1 ? type1 : IMotionEvent::TOOL_TYPE_UNKNOWN,
                state);

        if (offset != NULL) {
            AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(offset);
            inputEvent->Recycle();
        }

    // } finally {
         TraceEvent::End(String("onTouchEvent"));
    // }
        return consumed;
}

void ContentViewCore::SetIgnoreRemainingTouchEvents()
{
    ResetGestureDetection();
}

Boolean ContentViewCore::IsScrollInProgress()
{
    return mTouchScrollInProgress || mPotentiallyActiveFlingCount > 0;
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnFlingStartEventConsumed(
    /* [in] */ Int32 vx,
    /* [in] */ Int32 vy)
{
    mTouchScrollInProgress = FALSE;
    mPotentiallyActiveFlingCount++;
    TemporarilyHideTextHandles();
    Boolean bNext;
    mGestureStateListenersIterator->HasNext(&bNext);
    for (mGestureStateListenersIterator->Rewind();
                bNext; mGestureStateListenersIterator->HasNext(&bNext)) {
        AutoPtr<IInterface> listenerObj;
        mGestureStateListenersIterator->GetNext((IInterface**)&listenerObj);
        GestureStateListener* listener = (GestureStateListener*)(IObject::Probe(listenerObj));
        listener->OnFlingStartGesture(
                vx, vy, ComputeVerticalScrollOffset(), ComputeVerticalScrollExtent());
    }
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnFlingStartEventHadNoConsumer(
    /* [in] */ Int32 vx,
    /* [in] */ Int32 vy)
{
    mTouchScrollInProgress = FALSE;
    Boolean bNext;
    mGestureStateListenersIterator->HasNext(&bNext);
    for (mGestureStateListenersIterator->Rewind();
                bNext; mGestureStateListenersIterator->HasNext(&bNext)) {
        AutoPtr<IInterface> listenerObj;
        mGestureStateListenersIterator->GetNext((IInterface**)&listenerObj);
        GestureStateListener* listener = (GestureStateListener*)(IObject::Probe(listenerObj));
        listener->OnUnhandledFlingStartEvent(vx, vy);
    }
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnFlingCancelEventAck()
{
    UpdateGestureStateListener(GestureEventType::FLING_CANCEL);
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnScrollBeginEventAck()
{
    mTouchScrollInProgress = TRUE;
    TemporarilyHideTextHandles();
    mZoomControlsDelegate->InvokeZoomPicker();
    UpdateGestureStateListener(GestureEventType::SCROLL_START);
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnScrollUpdateGestureConsumed()
{
    mZoomControlsDelegate->InvokeZoomPicker();
    Boolean bNext;
    mGestureStateListenersIterator->HasNext(&bNext);
    for (mGestureStateListenersIterator->Rewind();
            bNext; mGestureStateListenersIterator->HasNext(&bNext)) {
        AutoPtr<IInterface> listenerObj;
        mGestureStateListenersIterator->GetNext((IInterface**)&listenerObj);
        GestureStateListener* listener = (GestureStateListener*)(IObject::Probe(listenerObj));;
        listener->OnScrollUpdateGestureConsumed();
    }
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnScrollEndEventAck()
{
    if (!mTouchScrollInProgress) {
        return;
    }
    mTouchScrollInProgress = FALSE;
    UpdateGestureStateListener(GestureEventType::SCROLL_END);
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnPinchBeginEventAck()
{
    TemporarilyHideTextHandles();
    UpdateGestureStateListener(GestureEventType::PINCH_BEGIN);
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnPinchEndEventAck()
{
    UpdateGestureStateListener(GestureEventType::PINCH_END);
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnSingleTapEventAck(
    /* [in] */ Boolean consumed,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Boolean bNext;
    mGestureStateListenersIterator->HasNext(&bNext);
    for (mGestureStateListenersIterator->Rewind();
            bNext; mGestureStateListenersIterator->HasNext(&bNext)) {
        AutoPtr<IInterface> listenerObj;
        mGestureStateListenersIterator->GetNext((IInterface**)&listenerObj);
        GestureStateListener* listener = (GestureStateListener*)(IObject::Probe(listenerObj));;
        listener->OnSingleTap(consumed, x, y);
    }
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnDoubleTapEventAck()
{
    TemporarilyHideTextHandles();
}

/**
 * Called just prior to a tap or press gesture being forwarded to the renderer.
 */
//@SuppressWarnings("unused")
//@CalledByNative
Boolean ContentViewCore::FilterTapOrPressEvent(
    /* [in] */ Int32 type,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (type == GestureEventType::LONG_PRESS && OfferLongPressToEmbedder()) {
        return TRUE;
    }

    UpdateForTapOrPress(type, x, y);

    return FALSE;
}

//@VisibleForTesting
void ContentViewCore::SendDoubleTapForTest(
    /* [in] */ Int64 timeMs,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeDoubleTap(mNativeContentViewCore, timeMs, x, y);
}

//@VisibleForTesting
void ContentViewCore::FlingForTest(
    /* [in] */ Int64 timeMs,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 velocityX,
    /* [in] */ Int32 velocityY)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeFlingCancel(mNativeContentViewCore, timeMs);
    NativeScrollBegin(mNativeContentViewCore, timeMs, x, y, velocityX, velocityY);
    NativeFlingStart(mNativeContentViewCore, timeMs, x, y, velocityX, velocityY);
}

/**
 * Cancel any fling gestures active.
 * @param timeMs Current time (in milliseconds).
 */
void ContentViewCore::CancelFling(
    /* [in] */ Int64 timeMs)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeFlingCancel(mNativeContentViewCore, timeMs);
}

/**
 * Add a listener that gets alerted on gesture state changes.
 * @param listener Listener to add.
 */
void ContentViewCore::AddGestureStateListener(
    /* [in] */ GestureStateListener* listener)
{
    mGestureStateListeners.AddObserver((IObject*)listener);
}

/**
 * Removes a listener that was added to watch for gesture state changes.
 * @param listener Listener to remove.
 */
void ContentViewCore::RemoveGestureStateListener(
    /* [in] */ GestureStateListener* listener)
{
    mGestureStateListeners.RemoveObserver((IObject*)listener);
}

void ContentViewCore::UpdateGestureStateListener(
    /* [in] */ Int32 gestureType)
{
    Boolean bNext;
    mGestureStateListenersIterator->HasNext(&bNext);
    for (mGestureStateListenersIterator->Rewind();
            bNext; mGestureStateListenersIterator->HasNext(&bNext)) {
        AutoPtr<IInterface> listenerObj;
        mGestureStateListenersIterator->GetNext((IInterface**)&listenerObj);
        GestureStateListener* listener = (GestureStateListener*)(IObject::Probe(listenerObj));;
        switch (gestureType) {
            case GestureEventType::PINCH_BEGIN:
                listener->OnPinchStarted();
                break;
            case GestureEventType::PINCH_END:
                listener->OnPinchEnded();
                break;
            case GestureEventType::FLING_END:
                listener->OnFlingEndGesture(
                        ComputeVerticalScrollOffset(),
                        ComputeVerticalScrollExtent());
                break;
            case GestureEventType::FLING_CANCEL:
                listener->OnFlingCancelGesture();
                break;
            case GestureEventType::SCROLL_START:
                listener->OnScrollStarted(
                        ComputeVerticalScrollOffset(),
                        ComputeVerticalScrollExtent());
                break;
            case GestureEventType::SCROLL_END:
                listener->OnScrollEnded(
                        ComputeVerticalScrollOffset(),
                        ComputeVerticalScrollExtent());
                break;
            default:
                break;
        }
    }
}

/**
 * Requests the renderer insert a link to the specified stylesheet in the
 * main frame's document.
 */
void ContentViewCore::AddStyleSheetByURL(
    /* [in] */ const String& url)
{
    NativeAddStyleSheetByURL(mNativeContentViewCore, url);
}

/**
 * Injects the passed Javascript code in the current page and evaluates it.
 * If a result is required, pass in a callback.
 * Used in automation tests.
 *
 * @param script The Javascript to execute.
 * @param callback The callback to be fired off when a result is ready. The script's
 *                 result will be json encoded and passed as the parameter, and the call
 *                 will be made on the main thread.
 *                 If no result is required, pass null.
 */
void ContentViewCore::EvaluateJavaScript(
    /* [in] */ const String& script,
    /* [in] */ JavaScriptCallback* callback)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeEvaluateJavaScript(mNativeContentViewCore, script, callback, FALSE);
}

/**
 * Injects the passed Javascript code in the current page and evaluates it.
 * If there is no page existing, a new one will be created.
 *
 * @param script The Javascript to execute.
 */
void ContentViewCore::EvaluateJavaScriptEvenIfNotYetNavigated(
    /* [in] */ const String& script)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeEvaluateJavaScript(mNativeContentViewCore, script, NULL, TRUE);
}

/**
 * To be called when the ContentView is shown.
 */
void ContentViewCore::OnShow()
{
    assert(mNativeContentViewCore != 0);
    NativeOnShow(mNativeContentViewCore);
    Boolean bFlag = FALSE;
    mAccessibilityManager->IsEnabled(&bFlag);
    SetAccessibilityState(bFlag);
}

/**
 * @return The ID of the renderer process that backs this tab or
 *         {@link #INVALID_RENDER_PROCESS_PID} if there is none.
 */
Int32 ContentViewCore::GetCurrentRenderProcessId()
{
    return NativeGetCurrentRenderProcessId(mNativeContentViewCore);
}

/**
 * To be called when the ContentView is hidden.
 */
void ContentViewCore::OnHide()
{
    assert(mNativeContentViewCore != 0);
    HidePopups();
    SetInjectedAccessibility(FALSE);
    NativeOnHide(mNativeContentViewCore);
}

/**
 * Return the ContentSettings object used to retrieve the settings for this
 * ContentViewCore. For modifications, ChromeNativePreferences is to be used.
 * @return A ContentSettings object that can be used to retrieve this
 *         ContentViewCore's settings.
 */
AutoPtr<ContentSettings> ContentViewCore::GetContentSettings()
{
    return mContentSettings;
}

void ContentViewCore::HidePopups()
{
    HideSelectPopup();
    HideHandles();
    HideSelectActionBar();
}

void ContentViewCore::HideSelectActionBar()
{
    if (mActionMode != NULL) {
        mActionMode->Finish();
        mActionMode = NULL;
    }
}

Boolean ContentViewCore::IsSelectActionBarShowing()
{
    return mActionMode != NULL;
}

void ContentViewCore::ResetGestureDetection()
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeResetGestureDetection(mNativeContentViewCore);
}

/**
 * @see View#onAttachedToWindow()
 */
//@SuppressWarnings("javadoc")
void ContentViewCore::OnAttachedToWindow()
{
    Boolean result;
    mAccessibilityManager->IsEnabled(&result);
    SetAccessibilityState(result);

    ScreenOrientationListener::GetInstance()->AddObserver(this, mContext);
    GamepadList::OnAttachedToWindow(mContext);
}

/**
 * @see View#onDetachedFromWindow()
 */
//@SuppressWarnings("javadoc")
//@SuppressLint("MissingSuperCall")
void ContentViewCore::OnDetachedFromWindow()
{
    SetInjectedAccessibility(FALSE);
    HidePopups();
    mZoomControlsDelegate->DismissZoomPicker();
    UnregisterAccessibilityContentObserver();

    ScreenOrientationListener::GetInstance()->RemoveObserver(this);
    GamepadList::OnDetachedFromWindow();
}

/**
 * @see View#onVisibilityChanged(android.view.View, int)
 */
void ContentViewCore::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    if (visibility != IView::VISIBLE) {
        mZoomControlsDelegate->DismissZoomPicker();
    }
}

/**
 * @see View#onCreateInputConnection(EditorInfo)
 */
AutoPtr<IInputConnection> ContentViewCore::OnCreateInputConnection(
    /* [in] */ IEditorInfo* outAttrs)
{
    if (!mImeAdapter->HasTextInputType()) {
        // Although onCheckIsTextEditor will return false in this case, the EditorInfo
        // is still used by the InputMethodService. Need to make sure the IME doesn't
        // enter fullscreen mode.
        outAttrs->SetImeOptions(IEditorInfo::IME_FLAG_NO_FULLSCREEN);
    }

    AutoPtr<IView> view = IView::Probe(mContainerView);
    mInputConnection = mAdapterInputConnectionFactory->Get(view, mImeAdapter,
            mEditable, outAttrs);

    return IInputConnection::Probe(mInputConnection);
}

//@VisibleForTesting
AutoPtr<AdapterInputConnection> ContentViewCore::GetAdapterInputConnectionForTest()
{
    return mInputConnection;
}

//@VisibleForTesting
AutoPtr<IEditable> ContentViewCore::GetEditableForTest()
{
    return mEditable;
}

/**
 * @see View#onCheckIsTextEditor()
 */
Boolean ContentViewCore::OnCheckIsTextEditor()
{
    return mImeAdapter->HasTextInputType();
}

/**
 * @see View#onConfigurationChanged(Configuration)
 */
//@SuppressWarnings("javadoc")
void ContentViewCore::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    TraceEvent::Begin();

    Int32 keyboard;
    newConfig->GetKeyboard(&keyboard);
    if (keyboard != IConfiguration::KEYBOARD_NOKEYS) {
        if (mNativeContentViewCore != 0) {
            mImeAdapter->Attach(NativeGetNativeImeAdapter(mNativeContentViewCore),
                    ImeAdapter::GetTextInputTypeNone());
        }
        AutoPtr<IView> view = IView::Probe(mContainerView);
        mInputMethodManagerWrapper->RestartInput(view);
    }
    mContainerViewInternals->Super_onConfigurationChanged(newConfig);

    // To request layout has side effect, but it seems OK as it only happen in
    // onConfigurationChange and layout has to be changed in most case.
    AutoPtr<IView> view = IView::Probe(mContainerView);
    view->RequestLayout();
    TraceEvent::End();
}

/**
 * @see View#onSizeChanged(int, int, int, int)
 */
//@SuppressWarnings("javadoc")
void ContentViewCore::OnSizeChanged(
    /* [in] */ Int32 wPix,
    /* [in] */ Int32 hPix,
    /* [in] */ Int32 owPix,
    /* [in] */ Int32 ohPix)
{
    if (GetViewportWidthPix() == wPix && GetViewportHeightPix() == hPix) {
        return;
    }

    mViewportWidthPix = wPix;
    mViewportHeightPix = hPix;
    if (mNativeContentViewCore != 0) {
        NativeWasResized(mNativeContentViewCore);
    }

    UpdateAfterSizeChanged();
}

/**
 * Called when the underlying surface the compositor draws to changes size.
 * This may be larger than the viewport size.
 */
void ContentViewCore::OnPhysicalBackingSizeChanged(
    /* [in] */ Int32 wPix,
    /* [in] */ Int32 hPix)
{
    if (mPhysicalBackingWidthPix == wPix && mPhysicalBackingHeightPix == hPix) {
        return;
    }

    mPhysicalBackingWidthPix = wPix;
    mPhysicalBackingHeightPix = hPix;

    if (mNativeContentViewCore != 0) {
        NativeWasResized(mNativeContentViewCore);
    }
}

/**
 * Called when the amount the surface is overdrawing off the bottom has changed.
 * @param overdrawHeightPix The overdraw height.
 */
void ContentViewCore::OnOverdrawBottomHeightChanged(
    /* [in] */ Int32 overdrawHeightPix)
{
    if (mOverdrawBottomHeightPix == overdrawHeightPix) {
        return;
    }

    mOverdrawBottomHeightPix = overdrawHeightPix;

    if (mNativeContentViewCore != 0) {
        NativeWasResized(mNativeContentViewCore);
    }
}

void ContentViewCore::UpdateAfterSizeChanged()
{
    mPopupZoomer->Hide(FALSE);

    // Execute a delayed form focus operation because the OSK was brought
    // up earlier.
    Boolean isEmpty = FALSE;
    mFocusPreOSKViewportRect->IsEmpty(&isEmpty);
    if (!isEmpty) {
        AutoPtr<IRect> rect;
        CRect::New((IRect**)&rect);
        AutoPtr<IView> view = IView::Probe(GetContainerView());
        view->GetWindowVisibleDisplayFrame(rect);
        Boolean bFlag = FALSE;
        rect->Equals(mFocusPreOSKViewportRect, &bFlag);
        if (!bFlag) {
            // Only assume the OSK triggered the onSizeChanged if width was preserved.
            Int32 width;
            rect->GetWidth(&width);
            Int32 w;
            mFocusPreOSKViewportRect->GetWidth(&w);
            if (width == w) {
                ScrollFocusedEditableNodeIntoView();
            }
            CancelRequestToScrollFocusedEditableNodeIntoView();
        }
    }
}

void ContentViewCore::CancelRequestToScrollFocusedEditableNodeIntoView()
{
    // Zero-ing the rect will prevent |updateAfterSizeChanged()| from
    // issuing the delayed form focus event.
    mFocusPreOSKViewportRect->SetEmpty();
}

void ContentViewCore::ScrollFocusedEditableNodeIntoView()
{
    if (mNativeContentViewCore == 0) return;
    // The native side keeps track of whether the zoom and scroll actually occurred. It is
    // more efficient to do it this way and sometimes fire an unnecessary message rather
    // than synchronize with the renderer and always have an additional message.
    NativeScrollFocusedEditableNodeIntoView(mNativeContentViewCore);
}

/**
 * Selects the word around the caret, if any.
 * The caller can check if selection actually occurred by listening to OnSelectionChanged.
 */
void ContentViewCore::SelectWordAroundCaret()
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeSelectWordAroundCaret(mNativeContentViewCore);
}

/**
 * @see View#onWindowFocusChanged(boolean)
 */
void ContentViewCore::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    if (!hasWindowFocus) {
        ResetGestureDetection();
    }
}

void ContentViewCore::OnFocusChanged(
    /* [in] */ Boolean gainFocus)
{
    if (!gainFocus) {
        HideImeIfNeeded();
        CancelRequestToScrollFocusedEditableNodeIntoView();
    }

    if (mNativeContentViewCore != 0) {
        NativeSetFocus(mNativeContentViewCore, gainFocus);
    }
}

/**
 * @see View#onKeyUp(int, KeyEvent)
 */
Boolean ContentViewCore::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    if (mPopupZoomer->IsShowing() && keyCode == IKeyEvent::KEYCODE_BACK) {
        mPopupZoomer->Hide(TRUE);
        return TRUE;
    }

    return mContainerViewInternals->Super_onKeyUp(keyCode, event);
}

/**
 * @see View#dispatchKeyEventPreIme(KeyEvent)
 */
Boolean ContentViewCore::DispatchKeyEventPreIme(
    /* [in] */ IKeyEvent* event)
{
    // try {
        //TraceEvent::Begin();
        return mContainerViewInternals->Super_dispatchKeyEventPreIme(event);
    // } finally {
        //TraceEvent::End();
    // }
}

/**
 * @see View#dispatchKeyEvent(KeyEvent)
 */
Boolean ContentViewCore::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    if (GamepadList::DispatchKeyEvent(event)) {
        return TRUE;
    }

    if (GetContentViewClient()->ShouldOverrideKeyEvent(event)) {
        return mContainerViewInternals->Super_dispatchKeyEvent(event);
    }

    if (mImeAdapter->DispatchKeyEvent(event)) {
        return TRUE;
    }

    return mContainerViewInternals->Super_dispatchKeyEvent(event);
}

/**
 * @see View#onHoverEvent(MotionEvent)
 * Mouse move events are sent on hover enter, hover move and hover exit.
 * They are sent on hover exit because sometimes it acts as both a hover
 * move and hover exit.
 */
Boolean ContentViewCore::OnHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    TraceEvent::Begin(String("onHoverEvent"));
    AutoPtr<IMotionEvent> offset = CreateOffsetMotionEvent(event);
    AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(offset);
    //try {
        if (mBrowserAccessibilityManager != NULL) {
            return mBrowserAccessibilityManager->OnHoverEvent(offset);
        }

        // Work around Android bug where the x, y coordinates of a hover exit
        // event are incorrect when touch exploration is on.
        Int32 action;
        offset->GetAction(&action);
        if (mTouchExplorationEnabled && action == IMotionEvent::ACTION_HOVER_EXIT) {
            return TRUE;
        }

        AutoPtr<IView> view = IView::Probe(mContainerView);
        Boolean res;
        view->RemoveCallbacks(mFakeMouseMoveRunnable, &res);
        if (mNativeContentViewCore != 0) {
            Int64 time;
            inputEvent->GetEventTime(&time);
            Float x, y;
            offset->GetX(&x);
            offset->GetY(&y);
            NativeSendMouseMoveEvent(mNativeContentViewCore, time,
                    x, y);
        }

    // } finally {
        inputEvent->Recycle();
        TraceEvent::End(String("onHoverEvent"));
    // }

    return TRUE;
}

/**
 * @see View#onGenericMotionEvent(MotionEvent)
 */
Boolean ContentViewCore::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    if (GamepadList::OnGenericMotionEvent(event)) {
        return TRUE;
    }

    Int32 source;
    AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(event);
    inputEvent->GetSource(&source);
    if ((source & IInputDevice::SOURCE_CLASS_POINTER) != 0) {
        Int32 action;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_SCROLL:
                if (mNativeContentViewCore == 0) {
                    return FALSE;
                }

                Int64 time;
                inputEvent->GetEventTime(&time);
                Float x, y;
                event->GetX(&x);
                event->GetY(&y);
                Float value;
                event->GetAxisValue(IMotionEvent::AXIS_VSCROLL, &value);
                NativeSendMouseWheelEvent(mNativeContentViewCore, time,
                        x, y,
                        value);

                Boolean res;
                IView::Probe(mContainerView)->RemoveCallbacks(mFakeMouseMoveRunnable, &res);

                // Send a delayed onMouseMove event so that we end
                // up hovering over the right position after the scroll.
                AutoPtr<IMotionEventHelper> helper;
                CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
                AutoPtr<IMotionEvent> eventFakeMouseMove;
                helper->Obtain(event, (IMotionEvent**)&eventFakeMouseMove);
                mFakeMouseMoveRunnable = new FakeMouseMoveRunnable(this, eventFakeMouseMove);
                AutoPtr<IView> view = IView::Probe(mContainerView);
                Boolean result;
                view->PostDelayed(mFakeMouseMoveRunnable, 250, &result);
                return TRUE;
        }
    }

    return mContainerViewInternals->Super_onGenericMotionEvent(event);
}

/**
 * Sets the current amount to offset incoming touch events by.  This is used to handle content
 * moving and not lining up properly with the android input system.
 * @param dx The X offset in pixels to shift touch events.
 * @param dy The Y offset in pixels to shift touch events.
 */
void ContentViewCore::SetCurrentMotionEventOffsets(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    mCurrentTouchOffsetX = dx;
    mCurrentTouchOffsetY = dy;
}

AutoPtr<IMotionEvent> ContentViewCore::CreateOffsetMotionEvent(
    /* [in] */ IMotionEvent* src)
{
    AutoPtr<IMotionEvent> dst;
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    helper->Obtain(src, (IMotionEvent**)&dst);
    dst->OffsetLocation(mCurrentTouchOffsetX, mCurrentTouchOffsetY);
    return dst;
}

/**
 * @see View#scrollBy(int, int)
 * Currently the ContentView scrolling happens in the native side. In
 * the Java view system, it is always pinned at (0, 0). scrollBy() and scrollTo()
 * are overridden, so that View's mScrollX and mScrollY will be unchanged at
 * (0, 0). This is critical for drawing ContentView correctly.
 */
void ContentViewCore::ScrollBy(
    /* [in] */ Int32 xPix,
    /* [in] */ Int32 yPix)
{
    if (mNativeContentViewCore != 0) {
        NativeScrollBy(mNativeContentViewCore,
                SystemClock::GetUptimeMillis(), 0, 0, xPix, yPix);
    }
}

/**
 * @see View#scrollTo(int, int)
 */
void ContentViewCore::ScrollTo(
    /* [in] */ Int32 xPix,
    /* [in] */ Int32 yPix)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    const Float xCurrentPix = mRenderCoordinates->GetScrollXPix();
    const Float yCurrentPix = mRenderCoordinates->GetScrollYPix();
    const Float dxPix = xPix - xCurrentPix;
    const Float dyPix = yPix - yCurrentPix;
    if (dxPix != 0 || dyPix != 0) {
        Int64 time = SystemClock::GetUptimeMillis();
        NativeScrollBegin(mNativeContentViewCore, time,
                xCurrentPix, yCurrentPix, -dxPix, -dyPix);
        NativeScrollBy(mNativeContentViewCore,
                time, xCurrentPix, yCurrentPix, dxPix, dyPix);
        NativeScrollEnd(mNativeContentViewCore, time);
    }
}

// NOTE: this can go away once ContentView.getScrollX() reports correct values.
//       see: b/6029133
Int32 ContentViewCore::GetNativeScrollXForTest()
{
    return mRenderCoordinates->GetScrollXPixInt();
}

// NOTE: this can go away once ContentView.getScrollY() reports correct values.
//       see: b/6029133
Int32 ContentViewCore::GetNativeScrollYForTest()
{
    return mRenderCoordinates->GetScrollYPixInt();
}

/**
 * @see View#computeHorizontalScrollExtent()
 */
//@SuppressWarnings("javadoc")
Int32 ContentViewCore::ComputeHorizontalScrollExtent()
{
    return mRenderCoordinates->GetLastFrameViewportWidthPixInt();
}

/**
 * @see View#computeHorizontalScrollOffset()
 */
//@SuppressWarnings("javadoc")
Int32 ContentViewCore::ComputeHorizontalScrollOffset()
{
    return mRenderCoordinates->GetScrollXPixInt();
}

/**
 * @see View#computeHorizontalScrollRange()
 */
//@SuppressWarnings("javadoc")
Int32 ContentViewCore::ComputeHorizontalScrollRange()
{
    return mRenderCoordinates->GetContentWidthPixInt();
}

/**
 * @see View#computeVerticalScrollExtent()
 */
//@SuppressWarnings("javadoc")
Int32 ContentViewCore::ComputeVerticalScrollExtent()
{
    return mRenderCoordinates->GetLastFrameViewportHeightPixInt();
}

/**
 * @see View#computeVerticalScrollOffset()
 */
//@SuppressWarnings("javadoc")
Int32 ContentViewCore::ComputeVerticalScrollOffset()
{
    return mRenderCoordinates->GetScrollYPixInt();
}

/**
 * @see View#computeVerticalScrollRange()
 */
//@SuppressWarnings("javadoc")
Int32 ContentViewCore::ComputeVerticalScrollRange()
{
    return mRenderCoordinates->GetContentHeightPixInt();
}

// End FrameLayout overrides.

/**
 * @see View#awakenScrollBars(int, boolean)
 */
//@SuppressWarnings("javadoc")
Boolean ContentViewCore::AwakenScrollBars(
    /* [in] */ Int32 startDelay,
    /* [in] */ Boolean invalidate)
{
    // For the default implementation of ContentView which draws the scrollBars on the native
    // side, calling this function may get us into a bad state where we keep drawing the
    // scrollBars, so disable it by always returning false.
    Int32 style;
    AutoPtr<IView> view = IView::Probe(mContainerView);
    view->GetScrollBarStyle(&style);
    if (style == IView::SCROLLBARS_INSIDE_OVERLAY) {
        return FALSE;
    }
    else {
        return mContainerViewInternals->Super_awakenScrollBars(startDelay, invalidate);
    }
}

void ContentViewCore::UpdateForTapOrPress(
    /* [in] */ Int32 type,
    /* [in] */ Float xPix,
    /* [in] */ Float yPix)
{
    if (type != GestureEventType::SINGLE_TAP_CONFIRMED
            && type != GestureEventType::SINGLE_TAP_UP
            && type != GestureEventType::LONG_PRESS
            && type != GestureEventType::LONG_TAP) {
        return;
    }

    Boolean bFocusable, bTouchMode, bFocused;
    AutoPtr<IView> view = IView::Probe(mContainerView);
    view->IsFocusable(&bFocusable);
    view->IsFocusableInTouchMode(&bTouchMode);
    view->IsFocused(&bFocused);
    if (bFocusable && bTouchMode
            && !bFocused)  {
        Boolean result;
        view->RequestFocus(&result);
    }

    if (!mPopupZoomer->IsShowing()) {
        mPopupZoomer->SetLastTouch(xPix, yPix);
    }

    mLastTapX = (Int32) xPix;
    mLastTapY = (Int32) yPix;

    if (type == GestureEventType::LONG_PRESS
            || type == GestureEventType::LONG_TAP) {
        GetInsertionHandleController()->AllowAutomaticShowing();
        GetSelectionHandleController()->AllowAutomaticShowing();
    }
    else {
        if (mSelectionEditable) {
            GetInsertionHandleController()->AllowAutomaticShowing();
        }
    }
}

/**
 * @return The x coordinate for the last point that a tap or press gesture was initiated from.
 */
Int32 ContentViewCore::GetLastTapX()
{
    return mLastTapX;
}

/**
 * @return The y coordinate for the last point that a tap or press gesture was initiated from.
 */
Int32 ContentViewCore::GetLastTapY()
{
    return mLastTapY;
}

void ContentViewCore::SetZoomControlsDelegate(
    /* [in] */ ZoomControlsDelegate* zoomControlsDelegate)
{
    mZoomControlsDelegate = zoomControlsDelegate;
}

void ContentViewCore::UpdateMultiTouchZoomSupport(
    /* [in] */ Boolean supportsMultiTouchZoom)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeSetMultiTouchZoomSupportEnabled(mNativeContentViewCore, supportsMultiTouchZoom);
}

void ContentViewCore::UpdateDoubleTapSupport(
    /* [in] */ Boolean supportsDoubleTap)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeSetDoubleTapSupportEnabled(mNativeContentViewCore, supportsDoubleTap);
}

void ContentViewCore::SelectPopupMenuItems(
    /* [in] */ ArrayOf<Int32>* indices)
{
    if (mNativeContentViewCore != 0) {
        NativeSelectPopupMenuItems(mNativeContentViewCore, indices);
    }

    mSelectPopup = NULL;
}

/**
 * Send the screen orientation value to the renderer.
 */
//@VisibleForTesting
void ContentViewCore::SendOrientationChangeEvent(
    /* [in] */ Int32 orientation)
{
    if (mNativeContentViewCore == 0) {
        return;
    }

    NativeSendOrientationChangeEvent(mNativeContentViewCore, orientation);
}

/**
 * Register the delegate to be used when content can not be handled by
 * the rendering engine, and should be downloaded instead. This will replace
 * the current delegate, if any.
 * @param delegate An implementation of ContentViewDownloadDelegate.
 */
void ContentViewCore::SetDownloadDelegate(
    /* [in] */ ContentViewDownloadDelegate* delegate)
{
    mDownloadDelegate = delegate;
}

// Called by DownloadController.
AutoPtr<ContentViewDownloadDelegate> ContentViewCore::GetDownloadDelegate()
{
    return mDownloadDelegate;
}

AutoPtr<SelectionHandleController> ContentViewCore::GetSelectionHandleController()
{
    if (mSelectionHandleController == NULL) {
        AutoPtr<IView> view = IView::Probe(GetContainerView());
        mSelectionHandleController = new InnerSelectionHandleController(this,
                view, mPositionObserver);
    }

    return mSelectionHandleController;
}

AutoPtr<InsertionHandleController> ContentViewCore::GetInsertionHandleController()
{
    if (mInsertionHandleController == NULL) {
        AutoPtr<IView> view = IView::Probe(GetContainerView());
        mInsertionHandleController = new InnerInsertionHandleController(this,
                view, mPositionObserver);

        mInsertionHandleController->HideAndDisallowAutomaticShowing();
    }

    return mInsertionHandleController;
}

//@VisibleForTesting
AutoPtr<InsertionHandleController> ContentViewCore::GetInsertionHandleControllerForTest()
{
    return mInsertionHandleController;
}

//@VisibleForTesting
AutoPtr<SelectionHandleController> ContentViewCore::GetSelectionHandleControllerForTest()
{
    return mSelectionHandleController;
}

void ContentViewCore::UpdateHandleScreenPositions()
{
    if (IsSelectionHandleShowing()) {
        mSelectionHandleController->SetStartHandlePosition(
                mStartHandlePoint->GetXPix(), mStartHandlePoint->GetYPix());
        mSelectionHandleController->SetEndHandlePosition(
                mEndHandlePoint->GetXPix(), mEndHandlePoint->GetYPix());
    }

    if (IsInsertionHandleShowing()) {
        mInsertionHandleController->SetHandlePosition(
                mInsertionHandlePoint->GetXPix(), mInsertionHandlePoint->GetYPix());
    }
}

void ContentViewCore::HideHandles()
{
    if (mSelectionHandleController != NULL) {
        mSelectionHandleController->HideAndDisallowAutomaticShowing();
    }

    if (mInsertionHandleController != NULL) {
        mInsertionHandleController->HideAndDisallowAutomaticShowing();
    }

    mPositionObserver->RemoveListener(mPositionListener);
}

void ContentViewCore::ShowSelectActionBar()
{
    if (mActionMode != NULL) {
        mActionMode->Invalidate();
        return;
    }

    // Start a new action mode with a SelectActionModeCallback.
    AutoPtr<SelectActionModeCallback::ActionHandler> actionHandler =
            new InnerActionHandler(this);

    mActionMode = NULL;
    // On ICS, startActionMode throws an NPE when getParent() is null.
    AutoPtr<IView> view = IView::Probe(mContainerView);
    AutoPtr<IViewParent> parent;
    view->GetParent((IViewParent**)&parent);
    if (parent != NULL) {
        view->StartActionMode(
                GetContentViewClient()->GetSelectActionModeCallback(GetContext(), actionHandler,
                        NativeIsIncognito(mNativeContentViewCore)),
                (IActionMode**)&mActionMode);
    }

    mUnselectAllOnActionModeDismiss = TRUE;
    if (mActionMode == NULL) {
        // There is no ActionMode, so remove the selection.
        mImeAdapter->Unselect();
    }
    else {
        GetContentViewClient()->OnContextualActionBarShown();
    }
}

Boolean ContentViewCore::GetUseDesktopUserAgent()
{
    if (mNativeContentViewCore != 0) {
        return NativeGetUseDesktopUserAgent(mNativeContentViewCore);
    }

    return FALSE;
}

/**
 * Set whether or not we're using a desktop user agent for the currently loaded page.
 * @param override If true, use a desktop user agent.  Use a mobile one otherwise.
 * @param reloadOnChange Reload the page if the UA has changed.
 */
void ContentViewCore::SetUseDesktopUserAgent(
    /* [in] */ Boolean _override,
    /* [in] */ Boolean reloadOnChange)
{
    if (mNativeContentViewCore != 0) {
        NativeSetUseDesktopUserAgent(mNativeContentViewCore, _override, reloadOnChange);
    }
}

void ContentViewCore::ClearSslPreferences()
{
    if (mNativeContentViewCore != 0) {
        NativeClearSslPreferences(mNativeContentViewCore);
    }
}

Boolean ContentViewCore::IsSelectionHandleShowing()
{
    Boolean isShowing;
    return mSelectionHandleController != NULL && (mSelectionHandleController->IsShowing(&isShowing), isShowing);
}

Boolean ContentViewCore::IsInsertionHandleShowing()
{
    Boolean isShowing;
    return mInsertionHandleController != NULL && (mInsertionHandleController->IsShowing(&isShowing), isShowing);
}

// Makes the insertion/selection handles invisible. They will fade back in shortly after the
// last call to scheduleTextHandleFadeIn (or temporarilyHideTextHandles).
void ContentViewCore::TemporarilyHideTextHandles()
{
    if (IsSelectionHandleShowing() && !mSelectionHandleController->IsDragging()) {
        mSelectionHandleController->SetHandleVisibility(IView::INVISIBLE);
    }

    if (IsInsertionHandleShowing() && !mInsertionHandleController->IsDragging()) {
        mInsertionHandleController->SetHandleVisibility(IView::INVISIBLE);
    }

    ScheduleTextHandleFadeIn();
}

Boolean ContentViewCore::AllowTextHandleFadeIn()
{
    if (mTouchScrollInProgress) {
        return FALSE;
    }

    if (mPopupZoomer->IsShowing()) {
        return FALSE;
    }

    return TRUE;
}

// Cancels any pending fade in and schedules a new one.
void ContentViewCore::ScheduleTextHandleFadeIn()
{
    if (!IsInsertionHandleShowing() && !IsSelectionHandleShowing()) {
        return;
    }

    if (mDeferredHandleFadeInRunnable == NULL) {
        mDeferredHandleFadeInRunnable = new DeferredHandleFadeInRunnable(this);
    }

    AutoPtr<IView> view = IView::Probe(mContainerView);
    Boolean res;
    view->RemoveCallbacks(mDeferredHandleFadeInRunnable, &res);
    Boolean result;
    view->PostDelayed(mDeferredHandleFadeInRunnable, TEXT_HANDLE_FADE_IN_DELAY, &result);
}

/**
 * Shows the IME if the focused widget could accept text input.
 */
void ContentViewCore::ShowImeIfNeeded()
{
    if (mNativeContentViewCore != 0) {
        NativeShowImeIfNeeded(mNativeContentViewCore);
    }
}

/**
 * Hides the IME if the containerView is the active view for IME.
 */
void ContentViewCore::HideImeIfNeeded()
{
    // Hide input method window from the current view synchronously
    // because ImeAdapter does so asynchronouly with a delay, and
    // by the time when ImeAdapter dismisses the input, the
    // containerView may have lost focus.
    // We cannot trust ContentViewClient#onImeStateChangeRequested to
    // hide the input window because it has an empty default implementation.
    // So we need to explicitly hide the input method window here.
    AutoPtr<IView> view = IView::Probe(mContainerView);
    if (mInputMethodManagerWrapper->IsActive(view)) {
        AutoPtr<IBinder> binder;
        view->GetWindowToken((IBinder**)&binder);
        mInputMethodManagerWrapper->HideSoftInputFromWindow(
                binder, 0, NULL);
    }

    GetContentViewClient()->OnImeStateChangeRequested(FALSE);
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::UpdateFrameInfo(
    /* [in] */ Float scrollOffsetX,
    /* [in] */ Float scrollOffsetY,
    /* [in] */ Float pageScaleFactor,
    /* [in] */ Float minPageScaleFactor,
    /* [in] */ Float maxPageScaleFactor,
    /* [in] */ Float contentWidth,
    /* [in] */ Float contentHeight,
    /* [in] */ Float viewportWidth,
    /* [in] */ Float viewportHeight,
    /* [in] */ Float controlsOffsetYCss,
    /* [in] */ Float contentOffsetYCss,
    /* [in] */ Float overdrawBottomHeightCss)
{
    TraceEvent::Instant(String("ContentViewCore:updateFrameInfo"));

    // Adjust contentWidth/Height to be always at least as big as
    // the actual viewport (as set by onSizeChanged).
    const Float deviceScale = mRenderCoordinates->GetDeviceScaleFactor();
    contentWidth = Elastos::Core::Math::Max(contentWidth,
            mViewportWidthPix / (deviceScale * pageScaleFactor));
    contentHeight = Elastos::Core::Math::Max(contentHeight,
            mViewportHeightPix / (deviceScale * pageScaleFactor));
    const Float contentOffsetYPix = mRenderCoordinates->FromDipToPix(contentOffsetYCss);

    const Boolean contentSizeChanged =
            contentWidth != mRenderCoordinates->GetContentWidthCss()
            || contentHeight != mRenderCoordinates->GetContentHeightCss();
    const Boolean scaleLimitsChanged =
            minPageScaleFactor != mRenderCoordinates->GetMinPageScaleFactor()
            || maxPageScaleFactor != mRenderCoordinates->GetMaxPageScaleFactor();
    const Boolean pageScaleChanged =
            pageScaleFactor != mRenderCoordinates->GetPageScaleFactor();
    const Boolean scrollChanged =
            pageScaleChanged
            || scrollOffsetX != mRenderCoordinates->GetScrollX()
            || scrollOffsetY != mRenderCoordinates->GetScrollY();
    const Boolean contentOffsetChanged =
            contentOffsetYPix != mRenderCoordinates->GetContentOffsetYPix();

    const Boolean needHidePopupZoomer = contentSizeChanged || scrollChanged;
    const Boolean needUpdateZoomControls = scaleLimitsChanged || scrollChanged;
    const Boolean needTemporarilyHideHandles = scrollChanged;

    if (needHidePopupZoomer) {
        mPopupZoomer->Hide(TRUE);
    }

    if (scrollChanged) {
        mContainerViewInternals->OnScrollChanged(
                (Int32) mRenderCoordinates->FromLocalCssToPix(scrollOffsetX),
                (Int32) mRenderCoordinates->FromLocalCssToPix(scrollOffsetY),
                (Int32) mRenderCoordinates->GetScrollXPix(),
                (Int32) mRenderCoordinates->GetScrollYPix());
    }

    mRenderCoordinates->UpdateFrameInfo(
            scrollOffsetX, scrollOffsetY,
            contentWidth, contentHeight,
            viewportWidth, viewportHeight,
            pageScaleFactor, minPageScaleFactor, maxPageScaleFactor,
            contentOffsetYPix);

    if (scrollChanged || contentOffsetChanged) {
        Boolean bNext;
        mGestureStateListenersIterator->HasNext(&bNext);
        for (mGestureStateListenersIterator->Rewind();
                bNext; mGestureStateListenersIterator->HasNext(&bNext)) {
            AutoPtr<IInterface> listenerObj;
            mGestureStateListenersIterator->GetNext((IInterface**)&listenerObj);
            GestureStateListener* listener = (GestureStateListener*)(IObject::Probe(listenerObj));
            listener->OnScrollOffsetOrExtentChanged(
                    ComputeVerticalScrollOffset(),
                    ComputeVerticalScrollExtent());
        }
    }

    if (needTemporarilyHideHandles) {
        TemporarilyHideTextHandles();
    }

    if (needUpdateZoomControls) {
        mZoomControlsDelegate->UpdateZoomControls();
    }

    if (contentOffsetChanged) {
        UpdateHandleScreenPositions();
    }

    // Update offsets for fullscreen.
    const Float controlsOffsetPix = controlsOffsetYCss * deviceScale;
    const Float overdrawBottomHeightPix = overdrawBottomHeightCss * deviceScale;
    GetContentViewClient()->OnOffsetsForFullscreenChanged(
            controlsOffsetPix, contentOffsetYPix, overdrawBottomHeightPix);

    if (mBrowserAccessibilityManager != NULL) {
        mBrowserAccessibilityManager->NotifyFrameInfoInitialized();
    }
}

//@CalledByNative
void ContentViewCore::UpdateImeAdapter(
    /* [in] */ Handle64 nativeImeAdapterAndroid,
    /* [in] */ Int32 textInputType,
    /* [in] */ const String& text,
    /* [in] */ Int32 selectionStart,
    /* [in] */ Int32 selectionEnd,
    /* [in] */ Int32 compositionStart,
    /* [in] */ Int32 compositionEnd,
    /* [in] */ Boolean showImeIfNeeded,
    /* [in] */ Boolean isNonImeChange)
{
    TraceEvent::Begin();
    mSelectionEditable = (textInputType != ImeAdapter::GetTextInputTypeNone());

    mImeAdapter->UpdateKeyboardVisibility(
            nativeImeAdapterAndroid, textInputType, showImeIfNeeded);

    if (mInputConnection != NULL) {
        mInputConnection->UpdateState(text, selectionStart, selectionEnd, compositionStart,
                compositionEnd, isNonImeChange);
    }

    if (mActionMode != NULL) {
        mActionMode->Invalidate();
    }

    TraceEvent::End();
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::SetTitle(
    /* [in] */ const String& title)
{
    GetContentViewClient()->OnUpdateTitle(title);
}

/**
 * Called (from native) when the <select> popup needs to be shown.
 * @param items           Items to show.
 * @param enabled         POPUP_ITEM_TYPEs for items.
 * @param multiple        Whether the popup menu should support multi-select.
 * @param selectedIndices Indices of selected items.
 */
//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::ShowSelectPopup(
    /* [in] */ IRect* bounds,
    /* [in] */ ArrayOf<String>* items,
    /* [in] */ ArrayOf<Int32>* enabled,
    /* [in] */ Boolean multiple,
    /* [in] */ ArrayOf<Int32>* selectedIndices)
{
    AutoPtr<IView> view = IView::Probe(mContainerView);
    AutoPtr<IViewParent> parent;
    view->GetParent((IViewParent**)&parent);
    Int32 visibility;
    if (parent == NULL || (view->GetVisibility(&visibility), visibility != IView::VISIBLE)) {
        SelectPopupMenuItems(NULL);
        return;
    }

    assert(items->GetLength() == enabled->GetLength());
    //List<SelectPopupItem> popupItems = new ArrayList<SelectPopupItem>();
    AutoPtr<IList> popupItems;
    CArrayList::New((IList**)&popupItems);
    for (Int32 i = 0; i < items->GetLength(); i++) {
        AutoPtr<SelectPopupItem> item = new SelectPopupItem((*items)[i], (*enabled)[i]);
        //popupItems->Add(new SelectPopupItem((*items)[i], (*enabled)[i]));
        Boolean modified;
        AutoPtr<IObject> iItem = IObject::Probe(TO_IINTERFACE(item));
        popupItems->Add(iItem, &modified);
    }

    HidePopups();
    if (DeviceUtils::IsTablet(mContext) && !multiple) {
        mSelectPopup = new SelectPopupDropdown(this, popupItems, bounds, selectedIndices);
    }
    else {
        mSelectPopup = new SelectPopupDialog(this, popupItems, multiple, selectedIndices);
    }

    mSelectPopup->Show();
}

/**
 * Called when the <select> popup needs to be hidden.
 */
//@CalledByNative
void ContentViewCore::HideSelectPopup()
{
    if (mSelectPopup != NULL) {
        mSelectPopup->Hide();
    }
}

/**
 * @return The visible select popup being shown.
 */
AutoPtr<SelectPopup> ContentViewCore::GetSelectPopupForTest()
{
    return mSelectPopup;
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::ShowDisambiguationPopup(
    /* [in] */ IRect* targetRect,
    /* [in] */ IBitmap* zoomedBitmap)
{
    mPopupZoomer->SetBitmap(zoomedBitmap);
    mPopupZoomer->Show(targetRect);
    TemporarilyHideTextHandles();
}

//@SuppressWarnings("unused")
//@CalledByNative
AutoPtr<TouchEventSynthesizer> ContentViewCore::CreateTouchEventSynthesizer()
{
    return new TouchEventSynthesizer(this);
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnSelectionChanged(
    /* [in] */ const String& text)
{
    mLastSelectedText = text;
    GetContentViewClient()->OnSelectionChanged(text);
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::ShowSelectionHandlesAutomatically()
{
    GetSelectionHandleController()->AllowAutomaticShowing();
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnSelectionBoundsChanged(
    /* [in] */ IRect* anchorRectDip,
    /* [in] */ Int32 anchorDir,
    /* [in] */ IRect* focusRectDip,
    /* [in] */ Int32 focusDir,
    /* [in] */ Boolean isAnchorFirst)
{
    // All coordinates are in DIP.
    Int32 x1;
    anchorRectDip->GetLeft(&x1);
    Int32 y1;
    anchorRectDip->GetBottom(&y1);
    Int32 x2;
    focusRectDip->GetLeft(&x2);
    Int32 y2;
    focusRectDip->GetBottom(&y2);

    if (x1 != x2 || y1 != y2 ||
            (mSelectionHandleController != NULL && mSelectionHandleController->IsDragging())) {
        if (mInsertionHandleController != NULL) {
            mInsertionHandleController->Hide();
        }

        if (isAnchorFirst) {
            mStartHandlePoint->SetLocalDip(x1, y1);
            mEndHandlePoint->SetLocalDip(x2, y2);
        }
        else {
            mStartHandlePoint->SetLocalDip(x2, y2);
            mEndHandlePoint->SetLocalDip(x1, y1);
        }

        Boolean wereSelectionHandlesShowing;
        GetSelectionHandleController()->IsShowing(&wereSelectionHandlesShowing);

        GetSelectionHandleController()->OnSelectionChanged(anchorDir, focusDir);
        UpdateHandleScreenPositions();
        mHasSelection = TRUE;

        Boolean isShowing;
        if (!wereSelectionHandlesShowing && (GetSelectionHandleController()->IsShowing(&isShowing), isShowing)) {
            // TODO(cjhopman): Remove this when there is a better signal that long press caused
            // a selection. See http://crbug.com/150151.
            AutoPtr<IView> view = IView::Probe(mContainerView);
            Boolean result;
            view->PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS, &result);
        }

    }
    else {
        mUnselectAllOnActionModeDismiss = FALSE;
        HideSelectActionBar();
        if (x1 != 0 && y1 != 0 && mSelectionEditable) {
            // Selection is a caret, and a text field is focused.
            if (mSelectionHandleController != NULL) {
                mSelectionHandleController->Hide();
            }

            mInsertionHandlePoint->SetLocalDip(x1, y1);

            GetInsertionHandleController()->OnCursorPositionChanged();
            UpdateHandleScreenPositions();
            AutoPtr<IView> view = IView::Probe(mContainerView);
            if (mInputMethodManagerWrapper->IsWatchingCursor(view)) {
                const Int32 xPix = (Int32) mInsertionHandlePoint->GetXPix();
                const Int32 yPix = (Int32) mInsertionHandlePoint->GetYPix();
                mInputMethodManagerWrapper->UpdateCursor(
                        view, xPix, yPix, xPix, yPix);
            }
        }
        else {
            // Deselection
            if (mSelectionHandleController != NULL) {
                mSelectionHandleController->HideAndDisallowAutomaticShowing();
            }

            if (mInsertionHandleController != NULL) {
                mInsertionHandleController->HideAndDisallowAutomaticShowing();
            }
        }
        mHasSelection = FALSE;
    }

    if (IsSelectionHandleShowing() || IsInsertionHandleShowing()) {
        mPositionObserver->AddListener(mPositionListener);
    }
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnEvaluateJavaScriptResult(
    /* [in] */ const String& jsonResult,
    /* [in] */ JavaScriptCallback* callback)
{
    callback->HandleJavaScriptResult(jsonResult);
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::ShowPastePopup(
    /* [in] */ Int32 xDip,
    /* [in] */ Int32 yDip)
{
    mInsertionHandlePoint->SetLocalDip(xDip, yDip);
    GetInsertionHandleController()->ShowHandle();
    UpdateHandleScreenPositions();
    GetInsertionHandleController()->ShowHandleWithPastePopup();
}

//@SuppressWarnings("unused")
//@CalledByNative
void ContentViewCore::OnRenderProcessChange()
{
    AttachImeAdapter();
}

/**
 * Attaches the native ImeAdapter object to the java ImeAdapter to allow communication via JNI.
 */
void ContentViewCore::AttachImeAdapter()
{
    if (mImeAdapter != NULL && mNativeContentViewCore != 0) {
        mImeAdapter->Attach(NativeGetNativeImeAdapter(mNativeContentViewCore));
    }
}

/**
 * @see View#hasFocus()
 */
//@CalledByNative
Boolean ContentViewCore::HasFocus()
{
    AutoPtr<IView> view = IView::Probe(mContainerView);
    Boolean result;
    view->HasFocus(&result);
    return result;
}

/**
 * Checks whether the ContentViewCore can be zoomed in.
 *
 * @return True if the ContentViewCore can be zoomed in.
 */
// This method uses the term 'zoom' for legacy reasons, but relates
// to what chrome calls the 'page scale factor'.
Boolean ContentViewCore::CanZoomIn()
{
    const Float zoomInExtent = mRenderCoordinates->GetMaxPageScaleFactor()
            - mRenderCoordinates->GetPageScaleFactor();
    return zoomInExtent > ZOOM_CONTROLS_EPSILON;
}

/**
 * Checks whether the ContentViewCore can be zoomed out.
 *
 * @return True if the ContentViewCore can be zoomed out.
 */
// This method uses the term 'zoom' for legacy reasons, but relates
// to what chrome calls the 'page scale factor'.
Boolean ContentViewCore::CanZoomOut()
{
    const Float zoomOutExtent = mRenderCoordinates->GetPageScaleFactor()
            - mRenderCoordinates->GetMinPageScaleFactor();
    return zoomOutExtent > ZOOM_CONTROLS_EPSILON;
}

/**
 * Zooms in the ContentViewCore by 25% (or less if that would result in
 * zooming in more than possible).
 *
 * @return True if there was a zoom change, false otherwise.
 */
// This method uses the term 'zoom' for legacy reasons, but relates
// to what chrome calls the 'page scale factor'.
Boolean ContentViewCore::ZoomIn()
{
    if (!CanZoomIn()) {
        return FALSE;
    }

    return PinchByDelta(1.25f);
}

/**
 * Zooms out the ContentViewCore by 20% (or less if that would result in
 * zooming out more than possible).
 *
 * @return True if there was a zoom change, false otherwise.
 */
// This method uses the term 'zoom' for legacy reasons, but relates
// to what chrome calls the 'page scale factor'.
Boolean ContentViewCore::ZoomOut()
{
    if (!CanZoomOut()) {
        return FALSE;
    }

    return PinchByDelta(0.8f);
}

/**
 * Resets the zoom factor of the ContentViewCore.
 *
 * @return True if there was a zoom change, false otherwise.
 */
// This method uses the term 'zoom' for legacy reasons, but relates
// to what chrome calls the 'page scale factor'.
Boolean ContentViewCore::ZoomReset()
{
    // The page scale factor is initialized to mNativeMinimumScale when
    // the page finishes loading. Thus sets it back to mNativeMinimumScale.
    if (!CanZoomOut()) {
        return FALSE;
    }

    return PinchByDelta(
            mRenderCoordinates->GetMinPageScaleFactor()
                    / mRenderCoordinates->GetPageScaleFactor());
}

/**
 * Simulate a pinch zoom gesture.
 *
 * @param delta the factor by which the current page scale should be multiplied by.
 * @return whether the gesture was sent.
 */
Boolean ContentViewCore::PinchByDelta(
    /* [in] */ Float delta)
{
    if (mNativeContentViewCore == 0) {
        return FALSE;
    }

    Int64 timeMs = SystemClock::GetUptimeMillis();
    Int32 xPix = GetViewportWidthPix() / 2;
    Int32 yPix = GetViewportHeightPix() / 2;

    NativePinchBegin(mNativeContentViewCore, timeMs, xPix, yPix);
    NativePinchBy(mNativeContentViewCore, timeMs, xPix, yPix, delta);
    NativePinchEnd(mNativeContentViewCore, timeMs);

    return TRUE;
}

/**
 * Invokes the graphical zoom picker widget for this ContentView.
 */
void ContentViewCore::InvokeZoomPicker()
{
    mZoomControlsDelegate->InvokeZoomPicker();
}

/**
 * Enables or disables inspection of JavaScript objects added via
 * {@link #addJavascriptInterface(Object, String)} by means of Object.keys() method and
 * &quot;for .. in&quot; loop. Being able to inspect JavaScript objects is useful
 * when debugging hybrid Android apps, but can't be enabled for legacy applications due
 * to compatibility risks.
 *
 * @param allow Whether to allow JavaScript objects inspection.
 */
void ContentViewCore::SetAllowJavascriptInterfacesInspection(
    /* [in] */ Boolean allow)
{
    NativeSetAllowJavascriptInterfacesInspection(mNativeContentViewCore, allow);
}

/**
 * Returns JavaScript interface objects previously injected via
 * {@link #addJavascriptInterface(Object, String)}.
 *
 * @return the mapping of names to interface objects and corresponding annotation classes
 */
AutoPtr<IMap> ContentViewCore::GetJavascriptInterfaces()
{
    return mJavaScriptInterfaces;
}

/**
 * This will mimic {@link #addPossiblyUnsafeJavascriptInterface(Object, String, Class)}
 * and automatically pass in {@link JavascriptInterface} as the required annotation.
 *
 * @param object The Java object to inject into the ContentViewCore's JavaScript context.  Null
 *               values are ignored.
 * @param name   The name used to expose the instance in JavaScript.
 */
void ContentViewCore::AddJavascriptInterface(
    /* [in] */ IInterface* object,
    /* [in] */ const String& name)
{
    // NULL means expose all methods to javascript
    // need update to expose the expected methods only
    AddPossiblyUnsafeJavascriptInterface(object, name, NULL/*TODO JavascriptInterface.class*/);
}

/**
 * This method injects the supplied Java object into the ContentViewCore.
 * The object is injected into the JavaScript context of the main frame,
 * using the supplied name. This allows the Java object to be accessed from
 * JavaScript. Note that that injected objects will not appear in
 * JavaScript until the page is next (re)loaded. For example:
 * <pre> view.addJavascriptInterface(new Object(), "injectedObject");
 * view.loadData("<!DOCTYPE html><title></title>", "text/html", null);
 * view.loadUrl("javascript:alert(injectedObject.toString())");</pre>
 * <p><strong>IMPORTANT:</strong>
 * <ul>
 * <li> addJavascriptInterface() can be used to allow JavaScript to control
 * the host application. This is a powerful feature, but also presents a
 * security risk. Use of this method in a ContentViewCore containing
 * untrusted content could allow an attacker to manipulate the host
 * application in unintended ways, executing Java code with the permissions
 * of the host application. Use extreme care when using this method in a
 * ContentViewCore which could contain untrusted content. Particular care
 * should be taken to avoid unintentional access to inherited methods, such
 * as {@link Object#getClass()}. To prevent access to inherited methods,
 * pass an annotation for {@code requiredAnnotation}.  This will ensure
 * that only methods with {@code requiredAnnotation} are exposed to the
 * Javascript layer.  {@code requiredAnnotation} will be passed to all
 * subsequently injected Java objects if any methods return an object.  This
 * means the same restrictions (or lack thereof) will apply.  Alternatively,
 * {@link #addJavascriptInterface(Object, String)} can be called, which
 * automatically uses the {@link JavascriptInterface} annotation.
 * <li> JavaScript interacts with Java objects on a private, background
 * thread of the ContentViewCore. Care is therefore required to maintain
 * thread safety.</li>
 * </ul></p>
 *
 * @param object             The Java object to inject into the
 *                           ContentViewCore's JavaScript context. Null
 *                           values are ignored.
 * @param name               The name used to expose the instance in
 *                           JavaScript.
 * @param requiredAnnotation Restrict exposed methods to ones with this
 *                           annotation.  If {@code null} all methods are
 *                           exposed.
 *
 */
void ContentViewCore::AddPossiblyUnsafeJavascriptInterface(
    /* [in] */ IInterface* object,
    /* [in] */ const String& name,
    /* [in] */ IInterface* requiredAnnotation)
{
    if (mNativeContentViewCore != 0 && object != NULL) {
        AutoPtr<ICharSequence> charSequenceTmp;
        CString::New(name, (ICharSequence**)&charSequenceTmp);

        AutoPtr<IPairHelper> helper;
        CPairHelper::AcquireSingleton((IPairHelper**)&helper);
        AutoPtr<IPair> pair;
        helper->Create(object, requiredAnnotation, (IPair**)&pair);
        mJavaScriptInterfaces->Put(TO_IINTERFACE(charSequenceTmp), TO_IINTERFACE(pair));
        NativeAddJavascriptInterface(mNativeContentViewCore, object, name, requiredAnnotation);
    }
}

/**
 * Removes a previously added JavaScript interface with the given name.
 *
 * @param name The name of the interface to remove.
 */
void ContentViewCore::RemoveJavascriptInterface(
    /* [in] */ const String& name)
{
    AutoPtr<ICharSequence> csname;
    CString::New(name, (ICharSequence**)&csname);
    mJavaScriptInterfaces->Remove(TO_IINTERFACE(csname));
    if (mNativeContentViewCore != 0) {
        NativeRemoveJavascriptInterface(mNativeContentViewCore, name);
    }
}

/**
 * Return the current scale of the ContentView.
 * @return The current page scale factor.
 */
Float ContentViewCore::GetScale()
{
    return mRenderCoordinates->GetPageScaleFactor();
}

/**
 * If the view is ready to draw contents to the screen. In hardware mode,
 * the initialization of the surface texture may not occur until after the
 * view has been added to the layout. This method will return {@code true}
 * once the texture is actually ready.
 */
Boolean ContentViewCore::IsReady()
{
    if (mNativeContentViewCore == 0) {
        return FALSE;
    }

    return NativeIsRenderWidgetHostViewReady(mNativeContentViewCore);
}

//@CalledByNative
void ContentViewCore::StartContentIntent(
    /* [in] */ const String& contentUrl)
{
    GetContentViewClient()->OnStartContentIntent(GetContext(), contentUrl);
}

//@Override
ECode ContentViewCore::OnAccessibilityStateChanged(
    /* [in] */ Boolean enabled)
{
    SetAccessibilityState(enabled);
    return NOERROR;
}

/**
 * Determines whether or not this ContentViewCore can handle this accessibility action.
 * @param action The action to perform.
 * @return Whether or not this action is supported.
 */
Boolean ContentViewCore::SupportsAccessibilityAction(
    /* [in] */ Int32 action)
{
    return mAccessibilityInjector->SupportsAccessibilityAction(action);
}

/**
 * Attempts to perform an accessibility action on the web content.  If the accessibility action
 * cannot be processed, it returns {@code null}, allowing the caller to know to call the
 * super {@link View#performAccessibilityAction(int, Bundle)} method and use that return value.
 * Otherwise the return value from this method should be used.
 * @param action The action to perform.
 * @param arguments Optional action arguments.
 * @return Whether the action was performed or {@code null} if the call should be delegated to
 *         the super {@link View} class.
 */
Boolean ContentViewCore::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    if (mAccessibilityInjector->SupportsAccessibilityAction(action)) {
        return mAccessibilityInjector->PerformAccessibilityAction(action, arguments);
    }

    return FALSE;
}

/**
 * Set the BrowserAccessibilityManager, used for native accessibility
 * (not script injection). This is only set when system accessibility
 * has been enabled.
 * @param manager The new BrowserAccessibilityManager.
 */
void ContentViewCore::SetBrowserAccessibilityManager(
    /* [in] */ BrowserAccessibilityManager* manager)
{
    mBrowserAccessibilityManager = manager;
}

/**
 * Get the BrowserAccessibilityManager, used for native accessibility
 * (not script injection). This will return null when system accessibility
 * is not enabled.
 * @return This view's BrowserAccessibilityManager.
 */
AutoPtr<BrowserAccessibilityManager> ContentViewCore::GetBrowserAccessibilityManager()
{
    return mBrowserAccessibilityManager;
}

/**
 * If native accessibility (not script injection) is enabled, and if this is
 * running on JellyBean or later, returns an AccessibilityNodeProvider that
 * implements native accessibility for this view. Returns null otherwise.
 * Lazily initializes native accessibility here if it's allowed.
 * @return The AccessibilityNodeProvider, if available, or null otherwise.
 */
AutoPtr<IAccessibilityNodeProvider> ContentViewCore::GetAccessibilityNodeProvider()
{
    if (mBrowserAccessibilityManager != NULL) {
        return mBrowserAccessibilityManager->GetAccessibilityNodeProvider();
    }

    if (mNativeAccessibilityAllowed &&
            !mNativeAccessibilityEnabled &&
            mNativeContentViewCore != 0 &&
            Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN) {
        mNativeAccessibilityEnabled = TRUE;
        NativeSetAccessibilityEnabled(mNativeContentViewCore, TRUE);
    }

    return NULL;
}

/**
 * @see View#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)
 */
void ContentViewCore::OnInitializeAccessibilityNodeInfo(
    /* [in] */  IAccessibilityNodeInfo* info)
{
    // Note: this is only used by the script-injecting accessibility code.
    mAccessibilityInjector->OnInitializeAccessibilityNodeInfo(info);
}

/**
 * @see View#onInitializeAccessibilityEvent(AccessibilityEvent)
 */
void ContentViewCore::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    // Note: this is only used by the script-injecting accessibility code.
    AutoPtr<ICharSequence> csClassName;
    CString::New(String("ContentViewCore"), (ICharSequence**)&csClassName);
    IAccessibilityRecord::Probe(event)->SetClassName(csClassName);

    // Identify where the top-left of the screen currently points to.
    AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(event);
    record->SetScrollX(mRenderCoordinates->GetScrollXPixInt());
    record->SetScrollY(mRenderCoordinates->GetScrollYPixInt());

    // The maximum scroll values are determined by taking the content dimensions and
    // subtracting off the actual dimensions of the ChromeView.
    Int32 maxScrollXPix = Elastos::Core::Math::Max(0, mRenderCoordinates->GetMaxHorizontalScrollPixInt());
    Int32 maxScrollYPix = Elastos::Core::Math::Max(0, mRenderCoordinates->GetMaxVerticalScrollPixInt());
    record->SetScrollable(maxScrollXPix > 0 || maxScrollYPix > 0);

    // Setting the maximum scroll values requires API level 15 or higher.
    const Int32 SDK_VERSION_REQUIRED_TO_SET_SCROLL = 15;
    if (Build::VERSION::SDK_INT >= SDK_VERSION_REQUIRED_TO_SET_SCROLL) {
        record->SetMaxScrollX(maxScrollXPix);
        record->SetMaxScrollY(maxScrollYPix);
    }
}

/**
 * Returns whether accessibility script injection is enabled on the device
 */
Boolean ContentViewCore::IsDeviceAccessibilityScriptInjectionEnabled()
{
    // try {
        // On JellyBean and higher, native accessibility is the default so script
        // injection is only allowed if enabled via a flag.
        if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN &&
                !CommandLine::GetInstance()->HasSwitch(
                        ContentSwitches::ENABLE_ACCESSIBILITY_SCRIPT_INJECTION)) {
            return FALSE;
        }

        if (!mContentSettings->GetJavaScriptEnabled()) {
            return FALSE;
        }

        Int32 result;
        AutoPtr<IContext> context = GetContext();
        context->CheckCallingOrSelfPermission(
                 Manifest::permission::INTERNET, &result);
        if (result != IPackageManager::PERMISSION_GRANTED) {
            return FALSE;
        }

        // Field field = Settings.Secure.class.getField("ACCESSIBILITY_SCRIPT_INJECTION");
        // field.setAccessible(true);
        String accessibilityScriptInjection = ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION;
        AutoPtr<IContentResolver> contentResolver;
        GetContext()->GetContentResolver((IContentResolver**)&contentResolver);

        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        if (mAccessibilityScriptInjectionObserver == NULL) {
            AutoPtr<IHandler> handle;
            CHandler::New((IHandler**)&handle);
            AutoPtr<ContentObserver> _contentObserver = new InnerContentObserver(this);
            _contentObserver->constructor(handle);
            AutoPtr<IContentObserver> contentObserver = _contentObserver;
            AutoPtr<IUri> uri;
            settingsSecure->GetUriFor(accessibilityScriptInjection, (IUri**)&uri);
            contentResolver->RegisterContentObserver(
                uri,
                FALSE,
                contentObserver);
            mAccessibilityScriptInjectionObserver = contentObserver;
        }

        Int32 value;
        settingsSecure->GetInt32(contentResolver, accessibilityScriptInjection, 0, &value);
        return (value == 1);
    // } catch (NoSuchFieldException e) {
    //     // Do nothing, default to false.
    // } catch (IllegalAccessException e) {
    //     // Do nothing, default to false.
    // }

    return FALSE;
}

/**
 * Returns whether or not accessibility injection is being used.
 */
Boolean ContentViewCore::IsInjectingAccessibilityScript()
{
    return mAccessibilityInjector->AccessibilityIsAvailable();
}

/**
 * Returns true if accessibility is on and touch exploration is enabled.
 */
Boolean ContentViewCore::IsTouchExplorationEnabled()
{
    return mTouchExplorationEnabled;
}

/**
 * Turns browser accessibility on or off.
 * If |state| is |false|, this turns off both native and injected accessibility.
 * Otherwise, if accessibility script injection is enabled, this will enable the injected
 * accessibility scripts. Native accessibility is enabled on demand.
 */
void ContentViewCore::SetAccessibilityState(
    /* [in] */ Boolean state)
{
    if (!state) {
        SetInjectedAccessibility(FALSE);
        mNativeAccessibilityAllowed = FALSE;
        mTouchExplorationEnabled = FALSE;
    }
    else {
        Boolean useScriptInjection = IsDeviceAccessibilityScriptInjectionEnabled();
        SetInjectedAccessibility(useScriptInjection);
        mNativeAccessibilityAllowed = !useScriptInjection;
        mAccessibilityManager->IsTouchExplorationEnabled(&mTouchExplorationEnabled);
    }
}

/**
 * Enable or disable injected accessibility features
 */
void ContentViewCore::SetInjectedAccessibility(
    /* [in] */ Boolean enabled)
{
    mAccessibilityInjector->AddOrRemoveAccessibilityApisIfNecessary();
    mAccessibilityInjector->SetScriptEnabled(enabled);
}

/**
 * Stop any TTS notifications that are currently going on.
 */
void ContentViewCore::StopCurrentAccessibilityNotifications()
{
    mAccessibilityInjector->OnPageLostFocus();
}

/**
 * Return whether or not we should set accessibility focus on page load.
 */
Boolean ContentViewCore::ShouldSetAccessibilityFocusOnPageLoad()
{
    return mShouldSetAccessibilityFocusOnPageLoad;
}

/**
 * Sets whether or not we should set accessibility focus on page load.
 * This only applies if an accessibility service like TalkBack is running.
 * This is desirable behavior for a browser window, but not for an embedded
 * WebView.
 */
void ContentViewCore::SetShouldSetAccessibilityFocusOnPageLoad(
    /* [in] */ Boolean on)
{
    mShouldSetAccessibilityFocusOnPageLoad = on;
}

/**
 * Inform WebKit that Fullscreen mode has been exited by the user.
 */
void ContentViewCore::ExitFullscreen()
{
    if (mNativeContentViewCore != 0) {
        NativeExitFullscreen(mNativeContentViewCore);
    }
}

/**
 * Changes whether hiding the top controls is enabled.
 *
 * @param enableHiding Whether hiding the top controls should be enabled or not.
 * @param enableShowing Whether showing the top controls should be enabled or not.
 * @param animate Whether the transition should be animated or not.
 */
void ContentViewCore::UpdateTopControlsState(
    /* [in] */ Boolean enableHiding,
    /* [in] */ Boolean enableShowing,
    /* [in] */ Boolean animate)
{
    if (mNativeContentViewCore != 0) {
        NativeUpdateTopControlsState(
                mNativeContentViewCore, enableHiding, enableShowing, animate);
    }
}

/**
 * Callback factory method for nativeGetNavigationHistory().
 */
//@CalledByNative
void ContentViewCore::AddToNavigationHistory(
    /* [in] */ IInterface* history,
    /* [in] */ Int32 index,
    /* [in] */ const String& url,
    /* [in] */ const String& virtualUrl,
    /* [in] */ const String& originalUrl,
    /* [in] */ const String& title,
    /* [in] */ IBitmap* favicon)
{
    AutoPtr<NavigationEntry> entry = new NavigationEntry(
            index, url, virtualUrl, originalUrl, title, favicon);

    // TODO don't kown (NavigationHistory*)history to (IInterface*) history way
    ((NavigationHistory*)(IObject::Probe(history)))->AddEntry(entry);
}

/**
 * Get a copy of the navigation history of the view.
 */
AutoPtr<NavigationHistory> ContentViewCore::GetNavigationHistory()
{
    AutoPtr<NavigationHistory> history = new NavigationHistory();
    if (mNativeContentViewCore != 0) {
        Int32 currentIndex = NativeGetNavigationHistory(mNativeContentViewCore, TO_IINTERFACE(history));
        history->SetCurrentEntryIndex(currentIndex);
    }

    return history;
}

//@Override
AutoPtr<NavigationHistory> ContentViewCore::GetDirectedNavigationHistory(
    /* [in] */ Boolean isForward,
    /* [in] */ Int32 itemLimit)
{
    AutoPtr<NavigationHistory> history = new NavigationHistory();
    if (mNativeContentViewCore != 0) {
        NativeGetDirectedNavigationHistory(
            mNativeContentViewCore, (IObject*)history, isForward, itemLimit);
    }
    return history;
}

/**
 * @return The original request URL for the current navigation entry, or null if there is no
 *         current entry.
 */
String ContentViewCore::GetOriginalUrlForActiveNavigationEntry()
{
    if (mNativeContentViewCore != 0) {
        return NativeGetOriginalUrlForActiveNavigationEntry(mNativeContentViewCore);
    }

    return String("");
}

/**
 * @return The cached copy of render positions and scales.
 */
AutoPtr<RenderCoordinates> ContentViewCore::GetRenderCoordinates()
{
    return mRenderCoordinates;
}

//@CalledByNative return IRect
AutoPtr<IInterface> ContentViewCore::CreateRect(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IRect> rect;
    CRect::New(x, y, right, bottom, (IRect**)&rect);
    return TO_IINTERFACE(rect);
}

void ContentViewCore::ExtractSmartClipData(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (mNativeContentViewCore != 0) {
        x += mSmartClipOffsetX;
        y += mSmartClipOffsetY;
        NativeExtractSmartClipData(mNativeContentViewCore, x, y, width, height);
    }
}

/**
 * Set offsets for smart clip.
 *
 * <p>This should be called if there is a viewport change introduced by,
 * e.g., show and hide of a location bar.
 *
 * @param offsetX Offset for X position.
 * @param offsetY Offset for Y position.
 */
void ContentViewCore::SetSmartClipOffsets(
    /* [in] */ Int32 offsetX,
    /* [in] */ Int32 offsetY)
{
    mSmartClipOffsetX = offsetX;
    mSmartClipOffsetY = offsetY;
}

//@CalledByNative
void ContentViewCore::OnSmartClipDataExtracted(
    /* [in] */ const String& text,
    /* [in] */ const String& html,
    /* [in] */ IRect* clipRect)
{
    if (mSmartClipDataListener != NULL) {
        mSmartClipDataListener->OnSmartClipDataExtracted(text, html, clipRect);
    }
}

void ContentViewCore::SetSmartClipDataListener(
    /* [in] */ SmartClipDataListener* listener)
{
    mSmartClipDataListener = listener;
}

void ContentViewCore::SetBackgroundOpaque(
    /* [in] */ Boolean opaque)
{
    if (mNativeContentViewCore != 0) {
        NativeSetBackgroundOpaque(mNativeContentViewCore, opaque);
    }
}

/**
 * Offer a long press gesture to the embedding View, primarily for WebView compatibility.
 *
 * @return true if the embedder handled the event.
 */
Boolean ContentViewCore::OfferLongPressToEmbedder()
{
    AutoPtr<IView> view = IView::Probe(mContainerView);
    Boolean result;
    view->PerformLongClick(&result);
    return result;
}

/**
 * Reset scroll and fling accounting, notifying listeners as appropriate.
 * This is useful as a failsafe when the input stream may have been interruped.
 */
void ContentViewCore::ResetScrollInProgress()
{
    if (!IsScrollInProgress()) {
        return;
    }

    const Boolean touchScrollInProgress = mTouchScrollInProgress;
    const Int32 potentiallyActiveFlingCount = mPotentiallyActiveFlingCount;

    mTouchScrollInProgress = FALSE;
    mPotentiallyActiveFlingCount = 0;

    if (touchScrollInProgress) {
        UpdateGestureStateListener(GestureEventType::SCROLL_END);
    }

    if (potentiallyActiveFlingCount > 0) {
        UpdateGestureStateListener(GestureEventType::FLING_END);
    }
}

Handle64 ContentViewCore::NativeInit(
    /* [in] */ Handle64 webContentsPtr,
    /* [in] */ Handle64 viewAndroidPtr,
    /* [in] */ Handle64 windowAndroidPtr,
    /* [in] */ IHashSet* retainedObjectSet)
{
    return Elastos_ContentViewCore_nativeInit(TO_IINTERFACE(this), webContentsPtr, viewAndroidPtr, windowAndroidPtr, TO_IINTERFACE(retainedObjectSet));
}

//@CalledByNative
AutoPtr<ContentVideoViewClient> ContentViewCore::GetContentVideoViewClient()
{
    return GetContentViewClient()->GetContentVideoViewClient();
}

//@CalledByNative
Boolean ContentViewCore::ShouldBlockMediaRequest(
    /* [in] */ const String& url)
{
    return GetContentViewClient()->ShouldBlockMediaRequest(url);
}

//@CalledByNative
void ContentViewCore::OnNativeFlingStopped()
{
    // Note that mTouchScrollInProgress should normally be false at this
    // point, but we reset it anyway as another failsafe.
    mTouchScrollInProgress = FALSE;
    if (mPotentiallyActiveFlingCount <= 0) {
        return;
    }
    mPotentiallyActiveFlingCount--;
    UpdateGestureStateListener(GestureEventType::FLING_END);
}

//@Override
ECode ContentViewCore::OnScreenOrientationChanged(
    /* [in] */ Int32 orientation)
{
    SendOrientationChangeEvent(orientation);
    return NOERROR;
}

AutoPtr<WebContents> ContentViewCore::NativeGetWebContentsAndroid(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    AutoPtr<IInterface> wc = Elastos_ContentViewCore_nativeGetWebContentsAndroid(TO_IINTERFACE(this), nativeContentViewCoreImpl);
    return (WebContents*)(IObject::Probe(wc));
}

void ContentViewCore::NativeOnJavaContentViewCoreDestroyed(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeOnJavaContentViewCoreDestroyed(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeLoadUrl(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ const String& url,
    /* [in] */ Int32 loadUrlType,
    /* [in] */ Int32 transitionType,
    /* [in] */ const String& referrerUrl,
    /* [in] */ Int32 referrerPolicy,
    /* [in] */ Int32 uaOverrideOption,
    /* [in] */ const String& extraHeaders,
    /* [in] */ ArrayOf<Byte>* postData,
    /* [in] */ const String& baseUrlForDataUrl,
    /* [in] */ const String& virtualUrlForDataUrl,
    /* [in] */ Boolean canLoadLocalResources,
    /* [in] */ Boolean isRendererInitiated)
{
    Elastos_ContentViewCore_nativeLoadUrl(TO_IINTERFACE(this), nativeContentViewCoreImpl, url,
            loadUrlType, transitionType, referrerUrl, referrerPolicy, uaOverrideOption, extraHeaders,
            postData, baseUrlForDataUrl, virtualUrlForDataUrl, canLoadLocalResources, isRendererInitiated);
}

String ContentViewCore::NativeGetURL(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    return Elastos_ContentViewCore_nativeGetURL(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeShowInterstitialPage(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ const String& url,
    /* [in] */ Handle64 nativeInterstitialPageDelegateAndroid)
{
    Elastos_ContentViewCore_nativeShowInterstitialPage(TO_IINTERFACE(this), nativeContentViewCoreImpl, url, nativeInterstitialPageDelegateAndroid);
}

Boolean ContentViewCore::NativeIsShowingInterstitialPage(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    return Elastos_ContentViewCore_nativeIsShowingInterstitialPage(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

Boolean ContentViewCore::NativeIsIncognito(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    return Elastos_ContentViewCore_nativeIsIncognito(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeSetFocus(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Boolean focused)
{
    Elastos_ContentViewCore_nativeSetFocus(TO_IINTERFACE(this), nativeContentViewCoreImpl, focused);
}

void ContentViewCore::NativeSendOrientationChangeEvent(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int32 orientation)
{
    Elastos_ContentViewCore_nativeSendOrientationChangeEvent(TO_IINTERFACE(this), nativeContentViewCoreImpl, orientation);
}

// All touch events (including flings, scrolls etc) accept coordinates in physical pixels.
Boolean ContentViewCore::NativeOnTouchEvent(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int64 timeMs,
    /* [in] */ Int32 action,
    /* [in] */ Int32 pointerCount,
    /* [in] */ Int32 historySize,
    /* [in] */ Int32 actionIndex,
    /* [in] */ Float x0,
    /* [in] */ Float y0,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Int32 pointerId0,
    /* [in] */ Int32 pointerId1,
    /* [in] */ Float touchMajor0,
    /* [in] */ Float touchMajor1,
    /* [in] */ Float rawX,
    /* [in] */ Float rawY,
    /* [in] */ Int32 androidToolType0,
    /* [in] */ Int32 androidToolType1,
    /* [in] */ Int32 androidButtonState)
{
    return Elastos_ContentViewCore_nativeOnTouchEvent(TO_IINTERFACE(this), nativeContentViewCoreImpl, TO_IINTERFACE(event),
            timeMs, action, pointerCount, historySize, actionIndex, x0, y0, x1, y1, pointerId0, pointerId1, touchMajor0,
            touchMajor1, rawX, rawY, androidToolType0, androidToolType1, androidButtonState);
}

Int32 ContentViewCore::NativeSendMouseMoveEvent(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return Elastos_ContentViewCore_nativeSendMouseMoveEvent(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs, x, y);
}

Int32 ContentViewCore::NativeSendMouseWheelEvent(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float verticalAxis)
{
    return Elastos_ContentViewCore_nativeSendMouseWheelEvent(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs, x, y, verticalAxis);
}

void ContentViewCore::NativeScrollBegin(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float hintX,
    /* [in] */ Float hintY)
{
    Elastos_ContentViewCore_nativeScrollBegin(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs, x, y, hintX, hintY);
}

void ContentViewCore::NativeScrollEnd(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs)
{
    Elastos_ContentViewCore_nativeScrollEnd(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs);
}

void ContentViewCore::NativeScrollBy(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float deltaX,
    /* [in] */ Float deltaY)
{
    Elastos_ContentViewCore_nativeScrollBy(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs, x, y, deltaX, deltaY);
}

void ContentViewCore::NativeFlingStart(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float vx,
    /* [in] */ Float vy)
{
    Elastos_ContentViewCore_nativeFlingStart(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs, x, y, vx, vy);
}

void ContentViewCore::NativeFlingCancel(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs)
{
    Elastos_ContentViewCore_nativeFlingCancel(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs);
}

void ContentViewCore::NativeSingleTap(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Elastos_ContentViewCore_nativeSingleTap(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs, x, y);
}

void ContentViewCore::NativeDoubleTap(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Elastos_ContentViewCore_nativeDoubleTap(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs, x, y);
}

void ContentViewCore::NativeLongPress(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Elastos_ContentViewCore_nativeLongPress(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs, x, y);
}

void ContentViewCore::NativePinchBegin(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Elastos_ContentViewCore_nativePinchBegin(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs, x, y);
}

void ContentViewCore::NativePinchEnd(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs)
{
    Elastos_ContentViewCore_nativePinchEnd(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs);
}

void ContentViewCore::NativePinchBy(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int64 timeMs,
    /* [in] */ Float anchorX,
    /* [in] */ Float anchorY,
    /* [in] */ Float deltaScale)
{
    Elastos_ContentViewCore_nativePinchBy(TO_IINTERFACE(this), nativeContentViewCoreImpl, timeMs, anchorX, anchorY, deltaScale);
}

void ContentViewCore::NativeSelectBetweenCoordinates(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float x2,
    /* [in] */ Float y2)
{
    Elastos_ContentViewCore_nativeSelectBetweenCoordinates(TO_IINTERFACE(this), nativeContentViewCoreImpl, x1, y1, x2, y2);
}

void ContentViewCore::NativeMoveCaret(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Elastos_ContentViewCore_nativeMoveCaret(TO_IINTERFACE(this), nativeContentViewCoreImpl, x, y);
}

void ContentViewCore::NativeResetGestureDetection(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeResetGestureDetection(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeSetDoubleTapSupportEnabled(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Boolean enabled)
{
    Elastos_ContentViewCore_nativeSetDoubleTapSupportEnabled(TO_IINTERFACE(this), nativeContentViewCoreImpl, enabled);
}

void ContentViewCore::NativeSetMultiTouchZoomSupportEnabled(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Boolean enabled)
{
    Elastos_ContentViewCore_nativeSetMultiTouchZoomSupportEnabled(TO_IINTERFACE(this), nativeContentViewCoreImpl, enabled);
}

void ContentViewCore::NativeLoadIfNecessary(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeLoadIfNecessary(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeRequestRestoreLoad(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeRequestRestoreLoad(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeReload(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Boolean checkForRepost)
{
    Elastos_ContentViewCore_nativeReload(TO_IINTERFACE(this), nativeContentViewCoreImpl, checkForRepost);
}

void ContentViewCore::NativeReloadIgnoringCache(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Boolean checkForRepost)
{
    Elastos_ContentViewCore_nativeReloadIgnoringCache(TO_IINTERFACE(this), nativeContentViewCoreImpl, checkForRepost);
}

void ContentViewCore::NativeCancelPendingReload(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeCancelPendingReload(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeContinuePendingReload(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeContinuePendingReload(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeSelectPopupMenuItems(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ ArrayOf<Int32>* indices)
{
    Elastos_ContentViewCore_nativeSelectPopupMenuItems(TO_IINTERFACE(this), nativeContentViewCoreImpl, indices);
}

void ContentViewCore::NativeScrollFocusedEditableNodeIntoView(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeScrollFocusedEditableNodeIntoView(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeSelectWordAroundCaret(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeSelectWordAroundCaret(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeClearHistory(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeClearHistory(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeAddStyleSheetByURL(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ const String& stylesheetUrl)
{
    Elastos_ContentViewCore_nativeAddStyleSheetByURL(TO_IINTERFACE(this), nativeContentViewCoreImpl, stylesheetUrl);
}

void ContentViewCore::NativeEvaluateJavaScript(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ const String& script,
    /* [in] */ JavaScriptCallback* callback,
    /* [in] */ Boolean startRenderer)
{
    Elastos_ContentViewCore_nativeEvaluateJavaScript(TO_IINTERFACE(this), nativeContentViewCoreImpl,
            script, TO_IINTERFACE(callback), startRenderer);
}

Handle64 ContentViewCore::NativeGetNativeImeAdapter(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    return Elastos_ContentViewCore_nativeGetNativeImeAdapter(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

Int32 ContentViewCore::NativeGetCurrentRenderProcessId(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    return Elastos_ContentViewCore_nativeGetCurrentRenderProcessId(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

Int32 ContentViewCore::NativeGetBackgroundColor(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    return Elastos_ContentViewCore_nativeGetBackgroundColor(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeOnShow(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeOnShow(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeOnHide(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeOnHide(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeSetUseDesktopUserAgent(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean reloadOnChange)
{
    Elastos_ContentViewCore_nativeSetUseDesktopUserAgent(TO_IINTERFACE(this), nativeContentViewCoreImpl, enabled, reloadOnChange);
}

Boolean ContentViewCore::NativeGetUseDesktopUserAgent(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    return Elastos_ContentViewCore_nativeGetUseDesktopUserAgent(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeClearSslPreferences(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeClearSslPreferences(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeSetAllowJavascriptInterfacesInspection(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Boolean allow)
{
    Elastos_ContentViewCore_nativeSetAllowJavascriptInterfacesInspection(TO_IINTERFACE(this), nativeContentViewCoreImpl, allow);
}

void ContentViewCore::NativeAddJavascriptInterface(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ IInterface* object,
    /* [in] */ const String& name,
    /* [in] */ IInterface* requiredAnnotation)
{
    Elastos_ContentViewCore_nativeAddJavascriptInterface(TO_IINTERFACE(this), nativeContentViewCoreImpl, object, name, requiredAnnotation);
}

void ContentViewCore::NativeRemoveJavascriptInterface(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ const String& name)
{
    Elastos_ContentViewCore_nativeRemoveJavascriptInterface(TO_IINTERFACE(this), nativeContentViewCoreImpl, name);
}

Int32 ContentViewCore::NativeGetNavigationHistory(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ IInterface* context)
{
    return Elastos_ContentViewCore_nativeGetNavigationHistory(TO_IINTERFACE(this), nativeContentViewCoreImpl, context);
}

void ContentViewCore::NativeGetDirectedNavigationHistory(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ IInterface* context,
    /* [in] */ Boolean isForward,
    /* [in] */ Int32 maxEntries)
{
    Elastos_ContentViewCore_nativeGetDirectedNavigationHistory(TO_IINTERFACE(this), nativeContentViewCoreImpl, context, isForward, maxEntries);
}

String ContentViewCore::NativeGetOriginalUrlForActiveNavigationEntry(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    return Elastos_ContentViewCore_nativeGetOriginalUrlForActiveNavigationEntry(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeWasResized(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeWasResized(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

Boolean ContentViewCore::NativeIsRenderWidgetHostViewReady(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    return Elastos_ContentViewCore_nativeIsRenderWidgetHostViewReady(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeExitFullscreen(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeExitFullscreen(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeUpdateTopControlsState(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Boolean enableHiding,
    /* [in] */ Boolean enableShowing,
    /* [in] */ Boolean animate)
{
    Elastos_ContentViewCore_nativeUpdateTopControlsState(TO_IINTERFACE(this), nativeContentViewCoreImpl, enableHiding, enableShowing, animate);
}

void ContentViewCore::NativeShowImeIfNeeded(
    /* [in] */ Handle64 nativeContentViewCoreImpl)
{
    Elastos_ContentViewCore_nativeShowImeIfNeeded(TO_IINTERFACE(this), nativeContentViewCoreImpl);
}

void ContentViewCore::NativeSetAccessibilityEnabled(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Boolean enabled)
{
    Elastos_ContentViewCore_nativeSetAccessibilityEnabled(TO_IINTERFACE(this), nativeContentViewCoreImpl, enabled);
}

void ContentViewCore::NativeExtractSmartClipData(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    Elastos_ContentViewCore_nativeExtractSmartClipData(TO_IINTERFACE(this), nativeContentViewCoreImpl, x, y, w, h);
}

void ContentViewCore::NativeSetBackgroundOpaque(
    /* [in] */ Handle64 nativeContentViewCoreImpl,
    /* [in] */ Boolean opaque)
{
    Elastos_ContentViewCore_nativeSetBackgroundOpaque(TO_IINTERFACE(this), nativeContentViewCoreImpl, opaque);
}

AutoPtr<IInterface> ContentViewCore::GetContext(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::GetContext, mObj is NULL");
        return NULL;
    }
    return TO_IINTERFACE(mObj->GetContext());
}

void ContentViewCore::OnNativeContentViewCoreDestroyed(
    /* [in] */ IInterface* obj,
    /* [in] */ Handle64 nativeContentViewCore)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnNativeContentViewCoreDestroyed, mObj is NULL");
        return;
    }
    mObj->OnNativeContentViewCoreDestroyed(nativeContentViewCore);
}

Handle64 ContentViewCore::GetNativeContentViewCore(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::GetNativeContentViewCore, mObj is NULL");
        return 0;
    }
    return mObj->GetNativeContentViewCore();
}

void ContentViewCore::OnBackgroundColorChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 color)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnBackgroundColorChanged, mObj is NULL");
        return;
    }
    mObj->OnBackgroundColorChanged(color);
}

Int32 ContentViewCore::GetViewportWidthPix(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::GetViewportWidthPix, mObj is NULL");
        return 0;
    }
    return mObj->GetViewportWidthPix();
}

Int32 ContentViewCore::GetViewportHeightPix(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::GetViewportHeightPix, mObj is NULL");
        return 0;
    }
    return mObj->GetViewportHeightPix();
}

Int32 ContentViewCore::GetPhysicalBackingWidthPix(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::GetPhysicalBackingWidthPix, mObj is NULL");
        return 0;
    }
    return mObj->GetPhysicalBackingWidthPix();
}

Int32 ContentViewCore::GetPhysicalBackingHeightPix(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::GetPhysicalBackingHeightPix, mObj is NULL");
        return 0;
    }
    return mObj->GetPhysicalBackingHeightPix();
}

Int32 ContentViewCore::GetOverdrawBottomHeightPix(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::GetOverdrawBottomHeightPix, mObj is NULL");
        return 0;
    }
    return mObj->GetOverdrawBottomHeightPix();
}

Int32 ContentViewCore::GetViewportSizeOffsetWidthPix(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::GetViewportSizeOffsetWidthPix, mObj is NULL");
        return 0;
    }
    return mObj->GetViewportSizeOffsetWidthPix();
}

Int32 ContentViewCore::GetViewportSizeOffsetHeightPix(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::GetViewportSizeOffsetHeightPix, mObj is NULL");
        return 0;
    }
    return mObj->GetViewportSizeOffsetHeightPix();
}

void ContentViewCore::OnFlingStartEventConsumed(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 vx,
    /* [in] */ Int32 vy)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnFlingStartEventConsumed, mObj is NULL");
        return;
    }
    mObj->OnFlingStartEventConsumed(vx, vy);
}

void ContentViewCore::OnFlingStartEventHadNoConsumer(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 vx,
    /* [in] */ Int32 vy)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnFlingStartEventHadNoConsumer, mObj is NULL");
        return;
    }
    mObj->OnFlingStartEventHadNoConsumer(vx, vy);
}

void ContentViewCore::OnFlingCancelEventAck(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnFlingCancelEventAck, mObj is NULL");
        return;
    }
    mObj->OnFlingCancelEventAck();
}

void ContentViewCore::OnScrollBeginEventAck(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnScrollBeginEventAck, mObj is NULL");
        return;
    }
    mObj->OnScrollBeginEventAck();
}

void ContentViewCore::OnScrollUpdateGestureConsumed(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnScrollUpdateGestureConsumed, mObj is NULL");
        return;
    }
    mObj->OnScrollUpdateGestureConsumed();
}

void ContentViewCore::OnScrollEndEventAck(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnScrollEndEventAck, mObj is NULL");
        return;
    }
    mObj->OnScrollEndEventAck();
}

void ContentViewCore::OnPinchBeginEventAck(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnPinchBeginEventAck, mObj is NULL");
        return;
    }
    mObj->OnPinchBeginEventAck();
}

void ContentViewCore::OnPinchEndEventAck(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnPinchEndEventAck, mObj is NULL");
        return;
    }
    mObj->OnPinchEndEventAck();
}

void ContentViewCore::OnSingleTapEventAck(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean consumed,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnSingleTapEventAck, mObj is NULL");
        return;
    }
    mObj->OnSingleTapEventAck(consumed, x, y);
}

void ContentViewCore::OnDoubleTapEventAck(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnDoubleTapEventAck, mObj is NULL");
        return;
    }
    mObj->OnDoubleTapEventAck();
}

Boolean ContentViewCore::FilterTapOrPressEvent(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 type,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::FilterTapOrPressEvent, mObj is NULL");
        return FALSE;
    }
    return mObj->FilterTapOrPressEvent(type, x, y);
}

void ContentViewCore::UpdateFrameInfo(
    /* [in] */ IInterface* obj,
    /* [in] */ Float scrollOffsetX,
    /* [in] */ Float scrollOffsetY,
    /* [in] */ Float pageScaleFactor,
    /* [in] */ Float minPageScaleFactor,
    /* [in] */ Float maxPageScaleFactor,
    /* [in] */ Float contentWidth,
    /* [in] */ Float contentHeight,
    /* [in] */ Float viewportWidth,
    /* [in] */ Float viewportHeight,
    /* [in] */ Float controlsOffsetYCss,
    /* [in] */ Float contentOffsetYCss,
    /* [in] */ Float overdrawBottomHeightCss)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::UpdateFrameInfo, mObj is NULL");
        return;
    }
    mObj->UpdateFrameInfo(scrollOffsetX, scrollOffsetY, pageScaleFactor, minPageScaleFactor, maxPageScaleFactor,
            contentWidth, contentHeight, viewportWidth, viewportHeight, controlsOffsetYCss, contentOffsetYCss, overdrawBottomHeightCss);
}

void ContentViewCore::UpdateImeAdapter(
    /* [in] */ IInterface* obj,
    /* [in] */ Handle64 nativeImeAdapterAndroid,
    /* [in] */ Int32 textInputType,
    /* [in] */ const String& text,
    /* [in] */ Int32 selectionStart,
    /* [in] */ Int32 selectionEnd,
    /* [in] */ Int32 compositionStart,
    /* [in] */ Int32 compositionEnd,
    /* [in] */ Boolean showImeIfNeeded,
    /* [in] */ Boolean isNonImeChange)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::UpdateImeAdapter, mObj is NULL");
        return;
    }
    mObj->UpdateImeAdapter(nativeImeAdapterAndroid, textInputType, text, selectionStart,
            selectionEnd, compositionStart, compositionEnd, showImeIfNeeded, isNonImeChange);
}

void ContentViewCore::SetTitle(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& title)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::SetTitle, mObj is NULL");
        return;
    }
    mObj->SetTitle(title);
}

void ContentViewCore::ShowSelectPopup(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* bounds,
    /* [in] */ ArrayOf<String>* items,
    /* [in] */ ArrayOf<Int32>* enabled,
    /* [in] */ Boolean multiple,
    /* [in] */ ArrayOf<Int32>* selectedIndices)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::ShowSelectPopup, mObj is NULL");
        return;
    }
    AutoPtr<IRect> r = IRect::Probe(bounds);
    mObj->ShowSelectPopup(r, items, enabled, multiple, selectedIndices);
}

void ContentViewCore::HideSelectPopup(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::HideSelectPopup, mObj is NULL");
        return;
    }
    mObj->HideSelectPopup();
}

void ContentViewCore::ShowDisambiguationPopup(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* targetRect,
    /* [in] */ IInterface* zoomedBitmap)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::ShowDisambiguationPopup, mObj is NULL");
        return;
    }
    AutoPtr<IRect> r = IRect::Probe(targetRect);
    AutoPtr<IBitmap> b = IBitmap::Probe(zoomedBitmap);
    mObj->ShowDisambiguationPopup(r, b);
}

AutoPtr<IInterface> ContentViewCore::CreateTouchEventSynthesizer(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::CreateTouchEventSynthesizer, mObj is NULL");
        return NULL;
    }
    return TO_IINTERFACE(mObj->CreateTouchEventSynthesizer());
}

void ContentViewCore::OnSelectionChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& text)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnSelectionChanged, mObj is NULL");
        return;
    }
    mObj->OnSelectionChanged(text);
}

void ContentViewCore::ShowSelectionHandlesAutomatically(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::ShowSelectionHandlesAutomatically, mObj is NULL");
        return;
    }
    mObj->ShowSelectionHandlesAutomatically();
}

void ContentViewCore::OnSelectionBoundsChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* anchorRectDip,
    /* [in] */ Int32 anchorDir,
    /* [in] */ IInterface* focusRectDip,
    /* [in] */ Int32 focusDir,
    /* [in] */ Boolean isAnchorFirst)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnSelectionBoundsChanged, mObj is NULL");
        return;
    }
    AutoPtr<IRect> r1 = IRect::Probe(anchorRectDip);
    AutoPtr<IRect> r2 = IRect::Probe(focusRectDip);
    mObj->OnSelectionBoundsChanged(r1, anchorDir, r2, focusDir, isAnchorFirst);
}

void ContentViewCore::OnEvaluateJavaScriptResult(
    /* [in] */ const String& jsonResult,
    /* [in] */ IInterface* callback)
{
    AutoPtr<JavaScriptCallback> jcb = (JavaScriptCallback*)(IObject::Probe(callback));
    OnEvaluateJavaScriptResult(jsonResult, jcb);
}

void ContentViewCore::ShowPastePopup(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 xDip,
    /* [in] */ Int32 yDip)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::ShowPastePopup, mObj is NULL");
        return;
    }
    mObj->ShowPastePopup(xDip, yDip);
}

void ContentViewCore::OnRenderProcessChange(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnRenderProcessChange, mObj is NULL");
        return;
    }
    mObj->OnRenderProcessChange();
}

Boolean ContentViewCore::HasFocus(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::HasFocus, mObj is NULL");
        return FALSE;
    }
    return mObj->HasFocus();
}

void ContentViewCore::StartContentIntent(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& contentUrl)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::StartContentIntent, mObj is NULL");
        return;
    }
    mObj->StartContentIntent(contentUrl);
}

void ContentViewCore::AddToNavigationHistory(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* history,
    /* [in] */ Int32 index,
    /* [in] */ const String& url,
    /* [in] */ const String& virtualUrl,
    /* [in] */ const String& originalUrl,
    /* [in] */ const String& title,
    /* [in] */ IInterface* favicon)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::AddToNavigationHistory, mObj is NULL");
        return;
    }
    AutoPtr<IBitmap> b = IBitmap::Probe(favicon);
    mObj->AddToNavigationHistory(history, index, url, virtualUrl, originalUrl, title, b);
}

void ContentViewCore::OnSmartClipDataExtracted(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& text,
    /* [in] */ const String& html,
    /* [in] */ IInterface* clipRect)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnSmartClipDataExtracted, mObj is NULL");
        return;
    }
    AutoPtr<IRect> r = IRect::Probe(clipRect);
    mObj->OnSmartClipDataExtracted(text, html, r);
}

AutoPtr<IInterface> ContentViewCore::GetContentVideoViewClient(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::GetContentVideoViewClient, mObj is NULL");
        return NULL;
    }
    return TO_IINTERFACE(mObj->GetContentVideoViewClient());
}

Boolean ContentViewCore::ShouldBlockMediaRequest(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::ShouldBlockMediaRequest, mObj is NULL");
        return FALSE;
    }
    return mObj->ShouldBlockMediaRequest(url);
}

void ContentViewCore::OnNativeFlingStopped(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ContentViewCore> mObj = (ContentViewCore*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentViewCore::OnNativeFlingStopped, mObj is NULL");
        return;
    }
    mObj->OnNativeFlingStopped();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
