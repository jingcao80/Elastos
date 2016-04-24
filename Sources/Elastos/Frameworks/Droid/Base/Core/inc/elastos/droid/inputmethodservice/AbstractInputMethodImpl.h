
#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_ABSTRACTINPUTMETHODIMPL_H__
#define __ELASTOS_DROID_INPUTMETHODSERVICE_ABSTRACTINPUTMETHODIMPL_H__

#include "Elastos.Droid.InputMethodService.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodService.h"

using Elastos::Droid::InputMethodService::AbstractInputMethodService;
using Elastos::Droid::InputMethodService::IAbstractInputMethodImpl;
using Elastos::Droid::View::InputMethod::IInputMethodSession;
using Elastos::Droid::View::InputMethod::IInputMethodSessionCallback;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

class ECO_PUBLIC AbstractInputMethodImpl
    : public Object
    , public IAbstractInputMethodImpl
    , public IInputMethod
{
public:
    CAR_INTERFACE_DECL();

    AbstractInputMethodImpl(
        /* [in] */ AbstractInputMethodService* host);

    CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference);

    CARAPI CreateSession(
        /* [in] */ IInputMethodSessionCallback* callback);

    CARAPI SetSessionEnabled(
        /* [in] */ IInputMethodSession* session,
        /* [in] */ Boolean enabled);

    CARAPI RevokeSession(
        /* [in] */ IInputMethodSession* session);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    AbstractInputMethodService* mHost;
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHODSERVICE_ABSTRACTINPUTMETHODIMPL_H__
