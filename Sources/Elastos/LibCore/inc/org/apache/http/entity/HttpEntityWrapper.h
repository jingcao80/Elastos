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

#ifndef __ORG_APACHE_HTTP_ENTITY_HTTPENTITYWRAPPER_H__
#define __ORG_APACHE_HTTP_ENTITY_HTTPENTITYWRAPPER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Org::Apache::Http::IHeader;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

/**
 * Base class for wrapping entities.
 * Keeps a {@link #wrappedEntity wrappedEntity} and delegates all
 * calls to it. Implementations of wrapping entities can derive
 * from this class and need to override only those methods that
 * should not be delegated to the wrapped entity.
 *
 * @version $Revision: 496070 $
 *
 * @since 4.0
 */
class HttpEntityWrapper
    : public Object
    , public IHttpEntity
{
public:
    CAR_INTERFACE_DECL()

    CARAPI IsRepeatable(
        /* [out] */ Boolean* isRepeatable);

    CARAPI IsChunked(
        /* [out] */ Boolean* isChunked);

    CARAPI GetContentLength(
        /* [out] */ Int64* length);

    CARAPI GetContentType(
        /* [out] */ IHeader** type);

    CARAPI GetContentEncoding(
        /* [out] */ IHeader** encoding);

    CARAPI GetContent(
        /* [out] */ IInputStream** inputStream);

    CARAPI WriteTo(
        /* [in] */ IOutputStream* outstream);

    CARAPI IsStreaming(
        /* [out] */ Boolean* isStreaming);

    CARAPI ConsumeContent();

protected:
    CARAPI Init(
        /* [in] */ IHttpEntity* entity);

protected:
    /** The wrapped entity. */
    AutoPtr<IHttpEntity> mWrappedEntity;
};

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_ENTITY_HTTPENTITYWRAPPER_H__
