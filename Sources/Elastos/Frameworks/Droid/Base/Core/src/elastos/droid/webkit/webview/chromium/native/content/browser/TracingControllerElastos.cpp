
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/TracingControllerElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/TracingControllerElastos_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Content::R;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CString;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::IDateFormat;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::CDate;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//          TracingControllerElastos::TracingBroadcastReceiver
//=====================================================================

TracingControllerElastos::TracingBroadcastReceiver::TracingBroadcastReceiver(
    /* [in] */ TracingControllerElastos* owner)
    : mOwner(owner)
{
}

ECode TracingControllerElastos::TracingBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.EndWith(ACTION_START)) {
        String categories;
        intent->GetStringExtra(CATEGORIES_EXTRA, &categories);
        if (TextUtils::IsEmpty(categories)) {
            categories = mOwner->NativeGetDefaultCategories();
        }
        else {
            //categories = categories.ReplaceFirst(
            //         DEFAULT_CHROME_CATEGORIES_PLACE_HOLDER, mOwner->NativeGetDefaultCategories());
            String strResult;
            StringUtils::ReplaceFirst(categories, DEFAULT_CHROME_CATEGORIES_PLACE_HOLDER, mOwner->NativeGetDefaultCategories(), &strResult);
            categories = strResult;
        }
        String strExtra;
        intent->GetStringExtra(RECORD_CONTINUOUSLY_EXTRA, &strExtra);
        Boolean recordContinuously = strExtra != NULL;
        String filename;
        intent->GetStringExtra(FILE_EXTRA, &filename);
        if (filename != NULL) {
            mOwner->StartTracing(filename, TRUE, categories, recordContinuously);
        }
        else {
            mOwner->StartTracing(TRUE, categories, recordContinuously);
        }
    }
    else if (action.EndWith(ACTION_STOP)) {
        mOwner->StopTracing();
    }
    else if (action.EndWith(ACTION_LIST_CATEGORIES)) {
        mOwner->GetCategoryGroups();
    }
    else {
        String log("Unexpected intent: ");
        String str;
        intent->ToString(&str);
        log += str;
        Slogger::E(TAG, str);
    }

    return NOERROR;
}

//=====================================================================
//            TracingControllerElastos::TracingIntentFilter
//=====================================================================

TracingControllerElastos::TracingIntentFilter::TracingIntentFilter(
    /* [in] */ IContext* context)
{
    String packageName;
    context->GetPackageName(&packageName);
    String start(packageName);
    start += ".";
    start += ACTION_START;
    AddAction(start);
    String stop(packageName);
    stop += ".";
    stop += ACTION_STOP;
    AddAction(stop);
    String categories(packageName);
    categories += ".";
    categories += ACTION_LIST_CATEGORIES;
    AddAction(categories);
}

//=====================================================================
//                       TracingControllerElastos
//=====================================================================
const String TracingControllerElastos::TAG("TracingControllerElastos");
const String TracingControllerElastos::ACTION_START("GPU_PROFILER_START");
const String TracingControllerElastos::ACTION_STOP("GPU_PROFILER_STOP");
const String TracingControllerElastos::ACTION_LIST_CATEGORIES("GPU_PROFILER_LIST_CATEGORIES");
const String TracingControllerElastos::FILE_EXTRA("file");
const String TracingControllerElastos::CATEGORIES_EXTRA("categories");
const String TracingControllerElastos::RECORD_CONTINUOUSLY_EXTRA("continuous");
const String TracingControllerElastos::DEFAULT_CHROME_CATEGORIES_PLACE_HOLDER("_DEFAULT_CHROME_CATEGORIES");
const String TracingControllerElastos::PROFILER_STARTED_FMT("Profiler started: %s");
const String TracingControllerElastos::PROFILER_FINISHED_FMT("Profiler finished. Results are in %s.");

TracingControllerElastos::TracingControllerElastos(
    /* [in] */ IContext* context)
    : mContext(context)
{
    mBroadcastReceiver = new TracingBroadcastReceiver(this);
    mIntentFilter = new TracingIntentFilter(context);
}

AutoPtr<IBroadcastReceiver> TracingControllerElastos::GetBroadcastReceiver()
{
    return mBroadcastReceiver;
}

