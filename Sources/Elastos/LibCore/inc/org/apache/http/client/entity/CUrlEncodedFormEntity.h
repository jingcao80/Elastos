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

#ifndef __ORG_APACHE_HTTP_CLIENT_ENTITY_CURLENCODEDFORMENTITY_H_
#define __ORG_APACHE_HTTP_CLIENT_ENTITY_CURLENCODEDFORMENTITY_H_

#include "_Org_Apache_Http_Client_Entity_CUrlEncodedFormEntity.h"
#include "org/apache/http/entity/StringEntity.h"

using Elastos::Utility::IList;
using Org::Apache::Http::Entity::StringEntity;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Entity {

CarClass(CUrlEncodedFormEntity)
    , public StringEntity
{
public:
    CAR_OBJECT_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    CARAPI constructor(
        /* [in] */ IList* parameters,
        /* [in] */ const String& encoding);

    CARAPI constructor(
        /* [in] */ IList* parameters);

    CARAPI constructor();
};

} // namespace Entity
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_ENTITY_CURLENCODEDFORMENTITY_H_
