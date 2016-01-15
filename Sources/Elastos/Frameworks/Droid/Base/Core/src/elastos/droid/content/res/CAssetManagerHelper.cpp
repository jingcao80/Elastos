
#include "elastos/droid/content/res/CAssetManagerHelper.h"
#include "elastos/droid/content/res/CAssetManager.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CAssetManagerHelper, Singleton, IAssetManagerHelper)

CAR_SINGLETON_IMPL(CAssetManagerHelper)

ECode CAssetManagerHelper::GetSystem(
    /* [out] */ IAssetManager** am)
{
    VALIDATE_NOT_NULL(am);

    AutoPtr<CAssetManager> amObj = CAssetManager::GetSystem();
    *am = (IAssetManager*)amObj.Get();
    REFCOUNT_ADD(*am);
    return NOERROR;
}

ECode CAssetManagerHelper::GetGlobalAssetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    *count = CAssetManager::GetGlobalAssetCount();
    return NOERROR;
}

ECode CAssetManagerHelper::GetAssetAllocations(
    /* [out] */ String* allocations)
{
    VALIDATE_NOT_NULL(allocations);

    *allocations = CAssetManager::GetAssetAllocations();
    return NOERROR;
}

ECode CAssetManagerHelper::GetGlobalAssetManagerCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    *count = CAssetManager::GetGlobalAssetManagerCount();
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
