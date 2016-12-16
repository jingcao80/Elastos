
#include "elastos/droid/systemui/recents/misc/SystemServicesProxy.h"
#include "elastos/droid/systemui/recents/misc/Console.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include "../R.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityManagerRecentTaskInfo;
using Elastos::Droid::App::CActivityManagerTaskDescription;
using Elastos::Droid::App::IActivityManagerRecentTaskInfo;
using Elastos::Droid::App::IActivityManagerTaskDescription;
using Elastos::Droid::App::IActivityManagerTaskThumbnail;
using Elastos::Droid::AppWidget::CAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::IAppWidgetManagerHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::CActivityInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::PorterDuffMode_DST_ATOP;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Utility::CPair;
using Elastos::Droid::View::CDisplayInfo;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Droid::View::IDisplayInfo;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CRandom;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IRandom;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

static AutoPtr<IBitmapFactoryOptions> InitBitmapOptions()
{
    AutoPtr<IBitmapFactoryOptions> bfo;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&bfo);
    bfo->SetInMutable(TRUE);
    return bfo;
}

const String SystemServicesProxy::TAG("SystemServicesProxy");
const AutoPtr<IBitmapFactoryOptions> SystemServicesProxy::sBitmapOptions = InitBitmapOptions();

SystemServicesProxy::SystemServicesProxy(
    /* [in] */ IContext* context)
{
    AutoPtr<IAccessibilityManagerHelper> amh;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&amh);
    amh->GetInstance(context, (IAccessibilityManager**)&mAccm);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    mAm = IActivityManager::Probe(obj);
    mIam = ActivityManagerNative::GetDefault();
    AutoPtr<IAppWidgetManagerHelper> awmh;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&awmh);
    awmh->GetInstance(context, (IAppWidgetManager**)&mAwm);
    context->GetPackageManager((IPackageManager**)&mPm);
    mIpm = AppGlobals::GetPackageManager();
    obj = NULL;
    context->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    mSm = ISearchManager::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    mWm = IWindowManager::Probe(obj);
    mWm->GetDefaultDisplay((IDisplay**)&mDisplay);
    context->GetPackageName(&mRecentsPackage);

    // Get the dummy thumbnail width/heights
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 wId = Elastos::Droid::R::dimen::thumbnail_width;
    Int32 hId = Elastos::Droid::R::dimen::thumbnail_height;
    res->GetDimensionPixelSize(wId, &mDummyThumbnailWidth);
    res->GetDimensionPixelSize(hId, &mDummyThumbnailHeight);

    // Create the protection paints
    CPaint::New((IPaint**)&mBgProtectionPaint);
    AutoPtr<IXfermode> xfermode;
    CPorterDuffXfermode::New(PorterDuffMode_DST_ATOP, (IXfermode**)&xfermode);
    mBgProtectionPaint->SetXfermode(xfermode);
    mBgProtectionPaint->SetColor(0xFFffffff);
    CCanvas::New((ICanvas**)&mBgProtectionCanvas);

    // Resolve the assist intent
    if (mSm != NULL) {
        AutoPtr<IIntent> assist;
        mSm->GetAssistIntent(context, FALSE, (IIntent**)&assist);
        if (assist != NULL) {
            assist->GetComponent((IComponentName**)&mAssistComponent);
        }
    }

    if (Constants::DebugFlags::App::EnableSystemServicesProxy) {
        // Create a dummy icon
        AutoPtr<IBitmapHelper> bh;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bh);
        bh->CreateBitmap(1, 1, BitmapConfig_ARGB_8888, (IBitmap**)&mDummyIcon);
        mDummyIcon->EraseColor(0xFF999999);
    }
}

AutoPtr<IList> SystemServicesProxy::GetRecentTasks(
    /* [in] */ Int32 numLatestTasks,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean isTopTaskHome)
{
    if (mAm == NULL) return NULL;

    // If we are mocking, then create some recent tasks
    if (Constants::DebugFlags::App::EnableSystemServicesProxy) {
        // ArrayList<ActivityManager.RecentTaskInfo> tasks =
        //         new ArrayList<ActivityManager.RecentTaskInfo>();
        AutoPtr<IArrayList> tasks;
        CArrayList::New((IArrayList**)&tasks);
        Int32 count = Elastos::Core::Math::Min(numLatestTasks, Constants::DebugFlags::App::SystemServicesProxyMockTaskCount);
        for (Int32 i = 0; i < count; i++) {
            // Create a dummy component name
            Int32 packageIndex = i % Constants::DebugFlags::App::SystemServicesProxyMockPackageCount;
            AutoPtr<IComponentName> cn;
            StringBuilder sb1("com.android.test");
            sb1 += packageIndex;
            StringBuilder sb2("com.android.test");
            sb2 += i;
            sb2 += ".Activity";
            CComponentName::New(sb1.ToString(), sb2.ToString(), (IComponentName**)&cn);
            AutoPtr<IRandom> random;
            CRandom::New((IRandom**)&random);
            Int64 value;
            random->NextInt64(&value);
            Int64 l = Elastos::Core::Math::Abs(value);
            StringBuilder sb3("");
            sb3 += i;
            sb3 += " - ";
            sb3 += StringUtils::ToString(l, 36);
            String description = sb3.ToString();
            // Create the recent task info
            AutoPtr<IActivityManagerRecentTaskInfo> rti;
            CActivityManagerRecentTaskInfo::New((IActivityManagerRecentTaskInfo**)&rti);
            rti->SetPersistentId(i);
            Int32 pid;
            rti->GetPersistentId(&pid);
            rti->SetId(pid);
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            rti->SetBaseIntent(intent);
            AutoPtr<IIntent> baseIntent;
            rti->GetBaseIntent((IIntent**)&baseIntent);
            baseIntent->SetComponent(cn);
            AutoPtr<ICharSequence> csDescription;
            CString::New(description, (ICharSequence**)&csDescription);
            rti->SetDescription(csDescription);
            rti->SetLastActiveTime(i);
            Int64 lastActiveTime;
            rti->GetLastActiveTime(&lastActiveTime);
            rti->SetFirstActiveTime(lastActiveTime);
            if (i % 2 == 0) {
                AutoPtr<IRandom> random;
                CRandom::New((IRandom**)&random);
                Int32 val;
                random->NextInt32(&val);
                AutoPtr<IBitmapHelper> bh;
                CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bh);
                AutoPtr<IBitmap> bitmap;
                bh->CreateBitmap(mDummyIcon, (IBitmap**)&bitmap);
                AutoPtr<IActivityManagerTaskDescription> amt;
                CActivityManagerTaskDescription::New(description, bitmap, 0xFF000000 | (0xFFFFFF & val),
                    (IActivityManagerTaskDescription**)&amt);
                rti->SetTaskDescription(amt);
            }
            else {
                AutoPtr<IActivityManagerTaskDescription> amt;
                CActivityManagerTaskDescription::New((IActivityManagerTaskDescription**)&amt);
                rti->SetTaskDescription(amt);
            }
            tasks->Add(rti);
        }
        return IList::Probe(tasks);
    }

    // Remove home/recents/excluded tasks
    Int32 minNumTasksToQuery = 10;
    Int32 numTasksToQuery = Elastos::Core::Math::Max(minNumTasksToQuery, numLatestTasks);
    AutoPtr<IList> tasks;
    mAm->GetRecentTasksForUser(numTasksToQuery,
        IActivityManager::RECENT_IGNORE_HOME_STACK_TASKS |
        IActivityManager::RECENT_IGNORE_UNAVAILABLE |
        IActivityManager::RECENT_INCLUDE_PROFILES |
        IActivityManager::RECENT_WITH_EXCLUDED, userId, (IList**)&tasks);

    // Break early if we can't get a valid set of tasks
    AutoPtr<IList> list;
    if (tasks == NULL) {
        CArrayList::New((IList**)&list);
        return list;
    }

    Boolean isFirstValidTask = TRUE;
    AutoPtr<IIterator> iter;
    IIterable::Probe(tasks)->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<IActivityManagerRecentTaskInfo> t = IActivityManagerRecentTaskInfo::Probe(obj);

        // NOTE: The order of these checks happens in the expected order of the traversal of the
        // tasks

        // Check the first non-recents task, include this task even if it is marked as excluded
        // from recents if we are currently in the app.  In other words, only remove excluded
        // tasks if it is not the first active task.
        AutoPtr<IIntent> baseIntent;
        t->GetBaseIntent((IIntent**)&baseIntent);
        Int32 flags;
        baseIntent->GetFlags(&flags);
        Boolean isExcluded = (flags & IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS)
                == IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS;
        if (isExcluded && (isTopTaskHome || !isFirstValidTask)) {
            iter->Remove();
            continue;
        }
        isFirstValidTask = FALSE;
    }

    Int32 size;
    tasks->GetSize(&size);
    tasks->GetSubList(0, Elastos::Core::Math::Min(size, numLatestTasks), (IList**)&list);
    return list;
}

