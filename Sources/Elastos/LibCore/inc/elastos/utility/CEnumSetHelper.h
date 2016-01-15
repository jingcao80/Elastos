#ifndef __ELASTOS_UTILITY_CENUMSETHELPER_H__
#define __ELASTOS_UTILITY_CENUMSETHELPER_H__

#include "_Elastos_Utility_CEnumSetHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos{
namespace Utility{

CarClass (CEnumSetHelper)
    , public Singleton
    , public IEnumSetHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI NoneOf(
        /* [in] */ const InterfaceID& type,
        /* [out] */ IEnumSet** res);

    CARAPI AllOf(
        /* [in] */ const InterfaceID& type,
        /* [out] */ IEnumSet** res);

    CARAPI CopyOf(
        /* [in] */ IEnumSet* s,
        /* [out] */ IEnumSet** res);

    CARAPI CopyOf(
        /* [in] */ ICollection* c,
        /* [out] */ IEnumSet** set);

    CARAPI ComplementOf(
        /* [in] */ IEnumSet* s,
        /* [out] */ IEnumSet** res);

   CARAPI  Of(
        /* [in] */ IInterface* i,
        /* [out] */ IEnumSet** res);

    CARAPI Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [out] */ IEnumSet** res);

    CARAPI Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [in] */ IInterface* i3,
        /* [out] */ IEnumSet** res);

    CARAPI Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [in] */ IInterface* i3,
        /* [in] */ IInterface* i4,
        /* [out] */ IEnumSet** res);

    CARAPI Of(
        /* [in] */ IInterface* i1,
        /* [in] */ IInterface* i2,
        /* [in] */ IInterface* i3,
        /* [in] */ IInterface* i4,
        /* [in] */ IInterface* i5,
        /* [out] */ IEnumSet** res);

    CARAPI Of(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [out] */ IEnumSet** res);

    CARAPI Range(
        /* [in] */ IInterface* start,
        /* [in] */ IInterface* end,
        /* [out] */ IEnumSet** set);
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CENUMSETHELPER_H__