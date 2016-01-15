
#ifndef __ELASTOS_DROID_UTILITY_CPARCELABLEOBJECTCONTAINER_H__
#define __ELASTOS_DROID_UTILITY_CPARCELABLEOBJECTCONTAINER_H__

#include "_Elastos_Droid_Utility_CParcelableObjectContainer.h"
#include <container.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CParcelableObjectContainer), public SimpleContainer
{
public:
    CParcelableObjectContainer();

    virtual ~CParcelableObjectContainer();

    CARAPI constructor();

    CARAPI Add(
        /* [in] */ IInterface* object);

    CARAPI Remove(
        /* [in] */ IInterface* object);

    CARAPI GetObjectEnumerator(
        /* [out] */ IObjectEnumerator** enumerator);

    CARAPI GetObjectCount(
        /* [out] */ Int32* count);

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* contains);

    CARAPI ContainsAll(
        /* [in] */ IObjectContainer* objectContainer,
        /* [out] */ Boolean* contains);

    CARAPI Dispose();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    Int32 mCount;
    Int32 mState;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_UTILITY_CPARCELABLEOBJECTCONTAINER_H__
