//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/accessibility/BrowserAccessibilityManager.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/BrowserAccessibilityManager_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/accessibility/JellyBeanBrowserAccessibilityManager.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/os/Build.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::EIID_ISpannable;
using Elastos::Droid::Text::Style::CURLSpan;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfoHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::CAccessibilityNodeInfoHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityRecord;
using Elastos::Droid::View::EIID_IView;
using Elastos::Core::CString;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Accessibility {

const String BrowserAccessibilityManager::TAG("BrowserAccessibilityManager");

static AutoPtr< ArrayOf<Int32> > mTempLocation_init()
{
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
    return array;
}

BrowserAccessibilityManager::BrowserAccessibilityManager(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ ContentViewCore* contentViewCore)
    : mContentViewCore(contentViewCore)
    , mNativeObj(nativeBrowserAccessibilityManagerAndroid)
    , mAccessibilityFocusId(IView::NO_ID)
    , mIsHovering(FALSE)
    , mLastHoverId(IView::NO_ID)
    , mCurrentRootId(IView::NO_ID)
    , mTempLocation(mTempLocation_init())
    , mUserHasTouchExplored(FALSE)
    , mPendingScrollToMakeNodeVisible(FALSE)
{
    mContentViewCore->SetBrowserAccessibilityManager(this);
    mView = mContentViewCore->GetContainerView();
    mRenderCoordinates = mContentViewCore->GetRenderCoordinates();
    AutoPtr<IInterface> obj;
    mContentViewCore->GetContext()
        ->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    mAccessibilityManager = IAccessibilityManager::Probe(obj);
}

/**
 * Create a BrowserAccessibilityManager object, which is owned by the C++
 * BrowserAccessibilityManagerAndroid instance, and connects to the content view.
 * @param nativeBrowserAccessibilityManagerAndroid A pointer to the counterpart native
 *     C++ object that owns this object.
 * @param contentViewCore The content view that this object provides accessibility for.
 */
//@CalledByNative
AutoPtr<BrowserAccessibilityManager> BrowserAccessibilityManager::Create(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ ContentViewCore* contentViewCore)
{
    // A bug in the KitKat framework prevents us from using these new APIs.
    // http://crbug.com/348088/
    // if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
    //     return new KitKatBrowserAccessibilityManager(
    //             nativeBrowserAccessibilityManagerAndroid, contentViewCore);

    if (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN) {
        AutoPtr<BrowserAccessibilityManager> ret = new JellyBeanBrowserAccessibilityManager(
                nativeBrowserAccessibilityManagerAndroid, contentViewCore);
        return ret;
    }
    else {
        AutoPtr<BrowserAccessibilityManager> ret = new BrowserAccessibilityManager(
                nativeBrowserAccessibilityManagerAndroid, contentViewCore);
        return ret;
    }
}

//@CalledByNative
void BrowserAccessibilityManager::OnNativeObjectDestroyed()
{
    if (mContentViewCore->GetBrowserAccessibilityManager().Get() == this) {
        mContentViewCore->SetBrowserAccessibilityManager(NULL);
    }

    mNativeObj = 0;
    mContentViewCore = NULL;
}

/**
 * @return An AccessibilityNodeProvider on JellyBean, and null on previous versions.
 */
AutoPtr<IAccessibilityNodeProvider> BrowserAccessibilityManager::GetAccessibilityNodeProvider()
{
    return NULL;
}

/**
 * @see AccessibilityNodeProvider#createAccessibilityNodeInfo(int)
 */
AutoPtr<IAccessibilityNodeInfo> BrowserAccessibilityManager::CreateAccessibilityNodeInfo(
    /* [in] */ Int32 virtualViewId)
{
    Boolean isEnabled;
    mAccessibilityManager->IsEnabled(&isEnabled);
    if (!isEnabled || mNativeObj == 0) {
        return NULL;
    }

    Int32 rootId = NativeGetRootId(mNativeObj);

    if (virtualViewId == IView::NO_ID) {
        return CreateNodeForHost(rootId);
    }

    if (!IsFrameInfoInitialized()) {
        return NULL;
    }

    AutoPtr<IAccessibilityNodeInfoHelper> helper;
    CAccessibilityNodeInfoHelper::AcquireSingleton((IAccessibilityNodeInfoHelper**)&helper);
    AutoPtr<IAccessibilityNodeInfo> info;
    AutoPtr<IView> view = IView::Probe(mView);
    helper->Obtain(view, (IAccessibilityNodeInfo**)&info);
    String pkName;
    mContentViewCore->GetContext()->GetPackageName(&pkName);
    AutoPtr<ICharSequence> pkNameCS;
    CString::New(pkName, (ICharSequence**)&pkNameCS);
    info->SetPackageName(pkNameCS);
    info->SetSource(view, virtualViewId);

    if (virtualViewId == rootId) {
        info->SetParent(view);
    }

    if (NativePopulateAccessibilityNodeInfo(mNativeObj, info, virtualViewId)) {
        return info;
    }
    else {
        info->Recycle();
        return NULL;
    }
}

/**
 * @see AccessibilityNodeProvider#findAccessibilityNodeInfosByText(String, int)
 */
AutoPtr<IList> BrowserAccessibilityManager::FindAccessibilityNodeInfosByText(
    /* [in] */ const String& text,
    /* [in] */ Int32 virtualViewId)
{
    AutoPtr<IList> list;
    CArrayList::New((IList**)&list);
    return list;
}

/**
 * @see AccessibilityNodeProvider#performAction(int, int, Bundle)
 */
Boolean BrowserAccessibilityManager::PerformAction(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    // We don't support any actions on the host view or nodes
    // that are not (any longer) in the tree.
    Boolean isEnabled;
    mAccessibilityManager->IsEnabled(&isEnabled);
    if (!isEnabled || mNativeObj == 0
            || !NativeIsNodeValid(mNativeObj, virtualViewId)) {
        return FALSE;
    }

    switch (action) {
        case IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS:
            if (mAccessibilityFocusId == virtualViewId) {
                return TRUE;
            }

            mAccessibilityFocusId = virtualViewId;
            SendAccessibilityEvent(mAccessibilityFocusId,
                    IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED);
            if (!mIsHovering) {
                NativeScrollToMakeNodeVisible(
                        mNativeObj, mAccessibilityFocusId);
            }
            else {
                mPendingScrollToMakeNodeVisible = TRUE;
            }
            return TRUE;
        case IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS:
            if (mAccessibilityFocusId == virtualViewId) {
                SendAccessibilityEvent(mAccessibilityFocusId,
                        IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED);
                mAccessibilityFocusId = IView::NO_ID;
            }
            return TRUE;
        case IAccessibilityNodeInfo::ACTION_CLICK:
            NativeClick(mNativeObj, virtualViewId);
            SendAccessibilityEvent(virtualViewId,
                    IAccessibilityEvent::TYPE_VIEW_CLICKED);
            return TRUE;
        case IAccessibilityNodeInfo::ACTION_FOCUS:
            NativeFocus(mNativeObj, virtualViewId);
            return TRUE;
        case IAccessibilityNodeInfo::ACTION_CLEAR_FOCUS:
            NativeBlur(mNativeObj);
            return TRUE;

        case IAccessibilityNodeInfo::ACTION_NEXT_HTML_ELEMENT: {
            if (arguments == NULL)
                return FALSE;
            String elementType;
            arguments->GetString(
                IAccessibilityNodeInfo::ACTION_ARGUMENT_HTML_ELEMENT_STRING, &elementType);
            if (elementType == NULL)
                return FALSE;

            elementType = elementType.ToUpperCase(/*ILocale::US*/);
            return JumpToElementType(elementType, TRUE);
        }
        case IAccessibilityNodeInfo::ACTION_PREVIOUS_HTML_ELEMENT: {
            if (arguments == NULL)
                return FALSE;
            String elementType;
            arguments->GetString(
                IAccessibilityNodeInfo::ACTION_ARGUMENT_HTML_ELEMENT_STRING,
                &elementType);
            if (elementType == NULL)
                return FALSE;

            elementType = elementType.ToUpperCase(/*ILocale::US*/);
            return JumpToElementType(elementType, FALSE);
        }

        default:
            break;
    }

    return FALSE;
}

/**
 * @see View#onHoverEvent(MotionEvent)
 */
Boolean BrowserAccessibilityManager::OnHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    Boolean isEnabled;
    mAccessibilityManager->IsEnabled(&isEnabled);
    if (!isEnabled || mNativeObj == 0) {
        return FALSE;
    }

    Int32 action;
    event->GetAction(&action);
    if (action == IMotionEvent::ACTION_HOVER_EXIT) {
        mIsHovering = FALSE;
        if (mPendingScrollToMakeNodeVisible) {
            NativeScrollToMakeNodeVisible(
                    mNativeObj, mAccessibilityFocusId);
        }
        mPendingScrollToMakeNodeVisible = FALSE;
        return TRUE;
    }

    mIsHovering = TRUE;
    mUserHasTouchExplored = TRUE;
    Float x;
    event->GetX(&x);
    Float y;
    event->GetY(&y);

    // Convert to CSS coordinates.
    Int32 cssX = (Int32) (mRenderCoordinates->FromPixToLocalCss(x));
    Int32 cssY = (Int32) (mRenderCoordinates->FromPixToLocalCss(y));

    // This sends an IPC to the render process to do the hit testing.
    // The response is handled by handleHover.
    NativeHitTest(mNativeObj, cssX, cssY);

    return TRUE;
}

