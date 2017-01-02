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

#include "elastos/droid/systemui/egg/LLandActivity.h"
#include "elastos/droid/systemui/egg/LLand.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Egg {

ECode LLandActivity::constructor()
{
    return Activity::constructor();
}

ECode LLandActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Activity::SetContentView(R::layout::lland);
    AutoPtr<IView> view;
    Activity::FindViewById(R::id::world, (IView**)&view);
    AutoPtr<LLand> world = (LLand*)(view.Get());

    view = NULL;
    Activity::FindViewById(R::id::score, (IView**)&view);
    world->SetScoreField(ITextView::Probe(view));
    view = NULL;
    Activity::FindViewById(R::id::welcome, (IView**)&view);
    world->SetSplash(view);
    Boolean result;
    world->RequestFocus(&result);
    Logger::V(LLand::TAG, "focus: %s", result ? "TRUE" : "FALSE");
    return NOERROR;
}

} // namespace Egg
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos