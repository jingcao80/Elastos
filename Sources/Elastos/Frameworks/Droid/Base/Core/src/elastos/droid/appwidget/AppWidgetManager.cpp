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

#include "elastos/droid/appwidget/AppWidgetManager.h"
#include "elastos/droid/os/CBundleHelper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/utility/CTypedValue.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Os::IBundleHelper;
using Elastos::Droid::Os::CBundleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace AppWidget {

CAR_INTERFACE_IMPL(AppWidgetManager, Object, IAppWidgetManager);

AppWidgetManager::AppWidgetManager()
    : mPackageName(NULL)
{}

AppWidgetManager::~AppWidgetManager()
{}

AutoPtr<IAppWidgetManager> AppWidgetManager::GetInstance(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> object;
    context->GetSystemService(IContext::APPWIDGET_SERVICE, (IInterface**)&object);
    AutoPtr<IAppWidgetManager> manager = IAppWidgetManager::Probe(object);

    return manager;
}

ECode AppWidgetManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIAppWidgetService* service)
{
    context->GetOpPackageName(&mPackageName);
    mService = service;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDisplayMetrics((IDisplayMetrics**)&mDisplayMetrics);
    return NOERROR;
}

ECode AppWidgetManager::UpdateAppWidget(
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ IRemoteViews* views)
{
    if (mService == NULL) {
        return NOERROR;
    }
    //try {
    ECode ec = mService->UpdateAppWidgetIds(mPackageName, appWidgetIds, views);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetManager::UpdateAppWidgetOptions(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IBundle* options)
{
    if (mService == NULL) {
        return NOERROR;
    }
    //try {
    ECode ec = mService->UpdateAppWidgetOptions(mPackageName, appWidgetId, options);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    //}
    //catch (RemoteException e) {
    //    throw new RuntimeException("system server dead?", e);
    //}
    return NOERROR;
}

ECode AppWidgetManager::GetAppWidgetOptions(
    /* [in] */ Int32 appWidgetId,
    /* [out] */ IBundle** options)
{
    VALIDATE_NOT_NULL(options);
    if (mService == NULL) {
        AutoPtr<IBundleHelper> helper;
        CBundleHelper::AcquireSingleton((IBundleHelper**)&helper);
        return helper->GetEMPTY(options);
    }
    //try {
    ECode ec = mService->GetAppWidgetOptions(mPackageName, appWidgetId, options);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    //}
    //catch (RemoteException e) {
    //    throw new RuntimeException("system server dead?", e);
    //}
    return NOERROR;
}

ECode AppWidgetManager::UpdateAppWidget(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IRemoteViews* views)
{
    if (mService == NULL) {
        return NOERROR;
    }
    AutoPtr< ArrayOf<Int32> > idsArray = ArrayOf<Int32>::Alloc(1);
    (*idsArray)[0] = appWidgetId;
    return UpdateAppWidget(idsArray, views);
}

ECode AppWidgetManager::PartiallyUpdateAppWidget(
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ IRemoteViews* views)
{
    if (mService == NULL) {
        return NOERROR;
    }
    //try {
    ECode ec = mService->PartiallyUpdateAppWidgetIds(mPackageName, appWidgetIds, views);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    //} catch (RemoteException e) {
    //    throw new RuntimeException("system server dead?", e);
    //}
    return NOERROR;
}

ECode AppWidgetManager::PartiallyUpdateAppWidget(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IRemoteViews* views)
{
    if (mService == NULL) {
        return NOERROR;
    }
    AutoPtr< ArrayOf<Int32> > idsArray = ArrayOf<Int32>::Alloc(1);
    (*idsArray)[0] = appWidgetId;
    return PartiallyUpdateAppWidget(idsArray, views);
}

ECode AppWidgetManager::UpdateAppWidget(
    /* [in] */ IComponentName* provider,
    /* [in] */ IRemoteViews* views)
{
    if (mService == NULL) {
        return NOERROR;
    }
    //try {
    ECode ec = mService->UpdateAppWidgetProvider(provider, views);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetManager::NotifyAppWidgetViewDataChanged(
    /* [in] */ ArrayOf<Int32>* appWidgetIds,
    /* [in] */ Int32 viewId)
{
    if (mService == NULL) {
        return NOERROR;
    }
    //try {
    ECode ec = mService->NotifyAppWidgetViewDataChanged(mPackageName, appWidgetIds, viewId);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    //}
    //catch (RemoteException e) {
    //    throw new RuntimeException("system server dead?", e);
    //}
    return NOERROR;
}

ECode AppWidgetManager::NotifyAppWidgetViewDataChanged(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 viewId)
{
    if (mService == NULL) {
        return NOERROR;
    }
    AutoPtr< ArrayOf<Int32> > idsArray = ArrayOf<Int32>::Alloc(1);
    (*idsArray)[0] = appWidgetId;
    return NotifyAppWidgetViewDataChanged(idsArray, viewId);
}

ECode AppWidgetManager::GetInstalledProvidersForProfile(
    /* [in] */ IUserHandle* profile,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (mService == NULL) {
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        return collections->GetEmptyList(result);
    }

    return GetInstalledProvidersForProfile(
            IAppWidgetProviderInfo::WIDGET_CATEGORY_HOME_SCREEN, profile, result);
}

ECode AppWidgetManager::GetInstalledProviders(
    /* [out] */ IList** providerInfos)
{
    VALIDATE_NOT_NULL(providerInfos);
    if (mService == NULL) {
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        return collections->GetEmptyList(providerInfos);
    }

    return GetInstalledProvidersForProfile(
            IAppWidgetProviderInfo::WIDGET_CATEGORY_HOME_SCREEN, NULL, providerInfos);
}

ECode AppWidgetManager::GetInstalledProviders(
    /* [in] */ Int32 categoryFilter,
    /* [out] */ IList** providerInfos)
{
    VALIDATE_NOT_NULL(providerInfos);
    if (mService == NULL) {
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        return collections->GetEmptyList(providerInfos);
    }

    return GetInstalledProvidersForProfile(categoryFilter, NULL, providerInfos);
}

ECode AppWidgetManager::GetInstalledProvidersForProfile(
    /* [in] */ Int32 categoryFilter,
    /* [in] */ IUserHandle* profile,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    if (mService == NULL) {
        return collections->GetEmptyList(result);
    }

    if (profile == NULL) {
        AutoPtr<IUserHandle> handle;
        Process::MyUserHandle((IUserHandle**)&handle);
        profile = handle;
    }

    // try {
    Int32 profileId;
    profile->GetIdentifier(&profileId);
    AutoPtr<IList> providers;
    ECode ec = mService->GetInstalledProvidersForProfile(
            categoryFilter, profileId, (IList**)&providers);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    if (providers == NULL) {
        return collections->GetEmptyList(result);
    }

    Int32 size;
    providers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        providers->Get(i, (IInterface**)&obj);
        AutoPtr<IAppWidgetProviderInfo> info = IAppWidgetProviderInfo::Probe(obj);
        // Converting complex to dp.
        ConvertSizesToPixels(info);
    }
    *result = providers;
    REFCOUNT_ADD(*result);
    return NOERROR;
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
}

ECode AppWidgetManager::GetAppWidgetInfo(
    /* [in] */ Int32 appWidgetId,
    /* [out] */ IAppWidgetProviderInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    if (mService == NULL) {
        return NOERROR;
    }

    //try {
    AutoPtr<IAppWidgetProviderInfo> _info;
    ECode ec = mService->GetAppWidgetInfo(mPackageName, appWidgetId, (IAppWidgetProviderInfo**)&_info);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }

    if (_info != NULL) {
        // Converting complex to dp.
        ConvertSizesToPixels(_info);
    }
    *info = _info;
    REFCOUNT_ADD(*info);
    return NOERROR;
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
}

