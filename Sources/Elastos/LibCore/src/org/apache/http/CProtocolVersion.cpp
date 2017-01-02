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

#include "org/apache/http/CProtocolVersion.h"

namespace Org {
namespace Apache {
namespace Http {

CAR_OBJECT_IMPL(CProtocolVersion)

ECode CProtocolVersion::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<IProtocolVersion> ver;
    CProtocolVersion::New((IProtocolVersion**)&ver);
    CloneImpl(ver);
    *obj = ver->Probe(EIID_IInterface);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CProtocolVersion::constructor(
    /* [in] */ const String& protocol,
    /* [in] */ Int32 major,
    /* [in] */ Int32 minor)
{
    return ProtocolVersion::Init(protocol, major, minor);
}

ECode CProtocolVersion::constructor()
{
    return NOERROR;
}

} // namespace Http
} // namespace Apache
} // namespace Org