/**
 * Called by ContentViewCore to notify us when the frame info is initialized,
 * the first time, since until that point, we can't use mRenderCoordinates to transform
 * web coordinates to screen coordinates.
 */
void BrowserAccessibilityManager::NotifyFrameInfoInitialized()
{
    // Invalidate the container view, since the chrome accessibility tree is now
    // ready and listed as the child of the container view.
    AutoPtr<IView> view = IView::Probe(mView);
    view->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED);

    // (Re-) focus focused element, since we weren't able to create an
    // AccessibilityNodeInfo for this element before.
    if (mAccessibilityFocusId != IView::NO_ID) {
        SendAccessibilityEvent(mAccessibilityFocusId,
                               IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED);
    }
}

Boolean BrowserAccessibilityManager::JumpToElementType(
    /* [in] */ const String& elementType,
    /* [in] */ Boolean forwards)
{
    Int32 id = NativeFindElementType(mNativeObj, mAccessibilityFocusId, elementType, forwards);
    if (id == 0)
        return FALSE;

    mAccessibilityFocusId = id;
    SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED);

    return TRUE;
}

void BrowserAccessibilityManager::SendAccessibilityEvent(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 eventType)
{
    // If we don't have any frame info, then the virtual hierarchy
    // doesn't exist in the view of the Android framework, so should
    // never send any events.
    Boolean isEnabled;
    mAccessibilityManager->IsEnabled(&isEnabled);
    if (!isEnabled || mNativeObj == 0
            || !IsFrameInfoInitialized()) {
        return;
    }

    // This is currently needed if we want Android to draw the yellow box around
    // the item that has accessibility focus. In practice, this doesn't seem to slow
    // things down, because it's only called when the accessibility focus moves.
    // TODO(dmazzoni): remove this if/when Android framework fixes bug.
    AutoPtr<IView> view = IView::Probe(mView);
    view->PostInvalidate();

    // The container view is indicated by a virtualViewId of NO_ID; post these events directly
    // since there's no web-specific information to attach.
    if (virtualViewId == IView::NO_ID) {
        view->SendAccessibilityEvent(eventType);
        return;
    }

    AutoPtr<IAccessibilityEventHelper> accessibilityEventHelper;
    CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&accessibilityEventHelper);
    AutoPtr<IAccessibilityEvent> event;
    accessibilityEventHelper->Obtain(eventType, (IAccessibilityEvent**)&event);
    String packeName;
    mContentViewCore->GetContext()->GetPackageName(&packeName);
    AutoPtr<ICharSequence> packeNameCS;
    CString::New(packeName, (ICharSequence**)&packeNameCS);
    event->SetPackageName(packeNameCS);
    IAccessibilityRecord::Probe(event)->SetSource(IView::Probe(mView), virtualViewId);
    if (!NativePopulateAccessibilityEvent(mNativeObj, event, virtualViewId, eventType)) {
        IAccessibilityRecord::Probe(event)->Recycle();
        return;
    }

    Boolean res;
    IViewParent::Probe(mView)->RequestSendAccessibilityEvent(IView::Probe(mView), event, &res);
}

