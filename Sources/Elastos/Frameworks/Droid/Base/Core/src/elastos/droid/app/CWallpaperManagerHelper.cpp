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

#include "elastos/droid/app/CWallpaperManagerHelper.h"
#include "elastos/droid/app/CWallpaperManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CWallpaperManagerHelper, Object, IWallpaperManagerHelper)

CAR_SINGLETON_IMPL(CWallpaperManagerHelper)

ECode CWallpaperManagerHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IWallpaperManager** manager)
{
    return CWallpaperManager::GetInstance(context, manager);
}

ECode CWallpaperManagerHelper::OpenDefaultWallpaper(
    /* [in] */ IContext* context,
    /* [out] */ IInputStream** result)
{
    VALIDATE_NOT_NULL(result)
    *result = CWallpaperManager::OpenDefaultWallpaper(context);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CWallpaperManagerHelper::GetDefaultWallpaperComponent(
    /* [in] */ IContext* context,
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)
    *result = CWallpaperManager::GetDefaultWallpaperComponent(context);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
