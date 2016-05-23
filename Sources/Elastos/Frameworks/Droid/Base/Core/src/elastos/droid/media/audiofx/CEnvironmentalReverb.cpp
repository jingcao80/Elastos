#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/audiofx/CEnvironmentalReverb.h"
#include "elastos/droid/media/audiofx/CEnvironmentalReverbSettings.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

const String CEnvironmentalReverb::TAG("EnvironmentalReverb");

const Int32 CEnvironmentalReverb::PARAM_PROPERTIES = 10;

const Int32 CEnvironmentalReverb::PROPERTY_SIZE = 26;

CAR_INTERFACE_IMPL(CEnvironmentalReverb, AudioEffect, IEnvironmentalReverb)

CAR_OBJECT_IMPL(CEnvironmentalReverb)

CAR_INTERFACE_IMPL(CEnvironmentalReverb::BaseParameterListener, Object, IAudioEffectOnParameterChangeListener)

ECode CEnvironmentalReverb::BaseParameterListener::OnParameterChange(
    /* [in] */ IAudioEffect* effect,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* param,
    /* [in] */ ArrayOf<Byte>* value)
{
    AutoPtr<IEnvironmentalReverbOnParameterChangeListener> l;
    {
        Object& lock = mHost->mParamListenerLock;
        AutoLock syncLock(lock);
        if (mHost->mParamListener != NULL) {
            l = mHost->mParamListener;
        }
    }
    if (l != NULL) {
        Int32 p = -1;
        Int32 v = -1;

        if (param->GetLength() == 4) {
            mHost->ByteArrayToInt32(param, 0, &p);
        }
        if (value->GetLength() == 2) {
            mHost->ByteArrayToInt32(value, 0, &v);
        }
        if (p != -1 && v != -1) {
            l->OnParameterChange((IEnvironmentalReverb*)mHost, status, p, v);
        }
    }
    return NOERROR;
}

CEnvironmentalReverb::CEnvironmentalReverb()
{
}

ECode CEnvironmentalReverb::constructor(
    /* [in] */ Int32 priority,
    /* [in] */ Int32 audioSession)
{
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> typeREVERB;
    AutoPtr<IUUID> typeNULL;
    helper->FromString(IAudioEffect::EFFECT_TYPE_AEC, (IUUID**)&typeREVERB);
    helper->FromString(IAudioEffect::EFFECT_TYPE_NULL, (IUUID**)&typeNULL);
    return AudioEffect::constructor(typeREVERB, typeNULL, priority, audioSession);
}

ECode CEnvironmentalReverb::SetRoomLevel(
    /* [in] */ Int16 room)
{
    AutoPtr< ArrayOf<Byte> > param;
    Int16ToByteArray(room, (ArrayOf<Byte>**)&param);
    Int32 status = 0;
    SetParameter(IEnvironmentalReverb::PARAM_ROOM_LEVEL, param.Get(), &status);
    return CheckStatus(status);
}

