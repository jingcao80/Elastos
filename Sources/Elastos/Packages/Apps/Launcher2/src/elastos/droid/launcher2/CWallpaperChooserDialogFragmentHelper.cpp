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

#include "elastos/droid/launcher2/CWallpaperChooserDialogFragmentHelper.h"
#include "elastos/droid/launcher2/WallpaperChooserDialogFragment.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CWallpaperChooserDialogFragmentHelper, Singleton, IWallpaperChooserDialogFragmentHelper)

CAR_SINGLETON_IMPL(CWallpaperChooserDialogFragmentHelper)

ECode CWallpaperChooserDialogFragmentHelper::NewInstance(
    /* [out] */ IWallpaperChooserDialogFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);

    return WallpaperChooserDialogFragment::NewInstance(fragment);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos