#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CDUMPUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CDUMPUTILS_H__

#include "_Elastos_Droid_Internal_Utility_CDumpUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::Singleton;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CDumpUtils)
    , public Singleton
    , public IDumpUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI DumpAsync(
        /* [in] */ IHandler* handler,
        /* [in] */ IDump* dump,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int64 timeout);
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CDUMPUTILS_H__
