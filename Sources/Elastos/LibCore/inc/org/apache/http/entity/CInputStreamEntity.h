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

#ifndef __ORG_APACHE_HTTP_ENTITY_CINPUTSTREAMENTITY_H_
#define __ORG_APACHE_HTTP_ENTITY_CINPUTSTREAMENTITY_H_

#include "Elastos.CoreLibrary.IO.h"
#include "_Org_Apache_Http_Entity_CInputStreamEntity.h"
#include "org/apache/http/entity/AbstractHttpEntity.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

/**
 * A streamed entity obtaining content from an {@link InputStream InputStream}.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 617591 $
 *
 * @since 4.0
 */
CarClass(CInputStreamEntity), public AbstractHttpEntity
{
public:
    CInputStreamEntity();

    CAR_OBJECT_DECL()

    CARAPI IsRepeatable(
        /* [out] */ Boolean* isRepeatable);

    CARAPI GetContentLength(
        /* [out] */ Int64* length);

    CARAPI GetContent(
        /* [out] */ IInputStream** inputStream);

    CARAPI WriteTo(
        /* [in] */ IOutputStream* outstream);

    CARAPI IsStreaming(
        /* [out] */ Boolean* isStreaming);

    CARAPI ConsumeContent();

    CARAPI constructor(
        /* [in] */ IInputStream* instream,
        /* [in] */ Int64 length);

private:
    static const Int32 BUFFER_SIZE = 2048;

    AutoPtr<IInputStream> mContent;
    Int64 mLength;
    Boolean mConsumed;
};

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_ENTITY_CINPUTSTREAMENTITY_H_
