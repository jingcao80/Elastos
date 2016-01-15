#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/audiofx/CPresetReverb.h"
#include "elastos/droid/media/audiofx/CPresetReverbSettings.h"
#include <elastos/core/AutoLock.h>

using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

const String CPresetReverb::TAG("PresetReverb");

CAR_INTERFACE_IMPL(CPresetReverb, AudioEffect, IPresetReverb)

CAR_OBJECT_IMPL(CPresetReverb)

CAR_INTERFACE_IMPL(CPresetReverb::BaseParameterListener, Object, IAudioEffectOnParameterChangeListener)

ECode CPresetReverb::BaseParameterListener::OnParameterChange(
    /* [in] */ IAudioEffect* effect,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* param,
    /* [in] */ ArrayOf<Byte>* value)
{
    AutoPtr<IPresetReverbOnParameterChangeListener> l;
    {
        Object& lock = mHost->mParamListenerLock;
        synchronized(lock);
        if (mHost->mParamListener != NULL) {
            l = mHost->mParamListener;
        }
    }
    if (l != NULL) {
        Int32 p = -1;
        Int16 v = -1;

        if (param->GetLength() == 4) {
            mHost->ByteArrayToInt32(param, 0, &p);
        }
        if (value->GetLength() == 2) {
            mHost->ByteArrayToInt16(value, 0, &v);
        }
        if (p != -1 && v != -1) {
            l->OnParameterChange((IPresetReverb*)mHost, status, p, v);
        }
    }
    return NOERROR;
}

CPresetReverb::CPresetReverb()
{
}

ECode CPresetReverb::constructor(
    /* [in] */ Int32 priority,
    /* [in] */ Int32 audioSession)
{
    // super(EFFECT_TYPE_PRESET_REVERB, EFFECT_TYPE_NULL, priority, audioSession);
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> typeREVERB;
    AutoPtr<IUUID> typeNULL;
    helper->FromString(IAudioEffect::EFFECT_TYPE_PRESET_REVERB, (IUUID**)&typeREVERB);
    helper->FromString(IAudioEffect::EFFECT_TYPE_NULL, (IUUID**)&typeNULL);
    return AudioEffect::constructor(typeREVERB, typeNULL, priority, audioSession);
}

ECode CPresetReverb::SetPreset(
    /* [in] */ Int16 preset)
{
    Int32 status;
    SetParameter(IPresetReverb::PARAM_PRESET, preset, &status);
    return CheckStatus(status);
}

ECode CPresetReverb::GetPreset(
    /* [out] */ Int16* preset)
{
    VALIDATE_NOT_NULL(preset);

    AutoPtr<ArrayOf<Int16> > value = ArrayOf<Int16>::Alloc(1);
    Int32 status;
    GetParameter(IPresetReverb::PARAM_PRESET, value, &status);
    CheckStatus(status);
    *preset = (*value)[0];
    return NOERROR;
}

ECode CPresetReverb::SetParameterListener(
        /* [in] */ IPresetReverbOnParameterChangeListener* listener)
{
    synchronized(mParamListenerLock);
    if (mParamListener != NULL) {
        mParamListener = listener;
        mBaseParamListener = new BaseParameterListener(this);
        AudioEffect::SetParameterListener(IAudioEffectOnParameterChangeListener::Probe(mBaseParamListener));
    }
    return NOERROR;
}

ECode CPresetReverb::GetProperties(
    /* [out] */ IPresetReverbSettings** properties)
{
    VALIDATE_NOT_NULL(properties);

    AutoPtr<IPresetReverbSettings> settings;
    CPresetReverbSettings::New((IPresetReverbSettings**)&settings);
    AutoPtr<ArrayOf<Int16> > value = ArrayOf<Int16>::Alloc(1);
    Int32 status;
    GetParameter(IPresetReverb::PARAM_PRESET, value, &status);
    CheckStatus(status);
    settings->SetPreset((*value)[0]);
    *properties = settings;
    REFCOUNT_ADD(*properties);
    return NOERROR;
}

ECode CPresetReverb::SetProperties(
    /* [in] */ IPresetReverbSettings* settings)
{
    Int16 preset;
    settings->GetPreset(&preset);
    Int32 status;
    SetParameter(IPresetReverb::PARAM_PRESET, preset, &status);
    return CheckStatus(status);
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
