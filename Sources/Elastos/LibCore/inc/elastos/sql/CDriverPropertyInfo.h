
#ifndef __ELASTOS_SQL_CDRIVERPROPERTYINFO_H__
#define __ELASTOS_SQL_CDRIVERPROPERTYINFO_H__

#include "_Elastos_Sql_CDriverPropertyInfo.h"
#include "Object.h"

namespace Elastos {
namespace Sql {

CarClass(CDriverPropertyInfo)
    , public Object
    , public IDriverPropertyInfo
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDriverPropertyInfo();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

public:
    AutoPtr<ArrayOf<String> > mChoices;

    String mDescription;

    String mName;

    Boolean mRequired;

    String mValue;
};

} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_CDRIVERPROPERTYINFO_H__
