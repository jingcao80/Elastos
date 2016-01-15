#include "elastos/droid/media/session/CPlaybackStateHelper.h"
#include "elastos/droid/media/session/CPlaybackState.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CPlaybackStateHelper, Singleton, IPlaybackStateHelper)

CAR_SINGLETON_IMPL(CPlaybackStateHelper)

ECode CPlaybackStateHelper::GetStateFromRccState(
    /* [in] */ Int32 rccState,
    /* [out] */ Int32 * result)
{
    return CPlaybackState::GetStateFromRccState(rccState, result);
}

ECode CPlaybackStateHelper::GetRccStateFromState(
    /* [in] */ Int32 state,
    /* [out] */ Int32 * result)
{
    return CPlaybackState::GetRccStateFromState(state, result);
}

ECode CPlaybackStateHelper::GetActionsFromRccControlFlags(
    /* [in] */ Int32 rccFlags,
    /* [out] */ Int64 * result)
{
    return CPlaybackState::GetActionsFromRccControlFlags(rccFlags, result);
}

ECode CPlaybackStateHelper::GetRccControlFlagsFromActions(
    /* [in] */ Int64 actions,
    /* [out] */ Int32 * result)
{
    return CPlaybackState::GetRccControlFlagsFromActions(actions, result);
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
