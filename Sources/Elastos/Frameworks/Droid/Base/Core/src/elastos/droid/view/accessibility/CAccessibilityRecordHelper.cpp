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

#include "elastos/droid/view/accessibility/CAccessibilityRecordHelper.h"
#include "elastos/droid/view/accessibility/AccessibilityRecord.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityRecordHelper, Singleton, IAccessibilityRecordHelper)

CAR_SINGLETON_IMPL(CAccessibilityRecordHelper)

ECode CAccessibilityRecordHelper::Obtain(
    /* [in] */ IAccessibilityRecord* record,
    /* [out] */ IAccessibilityRecord** newInstance)
{
    VALIDATE_NOT_NULL(newInstance);
    return AccessibilityRecord::Obtain(record, newInstance);
}

ECode CAccessibilityRecordHelper::Obtain(
    /* [out] */ IAccessibilityRecord** newInstance)
{
    VALIDATE_NOT_NULL(newInstance);
    return AccessibilityRecord::Obtain(newInstance);
}

} // Accessibility
} // View
} // Droid
} // Elastos
