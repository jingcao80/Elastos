#ifndef __ELASTOS_DROID_UTILITY_CEVENTLOGTAGSDESCRIPTION_H__
#define __ELASTOS_DROID_UTILITY_CEVENTLOGTAGSDESCRIPTION_H__

#include "_Elastos_Droid_Utility_CEventLogTagsDescription.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CEventLogTagsDescription)
{
public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CEventLogTagsDescription();

    ~CEventLogTagsDescription();

    CARAPI constructor(
        /* [in] */ Int32 * tag,
        /* [in] */ const String& * name);

public:
    Int32 mTag;
    String mName;
};

}
}
}

#endif // __ELASTOS_DROID_UTILITY_CEVENTLOGTAGSDESCRIPTION_H__
