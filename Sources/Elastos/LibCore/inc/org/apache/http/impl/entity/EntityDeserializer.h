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

#ifndef __ORG_APACHE_HTTP_IMPL_ENTITY_ENTITYDESERIALIZER_H__
#define __ORG_APACHE_HTTP_IMPL_ENTITY_ENTITYDESERIALIZER_H__

#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::Entity::IBasicHttpEntity;
using Org::Apache::Http::Entity::IContentLengthStrategy;
using Org::Apache::Http::IO::ISessionInputBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Entity {

class EntityDeserializer : public Object
{
public:
    EntityDeserializer(
        /* [in] */ IContentLengthStrategy* lenStrategy);

    CARAPI Deserialize(
        /* [in] */ ISessionInputBuffer* inbuffer,
        /* [in] */ IHttpMessage* message,
        /* [out] */ IHttpEntity** entity);

protected:
    CARAPI DoDeserialize(
        /* [in] */ ISessionInputBuffer* inbuffer,
        /* [in] */ IHttpMessage* message,
        /* [out] */ IBasicHttpEntity** entity);

private:
    AutoPtr<IContentLengthStrategy> mLenStrategy;
};

} // namespace Entity
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_ENTITY_ENTITYDESERIALIZER_H__
