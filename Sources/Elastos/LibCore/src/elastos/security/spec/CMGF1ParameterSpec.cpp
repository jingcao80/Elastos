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

#include "CMGF1ParameterSpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

const AutoPtr<IMGF1ParameterSpec> CMGF1ParameterSpec::SHA1 = CMGF1ParameterSpec::InitStatic(String("SHA-1"));

const AutoPtr<IMGF1ParameterSpec> CMGF1ParameterSpec::SHA256 = CMGF1ParameterSpec::InitStatic(String("SHA-256"));

const AutoPtr<IMGF1ParameterSpec> CMGF1ParameterSpec::SHA384 = CMGF1ParameterSpec::InitStatic(String("SHA-384"));

const AutoPtr<IMGF1ParameterSpec> CMGF1ParameterSpec::SHA512 = CMGF1ParameterSpec::InitStatic(String("SHA-512"));

AutoPtr<IMGF1ParameterSpec> CMGF1ParameterSpec::InitStatic(
    /* [in] */ const String& type)
{
    AutoPtr<CMGF1ParameterSpec> tmp, obj;
    CMGF1ParameterSpec::NewByFriend(type, (CMGF1ParameterSpec**)&tmp);
    return tmp;
}

CAR_OBJECT_IMPL(CMGF1ParameterSpec)
CAR_INTERFACE_IMPL_2(CMGF1ParameterSpec, Object, IMGF1ParameterSpec, IAlgorithmParameterSpec)
ECode CMGF1ParameterSpec::GetDigestAlgorithm(
    /* [out] */ String *algorithm)
{
    VALIDATE_NOT_NULL(algorithm)
    *algorithm = mMdName;
    return NOERROR;
}

ECode CMGF1ParameterSpec::constructor(
    /* [in] */ const String& mdName)
{
    mMdName = mdName;
    if (mMdName.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

}
}
}