AutoPtr<IBundle> BrowserAccessibilityManager::GetOrCreateBundleForAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AutoPtr<IParcelable> obj;
    IAccessibilityRecord::Probe(event)->GetParcelableData((IParcelable**)&obj);
    AutoPtr<IBundle> bundle = IBundle::Probe(obj);
    if (bundle == NULL) {
        CBundle::New((IBundle**)&bundle);
        IAccessibilityRecord::Probe(event)->SetParcelableData(IParcelable::Probe(bundle));
    }

    return bundle;
}

AutoPtr<IAccessibilityNodeInfo> BrowserAccessibilityManager::CreateNodeForHost(
    /* [in] */ Int32 rootId)
{
    // Since we don't want the parent to be focusable, but we can't remove
    // actions from a node, copy over the necessary fields.
    AutoPtr<IAccessibilityNodeInfoHelper> helper;
    CAccessibilityNodeInfoHelper::AcquireSingleton((IAccessibilityNodeInfoHelper**)&helper);

    AutoPtr<IAccessibilityNodeInfo> result;
    AutoPtr<IView> view = IView::Probe(mView);
    helper->Obtain(view, (IAccessibilityNodeInfo**)&result);
    AutoPtr<IAccessibilityNodeInfo> source;
    helper->Obtain(view, (IAccessibilityNodeInfo**)&source);
    view->OnInitializeAccessibilityNodeInfo(source);

    // Copy over parent and screen bounds.
    AutoPtr<IRect> rect;
    CRect::New((IRect**)&rect);
    source->GetBoundsInParent(rect);
    result->SetBoundsInParent(rect);
    source->GetBoundsInScreen(rect);
    result->SetBoundsInScreen(rect);

    // Set up the parent view, if applicable.
    AutoPtr<IViewParent> parent;
    view->GetParentForAccessibility((IViewParent**)&parent);
    if (parent->Probe(EIID_IView)) {
        result->SetParent(IView::Probe(parent));
    }

    // Populate the minimum required fields.
    Boolean isVisibleToUser;
    source->IsVisibleToUser(&isVisibleToUser);
    result->SetVisibleToUser(isVisibleToUser);
    Boolean isEnabled;
    source->IsEnabled(&isEnabled);
    result->SetEnabled(isEnabled);
    AutoPtr<ICharSequence> pkName;
    source->GetPackageName((ICharSequence**)&pkName);
    result->SetPackageName(pkName);
    AutoPtr<ICharSequence> className;
    source->GetClassName((ICharSequence**)&className);
    result->SetClassName(className);

    // Add the Chrome root node.
    if (IsFrameInfoInitialized()) {
        result->AddChild(view, rootId);
    }

    return result;
}

Boolean BrowserAccessibilityManager::IsFrameInfoInitialized()
{
    return mRenderCoordinates->GetContentWidthCss() != 0.0 ||
           mRenderCoordinates->GetContentHeightCss() != 0.0;
}

//@CalledByNative
void BrowserAccessibilityManager::HandlePageLoaded(
    /* [in] */ Int32 id)
{
    if (mUserHasTouchExplored) return;

    if (mContentViewCore->ShouldSetAccessibilityFocusOnPageLoad()) {
        mAccessibilityFocusId = id;
        SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED);
    }
}

//@CalledByNative
void BrowserAccessibilityManager::HandleFocusChanged(
    /* [in] */ Int32 id)
{
    SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_VIEW_FOCUSED);

    // Update accessibility focus if not already set to this node.
    if (mAccessibilityFocusId != id) {
        SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED);
        mAccessibilityFocusId = id;
    }
}

//@CalledByNative
void BrowserAccessibilityManager::HandleCheckStateChanged(
    /* [in] */ Int32 id)
{
    SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_VIEW_CLICKED);
}

//@CalledByNative
void BrowserAccessibilityManager::HandleTextSelectionChanged(
    /* [in] */ Int32 id)
{
    SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_VIEW_TEXT_SELECTION_CHANGED);
}

//@CalledByNative
void BrowserAccessibilityManager::HandleEditableTextChanged(
    /* [in] */ Int32 id)
{
    SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_VIEW_TEXT_CHANGED);
}

//@CalledByNative
void BrowserAccessibilityManager::HandleContentChanged(
    /* [in] */ Int32 id)
{
    Int32 rootId = NativeGetRootId(mNativeObj);
    if (rootId != mCurrentRootId) {
        mCurrentRootId = rootId;
        AutoPtr<IView> view = IView::Probe(mView);
        view->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED);
    }
    else {
        SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED);
    }
}

