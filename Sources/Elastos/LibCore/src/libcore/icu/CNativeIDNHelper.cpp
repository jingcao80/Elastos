
#include "CNativeIDNHelper.h"
#include "NativeIDN.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CNativeIDNHelper)

CAR_INTERFACE_IMPL(CNativeIDNHelper, Singleton, INativeIDNHelper)

ECode CNativeIDNHelper::ToASCII(
    /* [in] */ const String& s,
    /* [in] */ Int32 flags,
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)

    *str = NativeIDN::ToASCII(s,flags);
    return NOERROR;
}

ECode CNativeIDNHelper::ToUnicode(
    /* [in] */ const String& s,
    /* [in] */ Int32 flags,
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)

    *str = NativeIDN::ToUnicode(s,flags);
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
