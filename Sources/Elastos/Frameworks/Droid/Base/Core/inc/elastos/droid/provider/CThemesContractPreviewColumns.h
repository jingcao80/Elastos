#ifndef __ELASTOS_DROID_PROVIDER_CTHEMESCONTRACTPREVIEWCOLUMNS_H__
#define __ELASTOS_DROID_PROVIDER_CTHEMESCONTRACTPREVIEWCOLUMNS_H__

#include "_Elastos_Droid_Provider_CThemesContractPreviewColumns.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CThemesContractPreviewColumns)
    , public Singleton
    , public IThemesContractPreviewColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetAPPLIED_URI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CTHEMESCONTRACTPREVIEWCOLUMNS_H__