AutoPtr<IList> SystemServicesProxy::GetRunningTasks(
    /* [in] */ Int32 numTasks)
{
    if (mAm == NULL) return NULL;
    AutoPtr<IList> tasks;
    mAm->GetRunningTasks(numTasks, (IList**)&tasks);
    return tasks;
}

Boolean SystemServicesProxy::IsInHomeStack(
    /* [in] */ Int32 taskId)
{
    if (mAm == NULL) {
        return FALSE;
    }

    // If we are mocking, then just return false
    if (Constants::DebugFlags::App::EnableSystemServicesProxy) {
        return FALSE;
    }
    Boolean result;
    mAm->IsInHomeStack(taskId, &result);
    return result;
}

AutoPtr<IBitmap> SystemServicesProxy::GetTaskThumbnail(
    /* [in] */ Int32 taskId)
{
    if (mAm == NULL) return NULL;

    // If we are mocking, then just return a dummy thumbnail
    if (Constants::DebugFlags::App::EnableSystemServicesProxy) {
        AutoPtr<IBitmapHelper> bh;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bh);
        AutoPtr<IBitmap> thumbnail;
        bh->CreateBitmap(mDummyThumbnailWidth, mDummyThumbnailHeight,
                BitmapConfig_ARGB_8888, (IBitmap**)&thumbnail);
        thumbnail->EraseColor(0xff333333);
        return thumbnail;
    }

    AutoPtr<IBitmap> thumbnail = GetThumbnail(mAm, taskId);
    if (thumbnail != NULL) {
        // We use a dumb heuristic for now, if the thumbnail is purely transparent in the top
        // left pixel, then assume the whole thumbnail is transparent. Generally, proper
        // screenshots are always composed onto a bitmap that has no alpha.
        Int32 pixel;
        thumbnail->GetPixel(0, 0, &pixel);
        AutoPtr<IColor> color;
        CColor::AcquireSingleton((IColor**)&color);
        Int32 alpha;
        color->Alpha(pixel, &alpha);
        if (alpha == 0) {
            mBgProtectionCanvas->SetBitmap(thumbnail);
            Int32 width, height;
            thumbnail->GetWidth(&width);
            thumbnail->GetHeight(&height);
            mBgProtectionCanvas->DrawRect(0.0f, 0.0f, width, height, mBgProtectionPaint);
            mBgProtectionCanvas->SetBitmap(NULL);
            Logger::E(TAG, "Invalid screenshot detected from getTaskThumbnail()");
        }
    }
    return thumbnail;
}

AutoPtr<IBitmap> SystemServicesProxy::GetThumbnail(
    /* [in] */ IActivityManager* activityManager,
    /* [in] */ Int32 taskId)
{
    AutoPtr<IActivityManagerTaskThumbnail> taskThumbnail;
    activityManager->GetTaskThumbnail(taskId, (IActivityManagerTaskThumbnail**)&taskThumbnail);
    if (taskThumbnail == NULL) return NULL;

    AutoPtr<IBitmap> thumbnail;
    taskThumbnail->GetMainThumbnail((IBitmap**)&thumbnail);
    AutoPtr<IParcelFileDescriptor> descriptor;
    taskThumbnail->GetThumbnailFileDescriptor((IParcelFileDescriptor**)&descriptor);
    if (thumbnail == NULL && descriptor != NULL) {
        AutoPtr<IFileDescriptor> des;
        descriptor->GetFileDescriptor((IFileDescriptor**)&des);
        AutoPtr<IBitmapFactory> bf;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bf);
        bf->DecodeFileDescriptor(des, NULL, sBitmapOptions, (IBitmap**)&thumbnail);
    }
    if (descriptor != NULL) {
        ECode ec = ICloseable::Probe(descriptor)->Close();
        if (FAILED(ec)) {
            // return E_IO_EXCEPTION;
        }
    }
    return thumbnail;
}

