
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/WindowElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/api/WindowElastos_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::IO::ISerializable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

//=====================================================================
//               WindowElastos::InnerVSyncMonitorListener
//=====================================================================
WindowElastos::InnerVSyncMonitorListener::InnerVSyncMonitorListener(
    /* [in] */ WindowElastos* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WindowElastos::InnerVSyncMonitorListener::OnVSync(
    /* [in] */ VSyncMonitor* monitor,
    /* [in] */ Int64 vsyncTimeMicros)
{
    VALIDATE_NOT_NULL(monitor);
    // ==================before translated======================
    // if (mNativeWindowElastos != 0) {
    //     nativeOnVSync(mNativeWindowElastos, vsyncTimeMicros);
    // }

    if (mOwner->mNativeWindowElastos != 0) {
        mOwner->NativeOnVSync(mOwner->mNativeWindowElastos, vsyncTimeMicros);
    }
    return NOERROR;
}

//=====================================================================
//                            WindowElastos
//=====================================================================
const String WindowElastos::WINDOW_CALLBACK_ERRORS("window_callback_errors");
const Int32 WindowElastos::START_INTENT_FAILURE;
const String WindowElastos::TAG("WindowElastos");

WindowElastos::WindowElastos(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // assert context == context.getApplicationContext();
    // mApplicationContext = context;
    // mOutstandingIntents = new SparseArray<IntentCallback>();
    // mIntentErrors = new HashMap<Integer, String>();
    // mVSyncMonitor = new VSyncMonitor(context, mVSyncListener);

    AutoPtr<IContext> applicationContext;
    context->GetApplicationContext((IContext**)&applicationContext);
    assert (context == applicationContext);
    mApplicationContext = context;

    CSparseArray::New((ISparseArray**)&mOutstandingIntents);
    CHashMap::New((IHashMap**)&mIntentErrors);
    mVSyncMonitor = new VSyncMonitor();
    mVSyncMonitor->constructor(context, mVSyncListener);
}

Boolean WindowElastos::ShowIntent(
    /* [in] */ IPendingIntent* intent,
    /* [in] */ IntentCallback* callback,
    /* [in] */ Int32 errorId)
{
    // ==================before translated======================
    // return showCancelableIntent(intent, callback, errorId) >= 0;

    return (ShowCancelableIntent(intent, callback, errorId) >= 0);
}

Boolean WindowElastos::ShowIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ IntentCallback* callback,
    /* [in] */ Int32 errorId)
{
    // ==================before translated======================
    // return showCancelableIntent(intent, callback, errorId) >= 0;

    return (ShowCancelableIntent(intent, callback, errorId) >= 0);
}

Int32 WindowElastos::ShowCancelableIntent(
    /* [in] */ IPendingIntent* intent,
    /* [in] */ IntentCallback* callback,
    /* [in] */ Int32 errorId)
{
    // ==================before translated======================
    // Log.d(TAG, "Can't show intent as context is not an Activity: " + intent);
    // return START_INTENT_FAILURE;

    String intentStr;
    IObject* objTmp = IObject::Probe(intent);
    objTmp->ToString(&intentStr);
    Logger::D(TAG, String("Can't show intent as context is not an Activity: ") + intentStr);
    return START_INTENT_FAILURE;
}

Int32 WindowElastos::ShowCancelableIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ IntentCallback* callback,
    /* [in] */ Int32 errorId)
{
    // ==================before translated======================
    // Log.d(TAG, "Can't show intent as context is not an Activity: " + intent);
    // return START_INTENT_FAILURE;

    String intentStr;
    IObject* objTmp = IObject::Probe(intent);
    objTmp->ToString(&intentStr);
    Logger::D(TAG, String("Can't show intent as context is not an Activity: ") + intentStr);
    return START_INTENT_FAILURE;
}

ECode WindowElastos::CancelIntent(
    /* [in] */ Int32 requestCode)
{
    // ==================before translated======================
    // Log.d(TAG, "Can't cancel intent as context is not an Activity: " + requestCode);

    Logger::D(TAG, "Can't cancel intent as context is not an Activity: %d", requestCode);
    return NOERROR;
}

Boolean WindowElastos::RemoveIntentCallback(
    /* [in] */ IntentCallback* callback)
{
    // ==================before translated======================
    // int requestCode = mOutstandingIntents.indexOfValue(callback);
    // if (requestCode < 0) return false;
    // mOutstandingIntents.remove(requestCode);
    // mIntentErrors.remove(requestCode);
    // return true;

    IInterface* interfaceTmp = TO_IINTERFACE(callback);
    Int32 requestCode = 0;
    mOutstandingIntents->IndexOfValue(interfaceTmp, &requestCode);
    if (requestCode < 0) return FALSE;
    mOutstandingIntents->Remove(requestCode);

    AutoPtr<IInteger32> requestCodeTmp;
    CInteger32::New(requestCode, (IInteger32**)&requestCodeTmp);
    mIntentErrors->Remove(requestCodeTmp);
    return TRUE;
}

ECode WindowElastos::ShowError(
    /* [in] */ const String& error)
{
    // ==================before translated======================
    // if (error != null) {
    //     Toast.makeText(mApplicationContext, error, Toast.LENGTH_SHORT).show();
    // }

    if (!error.IsEmpty()) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);

        AutoPtr<ICharSequence> charSequence;
        CString::New(error, (ICharSequence**)&charSequence);

        AutoPtr<IToast> toast;
        helper->MakeText(mApplicationContext, charSequence, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
    return NOERROR;
}

