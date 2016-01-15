
#ifndef __ELASTOS_DROID_NET_PPPOE_CPPPOEMANAGER_H__
#define __ELASTOS_DROID_NET_PPPOE_CPPPOEMANAGER_H__

#include "_Elastos_Droid_Net_Pppoe_CPppoeManager.h"
#include "CPppoeStateTracker.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Net::Pppoe::IIPppoeManager;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Net::Pppoe::IPppoeDevInfo;
using Elastos::Droid::Net::IDhcpInfo;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Pppoe {

CarClass(CPppoeManager)
{
public:
    CARAPI GetIIPppoeManager(
        /* [out] */ IIPppoeManager** manager);

    CARAPI SetIIPppoeManager(
        /* [in] */ IIPppoeManager* manager);

    CARAPI GetHandler(
        /* [out] */ IHandler** handler);

    CARAPI SetHandler(
        /* [in] */ IHandler* handler);

    CARAPI IsPppoeConfigured(
        /* [out] */ Boolean* result);

    CARAPI GetSavedPppoeConfig(
        /* [out] */ IPppoeDevInfo** info);

    CARAPI UpdatePppoeDevInfo(
        /* [in] */ IPppoeDevInfo* Info);

    CARAPI GetDeviceNameList(
        /* [out, callee] */ ArrayOf<String>** list);

    CARAPI SetPppoeEnabled(
        /* [in] */ Boolean ebl);

    CARAPI GetPppoeState(
        /* [out] */ Int32* val);

    CARAPI PppoeConfigured(
        /* [out] */ Boolean* result);

    CARAPI GetDhcpInfo(
        /* [out] */ IDhcpInfo** info);

    CARAPI GetTotalInterface(
        /* [out] */ Int32* val);

    CARAPI PppoeSetDefaultConf();

    CARAPI IsPppoeDeviceUp(
        /* [out] */ Boolean* result);

    CARAPI constructor(
        /* [in] */ IIPppoeManager* service,
        /* [in] */ IHandler* handler);

public:
    static const String TAG;

private:
    AutoPtr<IIPppoeManager> mService;
    AutoPtr<IHandler> mHandler;
};

} // Pppoe
} // Net
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_NET_PPPOE_CPPPOEMANAGER_H__
