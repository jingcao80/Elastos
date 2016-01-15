
#ifndef __ELASTOS_DROID_NET_PPPOE_CPPPOENATIVE_H__
#define __ELASTOS_DROID_NET_PPPOE_CPPPOENATIVE_H__

#include "_Elastos_Droid_Net_Pppoe_CPppoeNative.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Pppoe {

CarClass(CPppoeNative)
{
public:

    CARAPI GetInterfaceName(
        /* [in] */ Int32 index,
        /* [out] */ String* str);

    CARAPI GetInterfaceCnt(
        /* [out] */ Int32* val);

    CARAPI InitPppoeNative(
        /* [out] */ Int32* val);

    CARAPI WaitForEvent(
        /* [out] */ String* str);

    CARAPI IsInterfaceAdded(
        /* [in] */ const String& ifname,
        /* [out] */ Int32* val);
public:
    static const String TAG;
private:
};

} // Pppoe
} // Net
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_NET_PPPOE_CPPPOENATIVE_H__
