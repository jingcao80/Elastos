
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/res/CThemeManager.h"
#include "elastos/droid/content/res/CThemeChangeRequestBuilder.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

const String CThemeManager::TAG("ThemeManager");

CThemeManager::CThemeManager()
{
    CThemeChangeListener::New((Handle64)this, (IIThemeChangeListener**)&mThemeChangeListener);
    CThemeProcessingListener::New((Handle64)this, (IIThemeProcessingListener**)&mThemeProcessingListener);
}

CAR_INTERFACE_IMPL(CThemeManager, Object, IThemeManager)

CAR_OBJECT_IMPL(CThemeManager)

ECode CThemeManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIThemeService* service)
{
    mContext = context;
    mService = service;
    AutoPtr<ILooper> looper;

    CHandler::New(Looper::GetMainLooper(), (IHandler**)&mHandler);
    return NOERROR;
}

ECode CThemeManager::AddClient(
    /* [in] */ IThemeChangeListener* listener)
{
    {    AutoLock syncLock(mChangeListenersLock);
        if (mChangeListeners.Find(listener) != mChangeListeners.End()) {
            Logger::E(TAG, "Client was already added ");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (mChangeListeners.IsEmpty()) {
            // try {
            if (FAILED(mService->RequestThemeChangeUpdates(mThemeChangeListener))) {
                Logger::W(TAG, "Unable to register listener");
            }
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Unable to register listener", e);
            // }
        }
        mChangeListeners.Insert(listener);
    }
    return NOERROR;
}

ECode CThemeManager::RemoveClient(
    /* [in] */ IThemeChangeListener* listener)
{
    {    AutoLock syncLock(mChangeListenersLock);
        mChangeListeners.Erase(listener);
        if (mChangeListeners.IsEmpty()) {
            // try {
            if (FAILED(mService->RemoveUpdates(mThemeChangeListener))) {
                Logger::W(TAG, "Unable to remove listener");
            }
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Unable to remove listener", e);
            // }
        }
    }
    return NOERROR;
}

ECode CThemeManager::OnClientPaused(
    /* [in] */ IThemeChangeListener* listener)
{
    return RemoveClient(listener);
}

ECode CThemeManager::OnClientResumed(
    /* [in] */ IThemeChangeListener* listener)
{
    return AddClient(listener);
}

ECode CThemeManager::OnClientDestroyed(
    /* [in] */ IThemeChangeListener* listener)
{
    return RemoveClient(listener);
}

ECode CThemeManager::RegisterProcessingListener(
    /* [in] */ IThemeProcessingListener* listener)
{
    {    AutoLock syncLock(mProcessingListenersLock);
        if (mProcessingListeners.Find(listener) != mProcessingListeners.End()) {
            Logger::E(TAG, "Listener was already added ");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (mProcessingListeners.IsEmpty()) {
            // try {
            if (FAILED(mService->RegisterThemeProcessingListener(mThemeProcessingListener))) {
                Logger::W(TAG, "Unable to register listener");
            }
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Unable to register listener", e);
            // }
        }
        mProcessingListeners.Insert(listener);
    }
    return NOERROR;
}

ECode CThemeManager::UnregisterProcessingListener(
    /* [in] */ IThemeProcessingListener* listener)
{
    {    AutoLock syncLock(mProcessingListenersLock);
        mProcessingListeners.Erase(listener);
        if (mProcessingListeners.IsEmpty()) {
            // try {
            if (FAILED(mService->UnregisterThemeProcessingListener(mThemeProcessingListener))) {
                Logger::W(TAG, "Unable to remove listener");
            }
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Unable to remove listener", e);
            // }
        }
    }
    return NOERROR;
}

ECode CThemeManager::RequestThemeChange(
    /* [in] */ const String& pkgName)
{
    //List<String> components = ThemeUtils.getSupportedComponents(mContext, pkgName);
    //requestThemeChange(pkgName, components);
    return NOERROR;
}

ECode CThemeManager::RequestThemeChange(
    /* [in] */ const String& pkgName,
    /* [in] */ IList* components)
{
    return RequestThemeChange(pkgName, components, TRUE);
}

ECode CThemeManager::RequestThemeChange(
    /* [in] */ const String& pkgName,
    /* [in] */ IList* components,
    /* [in] */ Boolean removePerAppThemes)
{
    Int32 size;
    components->GetSize(&size);
    AutoPtr<IMap> componentMap;
    CHashMap::New(size, (IMap**)&componentMap);
    AutoPtr<IIterator> it;
    components->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<ICharSequence> value;
        CString::New(pkgName, (ICharSequence**)&value);
        componentMap->Put(next, value);
    }
    return RequestThemeChange(componentMap, removePerAppThemes);
}

ECode CThemeManager::RequestThemeChange(
    /* [in] */ IMap* componentMap)
{
    return RequestThemeChange(componentMap, TRUE);
}

ECode CThemeManager::RequestThemeChange(
    /* [in] */ IMap* componentMap,
    /* [in] */ Boolean removePerAppThemes)
{
    AutoPtr<IThemeChangeRequestBuilder> builder;
    CThemeChangeRequestBuilder::New((IThemeChangeRequestBuilder**)&builder);
    AutoPtr<ISet> keySet;
    componentMap->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        String component;
        ICharSequence::Probe(next)->ToString(&component);
        AutoPtr<IInterface> value;
        componentMap->Get(next, (IInterface**)&value);
        String valueStr;
        ICharSequence::Probe(value)->ToString(&valueStr);
        AutoPtr<IThemeChangeRequestBuilder> b;
        builder->SetComponent(component, valueStr, (IThemeChangeRequestBuilder**)&b);
    }

    AutoPtr<IThemeChangeRequest> request;
    builder->Build((IThemeChangeRequest**)&request);
    return RequestThemeChange(request, removePerAppThemes);
}

