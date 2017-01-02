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

#ifndef __ELASTOS_DROID_APP_APPLICATION_LOADERS_H__
#define __ELASTOS_DROID_APP_APPLICATION_LOADERS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::IClassLoader;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace App {

class ApplicationLoaders
    : public Object
{
public:
    static AutoPtr<ApplicationLoaders> GetDefault();

    AutoPtr<IClassLoader> GetClassLoader(
        /* [in] */ const String& zip,
        /* [in] */ const String& libPath,
        /* [in] */ IClassLoader* parent);

private:
    static AutoPtr<ApplicationLoaders> gApplicationLoaders;

    HashMap<String, AutoPtr<IClassLoader> > mLoaders;
    Object mLoadersLock;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APP_APPLICATION_LOADERS_H__
