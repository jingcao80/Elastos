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

#include "elastos/droid/launcher2/CLauncherProviderHelper.h"
#include "elastos/droid/launcher2/LauncherProvider.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CLauncherProviderHelper)

CAR_INTERFACE_IMPL(CLauncherProviderHelper, Singleton, ILauncherProviderHelper)

ECode CLauncherProviderHelper::GetCONTENT_APPWIDGET_RESET_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return LauncherProvider::GetCONTENT_APPWIDGET_RESET_URI(uri);
}

ECode CLauncherProviderHelper::BuildOrWhereString(
    /* [in] */ const String& column,
    /* [in] */ ArrayOf<Int32>* values,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    return LauncherProvider::BuildOrWhereString(column, values, result);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos