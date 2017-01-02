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

#ifndef __ORG_APACHE_HTTP_IMPL_ENTITY_EntitySERIALIZER_H__
#define __ORG_APACHE_HTTP_IMPL_ENTITY_EntitySERIALIZER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::IO::IOutputStream;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::Entity::IBasicHttpEntity;
using Org::Apache::Http::Entity::IContentLengthStrategy;
using Org::Apache::Http::IO::ISessionOutputBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Entity {

/**
 * Default implementation of an entity serializer.
 * <p>
 * This entity serializer currently supports only "chunked" and "identitiy" transfer-coding</a>
 * </p>
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 560343 $
 *
 * @since 4.0
 */
class EntitySerializer : public Object
{
public:
    EntitySerializer(
        /* [in] */ IContentLengthStrategy* lenStrategy);

    CARAPI Serialize(
        /* [in] */ ISessionOutputBuffer* outbuffer,
        /* [in] */ IHttpMessage* message,
        /* [in] */ IHttpEntity* entity);

protected:
    CARAPI DoSerialize(
        /* [in] */ ISessionOutputBuffer* outbuffer,
        /* [in] */ IHttpMessage* message,
        /* [out] */ IOutputStream** output);

private:
    AutoPtr<IContentLengthStrategy> mLenStrategy;
};

} // namespace Entity
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_ENTITY_EntitySERIALIZER_H__
