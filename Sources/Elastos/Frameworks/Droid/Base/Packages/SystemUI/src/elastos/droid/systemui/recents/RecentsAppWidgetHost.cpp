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