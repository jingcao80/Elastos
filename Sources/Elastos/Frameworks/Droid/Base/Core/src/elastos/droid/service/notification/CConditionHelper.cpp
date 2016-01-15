#include "elastos/droid/service/notification/CConditionHelper.h"
#include "elastos/droid/service/notification/Condition.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CAR_SINGLETON_IMPL(CConditionHelper)

CAR_INTERFACE_IMPL(CConditionHelper, Singleton, IConditionHelper)

ECode CConditionHelper::StateToString(
    /* [in] */ Int32 state,
    /* [out] */ String* str)
{
    return Condition::StateToString(state, str);
}

ECode CConditionHelper::RelevanceToString(
    /* [in] */ Int32 flags,
    /* [out] */ String* str)
{
    return Condition::RelevanceToString(flags, str);
}

ECode CConditionHelper::NewId(
    /* [in] */ IContext* context,
    /* [out] */ IUriBuilder** id)
{
    return Condition::NewId(context, id);
}

ECode CConditionHelper::IsValidId(
    /* [in] */ IUri* id,
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* isValidId)
{
    return Condition::IsValidId(id, pkg, isValidId);
}

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos