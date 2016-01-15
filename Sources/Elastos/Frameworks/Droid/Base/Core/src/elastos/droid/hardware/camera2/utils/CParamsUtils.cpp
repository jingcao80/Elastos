
#include "elastos/droid/hardware/camera2/utils/CParamsUtils.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(CParamsUtils, Singleton, IParamsUtils)

CAR_SINGLETON_IMPL(CParamsUtils)

ECode CParamsUtils::CreateRect(
    /* [in] */ ISize* size,
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);

    return ParamsUtils::CreateRect(size, result);
}

ECode CParamsUtils::CreateRect(
    /* [in] */ IRectF* rect,
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);

    return ParamsUtils::CreateRect(rect, result);
}

ECode CParamsUtils::MapRect(
    /* [in] */ IMatrix* transform,
    /* [in] */ IRect* rect,
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result);

    return ParamsUtils::MapRect(transform, rect, result);
}

ECode CParamsUtils::CreateSize(
    /* [in] */ IRect* rect,
    /* [out] */ ISize** result)
{
    VALIDATE_NOT_NULL(result);

    return ParamsUtils::CreateSize(rect, result);
}

ECode CParamsUtils::CreateRational(
    /* [in] */ Float value,
    /* [out] */ IRational** result)
{
    VALIDATE_NOT_NULL(result);

    return ParamsUtils::CreateRational(value, result);
}

ECode CParamsUtils::ConvertRectF(
    /* [in] */ IRect* source,
    /* [in] */ IRectF* destination)
{
    return ParamsUtils::ConvertRectF(source, destination);
}

ECode CParamsUtils::GetOrDefault(
    /* [in] */ ICaptureRequest* r,
    /* [in] */ ICaptureRequestKey* key,
    /* [in] */ IInterface* defaultValue,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    return ParamsUtils::GetOrDefault(r, key, defaultValue, result);
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos