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

#include "elastos/droid/content/res/CSystemAppTheme.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {


CAR_OBJECT_IMPL(CSystemAppTheme)

ECode CSystemAppTheme::constructor()
{
    return ThemeConfig::AppTheme::constructor(
            IThemeConfig::SYSTEM_DEFAULT, IThemeConfig::SYSTEM_DEFAULT, IThemeConfig::SYSTEM_DEFAULT);
}

ECode CSystemAppTheme::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("No Theme Applied (Holo)");
    return NOERROR;
}

}
}
}
}
