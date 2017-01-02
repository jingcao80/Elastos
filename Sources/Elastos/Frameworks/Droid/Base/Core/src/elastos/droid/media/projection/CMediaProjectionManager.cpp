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

#include "Elastos.Droid.App.h"
#include "elastos/droid/media/projection/CMediaProjectionManager.h"
#include "elastos/droid/utility/CArrayMap.h"

using Elastos::Utility::IMap;
using Elastos::Droid::Utility::CArrayMap;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Projection {

ECode CMediaProjectionManager::OnStartRunnable::Run()
{
    return mCallback->OnStart(mInfo);
}

ECode CMediaProjectionManager::OnStopRunnable::Run()
{
    return mCallback->OnStop(mInfo);
}

CAR_INTERFACE_IMPL(CMediaProjectionManager::CallbackDelegate, Object, IIMediaProjectionWatcherCallback)

CMediaProjectionManager::CallbackDelegate::CallbackDelegate(
    /* [in] */ CMediaProjectionManager* host,
    /* [in] */ IMediaProjectionManagerCallback * callback,
    /* [in] */ IHandler * _handler)
{
    mHost = host;
    mCallback = callback;
    AutoPtr<IHandler> handler = _handler;
    if (handler == NULL) {
        CHandler::New((IHandler**)&handler);
    }
    mHandler = handler;
}

ECode CMediaProjectionManager::CallbackDelegate::OnStart(
    /* [in] */ IMediaProjectionInfo * info)
{
    AutoPtr<OnStartRunnable> runnable = new OnStartRunnable(this, mCallback, info);
    Boolean b;
    mHandler->Post(runnable, &b);
    return NOERROR;
}

ECode CMediaProjectionManager::CallbackDelegate::OnStop(
    /* [in] */ IMediaProjectionInfo * info)
{
    AutoPtr<OnStopRunnable> runnable = new OnStopRunnable(this, mCallback, info);
    Boolean b;
    mHandler->Post(runnable, &b);
    return NOERROR;
}

String CMediaProjectionManager::TAG("MediaProjectionManager");

CAR_INTERFACE_IMPL(CMediaProjectionManager, Object, IMediaProjectionManager)

CAR_OBJECT_IMPL(CMediaProjectionManager)

CMediaProjectionManager::CMediaProjectionManager()
{
}

CMediaProjectionManager::~CMediaProjectionManager()
{
}

ECode CMediaProjectionManager::constructor(
    /* [in] */ IContext * context)
{
    mContext = context;
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::MEDIA_PROJECTION_SERVICE);
    mService = IIMediaProjectionManager::Probe(service.Get());
    CArrayMap::New((IArrayMap**)&mCallbacks);
    return NOERROR;
}

ECode CMediaProjectionManager::CreateScreenCaptureIntent(
    /* [out] */ IIntent ** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIntent> i;
    CIntent::New((IIntent**)&i);
    i->SetClassName(String("Elastos.Droid.SystemUI"),
            String("Elastos.Droid.SystemUI.Media.CMediaProjectionPermissionActivity"));
    *result = i;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaProjectionManager::GetMediaProjection(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent * resultData,
    /* [out] */ IMediaProjection ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (resultCode != IActivity::RESULT_OK || resultData == NULL) {
        return NOERROR;
    }
    AutoPtr<IBinder> projection;
assert(0);
// TODO: Need IIntent::GetIBinderExtra
    // resultData->GetIBinderExtra(IMediaProjectionManager::EXTRA_MEDIA_PROJECTION, (IBinder**)&projection);
    if (projection == NULL) {
        return NOERROR;
    }
    return CMediaProjection::New(mContext, IIMediaProjection::Probe(projection), result);
}

ECode CMediaProjectionManager::GetActiveProjectionInfo(
    /* [out] */ IMediaProjectionInfo ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    // try {
    return mService->GetActiveProjectionInfo(result);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Unable to get the active projection info", e);
    // }
    // return null;
}

ECode CMediaProjectionManager::StopActiveProjection()
{
    // try {
    return mService->StopActiveProjection();
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Unable to stop the currently active media projection", e);
    // }
}

ECode CMediaProjectionManager::AddCallback(
    /* [in] */ IMediaProjectionManagerCallback * callback,
    /* [in] */ IHandler * handler)
{
    if (callback == NULL) {
        // throw new IllegalArgumentException("callback must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<CallbackDelegate> delegate = new CallbackDelegate(this, callback, handler);
    IMap::Probe(mCallbacks)->Put(callback, (IObject*)delegate);
    // try {
    mService->AddCallback(IIMediaProjectionWatcherCallback::Probe(delegate));
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Unable to add callbacks to MediaProjection service", e);
    // }
    return NOERROR;
}

ECode CMediaProjectionManager::RemoveCallback(
    /* [in] */ IMediaProjectionManagerCallback * callback)
{
    if (callback == NULL) {
        // throw new IllegalArgumentException("callback must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> obj;
    IMap::Probe(mCallbacks)->Remove(callback, (IInterface**)&obj);
    AutoPtr<CallbackDelegate> delegate = (CallbackDelegate*)IObject::Probe(obj);
    // try {
    if (delegate != NULL) {
        mService->RemoveCallback(IIMediaProjectionWatcherCallback::Probe(delegate));
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Unable to add callbacks to MediaProjection service", e);
    // }
    return NOERROR;
}

} // namespace Projection
} // namespace Media
} // namepsace Droid
} // namespace Elastos