ECode CThemeManager::RequestThemeChange(
    /* [in] */ IThemeChangeRequest* request,
    /* [in] */ Boolean removePerAppThemes)
{
    // try {
    if (FAILED(mService->RequestThemeChange(request, removePerAppThemes))) {
        Logger::W(TAG, "Unable to access ThemeService");
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     logThemeServiceException(e);
    // }
}

ECode CThemeManager::ApplyDefaultTheme()
{
    // try {
    if (FAILED(mService->ApplyDefaultTheme())) {
        Logger::W(TAG, "Unable to access ThemeService");
    }
    // } catch (RemoteException e) {
    //     logThemeServiceException(e);
    // }
    return NOERROR;
}

ECode CThemeManager::IsThemeApplying(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    if (FAILED(mService->IsThemeApplying(result))) {
        *result = FALSE;
        Logger::W(TAG, "Unable to access ThemeService");
    }
    // } catch (RemoteException e) {
    //     logThemeServiceException(e);
    // }
    return NOERROR;
}

ECode CThemeManager::IsThemeBeingProcessed(
    /* [in] */ const String& themePkgName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    if (FAILED(mService->IsThemeBeingProcessed(themePkgName, result))) {
        *result = FALSE;
        Logger::W(TAG, "Unable to access ThemeService");
    }
    // } catch (RemoteException e) {
    //     logThemeServiceException(e);
    // }
    return NOERROR;
}

ECode CThemeManager::GetProgress(
    /* [out] */ Int32* progress)
{
    VALIDATE_NOT_NULL(progress)
    // try {
    if (FAILED(mService->GetProgress(progress))) {
        *progress = -1;
        Logger::W(TAG, "Unable to access ThemeService");
    }
    // } catch (RemoteException e) {
    //     logThemeServiceException(e);
    // }
    return NOERROR;
}

ECode CThemeManager::ProcessThemeResources(
    /* [in] */ const String& themePkgName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    if (FAILED(mService->ProcessThemeResources(themePkgName, result))) {
        *result = FALSE;
        Logger::W(TAG, "Unable to access ThemeService");
    }
    // } catch (RemoteException e) {
    //     logThemeServiceException(e);
    // }
    return NOERROR;
}

}
}
}
}
