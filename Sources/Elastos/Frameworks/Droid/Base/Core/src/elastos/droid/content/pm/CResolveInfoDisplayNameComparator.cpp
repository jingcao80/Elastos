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

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/content/pm/CResolveInfoDisplayNameComparator.h"
#include "elastos/droid/content/pm/ResolveInfo.h"
#include "elastos/droid/os/UserHandle.h"

using Elastos::Droid::Os::UserHandle;

using Elastos::Core::EIID_IComparator;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Text::ICollatorHelper;
using Elastos::Text::CCollatorHelper;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CResolveInfoDisplayNameComparator, Object, IComparator)

CAR_OBJECT_IMPL(CResolveInfoDisplayNameComparator)

CResolveInfoDisplayNameComparator::CResolveInfoDisplayNameComparator()
{
    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    helper->GetInstance((ICollator**)&mCollator);
    mCollator->SetStrength(ICollator::PRIMARY);
}

ECode CResolveInfoDisplayNameComparator::constructor(
    /* [in] */ IPackageManager* pm)
{
    mPM = pm;
    return NOERROR;
}

ECode CResolveInfoDisplayNameComparator::Compare(
    /* [in] */ IInterface* ab,
    /* [in] */ IInterface* bb,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    IResolveInfo* a = IResolveInfo::Probe(ab);
    IResolveInfo* b = IResolveInfo::Probe(bb);

    // We want to put the one targeted to another user at the end of the dialog.
    if (((ResolveInfo*)a)->mTargetUserId != UserHandle::USER_CURRENT) {
        return 1;
    }
    if (((ResolveInfo*)b)->mTargetUserId != UserHandle::USER_CURRENT) {
        return -1;
    }

    String aStr;
    AutoPtr<ICharSequence> sa;
    a->LoadLabel(mPM, (ICharSequence**)&sa);
    if (sa == NULL) {
        AutoPtr<IActivityInfo> aInfo;
        a->GetActivityInfo((IActivityInfo**)&aInfo);
        IPackageItemInfo::Probe(aInfo)->GetName(&aStr);
    }
    else {
        sa->ToString(&aStr);
    }
    String bStr;
    AutoPtr<ICharSequence> sb;
    b->LoadLabel(mPM, (ICharSequence**)&sb);
    if (sb == NULL) {
        AutoPtr<IActivityInfo> aInfo;
        b->GetActivityInfo((IActivityInfo**)&aInfo);
        IPackageItemInfo::Probe(aInfo)->GetName(&bStr);
    }
    else {
        sb->ToString(&bStr);
    }

    return mCollator->Compare(aStr, bStr, result);
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