AutoPtr<IIntentFilter> TracingControllerElastos::GetIntentFilter()
{
    return mIntentFilter;
}

ECode TracingControllerElastos::RegisterReceiver(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntent> intent;
    return context->RegisterReceiver(GetBroadcastReceiver(), GetIntentFilter(), (IIntent**)&intent);
}

ECode TracingControllerElastos::UnregisterReceiver(
    /* [in] */ IContext* context)
{
    return context->UnregisterReceiver(GetBroadcastReceiver());
}

Boolean TracingControllerElastos::IsTracing()
{
    return mIsTracing;
}

String TracingControllerElastos::GetOutputPath()
{
    return mFilename;
}

Boolean TracingControllerElastos::StartTracing(
    /* [in] */ Boolean showToasts,
    /* [in] */ const String& categories,
    /* [in] */ Boolean recordContinuously)
{
    mShowToasts = showToasts;

    String filePath = GenerateTracingFilePath();
    if (filePath == NULL) {
        String str;
        mContext->GetString(R::string::profiler_no_storage_toast, &str);
        LogAndToastError(str);
    }

    return StartTracing(filePath, showToasts, categories, recordContinuously);
}

Boolean TracingControllerElastos::StartTracing(
    /* [in] */ const String& filename,
    /* [in] */ Boolean showToasts,
    /* [in] */ const String& categories,
    /* [in] */ Boolean recordContinuously)
{
    mShowToasts = showToasts;
    if (IsTracing()) {
        // Don't need a toast because this shouldn't happen via the UI.
        Slogger::E(TAG, "Received startTracing, but we're already tracing");
        return FALSE;
    }
    // Lazy initialize the native side, to allow construction before the library is loaded.
    InitializeNativeControllerIfNeeded();
    if (!NativeStartTracing(mNativeTracingControllerElastos, categories,
            recordContinuously)) {
        String str;
        mContext->GetString(R::string::profiler_error_toast, &str);
        LogAndToastError(str);
        return FALSE;
    }


    AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(categories, (ICharSequence**)&charSequenceTmp);
    formatArgs->Set(0, TO_IINTERFACE(charSequenceTmp));

    String lfp = StringUtils::Format(PROFILER_STARTED_FMT, formatArgs);
    LogForProfiler(lfp);

    String toast;
    mContext->GetString(R::string::profiler_started_toast, &toast);
    toast += ": ";
    toast += categories;
    ShowToast(toast);
    mFilename = filename;
    mIsTracing = TRUE;

    return TRUE;
}

ECode TracingControllerElastos::StopTracing()
{
    if (IsTracing()) {
        NativeStopTracing(mNativeTracingControllerElastos, mFilename);
    }

    return NOERROR;
}

ECode TracingControllerElastos::GetCategoryGroups()
{
    // Lazy initialize the native side, to allow construction before the library is loaded.
    InitializeNativeControllerIfNeeded();
    if (!NativeGetKnownCategoryGroupsAsync(mNativeTracingControllerElastos)) {
        Slogger::E(TAG, "Unable to fetch tracing record groups list.");
    }

    return NOERROR;
}

ECode TracingControllerElastos::OnTracingStopped()
{
    if (!IsTracing()) {
        // Don't need a toast because this shouldn't happen via the UI.
        Slogger::E(TAG, "Received onTracingStopped, but we aren't tracing");
        return NOERROR;
    }

    AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(mFilename, (ICharSequence**)&charSequenceTmp);
    formatArgs->Set(0, TO_IINTERFACE(charSequenceTmp));

    String lfp = StringUtils::Format(PROFILER_FINISHED_FMT, formatArgs);
    LogForProfiler(lfp);

    String str;
    mContext->GetString(R::string::profiler_stopped_toast, formatArgs, &str);
    ShowToast(str);
    mIsTracing = FALSE;
    mFilename = NULL;

    return NOERROR;
}

ECode TracingControllerElastos::Finalize()
{
    if (mNativeTracingControllerElastos != 0) {
        NativeDestroy(mNativeTracingControllerElastos);
        mNativeTracingControllerElastos = 0;
    }

    return NOERROR;
}

