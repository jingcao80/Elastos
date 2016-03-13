
#include "elastos/droid/view/CPointerIcon.h"
#include "elastos/droid/view/CPointerIconHelper.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CPointerIconHelper)

CAR_INTERFACE_IMPL(CPointerIconHelper, Singleton, IPointerIconHelper)


ECode CPointerIconHelper::GetNullIcon(
    /* [out] */ IPointerIcon** pointerIcon)
{
    return CPointerIcon::GetNullIcon(pointerIcon);
}

ECode CPointerIconHelper::GetDefaultIcon(
    /* [in] */ IContext* context,
    /* [out] */ IPointerIcon** pointerIcon)
{
    return CPointerIcon::GetDefaultIcon(context, pointerIcon);
}

ECode CPointerIconHelper::GetSystemIcon(
    /* [in] */ IContext* context,
    /* [in] */ Int32 style,
    /* [out] */ IPointerIcon** pointerIcon)
{
    return CPointerIcon::GetSystemIcon(context, style, pointerIcon);
}

ECode CPointerIconHelper::CreateCustomIcon(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Float hotSpotX,
    /* [in] */ Float hotSpotY,
    /* [out] */ IPointerIcon** pointerIcon)
{
    return CPointerIcon::CreateCustomIcon(bitmap, hotSpotX, hotSpotY, pointerIcon);
}

ECode CPointerIconHelper::LoadCustomIcon(
    /* [in] */ IResources* resources,
    /* [in] */ Int32 resourceId,
    /* [out] */ IPointerIcon** pointerIcon)
{
    return CPointerIcon::LoadCustomIcon(resources, resourceId, pointerIcon);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
