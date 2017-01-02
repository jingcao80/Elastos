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

#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGEPARSERHELPER_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGEPARSERHELPER_H__

#include "_Elastos_Droid_Content_Pm_CPackageParserHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CPackageParserHelper)
    , public Singleton
    , public IPackageParserHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsApkFile(
        /* [in] */ IFile* file,
        /* [out] */ Boolean* res);

    CARAPI ParsePackageLite(
        /* [in] */ IFile* packageFile,
        /* [in] */ Int32 flags,
        /* [out] */ IPackageLite** res);
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_PM_CPACKAGEPARSERHELPER_H__