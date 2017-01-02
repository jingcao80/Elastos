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

#include "elastos/droid/net/LocalSocketAddress.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(LocalSocketAddress, Object, ILocalSocketAddress)

ECode LocalSocketAddress::constructor(
    /* [in] */ const String& name,
    /* [in] */ LocalSocketAddressNamespace ns)
{
    mName = name;
    mNamespace = ns;
    return NOERROR;
}

ECode LocalSocketAddress::constructor(
    /* [in] */ const String& name)
{
    return constructor(name, LocalSocketAddressNamespace_ABSTRACT);
}

ECode LocalSocketAddress::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode LocalSocketAddress::GetNamespace(
    /* [out] */ LocalSocketAddressNamespace* ns)
{
    VALIDATE_NOT_NULL(ns);
    *ns = mNamespace;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
