#ifndef __ELASTOSX_XML_DATATYPE_XMLGREGORIANCALENDAR_H__
#define __ELASTOSX_XML_DATATYPE_XMLGREGORIANCALENDAR_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::ICloneable;
using Elastos::Math::IBigDecimal;

namespace Elastosx {
namespace Xml {
namespace Datatype {

class XMLGregorianCalendar
    : public Object
    , public IXMLGregorianCalendar
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI SetTime(
            /* [in] */ Int32 hour,
            /* [in] */ Int32 minute,
            /* [in] */ Int32 second);

    CARAPI SetTime(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second,
        /* [in] */ IBigDecimal* fractional);

    CARAPI SetTime(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second,
        /* [in] */ Int32 millisecond);

    CARAPI GetMillisecond(
        /* [out] */ Int32* second);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* info);
};

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx

#endif
