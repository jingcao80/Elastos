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

#include "elastos/droid/content/CClipDescriptionHelper.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CClipDescriptionHelper, Singleton, IClipDescriptionHelper)

CAR_SINGLETON_IMPL(CClipDescriptionHelper)

ECode CClipDescriptionHelper::CompareMimeTypes(
    /* [in] */ const String& concreteType,
    /* [in] */ const String& desiredType,
    /* [out] */ Boolean* isMatch)
{
    VALIDATE_NOT_NULL(isMatch)
    const Int32 typeLength = desiredType.GetLength();
    if (typeLength == 3 && desiredType.Equals(String("*/*"))) {
        *isMatch = TRUE;
        return NOERROR;
    }

    const Int32 slashpos = desiredType.IndexOf('/');
    if (slashpos > 0) {
        if (typeLength == slashpos+2 && desiredType.GetChar(slashpos+1) == '*') {
            if (desiredType.RegionMatches(0, concreteType, 0, slashpos+1)) {
                *isMatch = TRUE;
                return NOERROR;
            }
        }
        else if (desiredType.Equals(concreteType)) {
            *isMatch = TRUE;
            return NOERROR;
        }
    }

    *isMatch = FALSE;
    return NOERROR;
}

}
}
}

