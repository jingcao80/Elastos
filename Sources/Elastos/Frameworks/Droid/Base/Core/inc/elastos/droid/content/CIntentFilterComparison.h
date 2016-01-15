
#ifndef __ELASTOS_DROID_CONTENT_CINTENTFILTERCOMPARISON_H__
#define __ELASTOS_DROID_CONTENT_CINTENTFILTERCOMPARISON_H__

#include "_Elastos_Droid_Content_CIntentFilterComparison.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentFilterComparison)
    , public Object
    , public IIntentFilterComparison
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CIntentFilterComparison();

    /**
     * Return the Intent that this FilterComparison represents.
     * @return Returns the Intent held by the FilterComparison.  Do
     * not modify!
     */
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI constructor(
        /* [in] */ IIntent* intent);

private:
    AutoPtr<IIntent> mIntent;
    Int32 mHashCode;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTFILTERCOMPARISON_H__
