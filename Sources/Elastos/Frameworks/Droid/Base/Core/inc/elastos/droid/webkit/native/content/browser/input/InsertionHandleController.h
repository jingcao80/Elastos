
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_INSERTIONHANDLECONTROLLER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_INSERTIONHANDLECONTROLLER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/webkit/native/content/browser/input/CursorController.h"

using Elastos::Droid::Content::IClipboardManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::IPopupWindow;

// import com.google.common.annotations.VisibleForTesting;

// import org.chromium.content.browser.PositionObserver;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class PositionObserver;

namespace Input {

/**
 * CursorController for inserting text at the cursor position.
 */
class InsertionHandleController
    : public CursorController
{
public:
    /*
     * This class is based on TextView.PastePopupMenu.
     */
    class PastePopupMenu
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        PastePopupMenu(
            /* [in] */ InsertionHandleController* owner);

        CARAPI_(void) Show();

        CARAPI_(void) Hide();

        CARAPI_(Boolean) IsShowing();

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI_(void) PositionAtCursor();

    private:
        CARAPI_(Int32) ViewIndex(
            /* [in] */ Boolean onTop);

        CARAPI_(void) UpdateContent(
            /* [in] */ Boolean onTop);

    private:
        InsertionHandleController* mOwner;
        /*const*/ AutoPtr<IPopupWindow> mContainer;
        Int32 mPositionX;
        Int32 mPositionY;
        /*const*/ AutoPtr< ArrayOf<IView*> > mPasteViews;
        /*const*/ AutoPtr< ArrayOf<Int32> > mPasteViewLayouts;
    };

public:
    InsertionHandleController(
        /* [in] */ IView* parent,
        /* [in] */ PositionObserver* positionObserver);

    //CAR_INTERFACE_DECL();

    /** Allows the handle to be shown automatically when cursor position changes */
    CARAPI_(void) AllowAutomaticShowing();

    /** Disallows the handle from being shown automatically when cursor position changes */
    CARAPI_(void) HideAndDisallowAutomaticShowing();

    /**
     * Shows the handle.
     */
    CARAPI_(void) ShowHandle();

    CARAPI_(void) ShowPastePopup();

    CARAPI_(void) ShowHandleWithPastePopup();

    /**
     * @return whether the handle is being dragged.
     */
    CARAPI_(Boolean) IsDragging();

    /** Shows the handle at the given coordinates, as long as automatic showing is allowed */
    CARAPI_(void) OnCursorPositionChanged();

    /**
     * Moves the handle so that it points at the given coordinates.
     * @param x Handle x in physical pixels.
     * @param y Handle y in physical pixels.
     */
    CARAPI_(void) SetHandlePosition(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * If the handle is not visible, sets its visibility to View.VISIBLE and begins fading it in.
     */
    CARAPI_(void) BeginHandleFadeIn();

    /**
     * Sets the handle to the given visibility.
     */
    CARAPI_(void) SetHandleVisibility(
        /* [in] */ Int32 visibility);

    CARAPI_(Int32) GetHandleX();

    CARAPI_(Int32) GetHandleY();

    //@VisibleForTesting
    CARAPI_(AutoPtr<HandleView>) GetHandleViewForTest();

    //@Override
    CARAPI OnTouchModeChanged(
        /* [in] */ Boolean isInTouchMode);

    //@Override
    CARAPI Hide();

    //@Override
    CARAPI IsShowing(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI BeforeStartUpdatingPosition(
        /* [in] */ HandleView* handle);

    //@Override
    CARAPI UpdatePosition(
        /* [in] */ HandleView* handle,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    //@Override
    CARAPI OnDetached();

    CARAPI_(Boolean) CanPaste();

public:
    /**
     * The concrete implementation must cause the cursor position to move to the given
     * coordinates and (possibly asynchronously) set the insertion handle position
     * after the cursor position change is made via setHandlePosition.
     * @param x
     * @param y
     */
    virtual CARAPI_(void) SetCursorPosition(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y) = 0;

    /** Pastes the contents of clipboard at the current insertion point */
    virtual CARAPI_(void) Paste() = 0;

    /** Returns the current line height in pixels */
    virtual CARAPI_(Int32) GetLineHeight() = 0;

private:
    CARAPI_(void) CreateHandleIfNeeded();

    CARAPI_(void) ShowHandleIfNeeded();

private:
    /** The handle view, lazily created when first shown */
    AutoPtr<HandleView> mHandle;

    /** The view over which the insertion handle should be shown */
    /*const*/ AutoPtr<IView> mParent;

    /** True iff the insertion handle is currently showing */
    Boolean mIsShowing;

    /** True iff the insertion handle can be shown automatically when selection changes */
    Boolean mAllowAutomaticShowing;

    /*const*/ AutoPtr<IContext> mContext;

    /*const*/ AutoPtr<PositionObserver> mPositionObserver;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_INSERTIONHANDLECONTROLLER_H__
