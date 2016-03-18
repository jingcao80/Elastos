#include "elastos/droid/ims/CImsConferenceState.h"

using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Ims {

CAR_INTERFACE_IMPL_2(CImsConferenceState, Object, IImsConferenceState, IParcelable)

CAR_OBJECT_IMPL(CImsConferenceState)

CImsConferenceState::CImsConferenceState()
{
}

CImsConferenceState::~CImsConferenceState()
{
}

ECode CImsConferenceState::constructor()
{
    CHashMap::New((IHashMap**)&mParticipants);
    return NOERROR;
}

ECode CImsConferenceState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mParticipants = IHashMap::Probe(obj);
    return NOERROR;
}

ECode CImsConferenceState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mParticipants);
    return NOERROR;
}

} // namespace Ims
} // namespace Droid
} // namespace Elastos