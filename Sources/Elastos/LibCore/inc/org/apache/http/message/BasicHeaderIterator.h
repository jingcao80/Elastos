
#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICHEADERITERATOR_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICHEADERITERATOR_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IIterator;
using Org::Apache::Http::IHeaderIterator;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of a {@link HeaderIterator}.
 *
 * @version $Revision: 581981 $
 */
class BasicHeaderIterator
    : public Object
    , public IBasicHeaderIterator
    , public IHeaderIterator
    , public IIterator
{
public:
    BasicHeaderIterator();

    CAR_INTERFACE_DECL()

    virtual ~BasicHeaderIterator() {}

    /**
     * Indicates whether there is another header in this iteration.
     *
     * @return  <code>true</code> if there is another header,
     *          <code>false</code> otherwise
     */
    CARAPI HasNext(
        /* [out] */ Boolean* hasNext);


    /**
     * Obtains the next header from this iteration.
     *
     * @return  the next header in this iteration
     *
     * @throws NoSuchElementException   if there are no more headers
     */
    CARAPI NextHeader(
        /* [out] */ IHeader** nextHeader);

    /**
     * Returns the next header.
     * Same as {@link #nextHeader nextHeader}, but not type-safe.
     *
     * @return  the next header in this iteration
     *
     * @throws NoSuchElementException   if there are no more headers
     */
    CARAPI GetNext(
        /* [out] */ IInterface** object);

    /**
     * Removing headers is not supported.
     *
     * @throws UnsupportedOperationException    always
     */
    CARAPI Remove();

protected:
    /**
     * Creates a new header iterator.
     *
     * @param headers   an array of headers over which to iterate
     * @param name      the name of the headers over which to iterate, or
     *                  <code>null</code> for any
     */
    CARAPI Init(
        /* [in] */ ArrayOf<IHeader*>* headers,
        /* [in] */ const String& name);

    /**
     * Determines the index of the next header.
     *
     * @param from      one less than the index to consider first,
     *                  -1 to search for the first header
     *
     * @return  the index of the next header that matches the filter name,
     *          or negative if there are no more headers
     */
    CARAPI FindNext(
        /* [in] */ Int32 from,
        /* [out] */ Int32* index);

    /**
     * Checks whether a header is part of the iteration.
     *
     * @param index     the index of the header to check
     *
     * @return  <code>true</code> if the header should be part of the
     *          iteration, <code>false</code> to skip
     */
    CARAPI FilterHeader(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* result);

protected:
    /**
     * An array of headers to iterate over.
     * Not all elements of this array are necessarily part of the iteration.
     * This array will never be modified by the iterator.
     * Derived implementations are expected to adhere to this restriction.
     */
    AutoPtr< ArrayOf<IHeader*> > mAllHeaders;

    /**
     * The position of the next header in {@link #allHeaders allHeaders}.
     * Negative if the iteration is over.
     */
    Int32 mCurrentIndex;

    /**
     * The header name to filter by.
     * <code>null</code> to iterate over all headers in the array.
     */
    String mHeaderName;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BasicHeaderIterator_H_
