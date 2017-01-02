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

#include "ValidatorHandler.h"
#include "utility/logging/Slogger.h"

namespace Elastosx {
namespace Xml {
namespace Validation {

CAR_INTERFACE_IMPL_2(ValidatorHandler, Object, IValidatorHandler, IContentHandler)

ECode ValidatorHandler::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* feature)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode ValidatorHandler::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode ValidatorHandler::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface* object)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

ECode ValidatorHandler::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface** property)
{
    if (name == NULL) {
        SLOGGERD("Validator", "name == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return E_SAX_NOT_RECOGNIZED_EXCEPTION;
}

} // namespace Validation
} // namespace Xml
} // namespace Elastosx
