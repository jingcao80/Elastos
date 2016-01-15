
#ifndef __ORG_APACHE_HTTP_ENTITY_STRINGENTITY_H_
#define __ORG_APACHE_HTTP_ENTITY_STRINGENTITY_H_

#include "org/apache/http/entity/AbstractHttpEntity.h"

using Elastos::Core::ICloneable;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

/**
 *  An entity whose content is retrieved from a string.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 618367 $
 *
 * @since 4.0
 */
class StringEntity
    : public AbstractHttpEntity
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

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

protected:
    CARAPI Init(
        /* [in] */ const String& s,
        /* [in] */ const String& charset);

    CARAPI Init(
        /* [in] */ const String& s);

    CARAPI_(void) CloneImpl(
        /* [in] */ StringEntity* o);

protected:
    AutoPtr< ArrayOf<Byte> > mContent;
};

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_ENTITY_STRINGENTITY_H_
