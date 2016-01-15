
#ifndef __ORG_APACHE_HTTP_MESSAGE_BUFFEREDHEADER_H_
#define __ORG_APACHE_HTTP_MESSAGE_BUFFEREDHEADER_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IFormattedHeader;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * This class represents a raw HTTP header whose content is parsed 'on demand'
 * only when the header value needs to be consumed.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 604625 $ $Date: 2007-12-16 06:11:11 -0800 (Sun, 16 Dec 2007) $
 */
class BufferedHeader
    : public Object
    , public IFormattedHeader
    , public IHeader
    , public ICloneable
{
public:
    BufferedHeader();

    CAR_INTERFACE_DECL()

    virtual ~BufferedHeader() {}

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetValue(
        /* [out] */ String* value);

    CARAPI GetElements(
        /* [out, callee] */ ArrayOf<IHeaderElement*>** elements);

    CARAPI GetValuePos(
        /* [out] */ Int32* pos);

    CARAPI GetBuffer(
        /* [out] */ ICharArrayBuffer** buffer);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    /**
     * Creates a new header from a buffer.
     * The name of the header will be parsed immediately,
     * the value only if it is accessed.
     *
     * @param buffer    the buffer containing the header to represent
     *
     * @throws ParseException   in case of a parse error
     */
    CARAPI Init(
        /* [in] */ ICharArrayBuffer* buffer);

    CARAPI_(void) CloneImpl(
        /* [in] */ BufferedHeader* obj);

private:
    /**
     * Header name.
     */
    String mName;

    /**
     * The buffer containing the entire header line.
     */
    AutoPtr<ICharArrayBuffer> mBuffer;

    /**
     * The beginning of the header value in the buffer
     */
    Int32 mValuePos;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BUFFEREDHEADER_H_
