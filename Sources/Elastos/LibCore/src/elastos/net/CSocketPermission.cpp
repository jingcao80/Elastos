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

#include "CSocketPermission.h"
#include "InetAddress.h"
#include "StringBuffer.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL_2(CSocketPermission, Object, ISocketPermission, ISerializable)

CAR_OBJECT_IMPL(CSocketPermission)

CSocketPermission::CSocketPermission()
    // TODO : Permission(String(""))
{}

ECode CSocketPermission::constructor(
    /* [in] */ const String& host,
    /* [in] */ const String& action)
{
    //Permission::Permission(String(""));
    return NOERROR;
}

ECode CSocketPermission::GetActions(
    /* [out] */ String* actions)
{
    VALIDATE_NOT_NULL(actions);
    *actions = String(NULL);

    return NOERROR;
}

ECode CSocketPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* isImplied)
{
    VALIDATE_NOT_NULL(isImplied);

    *isImplied = TRUE;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
