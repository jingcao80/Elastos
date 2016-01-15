
#include "elastos/droid/preference/SeekBarPreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(SeekBarPreferenceSavedState, PreferenceBaseSavedState, ISeekBarPreferenceSavedState)

SeekBarPreferenceSavedState::SeekBarPreferenceSavedState()
    : mProgress(0)
    , mMax(0)
{}

ECode SeekBarPreferenceSavedState::constructor()
{
    return NOERROR;
}

ECode SeekBarPreferenceSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return PreferenceBaseSavedState::constructor(superState);
}

ECode SeekBarPreferenceSavedState::SetProgress(
    /* [in] */ Int32 progress)
{
    mProgress = progress;
    return NOERROR;
}

ECode SeekBarPreferenceSavedState::GetProgress(
    /* [out] */ Int32* progress)
{
    VALIDATE_NOT_NULL(progress)
    *progress = mProgress;
    return NOERROR;
}

ECode SeekBarPreferenceSavedState::SetMax(
    /* [in] */ Int32 max)
{
    mMax = max;
    return NOERROR;
}

ECode SeekBarPreferenceSavedState::GetMax(
    /* [out] */ Int32* max)
{
    VALIDATE_NOT_NULL(max)
    *max = mMax;
    return NOERROR;
}

ECode SeekBarPreferenceSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    dest->WriteInt32(mProgress);
    dest->WriteInt32(mMax);
    return NOERROR;
}

ECode SeekBarPreferenceSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    source->ReadInt32(&mProgress);
    source->ReadInt32(&mMax);
    return NOERROR;
}

ECode SeekBarPreferenceSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    return PreferenceBaseSavedState::GetSuperState(superState);
}


} // Preference
} // Droid
} // Elastos
