
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/ActivityWindowElastos.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

//=====================================================================
//                        ActivityWindowElastos
//=====================================================================
const Int32 ActivityWindowElastos::REQUEST_CODE_PREFIX;
const Int32 ActivityWindowElastos::REQUEST_CODE_RANGE_SIZE;
const String ActivityWindowElastos::TAG("ActivityWindowElastos");

ActivityWindowElastos::ActivityWindowElastos(
    /* [in] */ IActivity* activity)
    : WindowElastos(NULL) // WindowElastos(context)
{
    // ==================before translated======================
    // super(activity.getApplicationContext());
    // mActivityRef = new WeakReference<Activity>(activity);

    IContext* tmp = IContext::Probe(activity);
    AutoPtr<IContext> context;
    tmp->GetApplicationContext((IContext**)&context);

    IWeakReferenceSource* source = IWeakReferenceSource::Probe(activity);
    source->GetWeakReference((IWeakReference**)&mActivityRef);
}

Int32 ActivityWindowElastos::ShowCancelableIntent(
    /* [in] */ IPendingIntent* intent,
    /* [in] */ IntentCallback* callback,
    /* [in] */ Int32 errorId)
{
    // ==================before translated======================
    // Activity activity = mActivityRef.get();
    // if (activity == null) return START_INTENT_FAILURE;
    //
    // int requestCode = generateNextRequestCode();
    //
    // try {
    //     activity.startIntentSenderForResult(
    //             intent.getIntentSender(), requestCode, new Intent(), 0, 0, 0);
    // } catch (SendIntentException e) {
    //     return START_INTENT_FAILURE;
    // }
    //
    // storeCallbackData(requestCode, callback, errorId);
    // return requestCode;

    AutoPtr<IInterface> activityTmp;
    mActivityRef->Resolve(EIID_IInterface, (IInterface**)&activityTmp);
    IActivity* activity = IActivity::Probe(activityTmp);
    if (NULL == activity)
        return START_INTENT_FAILURE;

    Int32 requestCode = GenerateNextRequestCode();

    //try {
        AutoPtr<IIntentSender> sender;
        intent->GetIntentSender((IIntentSender**)&sender);
        AutoPtr<IIntent> newIntent;
        CIntent::New((IIntent**)&newIntent);
        activity->StartIntentSenderForResult(sender, requestCode, newIntent, 0, 0, 0);
    //} catch (SendIntentException e) {
    //    return START_INTENT_FAILURE;
    //}

    StoreCallbackData(requestCode, callback, errorId);
    return requestCode;
}

Int32 ActivityWindowElastos::ShowCancelableIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ IntentCallback* callback,
    /* [in] */ Int32 errorId)
{
    // ==================before translated======================
    // Activity activity = mActivityRef.get();
    // if (activity == null) return START_INTENT_FAILURE;
    //
    // int requestCode = generateNextRequestCode();
    //
    // try {
    //     activity.startActivityForResult(intent, requestCode);
    // } catch (ActivityNotFoundException e) {
    //     return START_INTENT_FAILURE;
    // }
    //
    // storeCallbackData(requestCode, callback, errorId);
    // return requestCode;

    AutoPtr<IInterface> activityTmp;
    mActivityRef->Resolve(EIID_IInterface, (IInterface**)&activityTmp);
    IActivity* activity = IActivity::Probe(activityTmp);
    if (NULL == activity)
        return START_INTENT_FAILURE;

    Int32 requestCode = GenerateNextRequestCode();

    //try {
        activity->StartActivityForResult(intent, requestCode);
    //} catch (SendIntentException e) {
    //    return START_INTENT_FAILURE;
    //}

    StoreCallbackData(requestCode, callback, errorId);
    return requestCode;
}

ECode ActivityWindowElastos::CancelIntent(
    /* [in] */ Int32 requestCode)
{
    // ==================before translated======================
    // Activity activity = mActivityRef.get();
    // if (activity == null) return;
    // activity.finishActivity(requestCode);

    AutoPtr<IInterface> activityTmp;
    mActivityRef->Resolve(EIID_IInterface, (IInterface**)&activityTmp);
    IActivity* activity = IActivity::Probe(activityTmp);
    if (NULL == activity)
        return NOERROR;
    activity->FinishActivity(requestCode);
    return NOERROR;
}

