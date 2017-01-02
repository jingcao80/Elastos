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

#ifndef __ORG_CONSCRYPT_CHAINSTRENGTHANALYZER_H__
#define __ORG_CONSCRYPT_CHAINSTRENGTHANALYZER_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Security.h>

using Elastos::Security::Cert::IX509Certificate;

namespace Org {
namespace Conscrypt {

class ChainStrengthAnalyzer
{
public:
    static CARAPI Check(
        /* [in] */  ArrayOf<IX509Certificate*>* chain);

private:
    static CARAPI CheckCert(
        /* [in] */ IX509Certificate* cert);

    static CARAPI CheckModulusLength(
        /* [in] */ IX509Certificate* cert);

    static CARAPI CheckNotMD5(
        /* [in] */ IX509Certificate* cert);

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitOID_BLACKLIST();

private:
    static const Int32 MIN_MODULUS = 1024;
    static AutoPtr<ArrayOf<String> > OID_BLACKLIST; // = {"1.2.840.113549.1.1.4"}; // MD5withRSA
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CHAINSTRENGTHANALYZER_H__
