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

#include "elastos/droid/systemui/recent/CRecentsPreloadReceiver.h"
#include "elastos/droid/systemui/recent/RecentTasksLoader.h"

using Elastos::Droid::SystemUI::Recent::IRecentsActivity;
using Elastos::Droid::SystemUI::Recent::IRecentTasksLoader;
using Elastos::Droid::SystemUI::Recent::RecentTasksLoader;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

CAR_OBJECT_IMPL(CRecentsPreloadReceiver)

ECode CRecentsPreloadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    AutoPtr<IRecentTasksLoader> rtl = RecentTasksLoader::GetInstance(context);
    if (IRecentsActivity::PRELOAD_INTENT.Equals(action)) {
        rtl->PreloadRecentTasksList();
    }
    else if (IRecentsActivity::CANCEL_PRELOAD_INTENT.Equals(action)){
        rtl->CancelPreloadingRecentTasksList();
    }
    return NOERROR;
}

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos