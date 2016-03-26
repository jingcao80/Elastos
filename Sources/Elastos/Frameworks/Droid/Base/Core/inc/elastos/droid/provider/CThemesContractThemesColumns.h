#ifndef __ELASTOS_DROID_PROVIDER_CTHEMESCONTRACTTHEMESCOLUMNS_H__
#define __ELASTOS_DROID_PROVIDER_CTHEMESCONTRACTTHEMESCOLUMNS_H__

#include "_Elastos_Droid_Provider_CThemesContractThemesColumns.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CThemesContractThemesColumns)
    , public Singleton
    , public IThemesContractThemesColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CTHEMESCONTRACTTHEMESCOLUMNS_H__