//@CalledByNative
void BrowserAccessibilityManager::HandleNavigate()
{
    mAccessibilityFocusId = IView::NO_ID;
    mUserHasTouchExplored = FALSE;
    // Invalidate the host, since its child is now gone.
    AutoPtr<IView> view = IView::Probe(mView);
    view->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED);
}

//@CalledByNative
void BrowserAccessibilityManager::HandleScrollPositionChanged(
    /* [in] */ Int32 id)
{
    SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_VIEW_SCROLLED);
}

//@CalledByNative
void BrowserAccessibilityManager::HandleScrolledToAnchor(
    /* [in] */ Int32 id)
{
    if (mAccessibilityFocusId == id) {
        return;
    }

    mAccessibilityFocusId = id;
    SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED);
}

//@CalledByNative
void BrowserAccessibilityManager::HandleHover(
    /* [in] */ Int32 id)
{
    if (mLastHoverId == id) return;

    // Always send the ENTER and then the EXIT event, to match a standard Android View.
    SendAccessibilityEvent(id, IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER);
    SendAccessibilityEvent(mLastHoverId, IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT);
    mLastHoverId = id;
}

//@CalledByNative
void BrowserAccessibilityManager::AnnounceLiveRegionText(
    /* [in] */ const String& text)
{
    AutoPtr<ICharSequence> textCS;
    CString::New(text, (ICharSequence**)&textCS);
    AutoPtr<IView> view = IView::Probe(mView);
    view->AnnounceForAccessibility(textCS);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityNodeInfoParent(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ Int32 parentId)
{
    AutoPtr<IView> view = IView::Probe(mView);
    node->SetParent(view, parentId);
}

//@CalledByNative
void BrowserAccessibilityManager::AddAccessibilityNodeInfoChild(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ Int32 childId)
{
    AutoPtr<IView> view = IView::Probe(mView);
    node->AddChild(view, childId);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityNodeInfoBooleanAttributes(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Boolean checkable,
    /* [in] */ Boolean checked,
    /* [in] */ Boolean clickable,
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean focusable,
    /* [in] */ Boolean focused,
    /* [in] */ Boolean password,
    /* [in] */ Boolean scrollable,
    /* [in] */ Boolean selected,
    /* [in] */ Boolean visibleToUser)
{
    node->SetCheckable(checkable);
    node->SetChecked(checked);
    node->SetClickable(clickable);
    node->SetEnabled(enabled);
    node->SetFocusable(focusable);
    node->SetFocused(focused);
    node->SetPassword(password);
    node->SetScrollable(scrollable);
    node->SetSelected(selected);
    node->SetVisibleToUser(visibleToUser);

    node->AddAction(IAccessibilityNodeInfo::ACTION_NEXT_HTML_ELEMENT);
    node->AddAction(IAccessibilityNodeInfo::ACTION_PREVIOUS_HTML_ELEMENT);

    if (focusable) {
        if (focused) {
            node->AddAction(IAccessibilityNodeInfo::ACTION_CLEAR_FOCUS);
        }
        else {
            node->AddAction(IAccessibilityNodeInfo::ACTION_FOCUS);
        }
    }

    if (mAccessibilityFocusId == virtualViewId) {
        node->SetAccessibilityFocused(TRUE);
        node->AddAction(IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS);
    }
    else {
        node->SetAccessibilityFocused(FALSE);
        node->AddAction(IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS);
    }

    if (clickable) {
        node->AddAction(IAccessibilityNodeInfo::ACTION_CLICK);
    }
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityNodeInfoClassName(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ const String& className)
{
    AutoPtr<ICharSequence> classNameCS;
    CString::New(className, (ICharSequence**)&classNameCS);
    node->SetClassName(classNameCS);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityNodeInfoContentDescription(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ const String& contentDescription,
    /* [in] */ Boolean annotateAsLink)
{
    if (annotateAsLink) {
        AutoPtr<ISpannableString> spannable;
        AutoPtr<ICharSequence> cdCS;
        CString::New(contentDescription, (ICharSequence**)&cdCS);
        CSpannableString::New(cdCS, (ISpannableString**)&spannable);
        AutoPtr<IURLSpan> URLSpan;
        CURLSpan::New(String(""), (IURLSpan**)&URLSpan);
        Int32 length;
        (ICharSequence::Probe(spannable))->GetLength(&length);
        (ISpannable::Probe(spannable))->SetSpan(URLSpan, 0, length, 0);
        node->SetContentDescription(ICharSequence::Probe(spannable));
    }
    else {
        AutoPtr<ICharSequence> contentDescriptionCS;
        CString::New(contentDescription, (ICharSequence**)&contentDescriptionCS);
        node->SetContentDescription(contentDescriptionCS);
    }
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityNodeInfoLocation(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ Int32 absoluteLeft,
    /* [in] */ Int32 absoluteTop,
    /* [in] */ Int32 parentRelativeLeft,
    /* [in] */ Int32 parentRelativeTop,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean isRootNode)
{
    // First set the bounds in parent.
    AutoPtr<IRect> boundsInParent;
    CRect::New(parentRelativeLeft, parentRelativeTop,
             parentRelativeLeft + width, parentRelativeTop + height, (IRect**)&boundsInParent);
    if (isRootNode) {
        // Offset of the web content relative to the View.
        boundsInParent->Offset(0, (Int32) mRenderCoordinates->GetContentOffsetYPix());
    }

    node->SetBoundsInParent(boundsInParent);

    // Now set the absolute rect, which requires several transformations.
    AutoPtr<IRect> rect;
    CRect::New(absoluteLeft, absoluteTop, absoluteLeft + width, absoluteTop + height, (IRect**)&rect);

    // Offset by the scroll position.
    rect->Offset(-(Int32) mRenderCoordinates->GetScrollX(),
                -(Int32) mRenderCoordinates->GetScrollY());

    // Convert CSS (web) pixels to Android View pixels
    Int32 left;
    rect->GetLeft(&left);
    rect->SetLeft((Int32) mRenderCoordinates->FromLocalCssToPix(left));
    Int32 top;
    rect->GetTop(&top);
    rect->SetTop((Int32) mRenderCoordinates->FromLocalCssToPix(top));
    Int32 bottom;
    rect->GetBottom(&bottom);
    rect->SetBottom((Int32) mRenderCoordinates->FromLocalCssToPix(bottom));
    Int32 right;
    rect->GetRight(&right);
    rect->SetRight((Int32) mRenderCoordinates->FromLocalCssToPix(right));

    // Offset by the location of the web content within the view.
    rect->Offset(0,
                (Int32) mRenderCoordinates->GetContentOffsetYPix());

    // Finally offset by the location of the view within the screen.
    AutoPtr< ArrayOf<Int32> > viewLocation = ArrayOf<Int32>::Alloc(2);
    AutoPtr<IView> view = IView::Probe(mView);
    view->GetLocationOnScreen(viewLocation);
    rect->Offset((*viewLocation)[0], (*viewLocation)[1]);

    node->SetBoundsInScreen(rect);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityNodeInfoKitKatAttributes(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ Boolean canOpenPopup,
    /* [in] */ Boolean contentInvalid,
    /* [in] */ Boolean dismissable,
    /* [in] */ Boolean multiLine,
    /* [in] */ Int32 inputType,
    /* [in] */ Int32 liveRegion)
{
    // Requires KitKat or higher.
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityNodeInfoCollectionInfo(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ Int32 rowCount,
    /* [in] */ Int32 columnCount,
    /* [in] */ Boolean hierarchical)
{
    // Requires KitKat or higher.
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityNodeInfoCollectionItemInfo(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 rowSpan,
    /* [in] */ Int32 columnIndex,
    /* [in] */ Int32 columnSpan,
    /* [in] */ Boolean heading)
{
    // Requires KitKat or higher.
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityNodeInfoRangeInfo(
    /* [in] */ IAccessibilityNodeInfo* node,
    /* [in] */ Int32 rangeType,
    /* [in] */ Float min,
    /* [in] */ Float max,
    /* [in] */ Float current)
{
    // Requires KitKat or higher.
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventBooleanAttributes(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Boolean checked,
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean password,
    /* [in] */ Boolean scrollable)
{
    AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(event);
    record->SetChecked(checked);
    record->SetEnabled(enabled);
    record->SetPassword(password);
    record->SetScrollable(scrollable);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventClassName(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ const String& className)
{
    AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(event);
    AutoPtr<ICharSequence> classNameCS;
    CString::New(className, (ICharSequence**)&classNameCS);
    record->SetClassName(classNameCS);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventListAttributes(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 currentItemIndex,
    /* [in] */ Int32 itemCount)
{
    AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(event);
    record->SetCurrentItemIndex(currentItemIndex);
    record->SetItemCount(itemCount);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventScrollAttributes(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Int32 maxScrollX,
    /* [in] */ Int32 maxScrollY)
{
    AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(event);
    record->SetScrollX(scrollX);
    record->SetScrollY(scrollY);
    record->SetMaxScrollX(maxScrollX);
    record->SetMaxScrollY(maxScrollY);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventTextChangedAttrs(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 addedCount,
    /* [in] */ Int32 removedCount,
    /* [in] */ const String& beforeText,
    /* [in] */ const String& text)
{
    AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(event);
    record->SetFromIndex(fromIndex);
    record->SetAddedCount(addedCount);
    record->SetRemovedCount(removedCount);
    AutoPtr<ICharSequence> beforeTextCS;
    CString::New(beforeText, (ICharSequence**)&beforeTextCS);
    record->SetBeforeText(beforeTextCS);
    AutoPtr<IList> container;
    record->GetText((IList**)&container);
    AutoPtr<ICharSequence> textCS;
    CString::New(text, (ICharSequence**)&textCS);
    container->Add(textCS);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventSelectionAttrs(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 addedCount,
    /* [in] */ Int32 itemCount,
    /* [in] */ const String& text)
{
    AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(event);
    record->SetFromIndex(fromIndex);
    record->SetAddedCount(addedCount);
    record->SetItemCount(itemCount);
    AutoPtr<IList> container;
    record->GetText((IList**)&container);
    AutoPtr<ICharSequence> textCS;
    CString::New(text, (ICharSequence**)&textCS);
    container->Add(textCS);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventKitKatAttributes(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Boolean canOpenPopup,
    /* [in] */ Boolean contentInvalid,
    /* [in] */ Boolean dismissable,
    /* [in] */ Boolean multiLine,
    /* [in] */ Int32 inputType,
    /* [in] */ Int32 liveRegion)
{
    // Backwards compatibility for KitKat AccessibilityNodeInfo fields.
    AutoPtr<IBundle> bundle = GetOrCreateBundleForAccessibilityEvent(event);
    bundle->PutBoolean(String("AccessibilityNodeInfo.canOpenPopup"), canOpenPopup);
    bundle->PutBoolean(String("AccessibilityNodeInfo.contentInvalid"), contentInvalid);
    bundle->PutBoolean(String("AccessibilityNodeInfo.dismissable"), dismissable);
    bundle->PutBoolean(String("AccessibilityNodeInfo.multiLine"), multiLine);
    bundle->PutInt32(String("AccessibilityNodeInfo.inputType"), inputType);
    bundle->PutInt32(String("AccessibilityNodeInfo.liveRegion"), liveRegion);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventCollectionInfo(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 rowCount,
    /* [in] */ Int32 columnCount,
    /* [in] */ Boolean hierarchical)
{
    // Backwards compatibility for KitKat AccessibilityNodeInfo fields.
    AutoPtr<IBundle> bundle = GetOrCreateBundleForAccessibilityEvent(event);
    bundle->PutInt32(String("AccessibilityNodeInfo.CollectionInfo.rowCount"), rowCount);
    bundle->PutInt32(String("AccessibilityNodeInfo.CollectionInfo.columnCount"), columnCount);
    bundle->PutBoolean(String("AccessibilityNodeInfo.CollectionInfo.hierarchical"), hierarchical);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventHeadingFlag(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Boolean heading)
{
    // Backwards compatibility for KitKat AccessibilityNodeInfo fields.
    AutoPtr<IBundle> bundle = GetOrCreateBundleForAccessibilityEvent(event);
    bundle->PutBoolean(String("AccessibilityNodeInfo.CollectionItemInfo.heading"), heading);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventCollectionItemInfo(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 rowSpan,
    /* [in] */ Int32 columnIndex,
    /* [in] */ Int32 columnSpan)
{
    // Backwards compatibility for KitKat AccessibilityNodeInfo fields.
    AutoPtr<IBundle> bundle = GetOrCreateBundleForAccessibilityEvent(event);
    bundle->PutInt32(String("AccessibilityNodeInfo.CollectionItemInfo.rowIndex"), rowIndex);
    bundle->PutInt32(String("AccessibilityNodeInfo.CollectionItemInfo.rowSpan"), rowSpan);
    bundle->PutInt32(String("AccessibilityNodeInfo.CollectionItemInfo.columnIndex"), columnIndex);
    bundle->PutInt32(String("AccessibilityNodeInfo.CollectionItemInfo.columnSpan"), columnSpan);
}

//@CalledByNative
void BrowserAccessibilityManager::SetAccessibilityEventRangeInfo(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 rangeType,
    /* [in] */ Float min,
    /* [in] */ Float max,
    /* [in] */ Float current)
{
    // Backwards compatibility for KitKat AccessibilityNodeInfo fields.
    AutoPtr<IBundle> bundle = GetOrCreateBundleForAccessibilityEvent(event);
    bundle->PutInt32(String("AccessibilityNodeInfo.RangeInfo.type"), rangeType);
    bundle->PutFloat(String("AccessibilityNodeInfo.RangeInfo.min"), min);
    bundle->PutFloat(String("AccessibilityNodeInfo.RangeInfo.max"), max);
    bundle->PutFloat(String("AccessibilityNodeInfo.RangeInfo.current"), current);
}

Int32 BrowserAccessibilityManager::NativeGetRootId(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid)
{
    return Elastos_BrowserAccessibilityManager_nativeGetRootId(TO_IINTERFACE(this), nativeBrowserAccessibilityManagerAndroid);
}

Boolean BrowserAccessibilityManager::NativeIsNodeValid(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ Int32 id)
{
    return Elastos_BrowserAccessibilityManager_nativeIsNodeValid(TO_IINTERFACE(this), nativeBrowserAccessibilityManagerAndroid, id);
}

void BrowserAccessibilityManager::NativeHitTest(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Elastos_BrowserAccessibilityManager_nativeHitTest(TO_IINTERFACE(this), nativeBrowserAccessibilityManagerAndroid, x, y);
}

Boolean BrowserAccessibilityManager::NativePopulateAccessibilityNodeInfo(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ IAccessibilityNodeInfo* info,
    /* [in] */ Int32 id)
{
    return Elastos_BrowserAccessibilityManager_nativePopulateAccessibilityNodeInfo(
            TO_IINTERFACE(this), nativeBrowserAccessibilityManagerAndroid, TO_IINTERFACE(info), id);
}

Boolean BrowserAccessibilityManager::NativePopulateAccessibilityEvent(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 id,
    /* [in] */ Int32 eventType)
{
    return Elastos_BrowserAccessibilityManager_nativePopulateAccessibilityEvent(
            TO_IINTERFACE(this), nativeBrowserAccessibilityManagerAndroid, TO_IINTERFACE(event), id, eventType);
}

void BrowserAccessibilityManager::NativeClick(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ Int32 id)
{
    Elastos_BrowserAccessibilityManager_nativeClick(TO_IINTERFACE(this), nativeBrowserAccessibilityManagerAndroid, id);
}

void BrowserAccessibilityManager::NativeFocus(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ Int32 id)
{
    Elastos_BrowserAccessibilityManager_nativeFocus(TO_IINTERFACE(this), nativeBrowserAccessibilityManagerAndroid, id);
}

void BrowserAccessibilityManager::NativeBlur(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid)
{
    Elastos_BrowserAccessibilityManager_nativeBlur(TO_IINTERFACE(this), nativeBrowserAccessibilityManagerAndroid);
}

void BrowserAccessibilityManager::NativeScrollToMakeNodeVisible(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ Int32 id)
{
    Elastos_BrowserAccessibilityManager_nativeScrollToMakeNodeVisible(
            TO_IINTERFACE(this), nativeBrowserAccessibilityManagerAndroid, id);
}

Int32 BrowserAccessibilityManager::NativeFindElementType(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ Int32 startId,
    /* [in] */ const String& elementType,
    /* [in] */ Boolean forwards)
{
    return Elastos_BrowserAccessibilityManager_nativeFindElementType(
            TO_IINTERFACE(this), nativeBrowserAccessibilityManagerAndroid, startId, elementType, forwards);
}

AutoPtr<IInterface> BrowserAccessibilityManager::Create(
    /* [in] */ Handle64 nativeBrowserAccessibilityManagerAndroid,
    /* [in] */ IInterface* contentViewCore)
{
    AutoPtr<ContentViewCore> cvc = (ContentViewCore*)(IObject::Probe(contentViewCore));
    return TO_IINTERFACE(Create(nativeBrowserAccessibilityManagerAndroid, cvc));
}

void BrowserAccessibilityManager::OnNativeObjectDestroyed(
    /* [in] */ IInterface* obj)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::OnNativeObjectDestroyed, mObj is NULL");
        return;
    }
    mObj->OnNativeObjectDestroyed();
}

void BrowserAccessibilityManager::HandlePageLoaded(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 id)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::HandlePageLoaded, mObj is NULL");
        return;
    }
    mObj->HandlePageLoaded(id);
}

void BrowserAccessibilityManager::HandleFocusChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 id)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::HandleFocusChanged, mObj is NULL");
        return;
    }
    mObj->HandleFocusChanged(id);
}

void BrowserAccessibilityManager::HandleCheckStateChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 id)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::HandleCheckStateChanged, mObj is NULL");
        return;
    }
    mObj->HandleCheckStateChanged(id);
}

void BrowserAccessibilityManager::HandleTextSelectionChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 id)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::HandleTextSelectionChanged, mObj is NULL");
        return;
    }
    mObj->HandleTextSelectionChanged(id);
}

void BrowserAccessibilityManager::HandleEditableTextChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 id)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::HandleEditableTextChanged, mObj is NULL");
        return;
    }
    mObj->HandleEditableTextChanged(id);
}

void BrowserAccessibilityManager::HandleContentChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 id)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::HandleContentChanged, mObj is NULL");
        return;
    }
    mObj->HandleContentChanged(id);
}

