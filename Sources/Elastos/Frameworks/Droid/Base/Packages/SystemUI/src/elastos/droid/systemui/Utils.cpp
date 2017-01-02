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

#include "elastos/droid/systemui/Utils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/ClassLoader.h>

using Elastos::Core::CPathClassLoader;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

static const String TAG("Elastos.Droid.SystemUI.Utils");
const String Utils::sModulePath("Elastos.Droid.SystemUI.eco");

AutoPtr<IClassLoader> Utils::sClassLoader;

AutoPtr<IClassInfo> Utils::GetClassInfo(
    /* [in] */ const String& className)
{
    if (sClassLoader == NULL) {
        ASSERT_SUCCEEDED(CPathClassLoader::New(sModulePath, NULL, (IClassLoader**)&sClassLoader))
        assert(sClassLoader != NULL);
    }

    AutoPtr<IClassInfo> classInfo;
    sClassLoader->LoadClass(className, (IClassInfo**)&classInfo);
    assert(classInfo != NULL);
    return classInfo;
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos