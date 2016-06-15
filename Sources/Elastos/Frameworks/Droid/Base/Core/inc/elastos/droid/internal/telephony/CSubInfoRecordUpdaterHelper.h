
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBINFORECORDUPDATERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBINFORECORDUPDATERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CSubInfoRecordUpdaterHelper.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSubInfoRecordUpdaterHelper)
    , public Singleton
    , public ISubInfoRecordUpdaterHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetDisplayNameForNewSub(
    /* [in] */ const String& newSubName,
    /* [in] */ Int32 subId,
    /* [in] */ Int32 newNameSource);

};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBINFORECORDUPDATERHELPER_H__
