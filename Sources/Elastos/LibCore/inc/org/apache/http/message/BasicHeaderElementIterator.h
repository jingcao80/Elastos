
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
