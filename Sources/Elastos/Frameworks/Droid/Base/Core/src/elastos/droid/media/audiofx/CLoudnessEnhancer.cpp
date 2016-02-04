#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/media/audiofx/CLoudnessEnhancer.h"
#include "elastos/droid/media/audiofx/CLoudnessEnhancerSettings.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::Math;
using Elastos::IO::ByteOrder;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

const String CLoudnessEnhancer::TAG("LoudnessEnhancer");

CAR_INTERFACE_IMPL(CLoudnessEnhancer, AudioEffect, ILoudnessEnhancer)

CAR_OBJECT_IMPL(CLoudnessEnhancer)

CAR_INTERFACE_IMPL(CLoudnessEnhancer::BaseParameterListener, Object, IAudioEffectOnParameterChangeListener)

ECode CLoudnessEnhancer::BaseParameterListener::OnParameterChange(
    /* [in] */ IAudioEffect* effect,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* param,
    /* [in] */ ArrayOf<Byte>* value)
{
    // only notify when the parameter was successfully change
    if (status != IAudioEffect::SUCCESS) {
        return NOERROR;
    }
    AutoPtr<ILoudnessEnhancerOnParameterChangeListener> l;
    {
        Object& lock = mHost->mParamListenerLock;
        synchronized(lock);
        if (mHost->mParamListener != NULL) {
            l = mHost->mParamListener;
        }
    }
    if (l != NULL) {
        Int32 p = -1;
        Int32 v = Elastos::Core::Math::INT32_MIN_VALUE;

        if (param->GetLength() == 4) {
            ByteArrayToInt32(param, 0, &p);
        }
        if (value->GetLength() == 4) {
            ByteArrayToInt32(value, 0, &v);
        }
        if (p != -1 && v != Elastos::Core::Math::INT32_MIN_VALUE) {
            l->OnParameterChange(mHost, p, v);
        }
    }
    return NOERROR;
}

CLoudnessEnhancer::CLoudnessEnhancer()
{
}

ECode CLoudnessEnhancer::constructor(
    /* [in] */ Int32 audioSession)
{
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> type, uuid;
    helper->FromString(IAudioEffect::EFFECT_TYPE_LOUDNESS_ENHANCER, (IUUID**)&type);
    helper->FromString(IAudioEffect::EFFECT_TYPE_NULL, (IUUID**)&uuid);
    AudioEffect::constructor(type, uuid, 0, audioSession);

    if (audioSession == 0) {
        Logger::W(TAG, "WARNING: attaching a LoudnessEnhancer to global output mix is deprecated!");
    }
    return NOERROR;
}

ECode CLoudnessEnhancer::constructor(
    /* [in] */ Int32 priority,
    /* [in] */ Int32 audioSession)
{
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> type, uuid;
    helper->FromString(IAudioEffect::EFFECT_TYPE_LOUDNESS_ENHANCER, (IUUID**)&type);
    helper->FromString(IAudioEffect::EFFECT_TYPE_NULL, (IUUID**)&uuid);
    AudioEffect::constructor(type, uuid, priority, audioSession);

    if (audioSession == 0) {
        Logger::W(TAG, "WARNING: attaching a LoudnessEnhancer to global output mix is deprecated!");
    }
    return NOERROR;
}

ECode CLoudnessEnhancer::SetTargetGain(
    /* [in] */ Int32 gainmB)
{
    Int32 status;
    SetParameter(ILoudnessEnhancer::PARAM_TARGET_GAIN_MB, gainmB, &status);
    return CheckStatus(status);
}

ECode CLoudnessEnhancer::GetTargetGain(
    /* [out] */ Float * result)
{
    AutoPtr<ArrayOf<Int32> > value = ArrayOf<Int32>::Alloc(1);
    Int32 status;
    GetParameter(ILoudnessEnhancer::PARAM_TARGET_GAIN_MB, value, &status);
    CheckStatus(status);
    *result = (*value)[0];
    return NOERROR;
}

ECode CLoudnessEnhancer::SetParameterListener(
    /* [in] */ ILoudnessEnhancerOnParameterChangeListener * listener)
{
    synchronized(mParamListenerLock) {
        if (mParamListener == NULL) {
            mBaseParamListener = new BaseParameterListener(this);
            AudioEffect::SetParameterListener(mBaseParamListener);
        }
        mParamListener = listener;
    }
    return NOERROR;
}

ECode CLoudnessEnhancer::GetProperties(
    /* [out] */ ILoudnessEnhancerSettings ** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Int32> > value = ArrayOf<Int32>::Alloc(1);
    Int32 status;
    GetParameter(ILoudnessEnhancer::PARAM_TARGET_GAIN_MB, value, &status);
    CheckStatus(status);
    ((CLoudnessEnhancerSettings*)result)->mTargetGainmB = (*value)[0];
    return NOERROR;
}

ECode CLoudnessEnhancer::SetProperties(
    /* [in] */ ILoudnessEnhancerSettings * settings)
{
    Int32 result;

    SetParameter(ILoudnessEnhancer::PARAM_TARGET_GAIN_MB, ((CLoudnessEnhancerSettings*)settings)->mTargetGainmB, &result);
    return CheckStatus(result);
}



} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
