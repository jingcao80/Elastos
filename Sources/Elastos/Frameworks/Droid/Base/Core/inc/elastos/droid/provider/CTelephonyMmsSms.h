#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYMMSSMS_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYMMSSMS_H__

#include "_Elastos_Droid_Provider_CTelephonyMmsSms.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonyMmsSms)
    , public Singleton
    , public ITelephonyMmsSms
    , public IBaseColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_CONVERSATIONS_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_FILTER_BYPHONE_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_UNDELIVERED_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_DRAFT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_LOCKED_URI(
        /* [out] */ IUri** uri);

    CARAPI GetSEARCH_URI(
        /* [out] */ IUri** uri);

};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYMMSSMS_H__
