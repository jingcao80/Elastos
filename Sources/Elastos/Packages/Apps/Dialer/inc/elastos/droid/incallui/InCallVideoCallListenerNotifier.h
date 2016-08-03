
#ifndef __ELASTOS_DROID_INCALLUI_INCALLVIDEOCALLLISTENERNOTIFIER_H__
#define __ELASTOS_DROID_INCALLUI_INCALLVIDEOCALLLISTENERNOTIFIER_H__

#include "elastos/droid/incallui/Call.h"
#include <elastos/utility/etl/Set.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::Set;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class InCallVideoCallListenerNotifier
    : public Object
{
public:
    /**
     * Static singleton accessor method.
     */
    static CARAPI_(AutoPtr<InCallVideoCallListenerNotifier>) GetInstance();

    /**
     * Adds a new {@link SessionModificationListener}.
     *
     * @param listener The listener.
     */
    CARAPI_(void) AddSessionModificationListener(
        /* [in] */ ISessionModificationListener* listener);

    /**
     * Remove a {@link SessionModificationListener}.
     *
     * @param listener The listener.
     */
    CARAPI_(void) RemoveSessionModificationListener(
        /* [in] */ ISessionModificationListener* listener);

    /**
     * Adds a new {@link VideoEventListener}.
     *
     * @param listener The listener.
     */
    CARAPI_(void) AddVideoEventListener(
        /* [in] */ IVideoEventListener* listener);

    /**
     * Remove a {@link VideoEventListener}.
     *
     * @param listener The listener.
     */
    CARAPI_(void) RemoveVideoEventListener(
        /* [in] */ IVideoEventListener* listener);

    /**
     * Adds a new {@link SurfaceChangeListener}.
     *
     * @param listener The listener.
     */
    CARAPI_(void) AddSurfaceChangeListener(
        /* [in] */ ISurfaceChangeListener* listener);

    /**
     * Remove a {@link SurfaceChangeListener}.
     *
     * @param listener The listener.
     */
    CARAPI_(void) RemoveSurfaceChangeListener(
        /* [in] */ ISurfaceChangeListener* listener);

    /**
     * Inform listeners of an upgrade to video request for a call.
     *
     * @param call The call.
     */
    CARAPI_(void) UpgradeToVideoRequest(
        /* [in] */ Call* call);

    /**
     * Inform listeners of a successful response to a video request for a call.
     *
     * @param call The call.
     */
    CARAPI_(void) UpgradeToVideoSuccess(
        /* [in] */ Call* call);

    /**
     * Inform listeners of an unsuccessful response to a video request for a call.
     *
     * @param call The call.
     */
    CARAPI_(void) UpgradeToVideoFail(
        /* [in] */ Call* call);

    /**
     * Inform listeners of a downgrade to audio.
     *
     * @param call The call.
     */
    CARAPI_(void) DowngradeToAudio(
        /* [in] */ Call* call);

    /**
     * Inform listeners of a downgrade to audio.
     *
     * @param call The call.
     * @param paused The paused state.
     */
    CARAPI_(void) PeerPausedStateChanged(
        /* [in] */ Call* call,
        /* [in] */ Boolean paused);

    /**
     * Inform listeners of a change to peer dimensions.
     *
     * @param call The call.
     * @param width New peer width.
     * @param height New peer height.
     */
    CARAPI_(void) PeerDimensionsChanged(
        /* [in] */ Call* call,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Inform listeners of a change to camera dimensions.
     *
     * @param call The call.
     * @param width The new camera video width.
     * @param height The new camera video height.
     */
    CARAPI_(void) CameraDimensionsChanged(
        /* [in] */ Call* call,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    /**
     * Private constructor.  Instance should only be acquired through getInstance().
     */
    InCallVideoCallListenerNotifier() {}

    static CARAPI_(AutoPtr<InCallVideoCallListenerNotifier>) InitNotifier();

private:
    /**
     * Singleton instance of this class.
     */
    static AutoPtr<InCallVideoCallListenerNotifier> sInstance;

    /**
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    Set<AutoPtr<ISessionModificationListener> > mSessionModificationListeners;
    Set<AutoPtr<IVideoEventListener> > mVideoEventListeners;
    Set<AutoPtr<ISurfaceChangeListener> > mSurfaceChangeListeners;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_INCALLVIDEOCALLLISTENERNOTIFIER_H__