void SystemServicesProxy::MoveTaskToFront(
    /* [in] */ Int32 taskId,
    /* [in] */ IActivityOptions* opts)
{
    if (mAm == NULL) return;
    if (Constants::DebugFlags::App::EnableSystemServicesProxy) return;

    if (opts != NULL) {
        AutoPtr<IBundle> bundle;
        opts->ToBundle((IBundle**)&bundle);
        mAm->MoveTaskToFront(taskId, IActivityManager::MOVE_TASK_WITH_HOME, bundle);
    }
    else {
        mAm->MoveTaskToFront(taskId, IActivityManager::MOVE_TASK_WITH_HOME);
    }
}

void SystemServicesProxy::RemoveTask(
    /* [in] */ Int32 taskId,
    /* [in] */ Boolean isDocument)
{
    if (mAm == NULL) return;
    if (Constants::DebugFlags::App::EnableSystemServicesProxy) return;

    // Remove the task, and only kill the process if it is not a document
    Boolean removed;
    mAm->RemoveTask(taskId, isDocument ? 0 : IActivityManager::REMOVE_TASK_KILL_PROCESS, &removed);
}

AutoPtr<IActivityInfo> SystemServicesProxy::GetActivityInfo(
    /* [in] */ IComponentName* cn,
    /* [in] */ Int32 userId)
{
    if (mIpm == NULL) return NULL;

    AutoPtr<IActivityInfo> ai;
    if (Constants::DebugFlags::App::EnableSystemServicesProxy) {
        CActivityInfo::New((IActivityInfo**)&ai);
        return ai;
    }

    ECode ec = mIpm->GetActivityInfo(cn, IPackageManager::GET_META_DATA, userId, (IActivityInfo**)&ai);
    if (FAILED(ec)) {
        return NULL;
    }
    return ai;
}

AutoPtr<IActivityInfo> SystemServicesProxy::GetActivityInfo(
    /* [in] */ IComponentName* cn)
{
    if (mPm == NULL) return NULL;
    AutoPtr<IActivityInfo> ai;
    if (Constants::DebugFlags::App::EnableSystemServicesProxy) {
        CActivityInfo::New((IActivityInfo**)&ai);
        return ai;
    }
    ECode ec = mPm->GetActivityInfo(cn, IPackageManager::GET_META_DATA, (IActivityInfo**)&ai);
    if (FAILED(ec)) {
        return NULL;
    }
    return ai;
}

String SystemServicesProxy::GetActivityLabel(
    /* [in] */ IActivityInfo* info)
{
    if (mPm == NULL) return String(NULL);

    // If we are mocking, then return a mock label
    if (Constants::DebugFlags::App::EnableSystemServicesProxy) return String("Recent Task");
    AutoPtr<ICharSequence> label;
    IPackageItemInfo::Probe(info)->LoadLabel(mPm, (ICharSequence**)&label);
    String str;
    label->ToString(&str);
    return str;
}

AutoPtr<IDrawable> SystemServicesProxy::GetActivityIcon(
    /* [in] */ IActivityInfo* info,
    /* [in] */ Int32 userId)
{
    if (mPm == NULL) return NULL;

    // If we are mocking, then return a mock label
    AutoPtr<IDrawable> drawable;
    if (Constants::DebugFlags::App::EnableSystemServicesProxy) {
        CColorDrawable::New(0xFF666666, (IDrawable**)&drawable);
        return drawable;
    }

    AutoPtr<IDrawable> icon;
    IPackageItemInfo::Probe(info)->LoadIcon(mPm, (IDrawable**)&icon);
    drawable = GetBadgedIcon(icon, userId);
    return drawable;
}

