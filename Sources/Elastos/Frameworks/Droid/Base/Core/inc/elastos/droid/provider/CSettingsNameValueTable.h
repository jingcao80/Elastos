#ifndef __ELASTOS_DROID_PROVIDER_CSETTINGSNAMEVALUETABLE_H__
#define __ELASTOS_DROID_PROVIDER_CSETTINGSNAMEVALUETABLE_H__

#include "_Elastos_Droid_Provider_CSettingsNameValueTable.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSettingsNameValueTable)
    , public Singleton
    , public ISettingsNameValueTable
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetUriFor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& name,
        /* [out] */ IUri** value);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CSETTINGSNAMEVALUETABLE_H__