String TracingControllerElastos::GenerateTracingFilePath()
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    String state;
    env->GetExternalStorageState(&state);
    if (!IEnvironment::MEDIA_MOUNTED.Equals(state)) {
        return String(NULL);
    }

    // Generate a hopefully-unique filename using the UTC timestamp.
    // (Not a huge problem if it isn't unique, we'll just append more data.)
    AutoPtr<ISimpleDateFormat> formatter;
    AutoPtr<ILocaleHelper> localHelper;
    AutoPtr<ILocale> us;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localHelper);
    localHelper->GetUS((ILocale**)&us);
    CSimpleDateFormat::New(
            String("yyyy-MM-dd-HHmmss"), us,
            (ISimpleDateFormat**)&formatter);
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> tz;
    tzHelper->GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    IDateFormat::Probe(formatter)->SetTimeZone(tz);
    AutoPtr<IFile> dir;
    env->GetExternalStoragePublicDirectory(
            IEnvironment::DIRECTORY_DOWNLOADS, (IFile**)&dir);
    AutoPtr<IFile> file;
    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    String formatStr;
    IDateFormat::Probe(formatter)->Format(date, &formatStr);
    String str("chrome-profile-results-");
    str += formatStr;
    CFile::New(dir,  str, (IFile**)&file);
    String path;
    file->GetPath(&path);
    return path;
}

ECode TracingControllerElastos::InitializeNativeControllerIfNeeded()
{
    if (mNativeTracingControllerElastos == 0) {
        mNativeTracingControllerElastos = NativeInit();
    }

    return NOERROR;
}

ECode TracingControllerElastos::LogAndToastError(
    /* [in] */ const String& str)
{
    Slogger::E(TAG, str);
    if (mShowToasts) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        AutoPtr<ICharSequence> cs = CoreUtils::Convert(str);
        helper->MakeText(mContext, cs, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }

    return NOERROR;
}

ECode TracingControllerElastos::LogForProfiler(
    /* [in] */ const String& str)
{
    Slogger::I(TAG, str);
    return NOERROR;
}

ECode TracingControllerElastos::ShowToast(
    /* [in] */ const String& str)
{
    if (mShowToasts) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        AutoPtr<ICharSequence> cs = CoreUtils::Convert(str);
        helper->MakeText(mContext, cs, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }

    return NOERROR;
}

Handle64 TracingControllerElastos::NativeInit()
{
    return Elastos_TracingControllerAndroid_nativeInit(THIS_PROBE(IInterface));
}

ECode TracingControllerElastos::NativeDestroy(
    /* [in] */ Handle64 nativeTracingControllerElastos)
{
    Elastos_TracingControllerAndroid_nativeDestroy(THIS_PROBE(IInterface), nativeTracingControllerElastos);
    return NOERROR;
}

Boolean TracingControllerElastos::NativeStartTracing(
    /* [in] */ Handle64 nativeTracingControllerElastos,
    /* [in] */ const String& categories,
    /* [in] */ Boolean recordContinuously)
{
    return Elastos_TracingControllerAndroid_nativeStartTracing(THIS_PROBE(IInterface),
        nativeTracingControllerElastos, categories, recordContinuously);
}

ECode TracingControllerElastos::NativeStopTracing(
    /* [in] */ Handle64 nativeTracingControllerElastos,
    /* [in] */ const String& filename)
{
    Elastos_TracingControllerAndroid_nativeStopTracing(THIS_PROBE(IInterface),
            nativeTracingControllerElastos, filename);
    return NOERROR;
}

Boolean TracingControllerElastos::NativeGetKnownCategoryGroupsAsync(
    /* [in] */ Handle64 nativeTracingControllerElastos)
{
    return Elastos_TracingControllerAndroid_nativeGetKnownCategoryGroupsAsync(THIS_PROBE(IInterface),
            nativeTracingControllerElastos);
}

String TracingControllerElastos::NativeGetDefaultCategories()
{
    return Elastos_TracingControllerAndroid_nativeGetDefaultCategories(THIS_PROBE(IInterface));
}

void TracingControllerElastos::OnTracingStopped(
    /* [in] */ IInterface* obj)
{
    AutoPtr<TracingControllerElastos> mObj = (TracingControllerElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "TracingControllerElastos::OnTracingStopped, mObj is NULL");
        return;
    }
    mObj->OnTracingStopped();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
