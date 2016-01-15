
#ifndef __ORG_APACHE_HTTP_ENTITY_CSERIALIZABLEENTITY_H_
#define __ORG_APACHE_HTTP_ENTITY_CSERIALIZABLEENTITY_H_

#include "_Org_Apache_Http_Entity_CSerializableEntity.h"
#include "org/apache/http/entity/AbstractHttpEntity.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::ISerializable;

namespace Org {
namespace Apache {
namespace Http {
namespace Entity {

CarClass(CSerializableEntity), public AbstractHttpEntity
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetContent(
        /* [out] */ IInputStream** inputStream);

    CARAPI GetContentLength(
        /* [out] */ Int64* length);

    CARAPI IsRepeatable(
        /* [out] */ Boolean* isRepeatable);

    CARAPI IsStreaming(
        /* [out] */ Boolean* isStreaming);

    CARAPI WriteTo(
        /* [in] */ IOutputStream* outstream);

    CARAPI constructor(
        /* [in] */ ISerializable* ser,
        /* [in] */ Boolean bufferize);

private:
    CARAPI CreateBytes(
        /* [in] */ ISerializable* ser);

private:
    AutoPtr< ArrayOf<Byte> > mObjSer;

    AutoPtr<ISerializable> mObjRef;
};

} // namespace Entity
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_ENTITY_CSERIALIZABLEENTITY_H_
