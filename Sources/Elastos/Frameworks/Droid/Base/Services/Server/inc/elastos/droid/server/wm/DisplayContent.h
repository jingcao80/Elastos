#ifndef __ELASTOS_DROID_SERVER_WM_DISPLAYCONTENT_H__
#define __ELASTOS_DROID_SERVER_WM_DISPLAYCONTENT_H__

#include "wm/MagnificationSpec.h"
#include "wm/WindowState.h"
#include "wm/StackTapPointerEventListener.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::View::IDisplayInfo;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Os::IRemoteCallbackList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

/**
 * Utility class for keeping track of the WindowStates and other pertinent contents of a
 * particular Display.
 *
 * IMPORTANT: No method from this class should ever be used without holding
 * WindowManagerService.mWindowMap.
 */
class DisplayContent : public Object
{
public:
    /**
     * @param display May not be null.
     */
    DisplayContent(
        /* [in] */ IDisplay* display,
        /* [in] */ CWindowManagerService* service);

    CARAPI_(Int32) GetDisplayId();

    CARAPI_(AutoPtr<WindowList>) GetWindowList();

    CARAPI_(AutoPtr<IDisplay>) GetDisplay();

    CARAPI_(AutoPtr<IDisplayInfo>) GetDisplayInfo();

    /**
     * Returns true if the specified UID has access to this display.
     */
    CARAPI_(Boolean) HasAccess(
        /* [in] */ Int32 uid);

    CARAPI_(Boolean) IsPrivate();

    CARAPI_(AutoPtr< List<AutoPtr<TaskStack> > >) GetStacks();

    /**
     * Retrieve the tasks on this display in stack order from the bottommost TaskStack up.
     * @return All the Tasks, in order, on this display.
     */
    CARAPI_(AutoPtr< List<AutoPtr<Task> > >) GetTasks();

    CARAPI_(AutoPtr<TaskStack>) GetHomeStack();

    CARAPI_(void) UpdateDisplayInfo();

    CARAPI_(void) GetLogicalDisplayRect(
        /* [in] */ IRect* out);

    /** Refer to {@link WindowManagerService#attachStack(int, int)} */
    CARAPI AttachStack(
        /* [in] */ TaskStack* stack);

    CARAPI_(void) MoveStack(
        /* [in] */ TaskStack* stack,
        /* [in] */ Boolean toTop);

    CARAPI_(void) DetachStack(
        /* [in] */ TaskStack* stack);

    /**
     * Propagate the new bounds to all child stacks.
     * @param contentRect The bounds to apply at the top level.
     */
    CARAPI_(void) Resize(
        /* [in] */ IRect* contentRect);

    CARAPI_(Int32) StackIdFromPoint(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) SetTouchExcludeRegion(
        /* [in] */ TaskStack* focusedStack);

    CARAPI_(void) SwitchUserStacks(
        /* [in] */ Int32 newUserId);

    CARAPI_(void) ResetAnimationBackgroundAnimator();

    CARAPI_(Boolean) AnimateDimLayers();

    CARAPI_(void) ResetDimming();

    CARAPI_(Boolean) IsDimming();

    CARAPI_(void) StopDimmingIfNeeded();

    CARAPI_(void) Close();

    CARAPI_(Boolean) IsAnimating();

    CARAPI_(void) CheckForDeferredActions();

    // void dump(String prefix, PrintWriter pw);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    // Specification for magnifying the display content.
    AutoPtr<MagnificationSpec> mMagnificationSpec;

    // Callback for observing content changes on a display.
    AutoPtr<IRemoteCallbackList> mDisplayContentChangeListeners;

    // This protects the following display size properties, so that
    // getDisplaySize() doesn't need to acquire the global lock.  This is
    // needed because the window manager sometimes needs to use ActivityThread
    // while it has its global state locked (for example to load animation
    // resources), but the ActivityThread also needs get the current display
    // size sometimes when it has its package lock held.
    //
    // These will only be modified with both mWindowMap and mDisplaySizeLock
    // held (in that order) so the window manager doesn't need to acquire this
    // lock when needing these values in its normal operation.
    Object mDisplaySizeLock;
    Int32 mInitialDisplayWidth;
    Int32 mInitialDisplayHeight;
    Int32 mInitialDisplayDensity;
    Int32 mBaseDisplayWidth;
    Int32 mBaseDisplayHeight;
    Int32 mBaseDisplayDensity;

    AutoPtr<IRect> mBaseDisplayRect;
    AutoPtr<IRect> mContentRect;

    // Accessed directly by all users.
    Boolean mLayoutNeeded;
    Int32 mPendingLayoutChanges;
    Boolean mIsDefaultDisplay;

    /** Window tokens that are in the process of exiting, but still on screen for animations. */
    List<AutoPtr<WindowToken> > mExitingTokens;

    /** Detect user tapping outside of current focused stack bounds .*/
    AutoPtr<StackTapPointerEventListener> mTapDetector;

    /** Detect user tapping outside of current focused stack bounds .*/
    AutoPtr<IRegion> mTouchExcludeRegion;

    /** Save allocating when calculating rects */
    AutoPtr<IRect> mTmpRect;

    /** For gathering Task objects in order. */
    List<AutoPtr<Task> > mTmpTaskHistory;

    AutoPtr<CWindowManagerService> mService;

    /** Remove this display when animation on it has completed. */
    Boolean mDeferredRemoval;

private:
    /** Unique identifier of this stack. */
    Int32 mDisplayId;

    /** Z-ordered (bottom-most first) list of all Window objects. Assigned to an element
     * from mDisplayWindows; */
    WindowList mWindows;

    AutoPtr<IDisplayInfo> mDisplayInfo;
    AutoPtr<IDisplay> mDisplay;

    /** Array containing all TaskStacks on this display.  Array
     * is stored in display order with the current bottom stack at 0. */
    List<AutoPtr<TaskStack> > mStacks;

    /** A special TaskStack with id==HOME_STACK_ID that moves to the bottom whenever any TaskStack
     * (except a future lockscreen TaskStack) moves to the top. */
    AutoPtr<TaskStack> mHomeStack;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_DISPLAYCONTENT_H__
