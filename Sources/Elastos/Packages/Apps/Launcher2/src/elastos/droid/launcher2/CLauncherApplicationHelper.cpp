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

#include "elastos/droid/launcher2/CLauncherApplicationHelper.h"
#include "elastos/droid/launcher2/LauncherApplication.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CLauncherApplicationHelper)

CAR_INTERFACE_IMPL(CLauncherApplicationHelper, Singleton, ILauncherApplicationHelper)

ECode CLauncherApplicationHelper::GetSharedPreferencesKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);

    return LauncherApplication::GetSharedPreferencesKey(key);
}

ECode CLauncherApplicationHelper::IsScreenLarge(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return LauncherApplication::IsScreenLarge(result);
}

ECode CLauncherApplicationHelper::IsScreenLandscape(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return LauncherApplication::IsScreenLandscape(context, result);
}

ECode CLauncherApplicationHelper::GetScreenDensity(
    /* [out] */ Float* density)
{
    VALIDATE_NOT_NULL(density);

    return LauncherApplication::GetScreenDensity(density);
}

ECode CLauncherApplicationHelper::GetLongPressTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout);

    return LauncherApplication::GetLongPressTimeout(timeout);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos