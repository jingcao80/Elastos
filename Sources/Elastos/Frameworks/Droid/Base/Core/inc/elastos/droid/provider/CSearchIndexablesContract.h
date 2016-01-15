#ifndef __ELASTOS_DROID_CSEARCHINDEXABLESCONTRACT_H_
#define __ELASTOS_DROID_CSEARCHINDEXABLESCONTRACT_H_

#include "_Elastos_Droid_Provider_CSearchIndexablesContract.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSearchIndexablesContract)
    , public Singleton
    , public ISearchIndexablesContract
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetINDEXABLES_XML_RES_PATH(
        /* [out] */ String* result);

    CARAPI GetINDEXABLES_RAW_PATH(
        /* [out] */ String* result);

    CARAPI GetNON_INDEXABLES_KEYS_PATH(
        /* [out] */ String* result);

    CARAPI GetINDEXABLES_XML_RES_COLUMNS(
        /* [out, callee] */ ArrayOf<String>** resColumns);

    CARAPI GetINDEXABLES_RAW_COLUMNS(
        /* [out, callee] */ ArrayOf<String>** rawColumns);

    CARAPI GetNON_INDEXABLES_KEYS_COLUMNS(
        /* [out, callee] */ ArrayOf<String>** nonColumns);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CSEARCHINDEXABLESCONTRACT_H_