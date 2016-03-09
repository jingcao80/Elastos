
#include "elastos/droid/utility/CNativeTextHelper.h"
#include "elastos/droid/utility/NativeTextHelper.h"

using Elastos::Droid::Utility::EIID_INativeTextHelper;
using Elastos::Droid::Utility::NativeTextHelper;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_SINGLETON_IMPL(CNativeTextHelper)

CAR_INTERFACE_IMPL(CNativeTextHelper, Singleton, INativeTextHelper)

ECode CNativeTextHelper::GetLocalString(
    /* [in] */ IContext* context,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 originNamesId,
    /* [in] */ Int32 localNamesId,
    /* [out] */ String* result)
{
    return NativeTextHelper::GetLocalString(context, originalString, originNamesId, localNamesId, result);
}

ECode CNativeTextHelper::GetInternalLocalString(
    /* [in] */ IContext* context,
    /* [in] */ const String& originalString,
    /* [in] */ Int32 originNamesId,
    /* [in] */ Int32 localNamesId,
    /* [out] */ String* result)
{
    return NativeTextHelper::GetInternalLocalString(context, originalString, originNamesId, localNamesId, result);
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
