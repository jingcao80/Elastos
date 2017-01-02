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

#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/content/pm/CApplicationInfoDisplayNameComparator.h"
#include "elastos/droid/content/pm/CApplicationInfo.h"

using Elastos::Core::EIID_IComparator;
using Elastos::Core::ICharSequence;
using Elastos::Text::ICollator;
using Elastos::Text::ICollatorHelper;
using Elastos::Text::CCollatorHelper;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CApplicationInfoDisplayNameComparator, Object, IComparator)

CAR_OBJECT_IMPL(CApplicationInfoDisplayNameComparator)

ECode CApplicationInfoDisplayNameComparator::constructor(
    /* [in] */ IPackageManager* pm)
{
    mPM = pm;
    return NOERROR;
}

ECode CApplicationInfoDisplayNameComparator::Compare(
    /* [in] */ IInterface* aaObj,
    /* [in] */ IInterface* abObj,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    IApplicationInfo* aa = IApplicationInfo::Probe(aaObj);
    IApplicationInfo* ab = IApplicationInfo::Probe(abObj);

    String a, b;
    AutoPtr<ICharSequence> sa;
    mPM->GetApplicationLabel(aa, (ICharSequence**)&sa);
    if (sa == NULL) {
        ((CApplicationInfo*)aa)->GetPackageName(&a);
    }
    else{
        a = Object::ToString(sa);
    }

    AutoPtr<ICharSequence> sb;
    mPM->GetApplicationLabel(ab, (ICharSequence**)&sb);
    if (sb == NULL) {
        ((CApplicationInfo*)ab)->GetPackageName(&b);
    }
    else{
        b = Object::ToString(sb);
    }

    AutoPtr<ICollator> collator;
    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    helper->GetInstance((ICollator**)&collator);
    return collator->Compare(a, b, result);
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
