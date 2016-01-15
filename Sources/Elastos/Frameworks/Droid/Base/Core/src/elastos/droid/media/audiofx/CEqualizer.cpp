#include "elastos/droid/media/audiofx/CEqualizer.h"
#include "elastos/droid/media/audiofx/CEqualizerSettings.h"
#include <elastos/core/AutoLock.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

const String CEqualizer::TAG("BassBoost");
const Int32 CEqualizer::PARAM_PROPERTIES = 9;

CAR_INTERFACE_IMPL(CEqualizer, AudioEffect, IEqualizer)

CAR_OBJECT_IMPL(CEqualizer)

CAR_INTERFACE_IMPL(CEqualizer::BaseParameterListener, Object, IAudioEffectOnParameterChangeListener)

ECode CEqualizer::BaseParameterListener::OnParameterChange(
    /* [in] */ IAudioEffect* effect,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* param,
    /* [in] */ ArrayOf<Byte>* value)
{
    AutoPtr<IEqualizerOnParameterChangeListener> l;
    {
        Object& lock = mHost->mParamListenerLock;
        synchronized(lock);
        if (mHost->mParamListener != NULL) {
            l = mHost->mParamListener;
        }
    }
    if (l != NULL) {
        Int32 p1 = -1;
        Int32 p2 = -1;
        Int32 v = -1;

        if (param->GetLength() >= 4) {
            mHost->ByteArrayToInt32(param, 0, &p1);
            if (param->GetLength() >= 8) {
                mHost->ByteArrayToInt32(param, 4, &p2);
            }
        }
        if (value->GetLength() == 2) {
            Int16 status;
            mHost->ByteArrayToInt16(value, 0, &status);
            v = (Int32) status;
        } else if (value->GetLength() == 4) {
            mHost->ByteArrayToInt32(value, 0, &v);
        }
        if (p1 != -1 && v != -1) {
            l->OnParameterChange((IEqualizer*)mHost, status, p1, p2, v);
        }
    }
    return NOERROR;
}

CEqualizer::CEqualizer()
    : mNumBands(0)
    , mNumPresets(0)
{
}

ECode CEqualizer::constructor(
    /* [in] */ Int32 priority,
    /* [in] */ Int32 audioSession)
{
    // super(EFFECT_TYPE_EQUALIZER, EFFECT_TYPE_NULL, priority, audioSession);
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> typeEQUALIZER;
    AutoPtr<IUUID> typeNULL;
    helper->FromString(IAudioEffect::EFFECT_TYPE_EQUALIZER, (IUUID**)&typeEQUALIZER);
    helper->FromString(IAudioEffect::EFFECT_TYPE_NULL, (IUUID**)&typeNULL);
    AudioEffect::constructor(typeEQUALIZER, typeNULL, priority, audioSession);

    if (audioSession == 0) {
        //Log.w(TAG, "WARNING: attaching an Equalizer to global output mix is deprecated!");
    }

    Int16 numOfBands;
    GetNumberOfBands(&numOfBands);
    Int16 tempInt16;
    GetNumberOfPresets(&tempInt16);
    mNumPresets = (Int32) tempInt16;
    if (mNumPresets != 0) {
        mPresetNames = ArrayOf<String>::Alloc(mNumPresets);
        AutoPtr<ArrayOf<Byte> > value = ArrayOf<Byte>::Alloc(PARAM_STRING_SIZE_MAX);
        AutoPtr<ArrayOf<Int32> > param = ArrayOf<Int32>::Alloc(2);
        param->Set(0, IEqualizer::PARAM_GET_PRESET_NAME);
        for (int i = 0; i < mNumPresets; i++) {
            param->Set(1, i);
            Int32 status;
            GetParameter(param, value, &status);
            FAIL_RETURN(CheckStatus(status));
            Int32 length = 0 ;
            while ((*value)[length] != 0 ) length++;

            // try {
            String s((const char*)value->GetPayload(), length);
            mPresetNames->Set(i, s);
            // } catch (java.io.UnsupportedEncodingException e) {
            //     Log.e(TAG, "preset name decode error");
            // }

        }
    }
    return NOERROR;
}

ECode CEqualizer::GetNumberOfBands(
    /* [out] */ Int16* numBands)
{
    VALIDATE_NOT_NULL(numBands);

    if (mNumBands != 0) {
        *numBands = mNumBands;
        return NOERROR;
    }
    AutoPtr<ArrayOf<Int32> > param = ArrayOf<Int32>::Alloc(1);
    (*param)[0] = IEqualizer::PARAM_NUM_BANDS;
    AutoPtr<ArrayOf<Int16> > result = ArrayOf<Int16>::Alloc(1);
    Int32 status;
    GetParameter(param, result, &status);
    FAIL_RETURN(CheckStatus(status));
    mNumBands = (*result)[0];
    *numBands = mNumBands;
    return NOERROR;
}

