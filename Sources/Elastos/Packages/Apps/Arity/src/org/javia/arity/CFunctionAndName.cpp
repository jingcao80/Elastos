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

#include "org/javia/arity/CFunctionAndName.h"

using Org::Javia::Arity::EIID_IFunctionAndName;

namespace Org {
namespace Javia {
namespace Arity {

CAR_OBJECT_IMPL(CFunctionAndName)

CAR_INTERFACE_IMPL(CFunctionAndName, Object, IFunctionAndName)

ECode CFunctionAndName::constructor(
    /* [in] */ IFunction* fun,
    /* [in] */ const String& name)
{
    mFunction = fun;
    mName = name;
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org