void BrowserAccessibilityManager::HandleNavigate(
    /* [in] */ IInterface* obj)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::HandleNavigate, mObj is NULL");
        return;
    }
    mObj->HandleNavigate();
}

void BrowserAccessibilityManager::HandleScrollPositionChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 id)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::HandleScrollPositionChanged, mObj is NULL");
        return;
    }
    mObj->HandleScrollPositionChanged(id);
}

void BrowserAccessibilityManager::HandleScrolledToAnchor(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 id)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::HandleScrolledToAnchor, mObj is NULL");
        return;
    }
    mObj->HandleScrolledToAnchor(id);
}

void BrowserAccessibilityManager::HandleHover(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 id)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::HandleHover, mObj is NULL");
        return;
    }
    mObj->HandleHover(id);
}

void BrowserAccessibilityManager::AnnounceLiveRegionText(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& text)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::AnnounceLiveRegionText, mObj is NULL");
        return;
    }
    mObj->AnnounceLiveRegionText(text);
}

void BrowserAccessibilityManager::SetAccessibilityNodeInfoParent(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* node,
    /* [in] */ Int32 parentId)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityNodeInfoParent, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityNodeInfo> ani = IAccessibilityNodeInfo::Probe(node);
    mObj->SetAccessibilityNodeInfoParent(ani, parentId);
}