ECode CEqualizer::GetBandLevelRange(
    /* [out, callee] */ ArrayOf<Int16>** bandLevelRange)
{
    VALIDATE_NOT_NULL(bandLevelRange);

    AutoPtr<ArrayOf<Int16> > result = ArrayOf<Int16>::Alloc(2);

    Int32 status;
    GetParameter(IEqualizer::PARAM_LEVEL_RANGE, result, &status);
    FAIL_RETURN(CheckStatus(status));
    *bandLevelRange = result;
    REFCOUNT_ADD(*bandLevelRange);

    return NOERROR;
}

ECode CEqualizer::SetBandLevel(
    /* [in] */ Int16 band,
    /* [in] */ Int16 level)
{
    AutoPtr<ArrayOf<Int32> > param = ArrayOf<Int32>::Alloc(2);
    AutoPtr<ArrayOf<Int16> > value = ArrayOf<Int16>::Alloc(2);

    (*param)[0] = IEqualizer::PARAM_BAND_LEVEL;
    (*param)[1] = (Int32) band;
    (*value)[0] = level;
    Int32 status;
    GetParameter(param, value, &status);
    FAIL_RETURN(CheckStatus(status));
    return NOERROR;
}

ECode CEqualizer::GetBandLevel(
    /* [in] */ Int16 band,
    /* [out] */ Int16* bandLevel)
{
    VALIDATE_NOT_NULL(bandLevel);

    AutoPtr<ArrayOf<Int32> > param = ArrayOf<Int32>::Alloc(2);
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(1);

    (*param)[0] = IEqualizer::PARAM_BAND_LEVEL;
    (*param)[1] = (Int32) band;
    Int32 status;
    GetParameter(param, result, &status);
    FAIL_RETURN(CheckStatus(status));
    *bandLevel = (*result)[0];
    return NOERROR;
}

ECode CEqualizer::GetCenterFreq(
    /* [in] */ Int16 band,
    /* [out] */ Int32* centerFreq)
{
    VALIDATE_NOT_NULL(centerFreq);

    AutoPtr<ArrayOf<Int32> > param = ArrayOf<Int32>::Alloc(2);
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(1);

    (*param)[0] = IEqualizer::PARAM_CENTER_FREQ;
    (*param)[1] = (Int32) band;
    Int32 status;
    GetParameter(param, result, &status);
    FAIL_RETURN(CheckStatus(status));
    *centerFreq = (*result)[0];
    return NOERROR;
}

ECode CEqualizer::GetBandFreqRange(
    /* [in] */ Int16 band,
    /* [out, callee] */ ArrayOf<Int32>** bandFreqRange)
{
    VALIDATE_NOT_NULL(bandFreqRange);

    AutoPtr<ArrayOf<Int32> > param = ArrayOf<Int32>::Alloc(2);
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(2);

    (*param)[0] = IEqualizer::PARAM_BAND_FREQ_RANGE;
    (*param)[1] = (Int32) band;
    Int32 status;
    GetParameter(param, result, &status);
    FAIL_RETURN(CheckStatus(status));
    *bandFreqRange = result;
    REFCOUNT_ADD(*bandFreqRange);
    return NOERROR;
}

ECode CEqualizer::GetBand(
    /* [in] */ Int32 frequency,
    /* [out] */ Int16* band)
{
    VALIDATE_NOT_NULL(band);

    AutoPtr<ArrayOf<Int32> > param = ArrayOf<Int32>::Alloc(2);
    AutoPtr<ArrayOf<Int16> > result = ArrayOf<Int16>::Alloc(1);

    (*param)[0] = IEqualizer::PARAM_GET_BAND;
    (*param)[1] = frequency;
    Int32 status;
    GetParameter(param, result, &status);
    FAIL_RETURN(CheckStatus(status));
    *band = (*result)[0];
    return NOERROR;
}

ECode CEqualizer::GetCurrentPreset(
        /* [out] */ Int16* preset)
{
    VALIDATE_NOT_NULL(preset);

    AutoPtr<ArrayOf<Int16> > result = ArrayOf<Int16>::Alloc(1);
    Int32 status;
    GetParameter(IEqualizer::PARAM_CURRENT_PRESET, result, &status);
    FAIL_RETURN(CheckStatus(status));
    *preset = (*result)[0];
    return NOERROR;
}

