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

#include "elastos/droid/content/res/CThemeConfigHelper.h"
#include "elastos/droid/content/res/ThemeConfig.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CThemeConfigHelper, Singleton, IThemeConfigHelper)

CAR_SINGLETON_IMPL(CThemeConfigHelper)

ECode CThemeConfigHelper::FromJson(
    /* [in] */ const String& json,
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    AutoPtr<IThemeConfig> tc = ThemeConfig::FromJson(json);
    *themeConfig = tc;
    REFCOUNT_ADD(*themeConfig)
    return NOERROR;
}

ECode CThemeConfigHelper::GetBootTheme(
    /* [in] */ IContentResolver* resolver,
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    return ThemeConfig::GetBootTheme(resolver, themeConfig);
}

ECode CThemeConfigHelper::GetBootThemeForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int32 userHandle,
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    return ThemeConfig::GetBootThemeForUser(resolver, userHandle, themeConfig);
}

ECode CThemeConfigHelper::GetSystemTheme(
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    AutoPtr<IThemeConfig> tc = ThemeConfig::GetSystemTheme();
    *themeConfig = tc;
    REFCOUNT_ADD(*themeConfig)
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
