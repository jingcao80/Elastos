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

#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Security.h>
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClientBridge.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwContentsClientBridge_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClient.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/JsResultHandler.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/SslUtil.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::Http::ISslCertificate;
using Elastos::Droid::Net::Http::ISslError;
using Elastos::Droid::Webkit::EIID_IValueCallback;
using Elastos::Security::Cert::ICertificate;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::IBoolean;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::EIID_IRunnable;

using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastosx::Security::Auth::X500::CX500Principal;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//========================================================================================
//       AwContentsClientBridge::ClientCertificateRequestCallback::ProceedRunnable
//========================================================================================
CAR_INTERFACE_IMPL(AwContentsClientBridge::ClientCertificateRequestCallback::ProceedRunnable, Object, IRunnable);

AwContentsClientBridge::ClientCertificateRequestCallback::ProceedRunnable::ProceedRunnable(
    /* [in] */ ClientCertificateRequestCallback* owner,
    /* [in] */ IPrivateKey* privateKey,
    /* [in] */ ArrayOf<IX509Certificate*>* chain)
    : mOwner(owner)
{
}

ECode AwContentsClientBridge::ClientCertificateRequestCallback::ProceedRunnable::Run()
{
    mOwner->ProceedOnUiThread(mPrivateKey, mChain);
    return NOERROR;
}

//========================================================================================
//       AwContentsClientBridge::ClientCertificateRequestCallback::IgnoreRunnable
//========================================================================================
CAR_INTERFACE_IMPL(AwContentsClientBridge::ClientCertificateRequestCallback::IgnoreRunnable, Object, IRunnable);

AwContentsClientBridge::ClientCertificateRequestCallback::IgnoreRunnable::IgnoreRunnable(
    /* [in] */ ClientCertificateRequestCallback* owner)
    : mOwner(owner)
{
}

ECode AwContentsClientBridge::ClientCertificateRequestCallback::IgnoreRunnable::Run()
{
    mOwner->IgnoreOnUiThread();
    return NOERROR;
}

//========================================================================================
//       AwContentsClientBridge::ClientCertificateRequestCallback::CancelRunnable
//========================================================================================
CAR_INTERFACE_IMPL(AwContentsClientBridge::ClientCertificateRequestCallback::CancelRunnable, Object, IRunnable);

AwContentsClientBridge::ClientCertificateRequestCallback::CancelRunnable::CancelRunnable(
    /* [in] */ ClientCertificateRequestCallback* owner)
    : mOwner(owner)
{
}

ECode AwContentsClientBridge::ClientCertificateRequestCallback::CancelRunnable::Run()
{
    mOwner->CancelOnUiThread();
    return NOERROR;
}

//==============================================================================
//          AwContentsClientBridge::ClientCertificateRequestCallback
//==============================================================================

