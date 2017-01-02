//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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