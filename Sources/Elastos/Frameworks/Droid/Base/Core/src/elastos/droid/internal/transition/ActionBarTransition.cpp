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

#include "elastos/droid/internal/transition/ActionBarTransition.h"
#include "elastos/droid/transition/CChangeBounds.h"
#include "elastos/droid/transition/CChangeText.h"
#include "elastos/droid/transition/CFade.h"
#include "elastos/droid/transition/CTransitionManager.h"
#include "elastos/droid/transition/CTransitionSet.h"

using Elastos::Droid::Transition::CChangeBounds;
using Elastos::Droid::Transition::CChangeText;
using Elastos::Droid::Transition::CFade;
using Elastos::Droid::Transition::CTransitionManager;
using Elastos::Droid::Transition::CTransitionSet;
using Elastos::Droid::Transition::IChangeText;
using Elastos::Droid::Transition::IFade;
using Elastos::Droid::Transition::ITransitionSet;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Transition {

Boolean ActionBarTransition::TRANSITIONS_ENABLED = FALSE;
const Int32 ActionBarTransition::TRANSITION_DURATION = 120; // ms
const AutoPtr<ITransition> ActionBarTransition::sTransition = ActionBarTransition::InitTransition();

void ActionBarTransition::BeginDelayedTransition(
    /* [in] */ IViewGroup* sceneRoot)
{
    if (TRANSITIONS_ENABLED) {
        CTransitionManager::BeginDelayedTransition(sceneRoot, sTransition);
    }
}

AutoPtr<ITransition> ActionBarTransition::InitTransition()
{
    if (TRANSITIONS_ENABLED) {
        AutoPtr<IChangeText> tc;
        CChangeText::New((IChangeText**)&tc);
        tc->SetChangeBehavior(IChangeText::CHANGE_BEHAVIOR_OUT_IN);
        AutoPtr<ITransitionSet> inner;
        CTransitionSet::New((ITransitionSet**)&inner);
        inner->AddTransition(ITransition::Probe(tc));
        AutoPtr<ITransition> cb;
        CChangeBounds::New((ITransition**)&cb);
        inner->AddTransition(cb);

        AutoPtr<ITransitionSet> tg;
        CTransitionSet::New((ITransitionSet**)&tg);
        AutoPtr<ITransition> fadeOut, fadeIn;
        CFade::New(CFade::OUT, (ITransition**)&fadeOut);
        CFade::New(CFade::IN, (ITransition**)&fadeIn);
        tg->AddTransition(fadeOut);
        tg->AddTransition(ITransition::Probe(inner));
        tg->AddTransition(fadeIn);
        tg->SetOrdering(ITransitionSet::ORDERING_SEQUENTIAL);
        ITransition::Probe(tg)->SetDuration(TRANSITION_DURATION);
        return ITransition::Probe(tg);
    }
    else {
        return NULL;
    }
}

} // namespace Transition
} // namespace Internal
} // namespace Droid
} // namespace Elastos
