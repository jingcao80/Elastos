
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/ContextThemeWrapperInLayoutInflater.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(ContextThemeWrapperInLayoutInflater, ContextThemeWrapper, IContextThemeWrapperInLayoutInflater)

ECode ContextThemeWrapperInLayoutInflater::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "ContextThemeWrapperInLayoutInflater";
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
