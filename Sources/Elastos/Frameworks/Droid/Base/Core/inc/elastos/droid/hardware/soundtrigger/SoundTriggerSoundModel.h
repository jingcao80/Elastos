
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGERSOUNDMODEL_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGERSOUNDMODEL_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

class SoundTriggerSoundModel
    : public Object
    , public ISoundTriggerSoundModel
{
public:
    CAR_INTERFACE_DECL()

    SoundTriggerSoundModel();

    virtual ~SoundTriggerSoundModel();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUUID* uuid,
        /* [in] */ IUUID* vendorUuid,
        /* [in] */ Int32 type,
        /* [in] */ ArrayOf<Byte>* data);

    /** Unique sound model identifier */
    CARAPI GetUuid(
        /* [out] */ IUUID** uuid);

    /** Sound model type (e.g. TYPE_KEYPHRASE); */
    CARAPI GetType(
        /* [out] */ Int32* type);

    /** Unique sound model vendor identifier */
    CARAPI GetVendorUuid(
        /* [out] */ IUUID** uuid);

    /** Opaque data. For use by vendor implementation and enrollment application */
    CARAPI Getdata(
        /* [out, callee] */ ArrayOf<Byte>** data);

public:
    /** Unique sound model identifier */
    AutoPtr<IUUID> mUuid;

    /** Sound model type (e.g. TYPE_KEYPHRASE); */
    Int32 mType;

    /** Unique sound model vendor identifier */
    AutoPtr<IUUID> mVendorUuid;

    /** Opaque data. For use by vendor implementation and enrollment application */
    AutoPtr<ArrayOf<Byte> > mData;
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGERSOUNDMODEL_H__
