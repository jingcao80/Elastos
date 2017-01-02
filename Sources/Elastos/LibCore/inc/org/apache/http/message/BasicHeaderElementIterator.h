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

#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICHEADERELEMENTITERATOR_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICHEADERELEMENTITERATOR_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IIterator;
using Org::Apache::Http::IHeaderElementIterator;
using Org::Apache::Http::IHeaderIterator;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of a {@link HeaderElementIterator}.
 *
 * @version $Revision: 592088 $
 *
 * @author Andrea Selva <selva.andre at gmail.com>
 * @author Oleg Kalnichevski <oleg at ural.ru>
 */
class BasicHeaderElementIterator
    : public Object
    , public IHeaderElementIterator
    , public IIterator
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicHeaderElementIterator() {}

    CARAPI HasNext(
        /* [out] */ Boolean* hasNext);

    CARAPI NextElement(
        /* [out] */ IHeaderElement** element);

    CARAPI GetNext(
        /* [out] */ IInterface** object);

    CARAPI Remove();

protected:
    /**
     * Creates a new instance of BasicHeaderElementIterator
     */
    CARAPI Init(
        /* [in] */ IHeaderIterator* headerIterator,
        /* [in] */ IHeaderValueParser* parser);

    CARAPI Init(
        /* [in] */ IHeaderIterator* headerIterator);

private:
    CARAPI_(void) BufferHeaderValue();

    CARAPI_(void) ParseNextElement();

private:
    AutoPtr<IHeaderIterator> mHeaderIt;
    AutoPtr<IHeaderValueParser> mParser;

    AutoPtr<IHeaderElement> mCurrentElement;
    AutoPtr<ICharArrayBuffer> mBuffer;
    AutoPtr<IParserCursor> mCursor;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICHEADERELEMENTITERATOR_H_
