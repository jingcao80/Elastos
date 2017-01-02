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

#include "org/apache/http/entity/CStringEntity.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CAR_OBJECT_IMPL(CStringEntity)

ECode CStringEntity::Clone(
    /* [out] */ IInterface** o)
{
    VALIDATE_NOT_NULL(o)
    AutoPtr<CStringEntity> entity;
    CStringEntity::NewByFriend((CStringEntity**)&entity);
    CloneImpl((StringEntity*)entity);
    *o = entity->Probe(EIID_IInterface);
    REFCOUNT_ADD(*o)
    return NOERROR;
}

ECode CStringEntity::constructor(
    /* [in] */ const String& s,
    /* [in] */ const String& charset)
{
    return Init(s, charset);
}

ECode CStringEntity::constructor(
    /* [in] */ const String& s)
{
    return Init(s);
}

ECode CStringEntity::constructor()
{
    return NOERROR;
}

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org
