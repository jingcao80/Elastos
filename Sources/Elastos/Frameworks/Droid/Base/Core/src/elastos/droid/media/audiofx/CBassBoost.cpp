#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/audiofx/CBassBoost.h"
#include "elastos/droid/media/audiofx/CBassBoostSettings.h"
#include <elastos/core/AutoLock.h>

using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

const String CBassBoost::TAG("BassBoost");

CAR_INTERFACE_IMPL(CBassBoost, AudioEffect, IBassBoost)

CAR_OBJECT_IMPL(CBassBoost)

CAR_INTERFACE_IMPL(CBassBoost::BaseParameterListener, Object, IAudioEffectOnParameterChangeListener)

ECode CBassBoost::BaseParameterListener::OnParameterChange(
    /* [in] */ IAudioEffect* effect,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* param,
    /* [in] */ ArrayOf<Byte>* value)
{
    AutoPtr<IBassBoostOnParameterChangeListener> l;
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
            l->OnParameterChange((IBassBoost*)mHost, status, p, v);
        }
    }
    return NOERROR;
}

CBassBoost::CBassBoost()
    : mStrengthSupported(FALSE)
{
}

ECode CBassBoost::constructor(
    /* [in] */ Int32 priority,
    /* [in] */ Int32 audioSession)
{
    // super(EFFECT_TYPE_BASS_BOOST, EFFECT_TYPE_NULL, priority, audioSession);
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> typeBOOST;
    AutoPtr<IUUID> typeNULL;
    helper->FromString(IAudioEffect::EFFECT_TYPE_BASS_BOOST, (IUUID**)&typeBOOST);
    helper->FromString(IAudioEffect::EFFECT_TYPE_NULL, (IUUID**)&typeNULL);
    AudioEffect::constructor(typeBOOST, typeNULL, priority, audioSession);

    if (audioSession == 0) {
        //Log.w(TAG, "WARNING: attaching a BassBoost to global output mix is deprecated!");
    }

    AutoPtr<ArrayOf<Int32> > value = ArrayOf<Int32>::Alloc(1);
    Int32 status;
    GetParameter(IBassBoost::PARAM_STRENGTH_SUPPORTED, value, &status);
    CheckStatus(status);
    mStrengthSupported = (value[0] != 0);
    return NOERROR;
}

ECode CBassBoost::GetStrengthSupported(
    /* [out] */ Boolean* isSupported)
{
    VALIDATE_NOT_NULL(isSupported);

    *isSupported = mStrengthSupported;
    return NOERROR;
}

ECode CBassBoost::SetStrength(
    /* [in] */ Int16 strength)
{
    Int32 status;
    SetParameter(IBassBoost::PARAM_STRENGTH, strength, &status);
    return CheckStatus(status);
}

ECode CBassBoost::GetRoundedStrength(
    /* [out] */ Int16* strength)
{
    VALIDATE_NOT_NULL(strength);

    AutoPtr<ArrayOf<Int16> > value = ArrayOf<Int16>::Alloc(1);
    Int32 status;
    GetParameter(IBassBoost::PARAM_STRENGTH, value, &status);
    CheckStatus(status);
    *strength = (*value)[0];
    return NOERROR;
}

ECode CBassBoost::SetParameterListener(
    /* [in] */ IBassBoostOnParameterChangeListener* listener)
{
    synchronized(mParamListenerLock);
    if (mParamListener != NULL) {
        mParamListener = listener;
        mBaseParamListener = new BaseParameterListener(this);
        AudioEffect::SetParameterListener(IAudioEffectOnParameterChangeListener::Probe(mBaseParamListener));
    }
    return NOERROR;
}

ECode CBassBoost::GetProperties(
    /* [out] */ IBassBoostSettings** properties)
{
    VALIDATE_NOT_NULL(properties);

    AutoPtr<IBassBoostSettings> settings;
    CBassBoostSettings::New((IBassBoostSettings**)&settings);
    AutoPtr<ArrayOf<Int16> > value = ArrayOf<Int16>::Alloc(1);
    Int32 status;
    GetParameter(IBassBoost::PARAM_STRENGTH, value, &status);
    CheckStatus(status);
    settings->SetStrength((*value)[0]);
    *properties = settings;
    REFCOUNT_ADD(*properties);
    return NOERROR;
}

ECode CBassBoost::SetProperties(
    /* in */ IBassBoostSettings* settings)
{
    Int16 strength;
    settings->GetStrength(&strength);
    Int32 status;
    SetParameter(IBassBoost::PARAM_STRENGTH, strength, &status);
    return CheckStatus(status);
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