AutoPtr<IDrawable> SystemServicesProxy::GetBadgedIcon(
    /* [in] */ IDrawable* icon,
    /* [in] */ Int32 userId)
{
    AutoPtr<IDrawable> result = icon;
    if (userId != UserHandle::GetMyUserId()) {
        result = NULL;
        AutoPtr<IUserHandle> uh;
        CUserHandle::New(userId, (IUserHandle**)&uh);
        mPm->GetUserBadgedIcon(icon, uh, (IDrawable**)&result);
    }
    return result;
}

String SystemServicesProxy::GetHomeActivityPackageName()
{
    if (mPm == NULL) return String(NULL);

    if (Constants::DebugFlags::App::EnableSystemServicesProxy) return String(NULL);

    AutoPtr<IArrayList> homeActivities;
    CArrayList::New((IArrayList**)&homeActivities);
    Int32 size;
    homeActivities->GetSize(&size);
    AutoPtr<IComponentName> defaultHomeActivity;
    mPm->GetHomeActivities(IList::Probe(homeActivities), (IComponentName**)&defaultHomeActivity);

    String name;
    if (defaultHomeActivity != NULL) {
        defaultHomeActivity->GetPackageName(&name);
        return name;
    }
    else if (size == 1) {
        AutoPtr<IInterface> obj;
        homeActivities->Get(0, (IInterface**)&obj);
        AutoPtr<IResolveInfo> info = IResolveInfo::Probe(obj);
        AutoPtr<IActivityInfo> ai;
        info->GetActivityInfo((IActivityInfo**)&ai);
        if (ai != NULL) {
            IPackageItemInfo::Probe(ai)->GetPackageName(&name);
            return name;
        }
    }
    return String(NULL);
}

AutoPtr<IAppWidgetProviderInfo> SystemServicesProxy::ResolveSearchAppWidget()
{
    if (mAwm == NULL) return NULL;
    if (mAssistComponent == NULL) return NULL;

    // Find the first Recents widget from the same package as the global assist activity
    AutoPtr<IList> widgets;
    mAwm->GetInstalledProviders(IAppWidgetProviderInfo::WIDGET_CATEGORY_SEARCHBOX,
        (IList**)&widgets);
    AutoPtr<IIterator> iter;
    IIterable::Probe(widgets)->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(obj);
        AutoPtr<IComponentName> provider;
        info->GetProvider((IComponentName**)&provider);
        String packageName1, packageName2;
        provider->GetPackageName(&packageName1);
        mAssistComponent->GetPackageName(&packageName2);
        if (packageName1.Equals(packageName2)) return info;
    }
    return NULL;
}

AutoPtr<IPair> SystemServicesProxy::BindSearchAppWidget(
    /* [in] */ IAppWidgetHost* host)
{
    if (mAwm == NULL) return NULL;
    if (mAssistComponent == NULL) return NULL;

    // Find the first Recents widget from the same package as the global assist activity
    AutoPtr<IAppWidgetProviderInfo> searchWidgetInfo = ResolveSearchAppWidget();

    // Return early if there is no search widget
    if (searchWidgetInfo == NULL) return NULL;

    // Allocate a new widget id and try and bind the app widget (if that fails, then just skip)
    Int32 searchWidgetId;
    host->AllocateAppWidgetId(&searchWidgetId);
    AutoPtr<IBundle> opts;
    CBundle::New((IBundle**)&opts);
    opts->PutInt32(IAppWidgetManager::OPTION_APPWIDGET_HOST_CATEGORY,
            IAppWidgetProviderInfo::WIDGET_CATEGORY_SEARCHBOX);
    AutoPtr<IComponentName> provider;
    searchWidgetInfo->GetProvider((IComponentName**)&provider);
    Boolean binded;
    mAwm->BindAppWidgetIdIfAllowed(searchWidgetId, provider, opts, &binded);
    if (!binded) return NULL;
    AutoPtr<IInteger32> a;
    CInteger32::New(searchWidgetId, (IInteger32**)&a);
    AutoPtr<IPair> result;
    CPair::New(a, searchWidgetInfo, (IPair**)&result);
    return result;
}

