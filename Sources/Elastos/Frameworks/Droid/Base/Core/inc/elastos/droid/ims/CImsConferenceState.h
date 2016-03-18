
#ifndef __ELASTOS_DROID_IMS_CIMSCONFERENCESTATE_H__
#define __ELASTOS_DROID_IMS_CIMSCONFERENCESTATE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Ims_CImsConferenceState.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsConferenceState)
    , public Object
    , public IImsConferenceState
    , public IParcelable
{
public:
    CImsConferenceState();

    virtual ~CImsConferenceState();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    AutoPtr<IHashMap> mParticipants;
};

} // namespace Ims
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSCONFERENCESTATE_H__
