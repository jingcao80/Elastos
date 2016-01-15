
#include "elastos/droid/internal/widget/CSmileysHelper.h"
#include "elastos/droid/internal/widget/Smileys.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_INTERFACE_IMPL(CSmileysHelper, Singleton, ISmileysHelper)
CAR_SINGLETON_IMPL(CSmileysHelper)

ECode CSmileysHelper::GetSmileyResource(
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    return Smileys::GetSmileyResource(which, result);
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
