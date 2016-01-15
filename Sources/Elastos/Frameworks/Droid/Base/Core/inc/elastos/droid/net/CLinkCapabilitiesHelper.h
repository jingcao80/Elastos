
#ifndef __ELASTOS_DROID_NET_CLINKCAPABILITIESHELPER_H__
#define __ELASTOS_DROID_NET_CLINKCAPABILITIESHELPER_H__

#include "_Elastos_Droid_Net_CLinkCapabilitiesHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CLinkCapabilitiesHelper)
{
public:
    CARAPI CreateNeedsMap(
        /* [in] */ const String& applicationRole,
        /* [out] */ ILinkCapabilities** result);
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CLINKCAPABILITIESHELPER_H__
