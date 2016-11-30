
#include "org/alljoyn/bus/NativePendingAsyncJoin.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

NativePendingAsyncJoin::NativePendingAsyncJoin(
    /* [in] */ ISessionListener* sessionListener,
    /* [in] */ IOnJoinSessionListener* onJoinSessionListener,
    /* [in] */ IInterface* context)
    : mSessionListener(sessionListener)
    , mOnJoinSessionListener(onJoinSessionListener)
    , mContext(context)
{}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
