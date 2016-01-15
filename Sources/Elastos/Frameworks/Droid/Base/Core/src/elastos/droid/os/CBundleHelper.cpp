
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CBundleHelper.h"
#include "elastos/droid/os/CBundle.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CBundleHelper, Singleton, IBundleHelper)

CAR_SINGLETON_IMPL(CBundleHelper)

ECode CBundleHelper::GetEMPTY(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    *bundle = CBundle::EMPTY;
    REFCOUNT_ADD(*bundle);
    return NOERROR;
}

ECode CBundleHelper::ForPair(
    /* [in] */ const String& key,
    /* [in] */ const String& value,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    AutoPtr<IBundle> b = CBundle::ForPair(key, value);
    *bundle = b;
    REFCOUNT_ADD(*bundle)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
