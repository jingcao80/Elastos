#ifndef __ELASTOS_DROID_SERVER_WM_APPWINDOWTOKEN_H__
#define __ELASTOS_DROID_SERVER_WM_APPWINDOWTOKEN_H__

#include "elastos/droid/server/wm/WindowToken.h"
#include "elastos/droid/server/wm/StartingData.h"
#include "elastos/droid/server/input/InputApplicationHandle.h"

using Elastos::Droid::View::IApplicationToken;
using Elastos::Droid::View::IView;
using Elastos::Droid::Server::Input::InputApplicationHandle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class AppWindowAnimator;
class WindowAnimator;

class AppWindowToken : public WindowToken
{
public:
    AppWindowToken(
        /* [in] */ CWindowManagerService* service,
        /* [in] */ IApplicationToken* token,
        /* [in] */ Boolean voiceInteraction);

    CARAPI Init();

    CARAPI_(void) SendAppVisibilityToClients();

    CARAPI_(void) UpdateReportedVisibilityLocked();

    CARAPI_(AutoPtr<WindowState>) FindMainWindow();

    CARAPI_(Boolean) IsVisible();

    CARAPI_(void) RemoveAllWindows();

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IApplicationToken> mAppToken;

    // All of the windows and child windows that are included in this
    // application token.  Note this list is NOT sorted!
    typedef List<AutoPtr<WindowState> > WindowList;
    WindowList mAllAppWindows;
    AutoPtr<AppWindowAnimator> mAppAnimator;

    AutoPtr<WindowAnimator> mAnimator;

    Boolean mVoiceInteraction;

    Int32 mGroupId;
    Boolean mAppFullscreen;
    Int32 mRequestedOrientation;
    Boolean mLayoutConfigChanges;
    Boolean mShowWhenLocked;

    // The input dispatching timeout for this application token in nanoseconds.
    Int64 mInputDispatchingTimeoutNanos;

    // These are used for determining when all windows associated with
    // an activity have been drawn, so they can be made visible together
    // at the same time.
    Int64 mLastTransactionSequence;
    Int32 mNumInterestingWindows;
    Int32 mNumDrawnWindows;
    Boolean mInPendingTransaction;
    Boolean mAllDrawn;
    // Set to true when this app creates a surface while in the middle of an animation. In that
    // case do not clear allDrawn until the animation completes.
    Boolean mDeferClearAllDrawn;

    // Is this token going to be hidden in a little while?  If so, it
    // won't be taken into account for setting the screen orientation.
    Boolean mWillBeHidden;

    // Is this window's surface needed?  This is almost like hidden, except
    // it will sometimes be true a little earlier: when the token has
    // been shown, but is still waiting for its app transition to execute
    // before making its windows shown.
    Boolean mHiddenRequested;

    // Have we told the window clients to hide themselves?
    Boolean mClientHidden;

    // Last visibility state we reported to the app token.
    Boolean mReportedVisible;

    // Last drawn state we reported to the app token.
    Boolean mReportedDrawn;

    // Set to true when the token has been removed from the window mgr.
    Boolean mRemoved;

    // Information about an application starting window if displayed.
    AutoPtr<StartingData> mStartingData;
    AutoPtr<WindowState> mStartingWindow;
    AutoPtr<IView> mStartingView;
    Boolean mStartingDisplayed;
    Boolean mStartingMoved;
    Boolean mFirstWindowDrawn;

    // Input application handle used by the input dispatcher.
    AutoPtr<InputApplicationHandle> mInputApplicationHandle;

    Boolean mDeferRemoval;

    Boolean mLaunchTaskBehind;
    Boolean mEnteringAnimation;

    typedef List<AutoPtr<AppWindowToken> > AppTokenList;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_APPWINDOWTOKEN_H__
