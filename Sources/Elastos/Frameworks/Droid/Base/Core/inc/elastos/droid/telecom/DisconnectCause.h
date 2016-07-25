#ifndef __ELASTOS_DROID_TELECOM_DISCONNECTCAUSE_H__
#define __ELASTOS_DROID_TELECOM_DISCONNECTCAUSE_H__

#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * Describes the cause of a disconnected call. This always includes a code describing the generic
 * cause of the disconnect. Optionally, it may include a localized label and/or localized description
 * to display to the user which is provided by the {@link ConnectionService}. It also may contain a
 * reason for the the disconnect, which is intended for logging and not for display to the user.
 * @hide
 */
class DisconnectCause
    : public Object
    , public IDisconnectCause
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Creates a new DisconnectCause.
     *
     * @param code The code for the disconnect cause.
     */
    CARAPI constructor(
        /* [in] */ Int32 code);

    /**
     * Creates a new DisconnectCause.
     *
     * @param code The code for the disconnect cause.
     * @param reason The reason for the disconnect.
     */
    CARAPI constructor(
        /* [in] */ Int32 code,
        /* [in] */ const String& reason);

    /**
     * Creates a new DisconnectCause.
     * @param label The localized label to show to the user to explain the disconnect.
     * @param code The code for the disconnect cause.
     * @param description The localized description to show to the user to explain the disconnect.
     * @param reason The reason for the disconnect.
     */
    CARAPI constructor(
        /* [in] */ Int32 code,
        /* [in] */ ICharSequence* label,
        /* [in] */ ICharSequence* description,
        /* [in] */ const String& reason);

    /**
     * Creates a new DisconnectCause.
     *
     * @param code The code for the disconnect cause.
     * @param label The localized label to show to the user to explain the disconnect.
     * @param description The localized description to show to the user to explain the disconnect.
     * @param reason The reason for the disconnect.
     * @param toneToPlay The tone to play on disconnect, as defined in {@link ToneGenerator}.
     */
    CARAPI constructor(
        /* [in] */ Int32 code,
        /* [in] */ ICharSequence* label,
        /* [in] */ ICharSequence* description,
        /* [in] */ const String& reason,
        /* [in] */ Int32 toneToPlay);

    /**
     * Returns the code for the reason for this disconnect.
     *
     * @return The disconnect code.
     */
    CARAPI GetCode(
        /* [out] */ Int32* code);

    /**
     * Returns a short label which explains the reason for the disconnect cause and is for display
     * in the user interface. The {@link ConnectionService } is responsible for providing and
     * localizing this label. If there is no string provided, returns null.
     *
     * @return The disconnect label.
     */
    CARAPI GetLabel(
        /* [out] */ ICharSequence** result);

    /**
     * Returns a description which explains the reason for the disconnect cause and is for display
     * in the user interface. The {@link ConnectionService } is responsible for providing and
     * localizing this message. If there is no string provided, returns null.
     *
     * @return The disconnect description.
     */
    CARAPI GetDescription(
        /* [out] */ ICharSequence** result);

    /**
     * Returns an explanation of the reason for the disconnect. This is not intended for display to
     * the user and is used mainly for logging.
     *
     * @return The disconnect reason.
     */
    CARAPI GetReason(
        /* [out] */ String* result);

    /**
     * Returns the tone to play when disconnected.
     *
     * @return the tone as defined in {@link ToneGenerator} to play when disconnected.
     */
    CARAPI GetTone(
        /* [out] */ Int32* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* destination);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    Int32 mDisconnectCode;
    AutoPtr<ICharSequence> mDisconnectLabel;
    AutoPtr<ICharSequence> mDisconnectDescription;
    String mDisconnectReason;
    Int32 mToneToPlay;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_DISCONNECTCAUSE_H__