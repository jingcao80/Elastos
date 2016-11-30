
#include "org/alljoyn/bus/NativePendingAsyncPing.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

NativePendingAsyncPing::NativePendingAsyncPing(
    /* [in] */ IOnPingListener* onPingListener,
    /* [in] */ IInterface* context)
    : mOnPingListener(onPingListener)
    , mContext(context)
{}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