Boolean ActivityWindowElastos::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    // ==================before translated======================
    // IntentCallback callback = mOutstandingIntents.get(requestCode);
    // mOutstandingIntents.delete(requestCode);
    // String errorMessage = mIntentErrors.remove(requestCode);
    //
    // if (callback != null) {
    //     callback.onIntentCompleted(this, resultCode,
    //             mApplicationContext.getContentResolver(), data);
    //     return true;
    // } else {
    //     if (errorMessage != null) {
    //         showCallbackNonExistentError(errorMessage);
    //         return true;
    //     }
    // }
    // return false;

    AutoPtr<IInterface> interfaceTmp;
    mOutstandingIntents->Get(requestCode, (IInterface**)&interfaceTmp);
    IObject* objectTmp = IObject::Probe(interfaceTmp);
    Object* objectTmp1 = (Object*)objectTmp;
    AutoPtr<IntentCallback> callback = (IntentCallback*)objectTmp1;
    mOutstandingIntents->Delete(requestCode);

    AutoPtr<IInteger32> requestCodeTmp;
    CInteger32::New(requestCode, (IInteger32**)&requestCodeTmp);
    AutoPtr<ICharSequence> errorMessageTmp;
    mIntentErrors->Remove(requestCodeTmp, (IInterface**)&errorMessageTmp);
    String errorMessage;
    errorMessageTmp->ToString(&errorMessage);

    if (NULL != callback) {
        AutoPtr<IContentResolver> contentResolver;
        mApplicationContext->GetContentResolver((IContentResolver**)&contentResolver);
        callback->OnIntentCompleted(this, resultCode, contentResolver, data);
        return TRUE;
    }
    else {
        if (NULL != errorMessage) {
            ShowCallbackNonExistentError(errorMessage);
            return TRUE;
        }
    }
    return FALSE;
}

AutoPtr<IWeakReference> ActivityWindowElastos::GetActivity()
{
    // ==================before translated======================
    // // Return a new WeakReference to prevent clients from releasing our internal WeakReference.
    // return new WeakReference<Activity>(mActivityRef.get());

    AutoPtr<IActivity> activityTmp;
    mActivityRef->Resolve(EIID_IInterface, (IInterface**)&activityTmp);
    IActivity* activity = IActivity::Probe(activityTmp);
    if (NULL == activity)
        return NULL;

    IWeakReferenceSource* source = IWeakReferenceSource::Probe(activity);
    AutoPtr<IWeakReference> result;
    source->GetWeakReference((IWeakReference**)&result);
    return result;
}

Int32 ActivityWindowElastos::GenerateNextRequestCode()
{
    // ==================before translated======================
    // int requestCode = REQUEST_CODE_PREFIX + mNextRequestCode;
    // mNextRequestCode = (mNextRequestCode + 1) % REQUEST_CODE_RANGE_SIZE;
    // return requestCode;

    Int32 requestCode = REQUEST_CODE_PREFIX + mNextRequestCode;
    mNextRequestCode = (mNextRequestCode + 1) % REQUEST_CODE_RANGE_SIZE;
    return requestCode;
}

ECode ActivityWindowElastos::StoreCallbackData(
    /* [in] */ Int32 requestCode,
    /* [in] */ IntentCallback* callback,
    /* [in] */ Int32 errorId)
{
    VALIDATE_NOT_NULL(callback);
    // ==================before translated======================
    // mOutstandingIntents.put(requestCode, callback);
    // mIntentErrors.put(requestCode, mApplicationContext.getString(errorId));

    IInterface* callbackTmp = TO_IINTERFACE(callback);
    mOutstandingIntents->Put(requestCode, callbackTmp);

    AutoPtr<IInteger32> requestCodeTmp;
    CInteger32::New(requestCode, (IInteger32**)&requestCodeTmp);

    String errorStr;
    mApplicationContext->GetString(errorId, &errorStr);
    AutoPtr<ICharSequence> charSequenceError;
    CString::New(errorStr, (ICharSequence**)&charSequenceError);
    mIntentErrors->Put(requestCodeTmp, charSequenceError);
    return NOERROR;
}

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


