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

#ifndef __ORG_APACHE_HTTP_ENTITY_CBUFFEREDHTTPENTITY_H_
#define __ORG_APACHE_HTTP_ENTITY_CBUFFEREDHTTPENTITY_H_

#include "_Org_Apache_Http_Entity_CBufferedHttpEntity.h"
#include "org/apache/http/entity/HttpEntityWrapper.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

/**
 * A wrapping entity that buffers it content if necessary.
 * The buffered entity is always repeatable.
 * If the wrapped entity is repeatable itself, calls are passed through.
 * If the wrapped entity is not repeatable, the content is read into a
 * buffer once and provided from there as often as required.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 496070 $
 *
 * @since 4.0
 */
CarClass(CBufferedHttpEntity) , public HttpEntityWrapper
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetContentLength(
        /* [out] */ Int64* length);

    CARAPI GetContent(
        /* [out] */ IInputStream** inputStream);

    CARAPI IsChunked(
        /* [out] */ Boolean* isChunked);

    CARAPI IsRepeatable(
        /* [out] */ Boolean* isRepeatable);

    CARAPI WriteTo(
        /* [in] */ IOutputStream* outstream);

    CARAPI IsStreaming(
        /* [out] */ Boolean* isStreaming);

    CARAPI constructor(
        /* [in] */ IHttpEntity* entity);

private:
    AutoPtr< ArrayOf<Byte> > mBuffer;
};

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_ENTITY_CBUFFEREDHTTPENTITY_H_
