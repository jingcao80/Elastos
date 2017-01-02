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

#include "elastos/droid/transition/CAutoTransition.h"
#include "elastos/droid/transition/CFade.h"
#include "elastos/droid/transition/CChangeBounds.h"

using Elastos::Droid::Transition::CFade;
using Elastos::Droid::Transition::CChangeBounds;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CAutoTransition::
//===============================================================
CAR_OBJECT_IMPL(CAutoTransition)

ECode CAutoTransition::constructor()
{
    TransitionSet::constructor();
    Init();
    return NOERROR;
}

ECode CAutoTransition::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    TransitionSet::constructor(context, attrs);
    Init();
    return NOERROR;
}

void CAutoTransition::Init()
{
    SetOrdering(ORDERING_SEQUENTIAL);
    AutoPtr<ITransition> f;
    CFade::New(CFade::OUT, (ITransition**)&f);
    AddTransition(f);
    AutoPtr<ITransition> c;
    CChangeBounds::New((ITransition**)&c);
    AddTransition(c);
    AutoPtr<ITransition> fIn;
    CFade::New(CFade::IN, (ITransition**)&fIn);
    AddTransition(fIn);
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
