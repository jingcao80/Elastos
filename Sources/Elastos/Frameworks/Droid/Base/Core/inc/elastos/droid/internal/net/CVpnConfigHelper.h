
#ifndef __ELASTOS_DROID_INTERNAL_NET_CVPNCONFIGHELPER_H__
#define __ELASTOS_DROID_INTERNAL_NET_CVPNCONFIGHELPER_H__

#include "_Elastos_Droid_Internal_Net_CVpnConfigHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::ICharSequence;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CarClass(CVpnConfigHelper)
    , public Singleton
    , public IVpnConfigHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetIntentForConfirmation(
        /* [out] */ IIntent** intent);

    CARAPI GetIntentForStatusPanel(
        /* [in] */ IContext* context,
        /* [out] */ IPendingIntent** intent);

    CARAPI GetVpnLabel(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [out] */ ICharSequence** label);
};

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_NET_CVPNCONFIGHELPER_H__
