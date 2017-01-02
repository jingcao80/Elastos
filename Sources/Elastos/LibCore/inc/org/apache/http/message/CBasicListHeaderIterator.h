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

#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICLISTHEADERITERATOR_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICLISTHEADERITERATOR_H_

#include "_Org_Apache_Http_Message_CBasicListHeaderIterator.h"
#include "org/apache/http/message/BasicListHeaderIterator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Implementation of a {@link HeaderIterator} based on a {@link List}.
 * For use by {@link HeaderGroup}.
 *
 * @version $Revision: 584542 $
 */
CarClass(CBasicListHeaderIterator) , public BasicListHeaderIterator
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IList* headers,
        /* [in] */ const String& value);
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICLISTHEADERITERATOR_H_
