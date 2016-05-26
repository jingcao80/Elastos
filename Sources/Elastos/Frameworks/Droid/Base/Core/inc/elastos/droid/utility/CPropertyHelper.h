#ifndef __ELASTOS_DROID_UTILITY_CPROPERTYHELPER_H__
#define __ELASTOS_DROID_UTILITY_CPROPERTYHELPER_H__
#include "_Elastos_Droid_Utility_CPropertyHelper.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CPropertyHelper)
{
public:
    CAR_OBJECT_DECL()

    CPropertyHelper();

    CARAPI constructor();

    CARAPI Of(
        /* [in] */ const ClassID& clsId,
        /* [in] */ CarDataType dataType,
        /* [in] */ const String& name,
        /* [out] */ IProperty** property);

};

} // namespace Utility
} // namepsace Droid
} // namespace Elastos

#endif
