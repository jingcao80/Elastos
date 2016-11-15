
#ifndef __GECKOSURFACEVIEW_H__
#define __GECKOSURFACEVIEW_H__

#include "_CGeckoSurfaceView.h"
//#include "ReentrantLock.h"
#include "CGeckoInputConnection.h"
//#include <elastos/ElRefBase.h>

using Elastos::IO::IBuffer;
using Elastos::Utility::IObjectInt32Map;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::View::IViewOnCreateContextMenuListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IDisplayList;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IContextMenu;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ITouchDelegate;
using Elastos::Droid::View::IDragEvent;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IDragShadowBuilder;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::ITouchDelegate;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IAccessibilityDelegate;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::ISurfaceHolderCallback;
using Elastos::Droid::View::IViewOnDragListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnHoverListener;
using Elastos::Droid::View::ITextSegmentIterator;
using Elastos::Droid::View::IViewOnFocusChangeListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Droid::View::IViewOnGenericMotionListener;
using Elastos::Droid::View::IViewOnAttachStateChangeListener;
using Elastos::Droid::View::IViewOnSystemUiVisibilityChangeListener;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::IEditableFactory;
using Elastos::Droid::Graphics::IInsets;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::Utility::IPredicate;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

CarClass(CGeckoSurfaceView)
{
public:
    class MySurfaceHoderCallback
        : public ElRefBase
        , public ISurfaceHolderCallback
    {
    public:
        MySurfaceHoderCallback(
            /* [in] */ CGeckoSurfaceView* host);

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface* pObject,
            /* [in] */ InterfaceID* pIID);

        CARAPI SurfaceCreated(
            /* [in] */ ISurfaceHolder* holder);

        CARAPI SurfaceChanged(
            /* [in] */ ISurfaceHolder* holder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SurfaceDestroyed(
            /* [in] */ ISurfaceHolder* holder);

    private:
        CGeckoSurfaceView* mHost;
    };

    class MyTouchDelegate
        : public ElRefBase
        , public ITouchDelegate
    {
    public:
        MyTouchDelegate(
            /* [in] */ CGeckoSurfaceView* host);

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface* pObject,
            /* [in] */ InterfaceID* pIID);

        CARAPI OnTouchEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        CGeckoSurfaceView* mHost;
    };