ECode CEnvironmentalReverb::GetRoomLevel(
    /* [out] */ Int16* room)
{
    VALIDATE_NOT_NULL(room);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(2);
    Int32 status = 0;
    GetParameter(IEnvironmentalReverb::PARAM_ROOM_LEVEL, param, &status);
    FAIL_RETURN(CheckStatus(status));
    ByteArrayToInt16(param, room);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetRoomHFLevel(
    /* [in] */ Int16 roomHF)
{
    AutoPtr< ArrayOf<Byte> > param;
    Int32 status = 0;
    Int16ToByteArray(roomHF, (ArrayOf<Byte>**)&param);
    SetParameter(IEnvironmentalReverb::PARAM_ROOM_HF_LEVEL, param.Get(), &status);
    return CheckStatus(status);
}

ECode CEnvironmentalReverb::GetRoomHFLevel(
    /* [out] */ Int16* roomHF)
{
    VALIDATE_NOT_NULL(roomHF);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(2);
    Int32 status = 0;
    GetParameter(IEnvironmentalReverb::PARAM_ROOM_HF_LEVEL, param, &status);
    FAIL_RETURN(CheckStatus(status));
    ByteArrayToInt16(param, roomHF);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetDecayTime(
    /* [in] */ Int32 decayTime)
{
    AutoPtr< ArrayOf<Byte> > param;
    Int32 status = 0;
    Int32ToByteArray(decayTime, (ArrayOf<Byte>**)&param);
    SetParameter(IEnvironmentalReverb::PARAM_DECAY_TIME, param.Get(), &status);
    return CheckStatus(status);
}

ECode CEnvironmentalReverb::GetDecayTime(
    /* [out] */ Int32* decayTime)
{
    VALIDATE_NOT_NULL(decayTime);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(4);
    Int32 status = 0;
    GetParameter(IEnvironmentalReverb::PARAM_DECAY_TIME, param, &status);
    FAIL_RETURN(CheckStatus(status));
    ByteArrayToInt32(param, decayTime);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetDecayHFRatio(
    /* [in] */ Int16 decayHFRatio)
{
    AutoPtr< ArrayOf<Byte> > param;
    Int16ToByteArray(decayHFRatio, (ArrayOf<Byte>**)&param);
    Int32 status = 0;
    SetParameter(IEnvironmentalReverb::PARAM_DECAY_HF_RATIO, param.Get(), &status);
    return CheckStatus(status);
}

ECode CEnvironmentalReverb::GetDecayHFRatio(
    /* [out] */ Int16* decayHFRatio)
{
    VALIDATE_NOT_NULL(decayHFRatio);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(2);
    Int32 status = 0;
    GetParameter(IEnvironmentalReverb::PARAM_DECAY_HF_RATIO, param, &status);
    FAIL_RETURN(CheckStatus(status));
    ByteArrayToInt16(param, decayHFRatio);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetReflectionsLevel(
    /* [in] */ Int16 reflectionsLevel)
{
    AutoPtr< ArrayOf<Byte> > param;
    Int16ToByteArray(reflectionsLevel, (ArrayOf<Byte>**)&param);
    Int32 status = 0;
    SetParameter(IEnvironmentalReverb::PARAM_REFLECTIONS_LEVEL, param.Get(), &status);
    return CheckStatus(status);
}

ECode CEnvironmentalReverb::GetReflectionsLevel(
    /* [out] */ Int16* reflectionsLevel)
{
    VALIDATE_NOT_NULL(reflectionsLevel);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(2);
    Int32 status = 0;
    GetParameter(IEnvironmentalReverb::PARAM_REFLECTIONS_LEVEL, param, &status);
    FAIL_RETURN(CheckStatus(status));
    ByteArrayToInt16(param, reflectionsLevel);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetReflectionsDelay(
    /* [in] */ Int32 reflectionsDelay)
{
    AutoPtr< ArrayOf<Byte> > param;
    Int32ToByteArray(reflectionsDelay, (ArrayOf<Byte>**)&param);
    Int32 status;
    SetParameter(IEnvironmentalReverb::PARAM_REFLECTIONS_DELAY, param.Get(), &status);
    return CheckStatus(status);
}

ECode CEnvironmentalReverb::GetReflectionsDelay(
    /* [out] */ Int32* reflectionsDelay)
{
    VALIDATE_NOT_NULL(reflectionsDelay);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(4);
    Int32 status;
    GetParameter(IEnvironmentalReverb::PARAM_REFLECTIONS_DELAY, param, &status);
    FAIL_RETURN(CheckStatus(status));
    ByteArrayToInt32(param, reflectionsDelay);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetReverbLevel(
    /* [in] */ Int16 reverbLevel)
{
    AutoPtr< ArrayOf<Byte> > param;
    Int16ToByteArray(reverbLevel, (ArrayOf<Byte>**)&param);
    Int32 status;
    SetParameter(IEnvironmentalReverb::PARAM_REVERB_LEVEL, param.Get(), &status);
    return CheckStatus(status);
}

ECode CEnvironmentalReverb::GetReverbLevel(
    /* [out] */ Int16* reverbLevel)
{
    VALIDATE_NOT_NULL(reverbLevel);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(2);
    Int32 status;
    GetParameter(IEnvironmentalReverb::PARAM_REVERB_LEVEL, param, &status);
    FAIL_RETURN(CheckStatus(status));
    ByteArrayToInt16(param, reverbLevel);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetReverbDelay(
    /* [in] */ Int32 reverbDelay)
{
    AutoPtr< ArrayOf<Byte> > param;
    Int32ToByteArray(reverbDelay, (ArrayOf<Byte>**)&param);
    Int32 status;
    SetParameter(IEnvironmentalReverb::PARAM_REVERB_DELAY, param.Get(), &status);
    return CheckStatus(status);
}

ECode CEnvironmentalReverb::GetReverbDelay(
    /* [out] */ Int32* reverbDelay)
{
    VALIDATE_NOT_NULL(reverbDelay);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(4);
    Int32 status;
    GetParameter(IEnvironmentalReverb::PARAM_REVERB_DELAY, param, &status);
    FAIL_RETURN(CheckStatus(status));
    ByteArrayToInt32(param, reverbDelay);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetDiffusion(
    /* [in] */ Int16 diffusion)
{
    AutoPtr< ArrayOf<Byte> > param;
    Int16ToByteArray(diffusion, (ArrayOf<Byte>**)&param);
    Int32 status = 0;
    SetParameter(IEnvironmentalReverb::PARAM_DIFFUSION, param.Get(), &status);
    return CheckStatus(status);
}

ECode CEnvironmentalReverb::GetDiffusion(
    /* [out] */ Int16* diffusion)
{
    VALIDATE_NOT_NULL(diffusion);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(2);
    Int32 status = 0;
    GetParameter(IEnvironmentalReverb::PARAM_DIFFUSION, param, &status);
    FAIL_RETURN(CheckStatus(status));
    ByteArrayToInt16(param, diffusion);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetDensity(
    /* [in] */ Int16 density)
{
    AutoPtr< ArrayOf<Byte> > param;
    Int16ToByteArray(density, (ArrayOf<Byte>**)&param);
    Int32 status;
    SetParameter(IEnvironmentalReverb::PARAM_DENSITY, param.Get(), &status);
    return CheckStatus(status);
}

ECode CEnvironmentalReverb::GetDensity(
    /* [out] */ Int16* density)
{
    VALIDATE_NOT_NULL(density);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(2);
    Int32 status = 0;
    GetParameter(IEnvironmentalReverb::PARAM_DENSITY, param, &status);
    FAIL_RETURN(CheckStatus(status));
    ByteArrayToInt16(param, density);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetParameterListener(
    /* [in] */ IEnvironmentalReverbOnParameterChangeListener* listener)
{
    AutoLock syncLock(mParamListenerLock);
    if (mParamListener != NULL) {
        mParamListener = listener;
        mBaseParamListener = new BaseParameterListener(this);
        SetParameterListener(IEnvironmentalReverbOnParameterChangeListener::Probe(mBaseParamListener));
    }
    return NOERROR;
}

ECode CEnvironmentalReverb::GetProperties(
        /* [out] */ IEnvironmentalReverbSettings** properties)
{
    VALIDATE_NOT_NULL(properties);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(PROPERTY_SIZE);
    Int32 status;
    GetParameter(PARAM_PROPERTIES, param, &status);
    FAIL_RETURN(CheckStatus(status));

    AutoPtr<IEnvironmentalReverbSettings> settings;
    Int16 statusInt16;
    ByteArrayToInt16(param, 0, &statusInt16);
    settings->SetRoomLevel(statusInt16);
    ByteArrayToInt16(param, 2, &statusInt16);
    settings->SetRoomHFLevel(statusInt16);
    ByteArrayToInt32(param, 4, &status);
    settings->SetDecayTime(status);
    ByteArrayToInt16(param, 8, &statusInt16);
    settings->SetDecayHFRatio(statusInt16);
    ByteArrayToInt16(param, 10, &statusInt16);
    settings->SetReflectionsLevel(statusInt16);
    ByteArrayToInt32(param, 12, &status);
    settings->SetReflectionsDelay(status);
    ByteArrayToInt16(param, 16, &statusInt16);
    settings->SetReverbLevel(statusInt16);
    ByteArrayToInt32(param, 18, &status);
    settings->SetReverbDelay(status);
    ByteArrayToInt16(param, 22, &statusInt16);
    settings->SetDiffusion(statusInt16);
    ByteArrayToInt16(param, 24, &statusInt16);
    settings->SetDensity(statusInt16);

    *properties = settings;
    REFCOUNT_ADD(*properties);
    return NOERROR;
}

ECode CEnvironmentalReverb::SetProperties(
    /* [in] */ IEnvironmentalReverbSettings* settings)
{
    AutoPtr<ArrayOf<Byte> > tempResult1;
    AutoPtr<ArrayOf<Byte> > tempResult2;
    Int16 tempInt16Parameter1,tempInt16Parameter2;
    Int32 tempInt32Parameter1;
    AutoPtr<ArrayOf<Byte> > tempByteArray1;
    AutoPtr<ArrayOf<Byte> > tempByteArray2;

    settings->GetRoomLevel(&tempInt16Parameter1);
    Int16ToByteArray(tempInt16Parameter1, (ArrayOf<Byte>**)&tempByteArray1);

    settings->GetRoomHFLevel(&tempInt16Parameter2);
    Int16ToByteArray(tempInt16Parameter2, (ArrayOf<Byte>**)&tempByteArray2);
    ConcatArrays(tempByteArray1, tempByteArray2, (ArrayOf<Byte>**)&tempResult1);

    settings->GetDecayTime(&tempInt32Parameter1);
    Int32ToByteArray(tempInt32Parameter1, (ArrayOf<Byte>**)&tempByteArray1);
    ConcatArrays(tempResult1, tempByteArray1, (ArrayOf<Byte>**)&tempResult2);

    settings->GetDecayHFRatio(&tempInt16Parameter1);
    Int16ToByteArray(tempInt16Parameter1, (ArrayOf<Byte>**)&tempByteArray1);
    ConcatArrays(tempResult2, tempByteArray1, (ArrayOf<Byte>**)&tempResult1);

    settings->GetReflectionsLevel(&tempInt16Parameter1);
    Int16ToByteArray(tempInt16Parameter1, (ArrayOf<Byte>**)&tempByteArray1);
    ConcatArrays(tempResult1, tempByteArray1, (ArrayOf<Byte>**)&tempResult2);

    settings->GetReflectionsDelay(&tempInt32Parameter1);
    Int32ToByteArray(tempInt32Parameter1, (ArrayOf<Byte>**)&tempByteArray1);
    ConcatArrays(tempResult2, tempByteArray1, (ArrayOf<Byte>**)&tempResult1);

    settings->GetReverbLevel(&tempInt16Parameter1);
    Int16ToByteArray(tempInt16Parameter1, (ArrayOf<Byte>**)&tempByteArray1);
    ConcatArrays(tempResult2, tempByteArray1, (ArrayOf<Byte>**)&tempResult1);

    settings->GetReverbDelay(&tempInt32Parameter1);
    Int32ToByteArray(tempInt32Parameter1, (ArrayOf<Byte>**)&tempByteArray1);
    ConcatArrays(tempResult1, tempByteArray1, (ArrayOf<Byte>**)&tempResult2);

    settings->GetDiffusion(&tempInt16Parameter1);
    Int16ToByteArray(tempInt16Parameter1, (ArrayOf<Byte>**)&tempByteArray1);
    ConcatArrays(tempResult2, tempByteArray1, (ArrayOf<Byte>**)&tempResult1);

    settings->GetDensity(&tempInt16Parameter1);
    Int16ToByteArray(tempInt16Parameter1, (ArrayOf<Byte>**)&tempByteArray1);

    ConcatArrays(tempResult1, tempByteArray1, (ArrayOf<Byte>**)&tempResult2);

    Int32 status;
    SetParameter(PARAM_PROPERTIES, tempResult2, &status);
    return CheckStatus(status);
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