AutoPtr<IAppWidgetProviderInfo> SystemServicesProxy::GetAppWidgetInfo(
    /* [in] */ Int32 appWidgetId)
{
    if (mAwm == NULL) return NULL;
    AutoPtr<IAppWidgetProviderInfo> result;
    mAwm->GetAppWidgetInfo(appWidgetId, (IAppWidgetProviderInfo**)&result);
    return result;
}

void SystemServicesProxy::UnbindSearchAppWidget(
    /* [in] */ IAppWidgetHost* host,
    /* [in] */ Int32 appWidgetId)
{
    if (mAwm == NULL) return;

    // Delete the app widget
    host->DeleteAppWidgetId(appWidgetId);
}

Boolean SystemServicesProxy::IsTouchExplorationEnabled()
{
    if (mAccm == NULL) {
        return FALSE;
    }
    Boolean b1, b2;
    mAccm->IsEnabled(&b1);
    mAccm->IsTouchExplorationEnabled(&b2);
    return b1 && b2;
}

Int32 SystemServicesProxy::GetGlobalSetting(
    /* [in] */ IContext* context,
    /* [in] */ const String& setting)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Int32 result;
    sg->GetInt32(cr, setting, 0, &result);
    return result;
}

Int32 SystemServicesProxy::GetSystemSetting(
    /* [in] */ IContext* context,
    /* [in] */ const String& setting)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    Int32 result;
    ss->GetInt32(cr, setting, 0, &result);
    return result;
}

AutoPtr<IRect> SystemServicesProxy::GetWindowRect()
{
    AutoPtr<IRect> windowRect;
    CRect::New((IRect**)&windowRect);
    if (mWm == NULL) {
        return windowRect;
    }

    AutoPtr<IPoint> p;
    CPoint::New((IPoint**)&p);
    AutoPtr<IDisplay> display;
    mWm->GetDefaultDisplay((IDisplay**)&display);
    display->GetRealSize(p);
    Int32 x, y;
    p->GetX(&x);
    p->GetY(&y);
    windowRect->Set(0, 0, x, y);
    return windowRect;
}

void SystemServicesProxy::LockCurrentTask()
{
    if (mIam == NULL) return;
    mIam->StartLockTaskModeOnCurrent();
}

AutoPtr<IBitmap> SystemServicesProxy::TakeScreenshot()
{
    AutoPtr<IDisplayInfo> di;
    CDisplayInfo::New((IDisplayInfo**)&di);
    Boolean isValid;
    mDisplay->GetDisplayInfo(di, &isValid);
    Int32 width, height;
    di->GetNaturalWidth(&width);
    di->GetNaturalHeight(&height);
    AutoPtr<ISurfaceControlHelper> sch;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&sch);
    AutoPtr<IBitmap> result;
    sch->Screenshot(width, height, (IBitmap**)&result);
    return result;
}

AutoPtr<IBitmap> SystemServicesProxy::TakeAppScreenshot()
{
    return TakeScreenshot();
}

Boolean SystemServicesProxy::StartActivityFromRecents(
    /* [in] */ IContext* context,
    /* [in] */ Int32 taskId,
    /* [in] */ const String& taskName,
    /* [in] */ IActivityOptions* options)
{
    if (mIam != NULL) {
        AutoPtr<IBundle> bundle;
        options->ToBundle((IBundle**)&bundle);
        Int32 i;
        ECode ec = mIam->StartActivityFromRecents(taskId, options == NULL ? NULL : bundle, &i);
        if (FAILED(ec)) {
            AutoPtr<ArrayOf<IInterface*> > params = ArrayOf<IInterface*>::Alloc(1);
            params->Set(0, CoreUtils::Convert(taskName));
            String str;
            context->GetString(R::string::recents_launch_error_message, params, &str);
            Logger::E(TAG, str.string());
            return ec;
        }
        return TRUE;
    }
    return FALSE;
}

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos