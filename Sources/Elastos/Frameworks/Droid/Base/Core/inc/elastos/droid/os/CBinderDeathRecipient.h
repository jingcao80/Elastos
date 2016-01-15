#ifndef __ELASTOS_DROID_OS_CBINDERDEATHRECIPIENT_H__
#define __ELASTOS_DROID_OS_CBINDERDEATHRECIPIENT_H__

#include "_Elastos_Droid_Os_CBinderDeathRecipient.h"

using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CBinderDeathRecipient)
{
    CARAPI ProxyDied();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CBINDERDEATHRECIPIENT_H__
