
#ifndef __ORG_ALLJOYN_BUS_NATIVEPENDINGASYNCPING_H__
#define __ORG_ALLJOYN_BUS_NATIVEPENDINGASYNCPING_H__

#include "_Org.Alljoyn.Bus.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativePendingAsyncPing
{
public:
    NativePendingAsyncPing(
        /* [in] */ IOnPingListener* onPingListener,
        /* [in] */ IContext* context);

private:
    /**
     * private copy constructor this object can not be copied or assigned
     */
    NativePendingAsyncPing(const NativePendingAsyncPing& other);
    /**
     * private assignment operator this object can not be copied or assigned
     */
    NativePendingAsyncPing& operator =(const NativePendingAsyncPing& other);

public:
    AutoPtr<IOnPingListener> mOnPingListener;
    AutoPtr<IContext> mContext;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEPENDINGASYNCPING_H__
