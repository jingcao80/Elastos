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

#include "elastos/droid/systemui/recents/RecentsAppWidgetHost.h"
#include "elastos/droid/systemui/recents/misc/SystemServicesProxy.h"
#include "elastos/droid/systemui/recents/model/RecentsTaskLoader.h"

using Elastos::Droid::SystemUI::Recents::Misc::SystemServicesProxy;
using Elastos::Droid::SystemUI::Recents::Model::RecentsTaskLoader;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

RecentsAppWidgetHost::RecentsAppWidgetHost(
    /* [in] */ IContext* context,
    /* [in] */ Int32 hostId)
    : mContext(context)
    , mIsListening(FALSE)
{
    AppWidgetHost::constructor(context, hostId);
    mConfig = RecentsConfiguration::GetInstance();
}

ECode RecentsAppWidgetHost::StartListening(
    /* [in] */ IRecentsAppWidgetHostCallbacks* cb)
{
    mCb = cb;
    mIsListening = TRUE;
    return AppWidgetHost::StartListening();
}

ECode RecentsAppWidgetHost::StopListening()
{
    AppWidgetHost::StopListening();
    // Ensure that we release any references to the callbacks
    mCb = NULL;
    mContext = NULL;
    mIsListening = FALSE;
    return NOERROR;
}

ECode RecentsAppWidgetHost::IsListening(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsListening;
    return NOERROR;
}

ECode RecentsAppWidgetHost::OnProviderChanged(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IAppWidgetProviderInfo* appWidgetInfo)
{
    if (mCb == NULL) return E_NULL_POINTER_EXCEPTION;

    AutoPtr<RecentsTaskLoader> rtl = RecentsTaskLoader::GetInstance();
    AutoPtr<SystemServicesProxy> ssp;
    rtl->GetSystemServicesProxy((SystemServicesProxy**)&ssp);
    if (appWidgetId > -1 && appWidgetId == mConfig->mSearchBarAppWidgetId) {
        // The search provider may have changed, so just delete the old widget and bind it again
        ssp->UnbindSearchAppWidget(this, appWidgetId);
        // Update the search widget
        mConfig->UpdateSearchBarAppWidgetId(mContext, -1);
        mCb->RefreshSearchWidget();
    }
    return NOERROR;
}

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos