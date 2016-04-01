
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_CINPUTBINDRESULT_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_CINPUTBINDRESULT_H__

#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Internal_View_CInputBindResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IInputChannel;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CarClass(CInputBindResult)
    , public Object
    , public IInputBindResult
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIInputMethodSession* method,
        /* [in] */ IInputChannel* channel,
        /* [in] */ const String& id,
        /* [in] */ Int32 sequence,
        /* [in] */ Int32 userActionNotificationSequenceNumber);

    CARAPI GetMethod(
        /* [out] */ IIInputMethodSession** session);

    CARAPI GetChannel(
        /* [out] */ IInputChannel** channel);

    CARAPI GetId(
        /* [out] */ String* id);

    CARAPI GetSequence(
        /* [out] */ Int32* sequence);

    CARAPI GetUserActionNotificationSequenceNumber(
        /* [out] */ Int32* userActionNotificationSequenceNumber);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    static const String TAG;
    /**
     * The input method service.
     */
    AutoPtr<IIInputMethodSession> mMethod;

    /**
     * The input channel used to send input events to this IME.
     */
    AutoPtr<IInputChannel> mChannel;

    /**
     * The ID for this input method, as found in InputMethodInfo; null if
     * no input method will be bound.
     */
    String mId;

    /**
     * Sequence number of this binding.
     */
    Int32 mSequence;

    /**
     * Sequence number of user action notification.
     */
    Int32 mUserActionNotificationSequenceNumber;
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_CINPUTBINDRESULT_H__