ECode CEqualizer::UsePreset(
    /* [in] */ Int16 preset)
{
    Int32 status;
    SetParameter(IEqualizer::PARAM_CURRENT_PRESET, preset, &status);
    return CheckStatus(status);
}

ECode CEqualizer::GetNumberOfPresets(
    /* [out] */ Int16* numPresets)
{
    VALIDATE_NOT_NULL(numPresets);

    AutoPtr<ArrayOf<Int16> > result = ArrayOf<Int16>::Alloc(1);
    Int32 status;
    GetParameter(IEqualizer::PARAM_GET_NUM_OF_PRESETS, result, &status);
    FAIL_RETURN(CheckStatus(status));
    *numPresets = (*result)[0];
    return NOERROR;
}

ECode CEqualizer::GetPresetName(
    /* [in] */ Int16 preset,
    /* [out] */ String* presetName)
{
    VALIDATE_NOT_NULL(presetName);

    if (preset >= 0 && preset < mNumPresets) {
        *presetName = (*mPresetNames)[preset];
    } else {
        *presetName = "";
    }
    return NOERROR;
}

ECode CEqualizer::SetParameterListener(
    /* [in] */ IEqualizerOnParameterChangeListener* listener)
{
    synchronized(mParamListenerLock);
    if (mParamListener != NULL) {
        mParamListener = listener;
        mBaseParamListener = new BaseParameterListener(this);
        AudioEffect::SetParameterListener(IAudioEffectOnParameterChangeListener::Probe(mBaseParamListener));
    }
    return NOERROR;
}

ECode CEqualizer::GetProperties(
    /* [out] */ IEqualizerSettings** properties)
{
    VALIDATE_NOT_NULL(properties);

    AutoPtr<ArrayOf<Byte> > param = ArrayOf<Byte>::Alloc(4 + mNumBands * 2);
    Int32 status;
    GetParameter(PARAM_PROPERTIES,param,&status);
    FAIL_RETURN(CheckStatus(status));
    AutoPtr<IEqualizerSettings> settings;
    CEqualizerSettings::New((IEqualizerSettings**)&settings);
    Int16 statusInt16;
    ByteArrayToInt16(param, 0, &statusInt16);
    settings->SetCurPreset(statusInt16);
    ByteArrayToInt16(param, 2, &statusInt16);
    settings->SetNumBands(statusInt16);
    AutoPtr<ArrayOf<Int16> > tempInt16Array1 = ArrayOf<Int16>::Alloc(mNumBands);
    for (int i = 0; i < mNumBands; i++) {
        ByteArrayToInt16(param, 4 + 2*i, &statusInt16);
        (*tempInt16Array1)[i] = statusInt16;
    }
    settings->SetBandLevels(tempInt16Array1);
    *properties = settings;
    REFCOUNT_ADD(*properties);
    return NOERROR;
}

ECode CEqualizer::SetProperties(
    /* [in] */ IEqualizerSettings* settings)
{
    Int16 tempInt16Parameter1;
    AutoPtr< ArrayOf<Byte> > tempByteArray1;
    AutoPtr< ArrayOf<Byte> > tempByteArray2;
    AutoPtr< ArrayOf<Byte> > param;
    AutoPtr< ArrayOf<Int16> > tempInt16Array1;

    settings->GetNumBands(&tempInt16Parameter1);
    settings->GetBandLevels((ArrayOf<Int16>**)&tempInt16Array1);
    if (tempInt16Parameter1 != tempInt16Array1->GetLength() ||
        tempInt16Parameter1 != mNumBands) {
           // throw new IllegalArgumentException("settings invalid band count: " +settings.numBands);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    settings->GetCurPreset(&tempInt16Parameter1);
    Int16ToByteArray(tempInt16Parameter1, (ArrayOf<Byte>**)&tempByteArray1);
    Int16ToByteArray(mNumBands, (ArrayOf<Byte>**)&tempByteArray2);
    ConcatArrays(tempByteArray1, tempByteArray2, (ArrayOf<Byte>**)&param);

    for (int i = 0; i < mNumBands; i++) {
        Int16ToByteArray((*tempInt16Array1)[i], (ArrayOf<Byte>**)&tempByteArray1);
        ConcatArrays(param, tempByteArray1, (ArrayOf<Byte>**)&param);
    }
    Int32 status;
    SetParameter(PARAM_PROPERTIES, param, &status);
    return CheckStatus(status);
}


} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