public:
    CGeckoSurfaceView();

    ~CGeckoSurfaceView();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    ECode Draw(
        /* [in] */ ISurfaceHolder* pHolder,
        /* [in] */ IBuffer* pBuffer);

    ECode Draw(
        /* [in] */ ISurfaceHolder* pHolder,
        /* [in] */ IBitmap* pBitmap);

    ECode SetEditable(
        /* [in] */ ICharSequence* pContents);

    ECode InitEditable(
        /* [in] */ ICharSequence* pContents);

    ECode GetSoftwareDrawBitmap(
        /* [out] */IBitmap** ppBitmap);

    ECode GetSoftwareDrawBuffer(
        /* [out] */ IBuffer** ppBuffer);

    ECode GetSurface(
        /* [out] */ ISurface** ppSurface);

    ECode BeginDrawing(
        /* [out] */ Int32* pResult);

    ECode EndDrawing();

    ECode Draw2D(
        /* [in] */ IBitmap* pBitmap,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    ECode Draw2D(
        /* [in] */ IBuffer* pBuffer,
        /* [in] */ Int32 stride);

    ECode DrawSplashScreen();

public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* pEvent,
        /* [out] */ Boolean* result);

    CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* pEvent,
        /* [out] */ Boolean* pResult);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* pEvent,
        /* [out] */ Boolean* pResult);

    CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* pEvent,
        /* [out] */ Boolean* pResult);

    CARAPI GetVerticalFadingEdgeLength(
        /* [out] */ Int32 * pLength);

    CARAPI SetFadingEdgeLength(
        /* [in] */ Int32 length);

    CARAPI GetHorizontalFadingEdgeLength(
        /* [out] */ Int32 * pLength);

    CARAPI GetVerticalScrollbarWidth(
        /* [out] */ Int32 * pWidth);

    CARAPI SetVerticalScrollbarPosition(
        /* [in] */ Int32 position);

    CARAPI GetVerticalScrollbarPosition(
        /* [out] */ Int32* position);

    CARAPI GetHorizontalScrollbarHeight(
        /* [out] */ Int32 * pHeight);

    CARAPI SetOnFocusChangeListener(
        /* [in] */ IViewOnFocusChangeListener * pL);

    CARAPI AddOnLayoutChangeListener(
        /* [in] */ IViewOnLayoutChangeListener* listener);

    CARAPI RemoveOnLayoutChangeListener(
        /* [in] */ IViewOnLayoutChangeListener* listener);

    CARAPI AddOnAttachStateChangeListener(
        /* [in] */ IViewOnAttachStateChangeListener* listener);

    CARAPI RemoveOnAttachStateChangeListener(
        /* [in] */ IViewOnAttachStateChangeListener* listener);

    CARAPI GetOnFocusChangeListener(
        /* [out] */ IViewOnFocusChangeListener ** ppL);

    CARAPI SetOnClickListener(
        /* [in] */ IViewOnClickListener * pL);

    CARAPI HasOnClickListeners(
        /* [out] */ Boolean* has);

    CARAPI SetOnLongClickListener(
        /* [in] */ IViewOnLongClickListener * pL);

    CARAPI SetOnCreateContextMenuListener(
        /* [in] */ IViewOnCreateContextMenuListener * pL);

    CARAPI PerformClick(
        /* [out] */ Boolean * pResult);

    CARAPI CallOnClick(
        /* [out] */ Boolean* result);

    CARAPI PerformLongClick(
        /* [out] */ Boolean * pResult);

    CARAPI ShowContextMenu(
        /* [out] */ Boolean * pResult);

    CARAPI ShowContextMenu(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 metaState,
        /* [out] */ Boolean* result);

    CARAPI StartActionMode(
        /* [in] */ IActionModeCallback* cback,
        /* [out] */ IActionMode** mode);

    CARAPI SetOnKeyListener(
        /* [in] */ IViewOnKeyListener * pL);

    CARAPI SetOnTouchListener(
        /* [in] */ IViewOnTouchListener * pL);

    CARAPI SetOnGenericMotionListener(
        /* [in] */ IViewOnGenericMotionListener* l);

    CARAPI SetOnHoverListener(
        /* [in] */ IViewOnHoverListener* l);

    CARAPI SetOnDragListener(
        /* [in] */ IViewOnDragListener* l);

    CARAPI RequestRectangleOnScreen(
        /* [in] */ IRect * pRectangle,
        /* [out] */ Boolean * pResult);

    CARAPI RequestRectangleOnScreen(
        /* [in] */ IRect * pRectangle,
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean * pResult);

    CARAPI ClearFocus();

    CARAPI HasFocus(
        /* [out] */ Boolean * pHasFocus);

    CARAPI HasFocusable(
        /* [out] */ Boolean * pHasFocusable);

    CARAPI AnnounceForAccessibility(
        /* [in] */ ICharSequence* text);

    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI CreateAccessibilityNodeInfo(
        /* [out] */ IAccessibilityNodeInfo** info);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI GetAccessibilityDelegate(
        /* [out] */ IAccessibilityDelegate** accessibilityDelegate);

    CARAPI SetAccessibilityDelegate(
        /* [in] */ IAccessibilityDelegate* accessibilityDelegate);

    CARAPI GetAccessibilityNodeProvider(
        /* [out] */ IAccessibilityNodeProvider** provider);

    CARAPI GetAccessibilityViewId(
        /* [out] */ Int32* id);

    CARAPI GetAccessibilityWindowId(
        /* [out] */ Int32* id);

    CARAPI GetLabelFor(
        /* [out] */ Int32* id);

    CARAPI SetLabelFor(
        /* [in] */ Int32 id);

    CARAPI GetContentDescription(
        /* [out, callee] */ ICharSequence ** ppDescription);

    CARAPI SetContentDescription(
        /* [in] */ ICharSequence* contentDescription);

    CARAPI IsFocused(
        /* [out] */ Boolean * pFocused);

    CARAPI FindFocus(
        /* [out] */ IView ** ppView);

    CARAPI IsScrollContainer(
        /* [out] */ Boolean* result);

    CARAPI SetScrollContainer(
        /* [in] */ Boolean isScrollContainer);

    CARAPI GetDrawingCacheQuality(
        /* [out] */ Int32 * pQuality);

    CARAPI SetDrawingCacheQuality(
        /* [in] */ Int32 quality);

    CARAPI GetKeepScreenOn(
        /* [out] */ Boolean * pResult);

    CARAPI SetKeepScreenOn(
        /* [in] */ Boolean keepScreenOn);

    CARAPI GetNextFocusLeftId(
        /* [out] */ Int32 * pNextFocusLeftId);

    CARAPI SetNextFocusLeftId(
        /* [in] */ Int32 nextFocusLeftId);

    CARAPI GetNextFocusRightId(
        /* [out] */ Int32 * pNextFocusRightId);

    CARAPI SetNextFocusRightId(
        /* [in] */ Int32 nextFocusRightId);

    CARAPI GetNextFocusUpId(
        /* [out] */ Int32 * pNextFocusUpId);

    CARAPI SetNextFocusUpId(
        /* [in] */ Int32 nextFocusUpId);

    CARAPI GetNextFocusDownId(
        /* [out] */ Int32 * pNextFocusDownId);

    CARAPI SetNextFocusDownId(
        /* [in] */ Int32 nextFocusDownId);

    CARAPI GetNextFocusForwardId(
        /* [out] */ Int32* id);

    CARAPI SetNextFocusForwardId(
        /* [in] */ Int32 nextFocusForwardId);

    CARAPI IsShown(
        /* [out] */ Boolean * pShown);

    CARAPI SetFitsSystemWindows(
        /* [in] */ Boolean fitSystemWindows);

    CARAPI GetFitsSystemWindows(
        /* [out] */ Boolean* result);

    CARAPI FitsSystemWindows(
        /* [out] */ Boolean* result);

    CARAPI RequestFitSystemWindows();

    CARAPI MakeOptionalFitsSystemWindows();

    CARAPI GetVisibility(
        /* [out] */ Int32 * pVisibility);

    CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    CARAPI IsEnabled(
        /* [out] */ Boolean * pEnabled);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI SetFocusable(
        /* [in] */ Boolean focusable);

    CARAPI SetFocusableInTouchMode(
        /* [in] */ Boolean focusableInTouchMode);

    CARAPI SetSoundEffectsEnabled(
        /* [in] */ Boolean soundEffectsEnabled);

    CARAPI IsSoundEffectsEnabled(
        /* [out] */ Boolean * pSoundEffectsEnabled);

    CARAPI SetHapticFeedbackEnabled(
        /* [in] */ Boolean hapticFeedbackEnabled);

    CARAPI IsHapticFeedbackEnabled(
        /* [out] */ Boolean * pHapticFeedbackEnabled);

    CARAPI GetRawLayoutDirection(
        /* [out] */ Int32* direction);

    CARAPI SetLayoutDirection(
        /* [in] */ Int32 layoutDirection);

    CARAPI GetLayoutDirection(
        /* [out] */ Int32* direction);

    CARAPI IsLayoutRtl(
        /* [out] */ Boolean* rtl);

    CARAPI HasTransientState(
        /* [out] */ Boolean* has);

    CARAPI SetHasTransientState(
        /* [in] */ Boolean hasTransientState);

    CARAPI SetWillNotDraw(
        /* [in] */ Boolean willNotDraw);

    CARAPI WillNotDraw(
        /* [out] */ Boolean * pWillNotDraw);

    CARAPI SetWillNotCacheDrawing(
        /* [in] */ Boolean willNotCacheDrawing);

    CARAPI WillNotCacheDrawing(
        /* [out] */ Boolean * pCache);

    CARAPI IsClickable(
        /* [out] */ Boolean * pClickable);

    CARAPI SetClickable(
        /* [in] */ Boolean clickable);

    CARAPI IsLongClickable(
        /* [out] */ Boolean * pLongClickable);

    CARAPI SetLongClickable(
        /* [in] */ Boolean longClickable);

    CARAPI SetPressed(
        /* [in] */ Boolean pressed);

    CARAPI IsPressed(
        /* [out] */ Boolean * pPressed);

    CARAPI IsSaveEnabled(
        /* [out] */ Boolean * pEnabled);

    CARAPI SetSaveEnabled(
        /* [in] */ Boolean enabled);

    CARAPI GetFilterTouchesWhenObscured(
        /* [out] */ Boolean * pEnabled);

    CARAPI SetFilterTouchesWhenObscured(
        /* [in] */ Boolean enabled);

    CARAPI IsSaveFromParentEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetSaveFromParentEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsFocusable(
        /* [out] */ Boolean * pFocusable);

    CARAPI IsFocusableInTouchMode(
        /* [out] */ Boolean * pFocusable);

    CARAPI FocusSearch(
        /* [in] */ Int32 direction,
        /* [out] */ IView ** ppView);

    CARAPI DispatchUnhandledMove(
        /* [in] */ IView * pFocused,
        /* [in] */ Int32 direction,
        /* [out] */ Boolean * pResult);

    CARAPI GetFocusables(
        /* [in] */ Int32 direction,
        /* [out] */ IObjectContainer ** ppViews);

    CARAPI AddFocusables(
        /* [in] */ IObjectContainer * pViews,
        /* [in] */ Int32 direction);

    CARAPI AddFocusables(
        /* [in] */ IObjectContainer * pViews,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 focusableMode);

    CARAPI FindViewsWithText(
        /* [in] */ IObjectContainer* outViews,
        /* [in] */ ICharSequence* searched,
        /* [in] */ Int32 flags);

    CARAPI GetTouchables(
        /* [out] */ IObjectContainer ** ppViews);

    CARAPI AddTouchables(
        /* [in] */ IObjectContainer * pViews);

    CARAPI RequestAccessibilityFocus(
        /* [out] */ Boolean* focus);

    CARAPI ClearAccessibilityFocus();

    CARAPI RequestFocus(
        /* [out] */ Boolean * pResult);

    CARAPI RequestFocus(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean * pResult);

    CARAPI RequestFocus(
        /* [in] */ Int32 direction,
        /* [in] */ IRect * pPreviouslyFocusedRect,
        /* [out] */ Boolean * pResult);

    CARAPI RequestFocusFromTouch(
        /* [out] */ Boolean * pResult);

    CARAPI GetImportantForAccessibility(
        /* [out] */ Int32* mode);

    CARAPI SetImportantForAccessibility(
        /* [in] */ Int32 mode);

    CARAPI IsImportantForAccessibility(
        /* [out] */ Boolean* exposed);

    CARAPI GetParentForAccessibility(
        /* [out] */ IViewParent** parent);

    CARAPI AddChildrenForAccessibility(
        /* [in] */ IObjectContainer* children);

    CARAPI IncludeForAccessibility(
        /* [out] */ Boolean* include);

    CARAPI IsActionableForAccessibility(
        /* [out] */ Boolean* actionable);

    CARAPI NotifyAccessibilityStateChanged();

    CARAPI ResetAccessibilityStateChanged();

    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* performed);

    CARAPI GetIterableTextForAccessibility(
        /* [out] */ ICharSequence** text);

    CARAPI GetAccessibilityCursorPosition(
        /* [out] */ Int32* pos);

    CARAPI SetAccessibilityCursorPosition(
        /* [in] */ Int32 position);

    CARAPI GetIteratorForGranularity(
        /* [in] */ Int32 granularity,
        /* [out] */ ITextSegmentIterator** iterator);

    CARAPI DispatchStartTemporaryDetach();

    CARAPI OnStartTemporaryDetach();

    CARAPI DispatchFinishTemporaryDetach();

    CARAPI OnFinishTemporaryDetach();

    CARAPI GetKeyDispatcherState(
        /* [out] */ IDispatcherState ** ppState);

    CARAPI DispatchKeyEventPreIme(
        /* [in] */ IKeyEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI DispatchKeyShortcutEvent(
        /* [in] */ IKeyEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI OnFilterTouchEventForSecurity(
        /* [in] */ IMotionEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI DispatchTrackballEvent(
        /* [in] */ IMotionEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI DispatchGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* handled);

    CARAPI DispatchPointerEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* handled);

    CARAPI DispatchWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    CARAPI HasWindowFocus(
        /* [out] */ Boolean * pFocus);

    CARAPI DispatchDisplayHint(
        /* [in] */ Int32 hint);

    CARAPI DispatchWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    CARAPI GetWindowVisibility(
        /* [out] */ Int32 * pVisibility);

    CARAPI GetWindowVisibleDisplayFrame(
        /* [in] */ IRect * pOutRect);

    CARAPI DispatchConfigurationChanged(
        /* [in] */ IConfiguration * pNewConfig);

    CARAPI IsInTouchMode(
        /* [out] */ Boolean * pResult);

    CARAPI GetContext(
        /* [out] */ IContext ** ppCtx);

    CARAPI OnKeyPreIme(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI OnKeyShortcut(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI OnCheckIsTextEditor(
        /* [out] */ Boolean * pResult);

    CARAPI OnCreateInputConnection(
        /* [in] */ IEditorInfo * pOutAttrs,
        /* [out] */ IInputConnection ** ppInputConnection);

    CARAPI CheckInputConnectionProxy(
        /* [in] */ IView * pView,
        /* [out] */ Boolean * pResult);

    CARAPI CreateContextMenu(
        /* [in] */ IContextMenu * pMenu);

    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* handled);

    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* handled);

    CARAPI IsHovered(
        /* [out] */ Boolean* hovered);

    CARAPI SetHovered(
        /* [in] */ Boolean hovered);

    CARAPI OnHoverChanged(
        /* [in] */ Boolean hovered);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent * pEvent,
        /* [out] */ Boolean * pResult);

    CARAPI IsInScrollingContainer(
        /* [out] */ Boolean* result);

    CARAPI SetTouchDelegate(
        /* [in] */ ITouchDelegate * pTouchDelegate);

    CARAPI GetTouchDelegate(
        /* [out] */ ITouchDelegate ** ppTouchDelegate);

    CARAPI CancelLongPress();

    CARAPI BringToFront();

    CARAPI GetParent(
        /* [out] */ IViewParent ** ppParent);

    CARAPI SetScrollX(
        /* [in] */ Int32 value);

    CARAPI SetScrollY(
        /* [in] */ Int32 value);

    CARAPI GetScrollX(
        /* [out] */ Int32 * pX);

    CARAPI GetScrollY(
        /* [out] */ Int32 * pY);

    CARAPI GetWidth(
        /* [out] */ Int32 * pWidth);

    CARAPI GetHeight(
        /* [out] */ Int32 * pHeight);

    CARAPI GetDrawingRect(
        /* [in] */ IRect * pOutRect);

    CARAPI GetMeasuredWidth(
        /* [out] */ Int32 * pWidth);

    CARAPI GetMeasuredWidthAndState(
        /* [out] */ Int32* width);

    CARAPI GetMeasuredHeight(
        /* [out] */ Int32 * pHeight);

    CARAPI GetMeasuredHeightAndState(
        /* [out] */ Int32* height);

    CARAPI GetMeasuredState(
        /* [out] */ Int32* state);

    CARAPI GetMatrix(
        /* [out] */ IMatrix** matrix);

    CARAPI GetCameraDistance(
        /* [out] */ Float* distance);

    CARAPI SetCameraDistance(
        /* [in] */ Float distance);

    CARAPI GetRotation(
        /* [out] */ Float* rotation);

    CARAPI SetRotation(
        /* [in] */ Float rotation);

    CARAPI GetRotationY(
        /* [out] */ Float* y);

    CARAPI SetRotationY(
        /* [in] */ Float rotationY);

    CARAPI GetRotationX(
        /* [out] */ Float* x);

    CARAPI SetRotationX(
        /* [in] */ Float rotationX);

    CARAPI GetScaleX(
        /* [out] */ Float* x);

    CARAPI SetScaleX(
        /* [in] */ Float scaleX);

    CARAPI GetScaleY(
        /* [out] */ Float* y);

    CARAPI SetScaleY(
        /* [in] */ Float scaleY);

    CARAPI GetPivotX(
        /* [out] */ Float* x);

    CARAPI SetPivotX(
        /* [in] */ Float pivotX);

    CARAPI GetPivotY(
        /* [out] */ Float* y);

    CARAPI SetPivotY(
        /* [in] */ Float pivotY);

    CARAPI GetAlpha(
        /* [out] */ Float* alpha);

    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* overlap);

    CARAPI SetAlpha(
        /* [in] */ Float alpha);

    CARAPI GetTop(
        /* [out] */ Int32 * pTop);

    CARAPI SetTop(
        /* [in] */ Int32 top);

    CARAPI GetBottom(
        /* [out] */ Int32 * pBottom);

    CARAPI IsDirty(
        /* [out] */ Boolean* dirty);

    CARAPI SetBottom(
        /* [in] */ Int32 bottom);

    CARAPI GetLeft(
        /* [out] */ Int32 * pLeft);

    CARAPI SetLeft(
        /* [in] */ Int32 left);

    CARAPI GetRight(
        /* [out] */ Int32 * pRight);

    CARAPI SetRight(
        /* [in] */ Int32 right);

    CARAPI GetX(
        /* [out] */ Float* x);

    CARAPI SetX(
        /* [in] */ Float x);

    CARAPI GetY(
        /* [out] */ Float* y);

    CARAPI SetY(
        /* [in] */ Float y);

    CARAPI GetTranslationX(
        /* [out] */ Float* x);

    CARAPI SetTranslationX(
        /* [in] */ Float translationX);

    CARAPI GetTranslationY(
        /* [out] */ Float* y);

    CARAPI SetTranslationY(
        /* [in] */ Float translationY);


    CARAPI GetHitRect(
        /* [in] */ IRect * pOutRect);

    CARAPI GetFocusedRect(
        /* [in] */ IRect * pOutRect);

    CARAPI GetGlobalVisibleRect(
        /* [in] */ IRect * pOutRect,
        /* [in] */ IPoint * pOutGlobalOffset,
        /* [out] */ Boolean * pResult);

    CARAPI GetGlobalVisibleRect(
        /* [in] */ IRect * pOutRect,
        /* [out] */ Boolean * pResult);

    CARAPI GetLocalVisibleRect(
        /* [in] */ IRect * pOutRect,
        /* [out] */ Boolean * pResult);

    CARAPI OffsetTopAndBottom(
        /* [in] */ Int32 offset);

    CARAPI OffsetLeftAndRight(
        /* [in] */ Int32 offset);

    CARAPI GetLayoutParams(
        /* [out] */ IViewGroupLayoutParams ** ppParams);

    CARAPI SetLayoutParams(
        /* [in] */ IViewGroupLayoutParams * pParams);

    CARAPI ResolveLayoutParams();

    CARAPI ScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI ScrollBy(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI Invalidate(
        /* [in] */ IRect * pDirty);

    CARAPI Invalidate(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI Invalidate();

    CARAPI IsOpaque(
        /* [out] */ Boolean * pOpaque);

    CARAPI GetHandler(
        /* [out] */ IHandler ** handler);

    CARAPI PostInvalidate();

    CARAPI PostInvalidate(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI PostInvalidateDelayed(
        /* [in] */ Int64 delayMilliseconds);

    CARAPI PostInvalidateDelayed(
        /* [in] */ Int64 delayMilliseconds,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI PostInvalidateOnAnimation();

    CARAPI PostInvalidateOnAnimation(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI ComputeScroll();

    CARAPI IsHorizontalFadingEdgeEnabled(
        /* [out] */ Boolean * pHorizontalFadingEdgeEnabled);

    CARAPI SetHorizontalFadingEdgeEnabled(
        /* [in] */ Boolean horizontalFadingEdgeEnabled);

    CARAPI IsVerticalFadingEdgeEnabled(
        /* [out] */ Boolean * pVerticalFadingEdgeEnabled);

    CARAPI SetVerticalFadingEdgeEnabled(
        /* [in] */ Boolean verticalFadingEdgeEnabled);

    CARAPI IsHorizontalScrollBarEnabled(
        /* [out] */ Boolean * pHorizontalScrollBarEnabled);

    CARAPI SetHorizontalScrollBarEnabled(
        /* [in] */ Boolean horizontalScrollBarEnabled);

    CARAPI IsVerticalScrollBarEnabled(
        /* [out] */ Boolean * pVerticalScrollBarEnabled);

    CARAPI SetVerticalScrollBarEnabled(
        /* [in] */ Boolean verticalScrollBarEnabled);

    CARAPI SetScrollbarFadingEnabled(
        /* [in] */ Boolean fadeScrollbars);

    CARAPI IsScrollbarFadingEnabled(
        /* [out] */ Boolean * pFadeScrollbars);

    CARAPI GetScrollBarDefaultDelayBeforeFade(
        /* [out] */ Int32* defaultDelay);

    CARAPI SetScrollBarDefaultDelayBeforeFade(
        /* [in] */ Int32 scrollBarDefaultDelayBeforeFade);

    CARAPI GetScrollBarFadeDuration(
        /* [out] */ Int32* duration);

    CARAPI SetScrollBarFadeDuration(
        /* [in] */ Int32 scrollBarFadeDuration);

    CARAPI GetScrollBarSize(
        /* [out] */ Int32* size);

    CARAPI SetScrollBarSize(
        /* [in] */ Int32 scrollBarSize);

    CARAPI SetScrollBarStyle(
        /* [in] */ Int32 style);

    CARAPI GetScrollBarStyle(
        /* [out] */ Int32 * pStyle);

    CARAPI CanScrollHorizontally(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* can);

    CARAPI CanScrollVertically(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* can);

    CARAPI ResolveRtlPropertiesIfNeeded();

    CARAPI ResetRtlProperties();

    CARAPI OnScreenStateChanged(
        /* [in] */ Int32 screenState);

    CARAPI OnRtlPropertiesChanged(
        /* [in] */ Int32 layoutDirection);

    CARAPI ResolveLayoutDirection(
        /* [out] */ Boolean* result);

    CARAPI CanResolveLayoutDirection(
        /* [out] */ Boolean*result);

    CARAPI ResetResolvedLayoutDirection();

    CARAPI IsLayoutDirectionInherited(
        /* [out] */ Boolean*result);

    CARAPI ResolvePadding();

    CARAPI ResetResolvedPadding();

    CARAPI AssignParent(
        /* [in] */ IViewParent * pParent);

    CARAPI GetWindowToken(
        /* [out] */ IBinder ** ppToken);

    CARAPI GetApplicationWindowToken(
        /* [out] */ IBinder ** ppToken);

    CARAPI GetDisplay(
        /* [out] */ IDisplay** display);

    CARAPI SaveHierarchyState(
        /* [in] */ IObjectInt32Map * pContainer);

    CARAPI RestoreHierarchyState(
        /* [in] */ IObjectInt32Map * pContainer);

    CARAPI GetDrawingTime(
        /* [out] */ Int64 * pTime);

    CARAPI SetDuplicateParentStateEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsDuplicateParentStateEnabled(
        /* [out] */ Boolean * pResult);

    CARAPI SetLayerType(
        /* [in] */ Int32 layerType,
        /* [in] */ IPaint* paint);

    CARAPI SetLayerPaint(
        /* [in] */ IPaint* paint);

    CARAPI GetLayerType(
        /* [out] */ Int32* type);

    CARAPI BuildLayer();

    CARAPI SetDrawingCacheEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsDrawingCacheEnabled(
        /* [out] */ Boolean * pEnabled);

    CARAPI CanHaveDisplayList(
        /* [out] */ Boolean* can);

    CARAPI GetDisplayList(
        /* [out] */ IDisplayList** list);

    CARAPI GetDrawingCache(
        /* [out] */ IBitmap ** ppBitmap);

    CARAPI GetDrawingCache(
        /* [in] */ Boolean autoScale,
        /* [out] */ IBitmap ** ppBitmap);

    CARAPI DestroyDrawingCache();

    CARAPI SetDrawingCacheBackgroundColor(
        /* [in] */ Int32 color);

    CARAPI GetDrawingCacheBackgroundColor(
        /* [out] */ Int32 * pColor);

    CARAPI BuildDrawingCache();

    CARAPI BuildDrawingCache(
        /* [in] */ Boolean autoScale);

    CARAPI CreateSnapshot(
        /* [in] */ BitmapConfig quality,
        /* [in] */ Int32 backgroundColor,
        /* [in] */ Boolean skipChildren,
        /* [out] */ IBitmap ** ppBitmap);

    CARAPI IsInEditMode(
        /* [out] */ Boolean * pResult);

    CARAPI IsHardwareAccelerated(
        /* [out] */ Boolean* attached);

    CARAPI Draw(
        /* [in] */ ICanvas * pCanvas);

    CARAPI GetSolidColor(
        /* [out] */ Int32 * pColor);

    CARAPI OnFinishInflate();

    CARAPI IsLayoutRequested(
        /* [out] */ Boolean * pResult);

    CARAPI Layout(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    CARAPI GetResources(
        /* [out] */ IResources ** ppResources);

    CARAPI UnscheduleDrawable(
        /* [in] */ IDrawable * pWho);

    CARAPI OnResolveDrawables(
        /* [in] */ Int64 layoutDirection);

    CARAPI RefreshDrawableState();

    CARAPI GetDrawableState(
        /* [out, callee] */ ArrayOf<Int32> ** ppState);

    CARAPI JumpDrawablesToCurrentState();

    CARAPI SetBackgroundColor(
        /* [in] */ Int32 color);

    CARAPI SetBackgroundResource(
        /* [in] */ Int32 resid);

    CARAPI SetBackground(
        /* [in] */ IDrawable* background);

    CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable * pD);

    CARAPI GetBackground(
        /* [out] */ IDrawable ** ppDrawable);

    CARAPI SetPadding(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI SetPaddingRelative(
        /* [in] */ Int32 start,
        /* [in] */ Int32 top,
        /* [in] */ Int32 end,
        /* [in] */ Int32 bottom);

    CARAPI GetPaddingTop(
        /* [out] */ Int32 * pTop);

    CARAPI GetPaddingBottom(
        /* [out] */ Int32 * pBottom);

    CARAPI GetPaddingLeft(
        /* [out] */ Int32 * pLeft);

    CARAPI GetPaddingStart(
        /* [out] */ Int32* start);

    CARAPI GetPaddingRight(
        /* [out] */ Int32 * pRight);

    CARAPI GetPaddingEnd(
        /* [out] */ Int32* end);

    CARAPI IsPaddingRelative(
        /* [out] */ Boolean* relative);

    CARAPI ResetPaddingToInitialValues();

    CARAPI GetOpticalInsets(
        /* [out] */ IInsets** sets);

    CARAPI SetLayoutInsets(
        /* [in] */ IInsets* layoutInsets);

    CARAPI SetSelected(
        /* [in] */ Boolean selected);

    CARAPI IsSelected(
        /* [out] */ Boolean * pSelected);

    CARAPI SetActivated(
        /* [in] */ Boolean activated);

    CARAPI IsActivated(
        /* [out] */ Boolean* activated);

    CARAPI GetViewTreeObserver(
        /* [out] */ IViewTreeObserver ** ppObserver);

    CARAPI GetRootView(
        /* [out] */ IView ** ppView);

    CARAPI GetLocationOnScreen(
        /* [out] */ Int32 * pX,
        /* [out] */ Int32 * pY);

    CARAPI GetLocationInWindow(
        /* [out] */ Int32 * pX,
        /* [out] */ Int32 * pY);

    CARAPI FindViewById(
        /* [in] */ Int32 id,
        /* [out] */ IView ** ppView);

    CARAPI FindViewByPredicate(
        /* [in] */ IPredicate* predicate,
        /* [out] */ IView** view);

    CARAPI FindViewByPredicateInsideOut(
        /* [in] */ IView* start,
        /* [in] */ IPredicate* predicate,
        /* [out] */ IView** view);

    CARAPI SetId(
        /* [in] */ Int32 id);

    CARAPI GetId(
        /* [out] */ Int32 * pId);

    CARAPI GetTag(
        /* [out] */ IInterface** tag);

    CARAPI SetTag(
        /* [in] */ IInterface* tag);

    CARAPI GetTag(
        /* [in] */ Int32 key,
        /* [out] */ IInterface** tag);

    CARAPI SetTag(
        /* [in] */ Int32 key,
        /* [in] */ IInterface* tag);

    CARAPI SetTagInternal(
        /* [in] */ Int32 key,
        /* [in] */ IInterface* tag);

    CARAPI HackTurnOffWindowResizeAnim(
        /* [in] */ Boolean off);

    CARAPI Animate(
        /* [out] */ IViewPropertyAnimator** animator);

    CARAPI GetInflaterContext(
        /* [out] */ IContext** context);

    CARAPI SetInflaterContext(
        /* [in] */ IContext* context);

    CARAPI GetXmlPath(
        /* [out] */ String* path);

    CARAPI SetXmlPath(
        /* [in] */ const String& path);

    CARAPI IsRootNamespace(
        /* [out] */ Boolean * pResult);

    CARAPI GetBaseline(
        /* [out] */ Int32 * pResult);

    CARAPI RequestLayout();

    CARAPI ForceLayout();

    CARAPI Measure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI GetMinimumHeight(
        /* [out] */ Int32* height);

    CARAPI SetMinimumHeight(
        /* [in] */ Int32 minHeight);

    CARAPI GetMinimumWidth(
        /* [out] */ Int32* width);

    CARAPI SetMinimumWidth(
        /* [in] */ Int32 minWidth);

    CARAPI GetAnimation(
        /* [out] */ IAnimation ** ppAnimation);

    CARAPI StartAnimation(
        /* [in] */ IAnimation * pAnimation);

    CARAPI ClearAnimation();

    CARAPI SetAnimation(
        /* [in] */ IAnimation * pAnimation);

    CARAPI GatherTransparentRegion(
        /* [in] */ IRegion * pRegion,
        /* [out] */ Boolean * pResult);

    CARAPI PlaySoundEffect(
        /* [in] */ Int32 soundConstant);

    CARAPI PerformHapticFeedback(
        /* [in] */ Int32 feedbackConstant,
        /* [out] */ Boolean * pResult);

    CARAPI PerformHapticFeedback(
        /* [in] */ Int32 feedbackConstant,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean * pResult);

    CARAPI SetSystemUiVisibility(
        /* [in] */ Int32 visibility);

    CARAPI GetSystemUiVisibility(
        /* [out] */ Int32* visibility);

    CARAPI GetWindowSystemUiVisibility(
        /* [out] */ Int32* visibility);

    CARAPI OnWindowSystemUiVisibilityChanged(
        /* [in] */ Int32 visible);

    CARAPI DispatchWindowSystemUiVisiblityChanged(
        /* [in] */ Int32 visible);

    CARAPI SetOnSystemUiVisibilityChangeListener(
        /* [in] */ IViewOnSystemUiVisibilityChangeListener* l);

    CARAPI DispatchSystemUiVisibilityChanged(
        /* [in] */ Int32 visibility);

    CARAPI SetDisabledSystemUiVisibility(
        /* [in] */ Int32 flags);

    CARAPI StartDrag(
        /* [in] */ IClipData* data,
        /* [in] */ IDragShadowBuilder* shadowBuilder,
        /* [in] */ IInterface* myLocalState,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* success);

    CARAPI OnDragEvent(
        /* [in] */ IDragEvent* event,
        /* [out] */ Boolean* consumed);

    CARAPI DispatchDragEvent(
        /* [in] */ IDragEvent* event,
        /* [out] */ Boolean* consumed);

    CARAPI OnCloseSystemDialogs(
        /* [in] */ const String& reason);

    CARAPI ApplyDrawableToTransparentRegion(
        /* [in] */ IDrawable * pDr,
        /* [in] */ IRegion * pRegion);

    CARAPI GetOverScrollMode(
        /* [out] */ Int32 * pMode);

    CARAPI SetOverScrollMode(
        /* [in] */ Int32 overScrollMode);

    CARAPI GetRawTextDirection(
        /* [out] */ Int32* direction);

    CARAPI SetTextDirection(
        /* [in] */ Int32 textDirection);

    CARAPI GetTextDirection(
        /* [out] */ Int32* direction);

    CARAPI ResolveTextDirection(
        /* [out] */ Boolean* done);

    CARAPI ResetResolvedTextDirection();

    CARAPI IsTextDirectionInherited(
        /* [out] */ Boolean* inherited);

    CARAPI GetRawTextAlignment(
        /* [out] */ Int32* alignment);

    CARAPI SetTextAlignment(
        /* [in] */ Int32 textAlignment);

    CARAPI GetTextAlignment(
        /* [out] */ Int32* alignment);

    CARAPI ResolveTextAlignment(
        /* [out] */ Boolean* done);

    CARAPI ResetResolvedTextAlignment();

    CARAPI IsTextAlignmentInherited(
        /* [out] */ Boolean* inherited);

    CARAPI GetHolder(
        /* [out] */ ISurfaceHolder ** ppHolder);

    CARAPI SetZOrderMediaOverlay(
        /* [in] */ Boolean isMediaOverlay);

    CARAPI SetZOrderOnTop(
        /* [in] */ Boolean onTop);

    CARAPI SetSecure(
        /* [in] */ Boolean isSecure);

    CARAPI SetWindowType(
        /* [in] */ Int32 type);

    CARAPI IsFixedSize(
        /* [out] */ Boolean * pSize);

    CARAPI SetCreateInputConnectionDelegate(
        /* [in] */ IView* view);

    CARAPI SetKeyEventCallbackDelegate(
        /* [in] */IKeyEventCallback* cb);
protected:
    ECode DrawSplashScreen(
        /* [in] */ ISurfaceHolder* pHodler,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    ECode ProcessKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* _event,
        /* [in] */ Boolean isPreIme,
        /* [out] */ Boolean* result);

    ECode ProcessKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* pEvent,
    /* [in] */ Boolean isPreIme,
    /* [out] */ Boolean* pResult);

public:
    static const Int32 DRAW_ERROR = 0;
    static const Int32 DRAW_GLES_2 = 1;
    static const Int32 DRAW_2D = 2;
    // Drawing is disable when the surface buffer
    // has changed size but we haven't yet processed the
    // resize event.
    static const Int32 DRAW_DISABLED = 3;
    static Boolean sShowingSplashScreen;
    static String sSplashURL;

    enum IME_STATE {
        IME_STATE_DISABLED = 0,
        IME_STATE_ENABLED,
        IME_STATE_PASSWORD,
        IME_STATE_PLUGIN
    };

    AutoPtr<IGeckoInputConnection> mInputConnection;
    IME_STATE mIMEState;
    String mIMETypeHint;
    String mIMEActionHint;
    Boolean mIMELandscapeFS;
    AutoPtr<IEditable> mEditable;
    Boolean mDrawSingleFrame;
    Boolean mAbortDraw;

private:
    Boolean mSurfaceValid;  //Is this surface valid for drawing into?
    Boolean mInDrawing; //Are we actively between beginDrawing/endDrawing
    Boolean mSyncDraw;  //Are we waiting for a buffer to draw in surfaceChanged?
    Int32 mDrawMode;    //True if gecko requests a buffer
    //ReentrantLock mSurfaceLock; //let's not change stuff around while we're in
                                //the middle of starting drawing, ending drawing,
                                //or changing surface characteristics
    pthread_mutex_t mSurfaceLock;
    Int32 mFormat;  //Surface format, from surfaceChanged.Largely useless

    //the dimensions  of the surface
    Int32 mWidth;
    Int32 mHeight;

    //the dimensions of the buffer we're using for drawing,
    //that is the software buffer or the EGLSurface
    Int32 mBufferWidth;
    Int32 mBufferHeight;

    AutoPtr<IKeyListener> mKeyListener;
    AutoPtr<IEditableFactory> mEditableFactory;

    //Software rendering
    AutoPtr<IBitmap> mSoftwareBitmap;
    Object mSoftwareBitmapLock;
    AutoPtr<IBuffer> mSoftwareBuffer;
    Object mSoftwareBufferLock;
    AutoPtr<IBitmap> mSoftwareBufferCopy;

    //const SynchronousQueue<IInterface*> mSyncDraws;
    AutoPtr<ISurfaceView> mSurfaceView;

    friend class MySurfaceHoderCallback;
    friend class GeckoAppShell;
};

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__GECKOSURFACEVIEW_H__

