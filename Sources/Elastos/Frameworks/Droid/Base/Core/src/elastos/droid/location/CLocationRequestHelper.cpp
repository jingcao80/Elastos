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

#include "elastos/droid/location/CLocationRequestHelper.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(CLocationRequestHelper, Singleton, ILocationRequestHelper)

CAR_SINGLETON_IMPL(CLocationRequestHelper)

ECode CLocationRequestHelper::Create(
    /* [out] */ ILocationRequest** lr)
{
    VALIDATE_NOT_NULL(lr)
    AutoPtr<ILocationRequest> request = LocationRequest::Create();
    *lr = request.Get();
    REFCOUNT_ADD(*lr)
    return NOERROR;
}

ECode CLocationRequestHelper::CreateFromDeprecatedProvider(
    /* [in] */ const String& provider,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ Boolean singleShot,
    /* [out] */ ILocationRequest** lr)
{
    VALIDATE_NOT_NULL(lr)
    AutoPtr<ILocationRequest> request = LocationRequest::CreateFromDeprecatedProvider(provider,minTime,minDistance,singleShot);
    *lr = request.Get();
    REFCOUNT_ADD(*lr)
    return NOERROR;
}

ECode CLocationRequestHelper::CreateFromDeprecatedCriteria(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ Boolean singleShot,
    /* [out] */ ILocationRequest** lr)
{
    VALIDATE_NOT_NULL(lr)
    AutoPtr<ILocationRequest> request = LocationRequest::CreateFromDeprecatedCriteria(criteria, minTime,minDistance,singleShot);
    *lr = request.Get();
    REFCOUNT_ADD(*lr)
    return NOERROR;
}

ECode CLocationRequestHelper::QualityToString(
    /* [in] */ Int32 quality,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = LocationRequest::QualityToString(quality);
    return NOERROR;
}

} // namespace Location
} // namespace Droid
} // namespace Elastos