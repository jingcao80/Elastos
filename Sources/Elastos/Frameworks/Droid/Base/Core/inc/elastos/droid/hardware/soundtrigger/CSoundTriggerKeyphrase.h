
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASE_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASE_H__

#include "_Elastos_Droid_Hardware_Soundtrigger_CSoundTriggerKeyphrase.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CarClass(CSoundTriggerKeyphrase)
    , public Object
    , public ISoundTriggerKeyphrase
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSoundTriggerKeyphrase();

    CARAPI constructor();

     /**
     * Create a segment of a carrier frequency range.
     *
     * @param min The minimum transmittable frequency in this range segment.
     * @param max The maximum transmittable frequency in this range segment.
     */
    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ Int32 recognitionModes,
        /* [in] */ const String& locale,
        /* [in] */ const String& text,
        /* [in] */ ArrayOf<Int32>* users);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface *obj,
        /* [out] */ Boolean *equal);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    // public static final Parcelable.Creator<Keyphrase> CREATOR
    //         = new Parcelable.Creator<Keyphrase>() {
    //     public Keyphrase createFromParcel(Parcel in) {
    //         return Keyphrase.fromParcel(in);
    //     }

    //     public Keyphrase[] newArray(int size) {
    //         return new Keyphrase[size];
    //     }
    // };

    CARAPI GetId(
        /* [out] */ Int32* id);

    CARAPI GetRecognitionModes(
        /* [out] */ Int32* modes);

    CARAPI GetLocale(
        /* [out] */ String* locale);

    CARAPI GetText(
        /* [out] */ String* text);

    CARAPI GetUsers(
        /* [out, callee] */ ArrayOf<Int32>** users);

private:
    static CARAPI FromParcel(
        /* [in] */ IParcel* source,
        /* [out] */ ISoundTriggerKeyphrase** result);

public:
    /** Unique identifier for this keyphrase */
    Int32 mId;

    /** Recognition modes supported for this key phrase in the model */
    Int32 mRecognitionModes;

    /** Locale of the keyphrase. JAVA Locale string e.g en_US */
    String mLocale;

    /** Key phrase text */
    String mText;

    /** Users this key phrase has been trained for. countains sound trigger specific user IDs
     * derived from system user IDs {@link android.os.UserHandle#getIdentifier()}. */
    AutoPtr<ArrayOf<Int32> > mUsers;
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASE_H__
