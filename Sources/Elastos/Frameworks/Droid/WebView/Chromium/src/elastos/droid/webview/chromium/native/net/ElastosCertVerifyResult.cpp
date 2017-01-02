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

#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/webview/chromium/native/net/ElastosCertVerifyResult.h"
#include "elastos/droid/webkit/webview/chromium/native/net/api/ElastosCertVerifyResult_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Security::Cert::ICertificate;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Net {

//=====================================================================
//                       ElastosCertVerifyResult
//=====================================================================
ElastosCertVerifyResult::ElastosCertVerifyResult(
    /* [in] */ Int32 status,
    /* [in] */ Boolean isIssuedByKnownRoot,
    /* [in] */ IList* certificateChain)
    : mStatus(status)
    , mIsIssuedByKnownRoot(isIssuedByKnownRoot)
{
    // ==================before translated======================
    // mStatus = status;
    // mIsIssuedByKnownRoot = isIssuedByKnownRoot;
    // mCertificateChain = new ArrayList<X509Certificate>(certificateChain);

    CArrayList::New((IList**)&mCertificateChain);
    Int32 size = 0;
    certificateChain->GetSize(&size);
    for (Int32 i=0; i<size; ++i) {
        AutoPtr<IInterface> item;
        certificateChain->Get(i, (IInterface**)&item);
        mCertificateChain->Add(item);
    }
}

ElastosCertVerifyResult::ElastosCertVerifyResult(
    /* [in] */ Int32 status)
    : mStatus(status)
    , mIsIssuedByKnownRoot(FALSE)
{
    // ==================before translated======================
    // mStatus = status;
    // mIsIssuedByKnownRoot = false;
    // mCertificateChain = Collections.<X509Certificate>emptyList();

    CArrayList::New((IList**)&mCertificateChain);
}

Int32 ElastosCertVerifyResult::GetStatus()
{
    // ==================before translated======================
    // return mStatus;

    return mStatus;
}

Boolean ElastosCertVerifyResult::IsIssuedByKnownRoot()
{
    // ==================before translated======================
    // return mIsIssuedByKnownRoot;

    return mIsIssuedByKnownRoot;
}

AutoPtr< ArrayOf< AutoPtr< ArrayOf<Byte> > > > ElastosCertVerifyResult::GetCertificateChainEncoded()
{
    // ==================before translated======================
    // byte[][] verifiedChainArray = new byte[mCertificateChain.size()][];
    // try {
    //     for (int i = 0; i < mCertificateChain.size(); i++) {
    //         verifiedChainArray[i] = mCertificateChain.get(i).getEncoded();
    //     }
    // } catch (CertificateEncodingException e) {
    //     return new byte[0][];
    // }
    // return verifiedChainArray;

    Int32 size = 0;
    mCertificateChain->GetSize(&size);
    AutoPtr< ArrayOf< AutoPtr< ArrayOf<Byte> > > > verifiedChainArray = ArrayOf< AutoPtr< ArrayOf<Byte> > >::Alloc(size);
    // try {
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> item;
            mCertificateChain->Get(i, (IInterface**)&item);
            ICertificate* x509 = ICertificate::Probe(item);
            AutoPtr< ArrayOf<Byte> > encoded;
            x509->GetEncoded((ArrayOf<Byte>**)&encoded);
            verifiedChainArray->Set(i, encoded);
        }
    // } catch (CertificateEncodingException e) {
    //    return new byte[0][];
    // }
    return verifiedChainArray;
}

Int32 ElastosCertVerifyResult::GetStatus(
    /* [in] */ IInterface* obj)
{
    ElastosCertVerifyResult* mObj = (ElastosCertVerifyResult*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosCertVerifyResult", "ElastosCertVerifyResult::GetStatus, mObj is NULL");
        return 0;
    }
    return mObj->GetStatus();
}

Boolean ElastosCertVerifyResult::IsIssuedByKnownRoot(
    /* [in] */ IInterface* obj)
{
    ElastosCertVerifyResult* mObj = (ElastosCertVerifyResult*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosCertVerifyResult", "ElastosCertVerifyResult::IsIssuedByKnownRoot, mObj is NULL");
        return FALSE;
    }
    return mObj->IsIssuedByKnownRoot();
}

AutoPtr<ArrayOf<AutoPtr<ArrayOf<Byte> > > > ElastosCertVerifyResult::GetCertificateChainEncoded(
    /* [in] */ IInterface* obj)
{
    ElastosCertVerifyResult* mObj = (ElastosCertVerifyResult*)(IObject::Probe(obj));
    if (NULL == mObj) {
        Logger::E("ElastosCertVerifyResult", "ElastosCertVerifyResult::GetCertificateChainEncoded, mObj is NULL");
        return NULL;
    }
    return mObj->GetCertificateChainEncoded();
}

} // namespace Net
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

