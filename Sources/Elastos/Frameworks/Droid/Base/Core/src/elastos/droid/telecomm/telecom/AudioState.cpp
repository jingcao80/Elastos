
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/AudioState.h"
#include <elastos/core/StringBuffer.h>
//#include <elastos/utility/CLocaleHelper.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::StringUtils;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CAR_INTERFACE_IMPL_2(AudioState, Object, IAudioState, IParcelable)

AudioState::AudioState()
{}

ECode AudioState::constructor()
{
    return NOERROR;
}

ECode AudioState::constructor(
    /* [in] */ Boolean isMuted,
    /* [in] */ Int32 route,
    /* [in] */ Int32 supportedRouteMask)
{
    mIsMuted = isMuted;
    mRoute = route;
    mSupportedRouteMask = supportedRouteMask;

    return NOERROR;
}

ECode AudioState::constructor(
    /* [in] */ IAudioState* state)
{
    AudioState* impl = (AudioState*)state;
    mIsMuted = impl->mIsMuted;
    mRoute = impl->mRoute;
    mSupportedRouteMask = impl->mSupportedRouteMask;

    return NOERROR;
}

ECode AudioState::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (obj == NULL) {
        *res = FALSE;
        return NOERROR;
    }
    if (IAudioState::Probe(obj) == NULL) {
        *res = FALSE;
        return NOERROR;
    }
    IAudioState* state = IAudioState::Probe(obj);
    AudioState* impl = (AudioState*)state;
    *res = (mIsMuted == impl->mIsMuted && mRoute == impl->mRoute &&
            mSupportedRouteMask == impl->mSupportedRouteMask);
    return NOERROR;
}

ECode AudioState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<ILocaleHelper> hlp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
    AutoPtr<ILocale> us;
    hlp->GetUS((ILocale**)&us);
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(3);
    AutoPtr<IBoolean> pMut;
    CBoolean::New(mIsMuted, (IBoolean**)&pMut);
    (*arr)[0] = pMut;

    String strRoute = AudioRouteToString(mRoute);
    AutoPtr<ICharSequence> pRoute;
    CString::New(strRoute, (ICharSequence**)&pRoute);
    (*arr)[1] = pRoute;

    String strMask = AudioRouteToString(mSupportedRouteMask);
    AutoPtr<ICharSequence> pMask;
    CString::New(strMask, (ICharSequence**)&pMask);
    (*arr)[2] = pMask;
    String res = StringUtils::Format(us,
                String("[AudioState isMuted: %b, route; %s, supportedRouteMask: %s]"),
                arr);
    *str = res;
    return NOERROR;
}

ECode AudioState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode AudioState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteByte((Byte) (mIsMuted ? 1 : 0));
    dest->WriteInt32(mRoute);
    dest->WriteInt32(mSupportedRouteMask);
    return NOERROR;
}

String AudioState::AudioRouteToString(
    /* [in] */ Int32 route)
{
    if (route == 0 || (route & ~ROUTE_ALL) != 0x0) {
        return String("UNKNOWN");
    }

    AutoPtr<StringBuffer> buffer = new StringBuffer();
    if ((route & ROUTE_EARPIECE) == ROUTE_EARPIECE) {
        ListAppend(buffer, String("EARPIECE"));
    }
    if ((route & ROUTE_BLUETOOTH) == ROUTE_BLUETOOTH) {
        ListAppend(buffer, String("BLUETOOTH"));
    }
    if ((route & ROUTE_WIRED_HEADSET) == ROUTE_WIRED_HEADSET) {
        ListAppend(buffer, String("WIRED_HEADSET"));
    }
    if ((route & ROUTE_SPEAKER) == ROUTE_SPEAKER) {
        ListAppend(buffer, String("SPEAKER"));
    }

    return buffer->ToString();
}

void AudioState::ListAppend(
    /* [in] */ StringBuffer* buffer,
    /* [in] */ const String& str)
{
    Int32 len = 0;
    buffer->GetLength(&len);
    if (len > 0) {
        buffer->Append(", ");
    }
    buffer->Append(str);
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos