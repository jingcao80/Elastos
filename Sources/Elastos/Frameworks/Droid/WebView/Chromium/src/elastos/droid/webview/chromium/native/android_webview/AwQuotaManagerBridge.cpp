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

#include <Elastos.CoreLibrary.Core.h>
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwQuotaManagerBridge.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwQuotaManagerBridge_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

//using Elastos::Droid::Utility::CSparseArray;TODO now use hashmap replace this
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//===============================================================
//                 AwQuotaManagerBridge::Origins
//===============================================================

AwQuotaManagerBridge::Origins::Origins(
    /* [in] */ ArrayOf<String>* origins,
    /* [in] */ ArrayOf<Int64>* usages,
    /* [in] */ ArrayOf<Int64>* quotas)
    : mOrigins(origins)
    , mUsages(usages)
    , mQuotas(quotas)
{
}

//===============================================================
//                   AwQuotaManagerBridge
//===============================================================

AutoPtr<AwQuotaManagerBridge> AwQuotaManagerBridge::sInstance;

AwQuotaManagerBridge::AwQuotaManagerBridge(
    /* [in] */ HANDLE nativeAwQuotaManagerBridgeImpl)
    : mNativeAwQuotaManagerBridgeImpl(nativeAwQuotaManagerBridgeImpl)
    , mNextId(0)
{
    //mPendingGetOriginCallbacks = new SparseArray<ValueCallback<Origins>>();TODO
    //mPendingGetQuotaForOriginCallbacks = new SparseArray<ValueCallback<Long>>();TODO
    //mPendingGetUsageForOriginCallbacks = new SparseArray<ValueCallback<Long>>();TODO
    NativeInit(mNativeAwQuotaManagerBridgeImpl);
}

// TODO(boliu): This should be obtained from Java AwBrowserContext that owns this.
HANDLE AwQuotaManagerBridge::NativeGetDefaultNativeAwQuotaManagerBridge()
{
    return Elastos_AwQuotaManagerBridge_nativeGetDefaultNativeAwQuotaManagerBridge();
}

AutoPtr<AwQuotaManagerBridge> AwQuotaManagerBridge::GetInstance()
{
    ThreadUtils::AssertOnUiThread();
    if (sInstance == NULL) {
        sInstance = new AwQuotaManagerBridge(NativeGetDefaultNativeAwQuotaManagerBridge());
    }
    return sInstance;
}

Int32 AwQuotaManagerBridge::GetNextId()
{
    ThreadUtils::AssertOnUiThread();
    return ++mNextId;
}

/*
 * There are five HTML5 offline storage APIs.
 * 1) Web Storage (ie the localStorage and sessionStorage variables)
 * 2) Web SQL database
 * 3) Application cache
 * 4) Indexed Database
 * 5) Filesystem API
 */

/**
 * Implements WebStorage.deleteAllData(). Clear the storage of all five offline APIs.
 *
 * TODO(boliu): Actually clear Web Storage.
 */
void AwQuotaManagerBridge::DeleteAllData()
{
    NativeDeleteAllData(mNativeAwQuotaManagerBridgeImpl);
}

/**
 * Implements WebStorage.deleteOrigin(). Clear the storage of APIs 2-5 for the given origin.
 */
void AwQuotaManagerBridge::DeleteOrigin(
    /* [in] */ const String& origin)
{
    NativeDeleteOrigin(mNativeAwQuotaManagerBridgeImpl, origin);
}

/**
 * Implements WebStorage.getOrigins. Get the per origin usage and quota of APIs 2-5 in
 * aggregate.
 */
void AwQuotaManagerBridge::GetOrigins(
    /* [in] */ IValueCallback * callback)
{
    Int32 callbackId = GetNextId();
    assert(mPendingGetOriginCallbacks[callbackId] == NULL);
    mPendingGetOriginCallbacks[callbackId] =  callback;
    NativeGetOrigins(mNativeAwQuotaManagerBridgeImpl, callbackId);
}

/**
 * Implements WebStorage.getQuotaForOrigin. Get the quota of APIs 2-5 in aggregate for given
 * origin.
 */
void AwQuotaManagerBridge::GetQuotaForOrigin(
    /* [in] */ const String& origin,
    /* [in] */ IValueCallback* callback)
{
    Int32 callbackId = GetNextId();
    assert(mPendingGetQuotaForOriginCallbacks[callbackId] == NULL);
    mPendingGetQuotaForOriginCallbacks[callbackId] = callback;
    NativeGetUsageAndQuotaForOrigin(mNativeAwQuotaManagerBridgeImpl, origin, callbackId, true);
}

/**
 * Implements WebStorage.getUsageForOrigin. Get the usage of APIs 2-5 in aggregate for given
 * origin.
 */
void AwQuotaManagerBridge::GetUsageForOrigin(
    /* [in] */ const String& origin,
    /* [in] */ IValueCallback * callback)
{
    Int32 callbackId = GetNextId();
    assert(mPendingGetUsageForOriginCallbacks[callbackId] == NULL);
    mPendingGetUsageForOriginCallbacks[callbackId]  = callback;
    NativeGetUsageAndQuotaForOrigin(mNativeAwQuotaManagerBridgeImpl, origin, callbackId, false);
}

