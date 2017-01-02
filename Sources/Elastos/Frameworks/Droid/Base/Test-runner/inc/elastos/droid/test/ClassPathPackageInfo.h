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

#ifndef __CLASSPATHPACKAGEINFO_H__
#define __CLASSPATHPACKAGEINFO_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "elastos/droid/test/ClassPathPackageInfoSource.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Test {

class ClassPathPackageInfoSource;

class ClassPathPackageInfo : public Object
{
public:
    ClassPathPackageInfo(
        /* [in] */ ClassPathPackageInfoSource* source,
        /* [in] */ const String& packageName,
        /* [in] */ ISet* subpackageNames,
        /* [in] */ ISet* topLevelClasses);

    CARAPI_(AutoPtr<ISet>) GetSubpackages();

    CARAPI_(AutoPtr<ISet>) GetTopLevelClassesRecursive();

private:
    CARAPI_(void) AddTopLevelClassesTo(
        /* [in] */ ISet* set);

private:
    AutoPtr<ClassPathPackageInfoSource> mSource;
    String mPackageName;
    AutoPtr<ISet> mSubpackageNames;
    AutoPtr<ISet> mTopLevelClasses;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__CLASSPATHPACKAGEINFO_H__