void BrowserAccessibilityManager::AddAccessibilityNodeInfoChild(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* node,
    /* [in] */ Int32 childId)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::AddAccessibilityNodeInfoChild, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityNodeInfo> ani = IAccessibilityNodeInfo::Probe(node);
    mObj->AddAccessibilityNodeInfoChild(ani, childId);
}

void BrowserAccessibilityManager::SetAccessibilityNodeInfoBooleanAttributes(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* node,
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Boolean checkable,
    /* [in] */ Boolean checked,
    /* [in] */ Boolean clickable,
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean focusable,
    /* [in] */ Boolean focused,
    /* [in] */ Boolean password,
    /* [in] */ Boolean scrollable,
    /* [in] */ Boolean selected,
    /* [in] */ Boolean visibleToUser)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityNodeInfoBooleanAttributes, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityNodeInfo> ani = IAccessibilityNodeInfo::Probe(node);
    mObj->SetAccessibilityNodeInfoBooleanAttributes(ani, virtualViewId, checkable, checked, clickable, enabled, focusable, focused, password, scrollable, selected, visibleToUser);
}

void BrowserAccessibilityManager::SetAccessibilityNodeInfoClassName(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* node,
    /* [in] */ const String& className)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityNodeInfoClassName, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityNodeInfo> ani = IAccessibilityNodeInfo::Probe(node);
    mObj->SetAccessibilityNodeInfoClassName(ani, className);
}

void BrowserAccessibilityManager::SetAccessibilityNodeInfoContentDescription(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* node,
    /* [in] */ const String& contentDescription,
    /* [in] */ Boolean annotateAsLink)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityNodeInfoContentDescription, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityNodeInfo> ani = IAccessibilityNodeInfo::Probe(node);
    mObj->SetAccessibilityNodeInfoContentDescription(ani, contentDescription, annotateAsLink);
}

void BrowserAccessibilityManager::SetAccessibilityNodeInfoLocation(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* node,
    /* [in] */ Int32 absoluteLeft,
    /* [in] */ Int32 absoluteTop,
    /* [in] */ Int32 parentRelativeLeft,
    /* [in] */ Int32 parentRelativeTop,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean isRootNode)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityNodeInfoLocation, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityNodeInfo> ani = IAccessibilityNodeInfo::Probe(node);
    mObj->SetAccessibilityNodeInfoLocation(ani, absoluteLeft, absoluteTop, parentRelativeLeft, parentRelativeTop, width, height, isRootNode);
}

void BrowserAccessibilityManager::SetAccessibilityNodeInfoKitKatAttributes(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* node,
    /* [in] */ Boolean canOpenPopup,
    /* [in] */ Boolean contentInvalid,
    /* [in] */ Boolean dismissable,
    /* [in] */ Boolean multiLine,
    /* [in] */ Int32 inputType,
    /* [in] */ Int32 liveRegion)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityNodeInfoKitKatAttributes, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityNodeInfo> ani = IAccessibilityNodeInfo::Probe(node);
    mObj->SetAccessibilityNodeInfoKitKatAttributes(ani, canOpenPopup, contentInvalid, dismissable, multiLine, inputType, liveRegion);
}

void BrowserAccessibilityManager::SetAccessibilityNodeInfoCollectionInfo(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* node,
    /* [in] */ Int32 rowCount,
    /* [in] */ Int32 columnCount,
    /* [in] */ Boolean hierarchical)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityNodeInfoCollectionInfo, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityNodeInfo> ani = IAccessibilityNodeInfo::Probe(node);
    mObj->SetAccessibilityNodeInfoCollectionInfo(ani, rowCount, columnCount, hierarchical);
}

void BrowserAccessibilityManager::SetAccessibilityNodeInfoCollectionItemInfo(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* node,
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 rowSpan,
    /* [in] */ Int32 columnIndex,
    /* [in] */ Int32 columnSpan,
    /* [in] */ Boolean heading)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityNodeInfoCollectionItemInfo, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityNodeInfo> ani = IAccessibilityNodeInfo::Probe(node);
    mObj->SetAccessibilityNodeInfoCollectionItemInfo(ani, rowIndex, rowSpan, columnIndex, columnSpan, heading);
}

void BrowserAccessibilityManager::SetAccessibilityNodeInfoRangeInfo(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* node,
    /* [in] */ Int32 rangeType,
    /* [in] */ Float min,
    /* [in] */ Float max,
    /* [in] */ Float current)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityNodeInfoRangeInfo, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityNodeInfo> ani = IAccessibilityNodeInfo::Probe(node);
    mObj->SetAccessibilityNodeInfoRangeInfo(ani, rangeType, min, max, current);
}

void BrowserAccessibilityManager::SetAccessibilityEventBooleanAttributes(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ Boolean checked,
    /* [in] */ Boolean enabled,
    /* [in] */ Boolean password,
    /* [in] */ Boolean scrollable)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventBooleanAttributes, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventBooleanAttributes(ae, checked, enabled, password, scrollable);
}

void BrowserAccessibilityManager::SetAccessibilityEventClassName(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ const String& className)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventClassName, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventClassName(ae, className);
}

void BrowserAccessibilityManager::SetAccessibilityEventListAttributes(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ Int32 currentItemIndex,
    /* [in] */ Int32 itemCount)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventListAttributes, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventListAttributes(ae, currentItemIndex, itemCount);
}

void BrowserAccessibilityManager::SetAccessibilityEventScrollAttributes(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Int32 maxScrollX,
    /* [in] */ Int32 maxScrollY)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventScrollAttributes, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventScrollAttributes(ae, scrollX, scrollY, maxScrollX, maxScrollY);
}

void BrowserAccessibilityManager::SetAccessibilityEventTextChangedAttrs(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 addedCount,
    /* [in] */ Int32 removedCount,
    /* [in] */ const String& beforeText,
    /* [in] */ const String& text)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventTextChangedAttrs, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventTextChangedAttrs(ae, fromIndex, addedCount, removedCount, beforeText, text);
}

void BrowserAccessibilityManager::SetAccessibilityEventSelectionAttrs(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 addedCount,
    /* [in] */ Int32 itemCount,
    /* [in] */ const String& text)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventSelectionAttrs, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventSelectionAttrs(ae, fromIndex, addedCount, itemCount, text);
}

void BrowserAccessibilityManager::SetAccessibilityEventKitKatAttributes(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ Boolean canOpenPopup,
    /* [in] */ Boolean contentInvalid,
    /* [in] */ Boolean dismissable,
    /* [in] */ Boolean multiLine,
    /* [in] */ Int32 inputType,
    /* [in] */ Int32 liveRegion)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventKitKatAttributes, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventKitKatAttributes(ae, canOpenPopup, contentInvalid, dismissable, multiLine, inputType, liveRegion);
}

void BrowserAccessibilityManager::SetAccessibilityEventCollectionInfo(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ Int32 rowCount,
    /* [in] */ Int32 columnCount,
    /* [in] */ Boolean hierarchical)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventCollectionInfo, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventCollectionInfo(ae, rowCount, columnCount, hierarchical);
}

void BrowserAccessibilityManager::SetAccessibilityEventHeadingFlag(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ Boolean heading)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventHeadingFlag, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventHeadingFlag(ae, heading);
}

void BrowserAccessibilityManager::SetAccessibilityEventCollectionItemInfo(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 rowSpan,
    /* [in] */ Int32 columnIndex,
    /* [in] */ Int32 columnSpan)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventCollectionItemInfo, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventCollectionItemInfo(ae, rowIndex, rowSpan, columnIndex, columnSpan);
}

void BrowserAccessibilityManager::SetAccessibilityEventRangeInfo(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* event,
    /* [in] */ Int32 rangeType,
    /* [in] */ Float min,
    /* [in] */ Float max,
    /* [in] */ Float current)
{
    AutoPtr<BrowserAccessibilityManager> mObj = (BrowserAccessibilityManager*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "BrowserAccessibilityManager::SetAccessibilityEventRangeInfo, mObj is NULL");
        return;
    }
    AutoPtr<IAccessibilityEvent> ae = IAccessibilityEvent::Probe(event);
    mObj->SetAccessibilityEventRangeInfo(ae, rangeType, min, max, current);
}

} // namespace Accessibility
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
