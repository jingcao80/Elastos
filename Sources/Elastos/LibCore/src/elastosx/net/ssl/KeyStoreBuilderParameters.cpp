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

#include "Elastos.CoreLibrary.Security.h"
#include "KeyStoreBuilderParameters.h"
#include "Collections.h"

using Elastos::Utility::Collections;

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL_2(KeyStoreBuilderParameters, Object, IKeyStoreBuilderParameters, IManagerFactoryParameters)

ECode KeyStoreBuilderParameters::constructor(
    /* [in] */ IKeyStoreBuilder* builder)
{
    if (builder == NULL) {
        //throw new NullPointerException("builder == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    return Collections::SingletonList(TO_IINTERFACE(builder), (IList**)&mKsbuilders);
}

ECode KeyStoreBuilderParameters::constructor(
    /* [in] */ IList* parameters)
{
    if (parameters == NULL) {
        //throw new NullPointerException("parameters == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean result;
    if (parameters->IsEmpty(&result), result) {
        //throw new IllegalArgumentException("parameters.isEmpty()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return Collections::UnmodifiableList(parameters, (IList**)&mKsbuilders);
}

ECode KeyStoreBuilderParameters::GetParameters(
    /* [out] */ IList** param)
{
    VALIDATE_NOT_NULL(param)

    *param = mKsbuilders;
    REFCOUNT_ADD(*param);
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
