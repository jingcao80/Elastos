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

#include "elastos/droid/dialer/dialpad/CSmartDialNameMatcherHelper.h"
#include "elastos/droid/dialer/dialpad/SmartDialNameMatcher.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CAR_INTERFACE_IMPL(CSmartDialNameMatcherHelper, Singleton, ISmartDialNameMatcherHelper);

CAR_SINGLETON_IMPL(CSmartDialNameMatcherHelper);

ECode CSmartDialNameMatcherHelper::NormalizeNumber(
    /* [in] */ const String& number,
    /* [in] */ ISmartDialMap* map,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SmartDialNameMatcher::NormalizeNumber(number, map);
    return NOERROR;
}

ECode CSmartDialNameMatcherHelper::NormalizeNumber(
    /* [in] */ String number,
    /* [in] */ Int32 offset,
    /* [in] */ ISmartDialMap* map,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SmartDialNameMatcher::NormalizeNumber(number, offset, map);
    return NOERROR;
}

ECode CSmartDialNameMatcherHelper::GetLATIN_SMART_DIAL_MAP(
    /* [out] */ ISmartDialMap** map)
{
    VALIDATE_NOT_NULL(map);
    *map = SmartDialNameMatcher::LATIN_SMART_DIAL_MAP;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

} // Dialpad
} // Dialer
} // Droid
} // Elastos
