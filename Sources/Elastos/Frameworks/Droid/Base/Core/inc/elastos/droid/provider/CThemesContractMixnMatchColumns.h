#ifndef __ELASTOS_DROID_PROVIDER_CTHEMESCONTRACTMIXNMATCHCOLUMNS_H__
#define __ELASTOS_DROID_PROVIDER_CTHEMESCONTRACTMIXNMATCHCOLUMNS_H__

#include "_Elastos_Droid_Provider_CThemesContractMixnMatchColumns.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CThemesContractMixnMatchColumns)
    , public Singleton
    , public IThemesContractMixnMatchColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetROWS(
        /* [out, callee] */ ArrayOf<String>** rows);

    CARAPI ComponentToImageColName(
        /* [in] */ const String& component,
        /* [out] */ String* imageColName);

    CARAPI ComponentToMixNMatchKey(
        /* [in] */ const String& component,
        /* [out] */ String* mixNMatchKey);

    CARAPI MixNMatchKeyToComponent(
        /* [in] */ const String& mixnmatchKey,
        /* [out] */ String* component);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CTHEMESCONTRACTMIXNMATCHCOLUMNS_H__
