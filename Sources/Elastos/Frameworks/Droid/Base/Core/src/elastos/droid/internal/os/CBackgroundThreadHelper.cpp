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

#include "elastos/droid/internal/os/CBackgroundThreadHelper.h"
#include "elastos/droid/internal/os/BackgroundThread.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CAR_INTERFACE_IMPL(CBackgroundThreadHelper, Singleton, IBackgroundThreadHelper)

CAR_SINGLETON_IMPL(CBackgroundThreadHelper)

ECode CBackgroundThreadHelper::GetInstance(
    /* [out] */ IBackgroundThread** instance)
{
    VALIDATE_NOT_NULL(instance)
    AutoPtr<IBackgroundThread> temp = BackgroundThread::GetInstance();
    *instance = temp;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode CBackgroundThreadHelper::GetHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    AutoPtr<IHandler> temp = BackgroundThread::GetHandler();
    *handler = temp;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos
