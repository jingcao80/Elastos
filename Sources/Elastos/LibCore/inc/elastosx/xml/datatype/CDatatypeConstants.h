
#ifndef __ELASTOSX_XML_DATATYPE_CQNAMEHELPER_H__
#define __ELASTOSX_XML_DATATYPE_CQNAMEHELPER_H__

#include "Singleton.h"
#include "_Elastosx_Xml_Datatype_CDatatypeConstants.h"

using Elastos::Core::Singleton;
using Elastos::Core::Object;
using Elastosx::Xml::Namespace::IQName;

namespace Elastosx {
namespace Xml {
namespace Datatype {

CarClass(CDatatypeConstants)
    , public Singleton
    , public IDatatypeConstants
{
public:
    class Field
        : public Object
        , public IDatatypeConstantsField
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI GetId(
            /* [out] */ Int32* id);

    private:
        Field(
            /* [in] */ String str,
            /* [in] */ Int32 id);

        String mStr;
        Int32 mId;
    friend class CDatatypeConstants;
    };

    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetQNameForDATETIME(
        /* [out] */ IQName** datetime);

    CARAPI GetQNameForTIME(
        /* [out] */ IQName** time);

    CARAPI GetQNameForDATE(
        /* [out] */ IQName** date);

    CARAPI GetQNameForGYEARMONTH(
        /* [out] */ IQName** gyearmonth);

    CARAPI GetQNameForGMONTHDAY(
        /* [out] */ IQName** gmonthday);

    CARAPI GetQNameForGYEAR(
        /* [out] */ IQName** gyear);

    CARAPI GetQNameForGMONTH(
        /* [out] */ IQName** gmonth);

    CARAPI GetQNameForGDAY(
        /* [out] */ IQName** gday);

    CARAPI GetQNameForDURATION(
        /* [out] */ IQName** duration);

    CARAPI GetQNameForDURATION_DAYTIME(
        /* [out] */ IQName** duration_daytime);

    CARAPI GetQNameForDURATION_YEARMONTH(
        /* [out] */ IQName** duration_yearmonth);

    CARAPI GetFieldForYEARS(
        /* [out] */ IDatatypeConstantsField** years);

    CARAPI GetFieldForMONTHS(
        /* [out] */ IDatatypeConstantsField** months);

    CARAPI GetFieldForDAYS(
        /* [out] */ IDatatypeConstantsField** days);

    CARAPI GetFieldForHOURS(
        /* [out] */ IDatatypeConstantsField** hours);

    CARAPI GetFieldForMINUTES(
        /* [out] */ IDatatypeConstantsField** minutes);

    CARAPI GetFieldForSECONDS(
        /* [out] */ IDatatypeConstantsField** seconds);

public:
    static const AutoPtr<IQName> DATETIME;
    static const AutoPtr<IQName> TIME;
    static const AutoPtr<IQName> DATE;
    static const AutoPtr<IQName> GYEARMONTH;
    static const AutoPtr<IQName> GMONTHDAY;
    static const AutoPtr<IQName> GYEAR;
    static const AutoPtr<IQName> GMONTH;
    static const AutoPtr<IQName> GDAY;
    static const AutoPtr<IQName> DURATION;
    static const AutoPtr<IQName> DURATION_DAYTIME;
    static const AutoPtr<IQName> DURATION_YEARMONTH;

    static const AutoPtr<IDatatypeConstantsField> YEARS;
    static const AutoPtr<IDatatypeConstantsField> MONTHS;
    static const AutoPtr<IDatatypeConstantsField> DAYS;
    static const AutoPtr<IDatatypeConstantsField> HOURS;
    static const AutoPtr<IDatatypeConstantsField> MINUTES;
    static const AutoPtr<IDatatypeConstantsField> SECONDS;
};

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx

#endif // __ELASTOSX_XML_DATATYPE_CQNAMEHELPER_H__
