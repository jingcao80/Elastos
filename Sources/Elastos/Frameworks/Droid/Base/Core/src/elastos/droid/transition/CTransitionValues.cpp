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

#include "elastos/droid/transition/CTransitionValues.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CTransitionValues::
//===============================================================
CAR_OBJECT_IMPL(CTransitionValues)

CAR_INTERFACE_IMPL(CTransitionValues, Object, ITransitionValues)

ECode CTransitionValues::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (ITransitionValues::Probe(other) != NULL) {
        AutoPtr<ITransitionValues> p = ITransitionValues::Probe(other);
        AutoPtr<CTransitionValues> cp = (CTransitionValues*)p.Get();
        if (Object::Equals(mView->Probe(EIID_IInterface), cp->mView->Probe(EIID_IInterface))) {
            if (Object::Equals(mValues->Probe(EIID_IInterface), cp->mValues->Probe(EIID_IInterface))) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CTransitionValues::GetHashCode(
    /* [out] */ Int32* hc)
{
    VALIDATE_NOT_NULL(hc)

    Int32 vHc = 0, vlHc = 0;
//    mView->GetHashCode(&vHc);
    mValues->GetHashCode(&vlHc);
    *hc = 31 * vHc + vlHc;
    return NOERROR;
}

ECode CTransitionValues::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 hc = 0;
    GetHashCode(&hc);
    StringBuilder returnValue("TransitionValues@");
    returnValue += StringUtils::ToHexString(hc);
    returnValue += ":\n";
    returnValue += "    view = ";
    returnValue += mView;
    returnValue += "\n";
    returnValue += "    values:";
    AutoPtr<ISet> st;
    mValues->GetKeySet((ISet**)&st);
    AutoPtr<IIterator> it;
    st->GetIterator((IIterator**)&it);
    Boolean b = FALSE;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IInterface> v;
        mValues->Get(p, (IInterface**)&v);
        returnValue += "    ";
        returnValue += p;
        returnValue += ": ";
        returnValue += v;
        returnValue += "\n";
    }
    *result = returnValue.ToString();
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
