
#include "elastos/droid/preference/VolumePreferenceSavedState.h"
#include "elastos/droid/preference/CVolumePreferenceVolumnStore.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL(VolumePreferenceSavedState, PreferenceBaseSavedState, IVolumePreferenceSavedState)

VolumePreferenceSavedState::VolumePreferenceSavedState()
{
    CVolumePreferenceVolumnStore::New((IVolumePreferenceVolumeStore**)&mVolumeStore);
}

ECode VolumePreferenceSavedState::constructor()
{
    return PreferenceBaseSavedState::constructor();
}

ECode VolumePreferenceSavedState::constructor(
    /* [in] */ IParcelable* source)
{
    return PreferenceBaseSavedState::constructor(source);
}

ECode VolumePreferenceSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    PreferenceBaseSavedState::ReadFromParcel(source);
    Int32 volumn, originalVolume;
    source->ReadInt32(&volumn);
    source->ReadInt32(&originalVolume);
    mVolumeStore->SetVolume(volumn);
    mVolumeStore->SetOriginalVolume(originalVolume);
    return NOERROR;
}

ECode VolumePreferenceSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    PreferenceBaseSavedState::WriteToParcel(dest);
    Int32 volumn, originalVolume;
    mVolumeStore->GetVolume(&volumn);
    mVolumeStore->GetOriginalVolume(&originalVolume);
    dest->WriteInt32(volumn);
    dest->WriteInt32(originalVolume);
    return NOERROR;
}

ECode VolumePreferenceSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    return PreferenceBaseSavedState::GetSuperState(superState);
}

ECode VolumePreferenceSavedState::GetVolumeStore(
     /* [out] */ IVolumePreferenceVolumeStore** vs)
{
    VALIDATE_NOT_NULL(vs)
    *vs = mVolumeStore;
    REFCOUNT_ADD(*vs)
    return NOERROR;
}

ECode VolumePreferenceSavedState::SetVolumeStore(
     /* [in] */ IVolumePreferenceVolumeStore* vs)
{
    mVolumeStore = vs;
    return NOERROR;
}

} // Preference
} // Droid
} // Elastos
