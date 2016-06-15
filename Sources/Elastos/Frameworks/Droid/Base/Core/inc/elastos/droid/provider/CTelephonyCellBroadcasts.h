#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYCELLBROADCASTS_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYCELLBROADCASTS_H__

#include "_Elastos_Droid_Provider_CTelephonyCellBroadcasts.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonyCellBroadcasts)
    , public Singleton
    , public ITelephonyCellBroadcasts
    , public IBaseColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetQUERY_COLUMNS(
        /* [out] */ ArrayOf<String>** result);

};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYCELLBROADCASTS_H__
