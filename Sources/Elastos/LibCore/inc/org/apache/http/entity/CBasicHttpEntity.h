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

#ifndef __ORG_APACHE_HTTP_ENTITY_CBASICHTTPENTITY_H_
#define __ORG_APACHE_HTTP_ENTITY_CBASICHTTPENTITY_H_

#include "Elastos.CoreLibrary.IO.h"
#include "_Org_Apache_Http_Entity_CBasicHttpEntity.h"
#include "org/apache/http/entity/AbstractHttpEntity.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

/**
 * A generic streamed entity being received on a connection.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 496070 $
 *
 * @since 4.0
 */
CarClass(CBasicHttpEntity)
    , public AbstractHttpEntity
    , public IBasicHttpEntity
{
public:
    CBasicHttpEntity();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetContentLength(
        /* [out] */ Int64* length);

    CARAPI GetContent(
        /* [out] */ IInputStream** inputStream);

    CARAPI IsRepeatable(
        /* [out] */ Boolean* isRepeatable);

    /**
     * Specifies the length of the content.
     *
     * @param len       the number of bytes in the content, or
     *                  a negative number to indicate an unknown length
     */
    CARAPI SetContentLength(
        /* [in] */ Int64 len);

    /**
     * Specifies the content.
     *
     * @param instream          the stream to return with the next call to
     *                          {@link #getContent getContent}
     */
    CARAPI SetContent(
        /* [in] */ IInputStream* instream);

    CARAPI WriteTo(
        /* [in] */ IOutputStream* outstream);

    CARAPI IsStreaming(
        /* [out] */ Boolean* isStreaming);

    CARAPI ConsumeContent();

    CARAPI constructor();

private:
    AutoPtr<IInputStream> mContent;
    Boolean mContentObtained;
    Int64 mLength;
};

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_ENTITY_CBASICHTTPENTITY_H_