ECode AppWidgetManager::BindAppWidgetId(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* provider)
{
    if (mService == NULL) {
        return NOERROR;
    }

    return BindAppWidgetId(appWidgetId, provider, NULL);
}

ECode AppWidgetManager::BindAppWidgetId(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* provider,
    /* [in] */ IBundle* options)
{
    if (mService == NULL) {
        return NOERROR;
    }

    AutoPtr<IUserHandle> handle;
    Process::MyUserHandle((IUserHandle**)&handle);

    Boolean res;
    BindAppWidgetIdIfAllowed(appWidgetId, handle, provider, options, &res);
    return NOERROR;
}

ECode AppWidgetManager::BindAppWidgetIdIfAllowed(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* provider,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mService == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    return BindAppWidgetIdIfAllowed(appWidgetId, UserHandle::GetMyUserId(),
            provider, NULL, result);
}

ECode AppWidgetManager::BindAppWidgetIdIfAllowed(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* provider,
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mService == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    return BindAppWidgetIdIfAllowed(
            appWidgetId, UserHandle::GetMyUserId(), provider, options, result);
}

ECode AppWidgetManager::BindAppWidgetIdIfAllowed(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IUserHandle* user,
    /* [in] */ IComponentName* provider,
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mService == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 profileId;
    user->GetIdentifier(&profileId);

    return BindAppWidgetIdIfAllowed(appWidgetId, profileId, provider, options, result);
}

