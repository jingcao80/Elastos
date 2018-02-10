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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACCESSIBILITY_BROWSERACCESSIBILITYMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACCESSIBILITY_BROWSERACCESSIBILITYMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IList;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::Style::IURLSpan;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.content.browser.ContentViewCore;
// import org.chromium.content.browser.RenderCoordinates;

// import java.util.ArrayList;
// import java.util.List;
// import java.util.Locale;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class ContentViewCore;
class RenderCoordinates;

namespace Accessibility {

/**
 * Native accessibility for a {@link ContentViewCore}.
 *
 * This class is safe to load on ICS and can be used to run tests, but
 * only the subclass, JellyBeanBrowserAccessibilityManager, actually
 * has a AccessibilityNodeProvider implementation needed for native
 * accessibility.
 */
//@JNINamespace("content")
class BrowserAccessibilityManager
    : public Object
{
public:
    /**
     * @return An AccessibilityNodeProvider on JellyBean, and null on previous versions.
     */
    CARAPI_(AutoPtr<IAccessibilityNodeProvider>) GetAccessibilityNodeProvider();

    /**
     * @see View#onHoverEvent(MotionEvent)
     */
    CARAPI_(Boolean) OnHoverEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * Called by ContentViewCore to notify us when the frame info is initialized,
     * the first time, since until that point, we can't use mRenderCoordinates to transform
     * web coordinates to screen coordinates.
     */
    CARAPI_(void) NotifyFrameInfoInitialized();

    static CARAPI_(void*) ElaBrowserAccessibilityManagerCallback_Init();

protected:
    BrowserAccessibilityManager(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ ContentViewCore* contentViewCore);

        /**
     * @see AccessibilityNodeProvider#createAccessibilityNodeInfo(int)
     */
    virtual CARAPI_(AutoPtr<IAccessibilityNodeInfo>) CreateAccessibilityNodeInfo(
        /* [in] */ Int32 virtualViewId);

    /**
     * @see AccessibilityNodeProvider#findAccessibilityNodeInfosByText(String, int)
     */
    virtual CARAPI_(AutoPtr<IList>) FindAccessibilityNodeInfosByText(
        /* [in] */ const String& text,
        /* [in] */ Int32 virtualViewId);

    /**
     * @see AccessibilityNodeProvider#performAction(int, int, Bundle)
     */
    virtual CARAPI_(Boolean)  PerformAction(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

    //@CalledByNative
    virtual CARAPI_(void)  SetAccessibilityNodeInfoKitKatAttributes(
        /* [in] */ IAccessibilityNodeInfo* node,
        /* [in] */ Boolean canOpenPopup,
        /* [in] */ Boolean contentInvalid,
        /* [in] */ Boolean dismissable,
        /* [in] */ Boolean multiLine,
        /* [in] */ Int32 inputType,
        /* [in] */ Int32 liveRegion);

    //@CalledByNative
    virtual CARAPI_(void)  SetAccessibilityNodeInfoCollectionInfo(
        /* [in] */ IAccessibilityNodeInfo* node,
        /* [in] */ Int32 rowCount,
        /* [in] */ Int32 columnCount,
        /* [in] */ Boolean hierarchical);

    //@CalledByNative
    virtual CARAPI_(void)  SetAccessibilityNodeInfoCollectionItemInfo(
        /* [in] */ IAccessibilityNodeInfo* node,
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 rowSpan,
        /* [in] */ Int32 columnIndex,
        /* [in] */ Int32 columnSpan,
        /* [in] */ Boolean heading);

    //@CalledByNative
    virtual CARAPI_(void)  SetAccessibilityNodeInfoRangeInfo(
        /* [in] */ IAccessibilityNodeInfo* node,
        /* [in] */ Int32 rangeType,
        /* [in] */ Float min,
        /* [in] */ Float max,
        /* [in] */ Float current);

    //@CalledByNative
    virtual CARAPI_(void)  SetAccessibilityEventKitKatAttributes(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Boolean canOpenPopup,
        /* [in] */ Boolean contentInvalid,
        /* [in] */ Boolean dismissable,
        /* [in] */ Boolean multiLine,
        /* [in] */ Int32 inputType,
        /* [in] */ Int32 liveRegion);

    //@CalledByNative
    virtual CARAPI_(void)  SetAccessibilityEventCollectionInfo(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 rowCount,
        /* [in] */ Int32 columnCount,
        /* [in] */ Boolean hierarchical);

    //@CalledByNative
    virtual CARAPI_(void)  SetAccessibilityEventHeadingFlag(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Boolean heading);

    //@CalledByNative
    virtual CARAPI_(void)  SetAccessibilityEventCollectionItemInfo(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 rowSpan,
        /* [in] */ Int32 columnIndex,
        /* [in] */ Int32 columnSpan);

    //@CalledByNative
    virtual CARAPI_(void)  SetAccessibilityEventRangeInfo(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 rangeType,
        /* [in] */ Float min,
        /* [in] */ Float max,
        /* [in] */ Float current);


private:
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ IInterface* contentViewCore);

    static CARAPI_(void) OnNativeObjectDestroyed(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) HandlePageLoaded(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleFocusChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleCheckStateChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleTextSelectionChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleEditableTextChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleContentChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleNavigate(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) HandleScrollPositionChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleScrolledToAnchor(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 id);

    static CARAPI_(void) HandleHover(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 id);

    static CARAPI_(void) AnnounceLiveRegionText(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& text);

    static CARAPI_(void) SetAccessibilityNodeInfoParent(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* node,
        /* [in] */ Int32 parentId);

    static CARAPI_(void) AddAccessibilityNodeInfoChild(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* node,
        /* [in] */ Int32 childId);

    static CARAPI_(void) SetAccessibilityNodeInfoBooleanAttributes(
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
        /* [in] */ Boolean visibleToUser);

    static CARAPI_(void) SetAccessibilityNodeInfoClassName(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* node,
        /* [in] */ const String& className);

    static CARAPI_(void) SetAccessibilityNodeInfoContentDescription(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* node,
        /* [in] */ const String& contentDescription,
        /* [in] */ Boolean annotateAsLink);

    static CARAPI_(void) SetAccessibilityNodeInfoLocation(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* node,
        /* [in] */ Int32 absoluteLeft,
        /* [in] */ Int32 absoluteTop,
        /* [in] */ Int32 parentRelativeLeft,
        /* [in] */ Int32 parentRelativeTop,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean isRootNode);

    static CARAPI_(void) SetAccessibilityNodeInfoKitKatAttributes(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* node,
        /* [in] */ Boolean canOpenPopup,
        /* [in] */ Boolean contentInvalid,
        /* [in] */ Boolean dismissable,
        /* [in] */ Boolean multiLine,
        /* [in] */ Int32 inputType,
        /* [in] */ Int32 liveRegion);

    static CARAPI_(void) SetAccessibilityNodeInfoCollectionInfo(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* node,
        /* [in] */ Int32 rowCount,
        /* [in] */ Int32 columnCount,
        /* [in] */ Boolean hierarchical);

    static CARAPI_(void) SetAccessibilityNodeInfoCollectionItemInfo(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* node,
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 rowSpan,
        /* [in] */ Int32 columnIndex,
        /* [in] */ Int32 columnSpan,
        /* [in] */ Boolean heading);

    static CARAPI_(void) SetAccessibilityNodeInfoRangeInfo(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* node,
        /* [in] */ Int32 rangeType,
        /* [in] */ Float min,
        /* [in] */ Float max,
        /* [in] */ Float current);

    static CARAPI_(void) SetAccessibilityEventBooleanAttributes(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ Boolean checked,
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean password,
        /* [in] */ Boolean scrollable);

    static CARAPI_(void) SetAccessibilityEventClassName(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ const String& className);

    static CARAPI_(void) SetAccessibilityEventListAttributes(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ Int32 currentItemIndex,
        /* [in] */ Int32 itemCount);

    static CARAPI_(void) SetAccessibilityEventScrollAttributes(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Int32 maxScrollX,
        /* [in] */ Int32 maxScrollY);

    static CARAPI_(void) SetAccessibilityEventTextChangedAttrs(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 addedCount,
        /* [in] */ Int32 removedCount,
        /* [in] */ const String& beforeText,
        /* [in] */ const String& text);

    static CARAPI_(void) SetAccessibilityEventSelectionAttrs(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 addedCount,
        /* [in] */ Int32 itemCount,
        /* [in] */ const String& text);

    static CARAPI_(void) SetAccessibilityEventKitKatAttributes(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ Boolean canOpenPopup,
        /* [in] */ Boolean contentInvalid,
        /* [in] */ Boolean dismissable,
        /* [in] */ Boolean multiLine,
        /* [in] */ Int32 inputType,
        /* [in] */ Int32 liveRegion);

    static CARAPI_(void) SetAccessibilityEventCollectionInfo(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ Int32 rowCount,
        /* [in] */ Int32 columnCount,
        /* [in] */ Boolean hierarchical);

    static CARAPI_(void) SetAccessibilityEventHeadingFlag(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ Boolean heading);

    static CARAPI_(void) SetAccessibilityEventCollectionItemInfo(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 rowSpan,
        /* [in] */ Int32 columnIndex,
        /* [in] */ Int32 columnSpan);

    static CARAPI_(void) SetAccessibilityEventRangeInfo(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* event,
        /* [in] */ Int32 rangeType,
        /* [in] */ Float min,
        /* [in] */ Float max,
        /* [in] */ Float current);

    /**
     * Create a BrowserAccessibilityManager object, which is owned by the C++
     * BrowserAccessibilityManagerAndroid instance, and connects to the content view.
     * @param nativeBrowserAccessibilityManagerAndroid A pointer to the counterpart native
     *     C++ object that owns this object.
     * @param contentViewCore The content view that this object provides accessibility for.
     */
    //@CalledByNative
    static CARAPI_(AutoPtr<BrowserAccessibilityManager>) Create(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ ContentViewCore* contentViewCore);

    //@CalledByNative
    CARAPI_(void) OnNativeObjectDestroyed();

    CARAPI_(Boolean)  JumpToElementType(
        /* [in] */ const String& elementType,
        /* [in] */ Boolean forwards);

    CARAPI_(void) SendAccessibilityEvent(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ Int32 eventType);

    CARAPI_(AutoPtr<IBundle>) GetOrCreateBundleForAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI_(AutoPtr<IAccessibilityNodeInfo>) CreateNodeForHost(
        /* [in] */ Int32 rootId);

    CARAPI_(Boolean)  IsFrameInfoInitialized();

    //@CalledByNative
    CARAPI_(void) HandlePageLoaded(
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleFocusChanged(
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleCheckStateChanged(
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleTextSelectionChanged(
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleEditableTextChanged(
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleContentChanged(
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleNavigate();

    //@CalledByNative
    CARAPI_(void) HandleScrollPositionChanged(
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleScrolledToAnchor(
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) HandleHover(
        /* [in] */ Int32 id);

    //@CalledByNative
    CARAPI_(void) AnnounceLiveRegionText(
        /* [in] */ const String& text);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityNodeInfoParent(
        /* [in] */ IAccessibilityNodeInfo* node,
        /* [in] */ Int32 parentId);

    //@CalledByNative
    CARAPI_(void) AddAccessibilityNodeInfoChild(
        /* [in] */ IAccessibilityNodeInfo* node,
        /* [in] */ Int32 childId);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityNodeInfoBooleanAttributes(
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
        /* [in] */ Boolean visibleToUser);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityNodeInfoClassName(
        /* [in] */ IAccessibilityNodeInfo* node,
        /* [in] */ const String& className);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityNodeInfoContentDescription(
        /* [in] */ IAccessibilityNodeInfo* node,
        /* [in] */ const String& contentDescription,
        /* [in] */ Boolean annotateAsLink);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityNodeInfoLocation(
        /* [in] */ IAccessibilityNodeInfo* node,
        /* [in] */ Int32 absoluteLeft,
        /* [in] */ Int32 absoluteTop,
        /* [in] */ Int32 parentRelativeLeft,
        /* [in] */ Int32 parentRelativeTop,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean isRootNode);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityEventBooleanAttributes(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Boolean checked,
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean password,
        /* [in] */ Boolean scrollable);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityEventClassName(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ const String& className);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityEventListAttributes(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 currentItemIndex,
        /* [in] */ Int32 itemCount);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityEventScrollAttributes(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Int32 maxScrollX,
        /* [in] */ Int32 maxScrollY);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityEventTextChangedAttrs(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 addedCount,
        /* [in] */ Int32 removedCount,
        /* [in] */ const String& beforeText,
        /* [in] */ const String& text);

    //@CalledByNative
    CARAPI_(void) SetAccessibilityEventSelectionAttrs(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 fromIndex,
        /* [in] */ Int32 addedCount,
        /* [in] */ Int32 itemCount,
        /* [in] */ const String& text);

    CARAPI_(Int32) NativeGetRootId(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid);

    CARAPI_(Boolean) NativeIsNodeValid(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ Int32 id);

    CARAPI_(void) NativeHitTest(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(Boolean) NativePopulateAccessibilityNodeInfo(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ IAccessibilityNodeInfo* info,
        /* [in] */ Int32 id);

    CARAPI_(Boolean) NativePopulateAccessibilityEvent(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 id,
        /* [in] */ Int32 eventType);

    CARAPI_(void) NativeClick(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ Int32 id);

    CARAPI_(void) NativeFocus(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ Int32 id);

    CARAPI_(void) NativeBlur(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid);

    CARAPI_(void) NativeScrollToMakeNodeVisible(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ Int32 id);

    CARAPI_(Int32) NativeFindElementType(
        /* [in] */ HANDLE nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ Int32 startId,
        /* [in] */ const String& elementType,
        /* [in] */ Boolean forwards);

private:
    static const String TAG;

    AutoPtr<ContentViewCore> mContentViewCore;
    AutoPtr<IAccessibilityManager> mAccessibilityManager;
    AutoPtr<RenderCoordinates> mRenderCoordinates;
    HANDLE mNativeObj;
    Int32 mAccessibilityFocusId;
    Boolean mIsHovering;
    Int32 mLastHoverId;
    Int32 mCurrentRootId;
    AutoPtr< ArrayOf<Int32> > mTempLocation;
    AutoPtr<IViewGroup> mView;
    Boolean mUserHasTouchExplored;
    Boolean mPendingScrollToMakeNodeVisible;
};

} // namespace Accessibility
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACCESSIBILITY_BROWSERACCESSIBILITYMANAGER_H__
