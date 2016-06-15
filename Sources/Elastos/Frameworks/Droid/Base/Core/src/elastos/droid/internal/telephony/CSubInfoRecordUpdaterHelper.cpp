
#include "elastos/droid/internal/telephony/CSubInfoRecordUpdaterHelper.h"
#include "elastos/droid/internal/telephony/SubInfoRecordUpdater.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CSubInfoRecordUpdaterHelper, Object, ISubInfoRecordUpdaterHelper)

CAR_SINGLETON_IMPL(CSubInfoRecordUpdaterHelper)

ECode CSubInfoRecordUpdaterHelper::SetDisplayNameForNewSub(
    /* [in] */ const String& newSubName,
    /* [in] */ Int32 subId,
    /* [in] */ Int32 newNameSource)
{
    return SubInfoRecordUpdater::SetDisplayNameForNewSub(newSubName, subId, newNameSource);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
