
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERMODULEPROPERTIES_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERMODULEPROPERTIES_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Hardware_Soundtrigger_CSoundTriggerModuleProperties.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CarClass(CSoundTriggerModuleProperties)
    , public Object
    , public ISoundTriggerModuleProperties
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSoundTriggerModuleProperties();

    CARAPI constructor();

     /**
     * Create a segment of a carrier frequency range.
     *
     * @param min The minimum transmittable frequency in this range segment.
     * @param max The maximum transmittable frequency in this range segment.
     */
    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ const String& implementor,
        /* [in] */ const String& description,
        /* [in] */ const String& uuid,
        /* [in] */ Int32 version,
        /* [in] */ Int32 maxSoundModels,
        /* [in] */ Int32 maxKeyphrases,
        /* [in] */ Int32 maxUsers,
        /* [in] */ Int32 recognitionModes,
        /* [in] */ Boolean supportsCaptureTransition,
        /* [in] */ Int32 maxBufferMs,
        /* [in] */ Boolean supportsConcurrentCapture,
        /* [in] */ Int32 powerConsumptionMw,
        /* [in] */ Boolean returnsTriggerInEvent);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    // public static final Parcelable.Creator<ModuleProperties> CREATOR
    //         = new Parcelable.Creator<ModuleProperties>() {
    //     public ModuleProperties createFromParcel(Parcel in) {
    //         return ModuleProperties.fromParcel(in);
    //     }

    //     public ModuleProperties[] newArray(int size) {
    //         return new ModuleProperties[size];
    //     }
    // };

private:
    static CARAPI FromParcel(
        /* [in] */ IParcel* source,
        /* [out] */ ISoundTriggerModuleProperties** result);

public:
    /** Unique module ID provided by the native service */
    Int32 mId;

    /** human readable voice detection engine implementor */
    String mImplementor;

    /** human readable voice detection engine description */
    String mDescription;

    /** Unique voice engine Id (changes with each version) */
    AutoPtr<IUUID> mUuid;

    /** Voice detection engine version */
    Int32 mVersion;

    /** Maximum number of active sound models */
    Int32 mMaxSoundModels;

    /** Maximum number of key phrases */
    Int32 mMaxKeyphrases;

    /** Maximum number of users per key phrase */
    Int32 mMaxUsers;

    /** Supported recognition modes (bit field, RECOGNITION_MODE_VOICE_TRIGGER ...) */
    Int32 mRecognitionModes;

    /** Supports seamless transition to capture mode after recognition */
    Boolean mSupportsCaptureTransition;

    /** Maximum buffering capacity in ms if supportsCaptureTransition() is true */
    Int32 mMaxBufferMs;

    /** Supports capture by other use cases while detection is active */
    Boolean mSupportsConcurrentCapture;

    /** Rated power consumption when detection is active with TDB silence/sound/speech ratio */
    Int32 mPowerConsumptionMw;

    /** Returns the trigger (key phrase) capture in the binary data of the
     * recognition callback event */
    Boolean mReturnsTriggerInEvent;
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERMODULEPROPERTIES_H__
