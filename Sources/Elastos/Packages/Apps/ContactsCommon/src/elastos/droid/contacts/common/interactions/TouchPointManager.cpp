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

#include "elastos/droid/contacts/common/interactions/TouchPointManager.h"

using Elastos::Droid::Graphics::CPoint;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Interactions {

CAR_INTERFACE_IMPL(TouchPointManager, Object, ITouchPointManager);

AutoPtr<ITouchPointManager> TouchPointManager::sInstance = new TouchPointManager();

TouchPointManager::TouchPointManager()
{
    CPoint::New((IPoint**)&mPoint);
}

AutoPtr<ITouchPointManager> TouchPointManager::GetInstance()
{
    return sInstance;
}

ECode TouchPointManager::GetPoint(
    /* [out] */ IPoint** point)
{
    VALIDATE_NOT_NULL(point);
    *point = mPoint;
    REFCOUNT_ADD(*point);
    return NOERROR;
}

ECode TouchPointManager::SetPoint(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return mPoint->Set(x, y);
}

ECode TouchPointManager::HasValidPoint(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 x, y;
    mPoint->Get(&x, &y);
    *result = x != 0 || y != 0;
    return NOERROR;
}

} // namespace Interactions
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

