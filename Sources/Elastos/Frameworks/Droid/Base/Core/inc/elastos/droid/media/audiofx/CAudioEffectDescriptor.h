
#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CAUDIOEFFECTDESCRIPTOR_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CAUDIOEFFECTDESCRIPTOR_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_Audiofx_CAudioEffectDescriptor.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * The effect descriptor contains information on a particular effect implemented in the
 * audio framework:<br>
 * <ul>
 *  <li>type: UUID corresponding to the OpenSL ES interface implemented by this effect</li>
 *  <li>uuid: UUID for this particular implementation</li>
 *  <li>connectMode: {@link #EFFECT_INSERT}, {@link #EFFECT_AUXILIARY} or
 *  {at_link #EFFECT_PRE_PROCESSING}</li>
 *  <li>name: human readable effect name</li>
 *  <li>implementor: human readable effect implementor name</li>
 * </ul>
 * The method {@link #queryEffects()} returns an array of Descriptors to facilitate effects
 * enumeration.
 */
CarClass(CAudioEffectDescriptor)
    , public Object
    , public IAudioEffectDescriptor
{
public:
    CAudioEffectDescriptor();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& type,
        /* [in] */ const String& uuid,
        /* [in] */ const String& connectMode,
        /* [in] */ const String& name,
        /* [in] */ const String& implementor);

    CARAPI GetType(
        /* [out] */ IUUID** type);

    CARAPI SetType(
        /* [in] */ IUUID* type);

    CARAPI GetUuid(
        /* [out] */ IUUID** uuid);

    CARAPI SetUuid(
        /* [in] */ IUUID* uuid);

    CARAPI GetConnectMode(
        /* [out] */ String* connectMode);

    CARAPI SetConnectMode(
        /* [in] */ const String& connectMode);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetImplementor(
        /* [out] */ String* implementor);

    CARAPI SetImplementor(
        /* [in] */ const String& implementor);

public:
    /**
     *  Indicates the generic type of the effect (Equalizer, Bass boost ...). The UUID
     *  corresponds to the OpenSL ES Interface ID for this type of effect.
     */
    AutoPtr<IUUID> mType;
    /**
     *  Indicates the particular implementation of the effect in that type. Several effects
     *  can have the same type but this uuid is unique to a given implementation.
     */
    AutoPtr<IUUID> mUuid;
    /**
     *  Indicates if the effect is of insert category {@link #EFFECT_INSERT} or auxiliary
     *  category {@link #EFFECT_AUXILIARY}. Insert effects (Typically an Equalizer) are applied
     *  to the entire audio source and usually not shared by several sources. Auxiliary effects
     *  (typically a reverberator) are applied to part of the signal (wet) and the effect output
     *  is added to the original signal (dry).
     */
    String mConnectMode;
    /**
     * Human readable effect name
     */
    String mName;
    /**
     * Human readable effect implementor name
     */
    String mImplementor;
};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CAUDIOEFFECTDESCRIPTOR_H__
