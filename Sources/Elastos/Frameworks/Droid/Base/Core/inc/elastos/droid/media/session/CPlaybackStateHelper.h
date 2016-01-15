#ifndef __ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATEHELPER_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATEHELPER_H__

#include "_Elastos_Droid_Media_Session_CPlaybackStateHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CarClass(CPlaybackStateHelper)
    , public Singleton
    , public IPlaybackStateHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Get the {@link PlaybackState} state for the given
     * {@link RemoteControlClient} state.
     *
     * @param rccState The state used by {@link RemoteControlClient}.
     * @return The equivalent state used by {@link PlaybackState}.
     * @hide
     */
    CARAPI GetStateFromRccState(
        /* [in] */ Int32 rccState,
        /* [out] */ Int32 * result);

    /**
     * Get the {@link RemoteControlClient} state for the given
     * {@link PlaybackState} state.
     *
     * @param state The state used by {@link PlaybackState}.
     * @return The equivalent state used by {@link RemoteControlClient}.
     * @hide
     */
    CARAPI GetRccStateFromState(
        /* [in] */ Int32 state,
        /* [out] */ Int32 * result);

    /**
     * @hide
     */
    CARAPI GetActionsFromRccControlFlags(
        /* [in] */ Int32 rccFlags,
        /* [out] */ Int64 * result);

    /**
     * @hide
     */
    CARAPI GetRccControlFlagsFromActions(
        /* [in] */ Int64 actions,
        /* [out] */ Int32 * result);
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CPLAYBACKSTATEHELPER_H__
