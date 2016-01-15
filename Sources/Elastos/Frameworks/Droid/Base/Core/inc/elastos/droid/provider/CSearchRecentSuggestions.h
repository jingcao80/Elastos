#ifndef __ELASTOS_DROID_PROVIDER_CSEARCHRECENTSUGGESTIONS_H__
#define __ELASTOS_DROID_PROVIDER_CSEARCHRECENTSUGGESTIONS_H__

#include "_Elastos_Droid_Provider_CSearchRecentSuggestions.h"
#include "elastos/droid/provider/SearchRecentSuggestions.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CSearchRecentSuggestions)
    , public SearchRecentSuggestions
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CSEARCHRECENTSUGGESTIONS_H__