//@CalledByNative
void AwQuotaManagerBridge::OnGetOriginsCallback(
    /* [in] */ Int32 callbackId,
    /* [in] */ ArrayOf<String>* origin,
    /* [in] */ ArrayOf<Int64>* usages,
    /* [in] */ ArrayOf<Int64>* quotas)
{
    assert(mPendingGetOriginCallbacks[callbackId] != NULL);
    AutoPtr<Origins> originsTmp = new Origins(origin, usages, quotas);
    mPendingGetOriginCallbacks[callbackId]->OnReceiveValue(TO_IINTERFACE(originsTmp));
    mPendingGetOriginCallbacks.Erase(callbackId);
}

//@CalledByNative
void AwQuotaManagerBridge::OnGetUsageAndQuotaForOriginCallback(
    /* [in] */ Int32 callbackId,
    /* [in] */ Boolean isQuota,
    /* [in] */ Int64 usage,
    /* [in] */ Int64 quota)
{
    if (isQuota) {
        assert(mPendingGetQuotaForOriginCallbacks[callbackId] != NULL);
        AutoPtr<IInteger64> iQuota;
        CInteger64::New(quota, (IInteger64**)&iQuota);
        mPendingGetQuotaForOriginCallbacks[callbackId]->OnReceiveValue(TO_IINTERFACE(iQuota));
        mPendingGetQuotaForOriginCallbacks.Erase(callbackId);
    }
    else {
        assert(mPendingGetUsageForOriginCallbacks[callbackId] != NULL);
        AutoPtr<IInteger64> iUsage;
        CInteger64::New(usage, (IInteger64**)&iUsage);
        mPendingGetUsageForOriginCallbacks[callbackId]->OnReceiveValue(TO_IINTERFACE(iUsage));
        mPendingGetUsageForOriginCallbacks.Erase(callbackId);
    }
}

void AwQuotaManagerBridge::NativeInit(
    /* [in] */ HANDLE nativeAwQuotaManagerBridgeImpl)
{
    Elastos_AwQuotaManagerBridge_nativeInit(TO_IINTERFACE(this), nativeAwQuotaManagerBridgeImpl);
}

void AwQuotaManagerBridge::NativeDeleteAllData(
    /* [in] */ HANDLE nativeAwQuotaManagerBridgeImpl)
{
    Elastos_AwQuotaManagerBridge_nativeDeleteAllData(TO_IINTERFACE(this), nativeAwQuotaManagerBridgeImpl);
}

void AwQuotaManagerBridge::NativeDeleteOrigin(
    /* [in] */ HANDLE nativeAwQuotaManagerBridgeImpl,
    /* [in] */ const String& origin)
{
    Elastos_AwQuotaManagerBridge_nativeDeleteOrigin(TO_IINTERFACE(this), nativeAwQuotaManagerBridgeImpl, origin);
}

void AwQuotaManagerBridge::NativeGetOrigins(
    /* [in] */ HANDLE nativeAwQuotaManagerBridgeImpl,
    /* [in] */ Int32 callbackId)
{
    Elastos_AwQuotaManagerBridge_nativeGetOrigins(TO_IINTERFACE(this), nativeAwQuotaManagerBridgeImpl, callbackId);
}

void AwQuotaManagerBridge::NativeGetUsageAndQuotaForOrigin(
    /* [in] */ HANDLE nativeAwQuotaManagerBridgeImpl,
    /* [in] */ const String& origin,
    /* [in] */ Int32 callbackId,
    /* [in] */ Boolean isQuota)
{
    Elastos_AwQuotaManagerBridge_nativeGetUsageAndQuotaForOrigin(TO_IINTERFACE(this), nativeAwQuotaManagerBridgeImpl, origin, callbackId, isQuota);
}
//callback function definition
void AwQuotaManagerBridge::OnGetOriginsCallback(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 callbackId,
    /* [in] */ ArrayOf<String>* origin,
    /* [in] */ ArrayOf<Int64>* usages,
    /* [in] */ ArrayOf<Int64>* quotas)
{
    AutoPtr<AwQuotaManagerBridge> mObj = (AwQuotaManagerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwQuotaManagerBridge", "AwQuotaManagerBridge::OnGetOriginsCallback, mObj is NULL");
        return;
    }
    mObj->OnGetOriginsCallback(callbackId, origin, usages, quotas);
}

void AwQuotaManagerBridge::OnGetUsageAndQuotaForOriginCallback(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 callbackId,
    /* [in] */ Boolean isQuota,
    /* [in] */ Int64 usage,
    /* [in] */ Int64 quota)
{
    AutoPtr<AwQuotaManagerBridge> mObj = (AwQuotaManagerBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwQuotaManagerBridge", "AwQuotaManagerBridge::OnGetUsageAndQuotaForOriginCallback, mObj is NULL");
        return;
    }
    mObj->OnGetUsageAndQuotaForOriginCallback(callbackId, isQuota, usage, quota);
}


} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