ECode AppWidgetManager::HasBindAppWidgetPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mService == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    // try {
    ECode ec = mService->HasBindAppWidgetPermission(packageName, userId, result);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetManager::HasBindAppWidgetPermission(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mService == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    //try {
    ECode ec = mService->HasBindAppWidgetPermission(packageName, UserHandle::GetMyUserId(), result);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    //}
    //catch (RemoteException e) {
    //    throw new RuntimeException("system server dead?", e);
    //}
    return NOERROR;
}

ECode AppWidgetManager::SetBindAppWidgetPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean permission)
{
    if (mService == NULL) {
        return NOERROR;
    }
    return SetBindAppWidgetPermission(packageName, UserHandle::GetMyUserId(), permission);
}

ECode AppWidgetManager::SetBindAppWidgetPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean permission)
{
    if (mService == NULL) {
        return NOERROR;
    }

    // try {
    ECode ec = mService->SetBindAppWidgetPermission(packageName, userId, permission);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetManager::BindRemoteViewsService(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IIntent* intent,
    /* [in] */ IBinder* connection)
{
    if (mService == NULL) {
        return NOERROR;
    }

    // try {
    ECode ec = mService->BindRemoteViewsService(packageName, appWidgetId, intent, connection);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetManager::UnbindRemoteViewsService(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IIntent* intent)
{
    if (mService == NULL) {
        return NOERROR;
    }

    // try {
    ECode ec = mService->UnbindRemoteViewsService(packageName, appWidgetId, intent);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetManager::GetAppWidgetIds(
    /* [in] */ IComponentName* provider,
    /* [out, callee] */ ArrayOf<Int32>** appWidgetIds)
{
    VALIDATE_NOT_NULL(appWidgetIds);

    if (mService == NULL) {
        AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(0);
        *appWidgetIds = args;
        REFCOUNT_ADD(*appWidgetIds);
        return NOERROR;
    }

    //try {
    ECode ec = mService->GetAppWidgetIds(provider, appWidgetIds);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

ECode AppWidgetManager::BindAppWidgetIdIfAllowed(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ Int32 profileId,
    /* [in] */ IComponentName* provider,
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mService == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    // try {
    ECode ec = mService->BindAppWidgetId(mPackageName, appWidgetId,
            profileId, provider, options, result);
    if (FAILED(ec)) {
        Logger::E("AppWidgetManager", "system server dead? ec=%08x", ec);
        return E_RUNTIME_EXCEPTION;
    }
    // }
    // catch (RemoteException e) {
    //     throw new RuntimeException("system server dead?", e);
    // }
    return NOERROR;
}

void AppWidgetManager::ConvertSizesToPixels(
    /* [in] */ IAppWidgetProviderInfo* info)
{
    // Converting complex to dp.
    Int32 minWidth, minHeight, minResizeWidth, minResizeHeight;
    info->GetMinWidth(&minWidth);
    info->GetMinHeight(&minHeight);
    info->GetMinResizeWidth(&minResizeWidth);
    info->GetMinResizeHeight(&minResizeHeight);

    info->SetMinWidth(CTypedValue::ComplexToDimensionPixelSize(minWidth, mDisplayMetrics));
    info->SetMinHeight(CTypedValue::ComplexToDimensionPixelSize(minHeight, mDisplayMetrics));
    info->SetMinResizeWidth(CTypedValue::ComplexToDimensionPixelSize(minResizeWidth, mDisplayMetrics));
    info->SetMinResizeHeight(CTypedValue::ComplexToDimensionPixelSize(minResizeHeight, mDisplayMetrics));
}

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos
