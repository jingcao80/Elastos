#ifndef __ELASTOS_DROID_SERVER_WM_DRAGSTATE_H__
#define __ELASTOS_DROID_SERVER_WM_DRAGSTATE_H__

#include "wm/CWindowManagerService.h"
#include "input/InputApplicationHandle.h"
#include "input/InputWindowHandle.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::ISurfacecontrol;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IDragEvent;
using Elastos::Droid::View::IInputChannel;
using Elastos::Droid::Content::IClipDescription;
using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Server::Input::InputApplicationHandle;
using Elastos::Droid::Server::Input::InputWindowHandle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm{

class DragState : public Object
{
public:
    DragState(
        /* [in] */ CWindowManagerService* service,
        /* [in] */ IBinder* token,
        /* [in] */ ISurfaceControl* surfaceControl,
        /* [in] */ Int32 flags,
        /* [in] */ IBinder* localWin);

    CARAPI_(void) Reset();

    /**
     * @param display The Display that the window being dragged is on.
     */
    CARAPI_(void) Register(
        /* [in] */ IDisplay* display);

    CARAPI_(void) Unregister();

    CARAPI_(Int32) GetDragLayerLw();

    /* call out to each visible window/session informing it about the drag
     */
    CARAPI_(void) BroadcastDragStartedLw(
        /* [in] */ Float touchX,
        /* [in] */ Float touchY);

    /* helper - construct and send a DRAG_STARTED event only if the window has not
     * previously been notified, i.e. it became visible after the drag operation
     * was begun.  This is a rare case.
     */
    CARAPI_(void) SendDragStartedIfNeededLw(
        /* [in] */ WindowState* newWin);

    CARAPI_(void) BroadcastDragEndedLw();

    CARAPI_(void) EndDragLw();

    CARAPI_(void) NotifyMoveLw(
        /* [in] */ Float x,
        /* [in] */ Float y);

    // Tell the drop target about the data.  Returns 'true' if we can immediately
    // dispatch the global drag-ended message, 'false' if we need to wait for a
    // result from the recipient.
    CARAPI_(Boolean) NotifyDropLw(
        /* [in] */ Float x,
        /* [in] */ Float y);

private:
    /* helper - send a caller-provided event, presumed to be DRAG_STARTED, if the
     * designated window is potentially a drop recipient.  There are race situations
     * around DRAG_ENDED broadcast, so we make sure that once we've declared that
     * the drag has ended, we never send out another DRAG_STARTED for this drag action.
     *
     * This method clones the 'event' parameter if it's being delivered to the same
     * process, so it's safe for the caller to call recycle() on the event afterwards.
     */
    CARAPI_(void) SendDragStartedLw(
        /* [in] */ WindowState* newWin,
        /* [in] */ Float touchX,
        /* [in] */ Float touchY,
        /* [in] */ IClipDescription* desc);

    // Find the visible, touch-deliverable window under the given point
    CARAPI_(AutoPtr<WindowState>) GetTouchedWinAtPointLw(
        /* [in] */ Float xf,
        /* [in] */ Float yf);

    static CARAPI_(AutoPtr<IDragEvent>) ObtainDragEvent(
        /* [in] */ WindowState* win,
        /* [in] */ Int32 action,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ IInterface* localState,
        /* [in] */ IClipDescription* description,
        /* [in] */ IClipData* data,
        /* [in] */ Boolean result);

public:
    CWindowManagerService* mService;
    AutoPtr<IBinder> mToken;
    AutoPtr<ISurfaceControl> mSurfaceControl;
    Int32 mFlags;
    AutoPtr<IBinder> mLocalWin;
    AutoPtr<IClipData> mData;
    AutoPtr<IClipDescription> mDataDescription;
    Boolean mDragResult;
    Float mCurrentX;
    Float mCurrentY;
    Float mThumbOffsetX;
    Float mThumbOffsetY;
    AutoPtr<IInputChannel> mServerChannel;
    AutoPtr<IInputChannel> mClientChannel;
    AutoPtr<CWindowManagerService::DragInputEventReceiver> mInputEventReceiver;
    AutoPtr<InputApplicationHandle> mDragApplicationHandle;
    AutoPtr<InputWindowHandle> mDragWindowHandle;
    AutoPtr<WindowState> mTargetWindow;
    List< AutoPtr<WindowState> > mNotifiedWindows;
    Boolean mDragInProgress;
    AutoPtr<IDisplay> mDisplay;

private:
    AutoPtr<IRegion> mTmpRegion;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_DRAGSTATE_H__
