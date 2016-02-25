
#include "elastos/droid/app/CAccessibilityServiceClientImpl.h"
#include "elastos/droid/app/CActionBarLayoutParams.h"
#include "elastos/droid/app/CActivity.h"
#include "elastos/droid/app/CActivityGroup.h"
#include "elastos/droid/app/CActivityView.h"
#include "elastos/droid/app/CActivityViewActivityContainerCallback.h"
#include "elastos/droid/app/CAlertDialog.h"
#include "elastos/droid/app/CAliasActivity.h"
#include "elastos/droid/app/CApplication.h"
#include "elastos/droid/app/CAppImportanceMonitorProcessObserver.h"
#include "elastos/droid/app/CAppOpsManagerPackageOps.h"
#include "elastos/droid/app/CAppOpsManagerOpEntry.h"
#include "elastos/droid/app/CBackStackRecord.h"
#include "elastos/droid/app/CDatePickerDialog.h"
#include "elastos/droid/app/CDialog.h"
#include "elastos/droid/app/CDialogFragment.h"
#include "elastos/droid/app/CExpandableListActivity.h"
#include "elastos/droid/app/CFastBitmapDrawable.h"
#include "elastos/droid/app/CFragmentBreadCrumbs.h"
#include "elastos/droid/app/CFragmentState.h"
#include "elastos/droid/app/CInstrumentation.h"
#include "elastos/droid/app/CJobSchedulerImpl.h"
#include "elastos/droid/app/CKeyguardManagerKeyguardLock.h"
#include "elastos/droid/app/CLauncherActivityIconResizer.h"
#include "elastos/droid/app/CLauncherActivityListItem.h"
#include "elastos/droid/app/CListActivity.h"
#include "elastos/droid/app/CListFragment.h"
#include "elastos/droid/app/CLocalActivityManager.h"
#include "elastos/droid/app/CLocalActivityRecord.h"
#include "elastos/droid/app/CMediaRouteButton.h"
#include "elastos/droid/app/CNotificationManager.h"
#include "elastos/droid/app/CNativeActivity.h"
#include "elastos/droid/app/CNativeContentView.h"
#include "elastos/droid/app/CBuilderRemoteViews.h"
#include "elastos/droid/app/CPresentation.h"
#include "elastos/droid/app/CProgressDialog.h"
#include "elastos/droid/app/CReceiverRestrictedContext.h"
#include "elastos/droid/app/CRemoteInputBuilder.h"
#include "elastos/droid/app/CSearchBar.h"
#include "elastos/droid/app/CSearchDialog.h"
#include "elastos/droid/app/CTabActivity.h"
#include "elastos/droid/app/CTimePickerDialog.h"
#include "elastos/droid/app/CUiAutomation.h"
#include "elastos/droid/app/CUiAutomationConnection.h"
#include "elastos/droid/app/CVoiceInteractor.h"
#include "elastos/droid/app/CVoiceInteractorCallback.h"
#include "elastos/droid/app/CVoiceInteractorConfirmationRequest.h"
#include "elastos/droid/app/CVoiceInteractorCompleteVoiceRequest.h"
#include "elastos/droid/app/CVoiceInteractorAbortVoiceRequest.h"
#include "elastos/droid/app/CVoiceInteractorCommandRequest.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_OBJECT_IMPL(CAccessibilityServiceClientImpl)
CAR_OBJECT_IMPL(CActionBarLayoutParams)
CAR_OBJECT_IMPL(CActivity)
CAR_OBJECT_IMPL(CActivityGroup)
CAR_OBJECT_IMPL(CActivityView)
CAR_OBJECT_IMPL(CActivityViewActivityContainerCallback)
CAR_OBJECT_IMPL(CAlertDialog)
CAR_OBJECT_IMPL(CAliasActivity)
CAR_OBJECT_IMPL(CApplication)
CAR_OBJECT_IMPL(CAppImportanceMonitorProcessObserver)
CAR_OBJECT_IMPL(CAppOpsManagerPackageOps)
CAR_OBJECT_IMPL(CAppOpsManagerOpEntry)
CAR_OBJECT_IMPL(CBackStackRecord)
CAR_OBJECT_IMPL(CDatePickerDialog)
CAR_OBJECT_IMPL(CDialog)
CAR_OBJECT_IMPL(CDialogFragment)
CAR_OBJECT_IMPL(CExpandableListActivity)
CAR_OBJECT_IMPL(CFastBitmapDrawable)
CAR_OBJECT_IMPL(CFragmentBreadCrumbs)
CAR_OBJECT_IMPL(CFragmentState)
CAR_OBJECT_IMPL(CInstrumentation)
CAR_OBJECT_IMPL(CJobSchedulerImpl)
CAR_OBJECT_IMPL(CKeyguardManagerKeyguardLock)
CAR_OBJECT_IMPL(CLauncherActivityListItem)
CAR_OBJECT_IMPL(CLauncherActivityIconResizer)
CAR_OBJECT_IMPL(CListActivity)
CAR_OBJECT_IMPL(CListFragment)
CAR_OBJECT_IMPL(CLocalActivityManager)
CAR_OBJECT_IMPL(CLocalActivityRecord)
CAR_OBJECT_IMPL(CMediaRouteButton)
CAR_OBJECT_IMPL(CNativeActivity)
CAR_OBJECT_IMPL(CNativeContentView)
CAR_OBJECT_IMPL(CNotificationManager)
CAR_OBJECT_IMPL(CBuilderRemoteViews)
CAR_OBJECT_IMPL(CPresentation)
CAR_OBJECT_IMPL(CProgressDialog)
CAR_OBJECT_IMPL(CReceiverRestrictedContext)
CAR_OBJECT_IMPL(CRemoteInputBuilder)
CAR_OBJECT_IMPL(CSearchBar)
CAR_OBJECT_IMPL(CSearchDialog)
CAR_OBJECT_IMPL(CTabActivity)
CAR_OBJECT_IMPL(CTimePickerDialog)
CAR_OBJECT_IMPL(CUiAutomation)
CAR_OBJECT_IMPL(CUiAutomationConnection)
CAR_OBJECT_IMPL(CVoiceInteractor)
CAR_OBJECT_IMPL(CVoiceInteractorCallback)
CAR_OBJECT_IMPL(CVoiceInteractorConfirmationRequest)
CAR_OBJECT_IMPL(CVoiceInteractorCompleteVoiceRequest)
CAR_OBJECT_IMPL(CVoiceInteractorAbortVoiceRequest)
CAR_OBJECT_IMPL(CVoiceInteractorCommandRequest)

} // namespace App
} // namespace Droid
} // namespace Elastos