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

#include "org/apache/http/message/CBasicStatusLine.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_OBJECT_IMPL(CBasicStatusLine)

ECode CBasicStatusLine::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<CBasicStatusLine> line;
    CBasicStatusLine::NewByFriend((CBasicStatusLine**)&line);
    CloneImpl((BasicStatusLine*)line.Get());
    return NOERROR;
}

ECode CBasicStatusLine::constructor(
    /* [in] */ IProtocolVersion* version,
    /* [in] */ Int32 statusCode,
    /* [in] */ const String& reasonPhrase)
{
    return Init(version, statusCode, reasonPhrase);
}

ECode CBasicStatusLine::constructor()
{
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
