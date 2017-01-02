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

#ifndef __ELASTOS_DROID_CONTENT_PM_CPATHPERMISSION_H__
#define __ELASTOS_DROID_CONTENT_PM_CPATHPERMISSION_H__

#include "_Elastos_Droid_Content_Pm_CPathPermission.h"
#include "elastos/droid/os/PatternMatcher.h"

using Elastos::Droid::Os::PatternMatcher;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Description of permissions needed to access a particular path
 * in a {@link ProviderInfo}.
 */
CarClass(CPathPermission)
    , public PatternMatcher
    , public IPathPermission
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& pattern,
        /* [in] */ Int32 type,
        /* [in] */ const String& readPermission,
        /* [in] */ const String& writePermission);

    CARAPI GetReadPermission(
        /* [out] */ String* readPermission);

    CARAPI GetWritePermission(
        /* [out] */ String* writePermission);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    String mReadPermission;
    String mWritePermission;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPATHPERMISSION_H__
