#include "elastos/droid/media/session/CMediaSessionHelper.h"
#include "elastos/droid/media/session/CMediaSession.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CMediaSessionHelper, Singleton, IMediaSessionHelper)

CAR_SINGLETON_IMPL(CMediaSessionHelper)

ECode CMediaSessionHelper::IsActiveState(
    /* [in] */ Int32 state,
    /* [out] */ Boolean * result)
{
    return CMediaSession::IsActiveState(state, result);
}


} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