ECode WindowElastos::ShowError(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // showError(mApplicationContext.getString(resId));

    String str;
    mApplicationContext->GetString(resId, &str);
    ShowError(str);
    return NOERROR;
}

ECode WindowElastos::SendBroadcast(
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // mApplicationContext.sendBroadcast(intent);

    mApplicationContext->SendBroadcast(intent);
    return NOERROR;
}

AutoPtr<IWeakReference> WindowElastos::GetActivity()
{
    // ==================before translated======================
    // return new WeakReference<Activity>(null);

    IWeakReferenceSource* source = IWeakReferenceSource::Probe(NULL);
    AutoPtr<IWeakReference> wr;
    source->GetWeakReference((IWeakReference**)&wr);
    return wr;
}

AutoPtr<IContext> WindowElastos::GetApplicationContext()
{
    // ==================before translated======================
    // return mApplicationContext;

    return mApplicationContext;
}

ECode WindowElastos::SaveInstanceState(
    /* [in] */ IBundle* bundle)
{
    VALIDATE_NOT_NULL(bundle);
    // ==================before translated======================
    // bundle.putSerializable(WINDOW_CALLBACK_ERRORS, mIntentErrors);

    ISerializable* serialize = ISerializable::Probe(mIntentErrors);
    bundle->PutSerializable(WINDOW_CALLBACK_ERRORS, (ISerializable*)serialize);
    return NOERROR;
}

ECode WindowElastos::RestoreInstanceState(
    /* [in] */ IBundle* bundle)
{
    VALIDATE_NOT_NULL(bundle);
    // ==================before translated======================
    // if (bundle == null) return;
    //
    // Object errors = bundle.getSerializable(WINDOW_CALLBACK_ERRORS);
    // if (errors instanceof HashMap) {
    //     @SuppressWarnings("unchecked")
    //     HashMap<Integer, String> intentErrors = (HashMap<Integer, String>) errors;
    //     mIntentErrors = intentErrors;
    // }

    if (bundle == NULL) return NOERROR;
    AutoPtr<ISerializable> errors;
    bundle->GetSerializable(WINDOW_CALLBACK_ERRORS, (ISerializable**)&errors);

    IInterface* interfaceTmp = (IInterface*)errors;
    IHashMap* hashMap = IHashMap::Probe(interfaceTmp);
    if (hashMap != NULL) {
        mIntentErrors = hashMap;
    }
    return NOERROR;
}

Boolean WindowElastos::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    // ==================before translated======================
    // return false;

    return FALSE;
}

Boolean WindowElastos::CanResolveActivity(
    /* [in] */ IIntent* intent)
{
    // ==================before translated======================
    // return mApplicationContext.getPackageManager().resolveActivity(intent, 0) != null;

    AutoPtr<IPackageManager> packageManager;
    mApplicationContext->GetPackageManager((IPackageManager**)&packageManager);

    AutoPtr<IResolveInfo> resolveInfo;
    packageManager->ResolveActivity(intent, 0, (IResolveInfo**)&resolveInfo);
    return resolveInfo != NULL;
}

ECode WindowElastos::Destroy()
{
    // ==================before translated======================
    // if (mNativeWindowElastos != 0) {
    //     nativeDestroy(mNativeWindowElastos);
    //     mNativeWindowElastos = 0;
    // }

    if (mNativeWindowElastos != 0) {
        NativeDestroy(mNativeWindowElastos);
        mNativeWindowElastos = 0;
    }
    return NOERROR;
}

Handle64 WindowElastos::GetNativePointer()
{
    // ==================before translated======================
    // if (mNativeWindowElastos == 0) {
    //     mNativeWindowElastos = nativeInit(mVSyncMonitor.getVSyncPeriodInMicroseconds());
    // }
    // return mNativeWindowElastos;

    if (mNativeWindowElastos == 0) {
        mNativeWindowElastos = NativeInit(mVSyncMonitor->GetVSyncPeriodInMicroseconds());
    }
    return mNativeWindowElastos;
}

ECode WindowElastos::ShowCallbackNonExistentError(
    /* [in] */ const String& error)
{
    // ==================before translated======================
    // showError(error);

    ShowError(error);
    return NOERROR;
}

ECode WindowElastos::RequestVSyncUpdate()
{
    // ==================before translated======================
    // mVSyncMonitor.requestUpdate();

    mVSyncMonitor->RequestUpdate();
    return NOERROR;
}

Handle64 WindowElastos::NativeInit(
    /* [in] */ Int64 vsyncPeriod)
{
    return Elastos_WindowAndroid_nativeInit(THIS_PROBE(IInterface), vsyncPeriod);
}

ECode WindowElastos::NativeOnVSync(
    /* [in] */ Handle64 nativeWindowElastos,
    /* [in] */ Int64 vsyncTimeMicros)
{
    Elastos_WindowAndroid_nativeOnVSync(THIS_PROBE(IInterface), (Handle64)nativeWindowElastos, vsyncTimeMicros);
    return NOERROR;
}

ECode WindowElastos::NativeDestroy(
    /* [in] */ Handle64 nativeWindowElastos)
{
    Elastos_WindowAndroid_nativeDestroy(THIS_PROBE(IInterface), (Handle64)nativeWindowElastos);
    return NOERROR;
}

void WindowElastos::RequestVSyncUpdate(
    /* [in] */ IInterface* obj)
{
    WindowElastos* mObj = (WindowElastos*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E(TAG, "WindowElastos::RequestVSyncUpdate, mObj is NULL");
        return;
    }
    mObj->RequestVSyncUpdate();
}

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