AwContentsClientBridge::ClientCertificateRequestCallback::ClientCertificateRequestCallback(
    /* [in] */ AwContentsClientBridge* owner,
    /* [in] */ Int32 id,
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
    : mOwner(owner)
    , mId(id)
    , mHost(host)
    , mPort(port)
    , mIsCalled(FALSE)
{
}

void AwContentsClientBridge::ClientCertificateRequestCallback::Proceed(
    /* [in] */ IPrivateKey* privateKey,
    /* [in] */ ArrayOf<IX509Certificate*>* chain)
{
    AutoPtr<IRunnable> runnable = new ProceedRunnable(this, privateKey, chain);
    ThreadUtils::RunOnUiThread(runnable);
}

void AwContentsClientBridge::ClientCertificateRequestCallback::Ignore()
{
    AutoPtr<IRunnable> runnable = new IgnoreRunnable(this);
    ThreadUtils::RunOnUiThread(runnable);
}

void AwContentsClientBridge::ClientCertificateRequestCallback::Cancel()
{
    AutoPtr<IRunnable> runnable = new CancelRunnable(this);
    ThreadUtils::RunOnUiThread(runnable);
}

void AwContentsClientBridge::ClientCertificateRequestCallback::ProceedOnUiThread(
    /* [in] */ IPrivateKey* privateKey,
    /* [in] */ ArrayOf<IX509Certificate*>* chain)
{
    ECode ecode = CheckIfCalled();
    if (FAILED(ecode))
    {
        Logger::E(TAG, "ClientCertificateRequestCallback::ProceedOnUiThread");
        return;
    }

    AutoPtr<ElastosPrivateKey> key = mOwner->mLocalKeyStore->CreateKey(privateKey);

    if (key == NULL || chain == NULL || chain->GetLength() == 0) {
        Logger::W(TAG, "Empty client certificate chain?");
        ProvideResponse(NULL, NULL);
        return;
    }
    // Encode the certificate chain.
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Byte> > > > encodedChain = ArrayOf<AutoPtr<ArrayOf<Byte> > >::Alloc(chain->GetLength());
    //try {
        for (Int32 i = 0; i < chain->GetLength(); ++i) {
            AutoPtr<ICertificate> certificate = ICertificate::Probe((*chain)[i]);
            certificate->GetEncoded((ArrayOf<Byte>**)&((*encodedChain)[i]));
        }
    //} catch (CertificateEncodingException e) {
    //    Log.w(TAG, "Could not retrieve encoded certificate chain: " + e);
    //    provideResponse(null, null);
    //    return;
    //}
    mOwner->mLookupTable->Allow(mHost, mPort, key, encodedChain);
    ProvideResponse(key, encodedChain);
}

void AwContentsClientBridge::ClientCertificateRequestCallback::IgnoreOnUiThread()
{
    ECode ecode = CheckIfCalled();
    if (FAILED(ecode))
    {
        Logger::E(TAG, "ClientCertificateRequestCallback::IgnoreOnUiThread");
        return;
    }
    ProvideResponse(NULL, NULL);
}

void AwContentsClientBridge::ClientCertificateRequestCallback::CancelOnUiThread()
{
    ECode ecode = CheckIfCalled();
    if (FAILED(ecode))
    {
        Logger::E(TAG, "ClientCertificateRequestCallback::CancelOnUiThread");
        return;
    }
    mOwner->mLookupTable->Deny(mHost, mPort);
    ProvideResponse(NULL, NULL);
}

ECode AwContentsClientBridge::ClientCertificateRequestCallback::CheckIfCalled()
{
    if (mIsCalled) {
        //throw new IllegalStateException("The callback was already called.");
        Logger::E(TAG, "The callback was already called");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mIsCalled = TRUE;
    return NOERROR;
}

void AwContentsClientBridge::ClientCertificateRequestCallback::ProvideResponse(
    /* [in] */ ElastosPrivateKey* androidKey,
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* certChain)
{
    if (mOwner->mNativeContentsClientBridge == 0) {
        return;
    }

    mOwner->NativeProvideClientCertificateResponse(mOwner->mNativeContentsClientBridge, mId,
            certChain, androidKey);
}

//=========================================================================
//               AwContentsClientBridge::InnerValueCallback
//=========================================================================
CAR_INTERFACE_IMPL(AwContentsClientBridge::InnerValueCallback, Object, IValueCallback);

AwContentsClientBridge::InnerValueCallback::InnerValueCallback(
    /* [in] */ AwContentsClientBridge* owner,
    /* [in] */ Int32 id)
    : mOwner(owner)
    , mId(id)
{
}

ECode AwContentsClientBridge::InnerValueCallback::OnReceiveValue(
    /* [in] */ IInterface* value)
{
    AutoPtr<IRunnable> runnable = new InnerRunnable(this, value, mId);
    ThreadUtils::RunOnUiThread(runnable);
    return NOERROR;
}

//=========================================================================
//         AwContentsClientBridge::InnerValueCallback::InnerRunnable
//=========================================================================
CAR_INTERFACE_IMPL(AwContentsClientBridge::InnerValueCallback::InnerRunnable, Object, IRunnable);

AwContentsClientBridge::InnerValueCallback::InnerRunnable::InnerRunnable(
    /* [in] */ InnerValueCallback* owner,
    /* [in] */ IInterface* value,
    /* [in] */ Int32 id)
    : mOwner(owner)
    , mValue(value)
    , mId(id)
{
}

ECode AwContentsClientBridge::InnerValueCallback::InnerRunnable::Run()
{
    AutoPtr<IBoolean> iValue = IBoolean::Probe(mValue);
    Boolean value;
    iValue->GetValue(&value);
    mOwner->mOwner->ProceedSslError(value, mId);
    return NOERROR;
}

//===============================================================
//                    AwContentsClientBridge
//===============================================================

const String AwContentsClientBridge::TAG("AwContentsClientBridge");


// Used for mocking this class in tests.
AwContentsClientBridge::AwContentsClientBridge(
    /* [in] */ DefaultElastosKeyStore* keyStore,
    /* [in] */ ClientCertLookupTable* table)
    : mNativeContentsClientBridge(0)
    , mLocalKeyStore(keyStore)
    , mLookupTable(table)
{
}

AwContentsClientBridge::AwContentsClientBridge(
    /* [in] */ AwContentsClient* client,
    /* [in] */ DefaultElastosKeyStore* keyStore,
    /* [in] */ ClientCertLookupTable* table)
    : mNativeContentsClientBridge(0)
{
    assert(client != NULL);
    mClient = client;
    mLocalKeyStore = keyStore;
    mLookupTable = table;
}

// Used by the native peer to set/reset a weak ref to the native peer.
//@CalledByNative
void AwContentsClientBridge::SetNativeContentsClientBridge(
    /* [in] */ Int64 nativeContentsClientBridge)
{
    mNativeContentsClientBridge = nativeContentsClientBridge;
}

// If returns false, the request is immediately canceled, and any call to proceedSslError
// has no effect. If returns true, the request should be canceled or proceeded using
// proceedSslError().
// Unlike the webview classic, we do not keep keep a database of certificates that
// are allowed by the user, because this functionality is already handled via
// ssl_policy in native layers.
//@CalledByNative
Boolean AwContentsClientBridge::AllowCertificateError(
    /* [in] */ Int32 certError,
    /* [in] */ ArrayOf<Byte>* derBytes,
    /* [in] */ const String& url,
    /* [in] */ Int32 id)
{
    AutoPtr<ISslCertificate> cert = SslUtil::GetCertificateFromDerBytes(derBytes);
    if (cert == NULL) {
        // if the certificate or the client is null, cancel the request
        return FALSE;
    }
    AutoPtr<ISslError> sslError = SslUtil::SslErrorFromNetErrorCode(certError, cert, url);
    AutoPtr<IValueCallback> callback = new InnerValueCallback(this, id);
    mClient->OnReceivedSslError(callback, sslError);
    return TRUE;
}

void AwContentsClientBridge::ProceedSslError(
    /* [in] */ Boolean proceed,
    /* [in] */ Int32 id)
{
    if (mNativeContentsClientBridge == 0) return;
    NativeProceedSslError(mNativeContentsClientBridge, proceed, id);
}

// Intentionally not private for testing the native peer of this class.
//@CalledByNative
void AwContentsClientBridge::SelectClientCertificate(
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* encodedPrincipals,
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    assert(mNativeContentsClientBridge != 0);
    AutoPtr<ClientCertLookupTable::Cert> cert = mLookupTable->GetCertData(host, port);
    if (mLookupTable->IsDenied(host, port)) {
        NativeProvideClientCertificateResponse(mNativeContentsClientBridge, id,
                NULL, NULL);
        return;
    }
    if (cert != NULL) {
        NativeProvideClientCertificateResponse(mNativeContentsClientBridge, id,
                cert->certChain, cert->privateKey);
        return;
    }
    // Build the list of principals from encoded versions.
    AutoPtr<ArrayOf<IPrincipal*> > principals;
    if (encodedPrincipals->GetLength() > 0) {
        principals = ArrayOf<IPrincipal*>::Alloc(encodedPrincipals->GetLength());
        for (Int32 n = 0; n < encodedPrincipals->GetLength(); ++n) {
            //try {
            AutoPtr<IPrincipal> principalTmp;
            CX500Principal::New((*encodedPrincipals)[n], (IPrincipal**)&principalTmp);
            principals->Set(n, principalTmp);
            AutoPtr<IX500Principal> ixp;
            String name((char*)((*encodedPrincipals)[n]->GetPayload()));
            CX500Principal::New(name, (IX500Principal**)&ixp);
            AutoPtr<IPrincipal> p = IPrincipal::Probe(ixp);
            principals->Set(n, p);
            //} catch (IllegalArgumentException e) {
            //    Log.w(TAG, "Exception while decoding issuers list: " + e);
            //    nativeProvideClientCertificateResponse(mNativeContentsClientBridge, id,
            //        null, null);
            //    return;
            //}
        }

    }

    AutoPtr<ClientCertificateRequestCallback> callback =
            new ClientCertificateRequestCallback(this, id, host, port);
    mClient->OnReceivedClientCertRequest(callback, keyTypes, principals, host, port);
}

//@CalledByNative
void AwContentsClientBridge::HandleJsAlert(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ Int32 id)
{
    AutoPtr<JsResultHandler> handler = new JsResultHandler(this, id);
    mClient->HandleJsAlert(url, message, handler);
}

//@CalledByNative
void AwContentsClientBridge::HandleJsConfirm(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ Int32 id)
{
    AutoPtr<JsResultHandler> handler = new JsResultHandler(this, id);
    mClient->HandleJsConfirm(url, message, handler);
}

//@CalledByNative
void AwContentsClientBridge::HandleJsPrompt(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ const String& defaultValue,
    /* [in] */ Int32 id)
{
    AutoPtr<JsResultHandler> handler = new JsResultHandler(this, id);
    mClient->HandleJsPrompt(url, message, defaultValue, handler);
}

//@CalledByNative
void AwContentsClientBridge::HandleJsBeforeUnload(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ Int32 id)
{
    AutoPtr<JsResultHandler> handler = new JsResultHandler(this, id);
    mClient->HandleJsBeforeUnload(url, message, handler);
}

//@CalledByNative
Boolean AwContentsClientBridge::ShouldOverrideUrlLoading(
    /* [in] */ const String& url)
{
    return mClient->ShouldOverrideUrlLoading(url);
}

void AwContentsClientBridge::ConfirmJsResult(
    /* [in] */ Int32 id,
    /* [in] */ const String& prompt)
{
    if (mNativeContentsClientBridge == 0) return;
    NativeConfirmJsResult(mNativeContentsClientBridge, id, prompt);
}

void AwContentsClientBridge::CancelJsResult(
    /* [in] */ Int32 id)
{
    if (mNativeContentsClientBridge == 0) return;
    NativeCancelJsResult(mNativeContentsClientBridge, id);
}

//--------------------------------------------------------------------------------------------
//  Native methods
//--------------------------------------------------------------------------------------------
void AwContentsClientBridge::NativeProceedSslError(
    /* [in] */ Int64 nativeAwContentsClientBridge,
    /* [in] */ Boolean proceed,
    /* [in] */ Int32 id)
{
    Elastos_AwContentsClientBridge_nativeProceedSslError(TO_IINTERFACE(this), nativeAwContentsClientBridge, proceed, id);
}

void AwContentsClientBridge::NativeProvideClientCertificateResponse(
    /* [in] */ Int64 nativeAwContentsClientBridge,
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* certChain,
    /* [in] */ ElastosPrivateKey* androidKey)
{
    Elastos_AwContentsClientBridge_nativeProvideClientCertificateResponse(TO_IINTERFACE(this), nativeAwContentsClientBridge, id, certChain, TO_IINTERFACE(androidKey));
}

void AwContentsClientBridge::NativeConfirmJsResult(
    /* [in] */ Int64 nativeAwContentsClientBridge,
    /* [in] */ Int32 id,
    /* [in] */ const String& prompt)
{
    Elastos_AwContentsClientBridge_nativeConfirmJsResult(TO_IINTERFACE(this), nativeAwContentsClientBridge, id, prompt);
}

void AwContentsClientBridge::NativeCancelJsResult(
    /* [in] */ Int64 nativeAwContentsClientBridge,
    /* [in] */ Int32 id)
{
    Elastos_AwContentsClientBridge_nativeCancelJsResult(TO_IINTERFACE(this), nativeAwContentsClientBridge, id);
}
//callback function definition
void AwContentsClientBridge::SetNativeContentsClientBridge(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 nativeContentsClientBridge)
{
    AutoPtr<AwContentsClientBridge> mObj = (AwContentsClientBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContentsClientBridge::SetNativeContentsClientBridge, mObj is NULL");
        return;
    }
    mObj->SetNativeContentsClientBridge(nativeContentsClientBridge);
}

Boolean AwContentsClientBridge::AllowCertificateError(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 certError,
    /* [in] */ ArrayOf<Byte>* derBytes,
    /* [in] */ const String& url,
    /* [in] */ Int32 id)
{
    AutoPtr<AwContentsClientBridge> mObj = (AwContentsClientBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContentsClientBridge::AllowCertificateError, mObj is NULL");
        return FALSE;
    }
    return mObj->AllowCertificateError(certError, derBytes, url, id);
}

void AwContentsClientBridge::SelectClientCertificate(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 id,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* encodedPrincipals,
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
    AutoPtr<AwContentsClientBridge> mObj = (AwContentsClientBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContentsClientBridge::SelectClientCertificate, mObj is NULL");
        return;
    }
    mObj->SelectClientCertificate(id, keyTypes, encodedPrincipals, host, port);
}

void AwContentsClientBridge::HandleJsAlert(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ Int32 id)
{
    AutoPtr<AwContentsClientBridge> mObj = (AwContentsClientBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContentsClientBridge::HandleJsAlert, mObj is NULL");
        return;
    }
    mObj->HandleJsAlert(url, message, id);
}

void AwContentsClientBridge::HandleJsConfirm(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ Int32 id)
{
    AutoPtr<AwContentsClientBridge> mObj = (AwContentsClientBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContentsClientBridge::HandleJsConfirm, mObj is NULL");
        return;
    }
    mObj->HandleJsConfirm(url, message, id);
}

void AwContentsClientBridge::HandleJsPrompt(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ const String& defaultValue,
    /* [in] */ Int32 id)
{
    AutoPtr<AwContentsClientBridge> mObj = (AwContentsClientBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContentsClientBridge::HandleJsPrompt, mObj is NULL");
        return;
    }
    mObj->HandleJsPrompt(url, message, defaultValue, id);
}

void AwContentsClientBridge::HandleJsBeforeUnload(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ Int32 id)
{
    AutoPtr<AwContentsClientBridge> mObj = (AwContentsClientBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContentsClientBridge::HandleJsBeforeUnload, mObj is NULL");
        return;
    }
    mObj->HandleJsBeforeUnload(url, message, id);
}

Boolean AwContentsClientBridge::ShouldOverrideUrlLoading(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& url)
{
    AutoPtr<AwContentsClientBridge> mObj = (AwContentsClientBridge*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwContentsClientBridge::ShouldOverrideUrlLoading, mObj is NULL");
        return FALSE;
    }
    return mObj->ShouldOverrideUrlLoading(url);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
