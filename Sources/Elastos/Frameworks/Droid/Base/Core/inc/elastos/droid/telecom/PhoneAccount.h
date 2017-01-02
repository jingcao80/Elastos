//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_TELECOM_PHONEACCOUNT_H__
#define __ELASTOS_DROID_TELECOM_PHONEACCOUNT_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;
using Elastos::Utility::IBitSet;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * Describes a distinct account, line of service or call placement method that the system
 * can use to place phone calls.
 * @hide
 */
class PhoneAccount
    : public Object
    , public IPhoneAccount
    , public IParcelable
{
public:
    class Builder
        : public Object
        , public IPhoneAccountBuilder
    {
    public:
        CAR_INTERFACE_DECL()

        Builder();

        CARAPI constructor(
            /* [in] */ IPhoneAccountHandle* accountHandle,
            /* [in] */ ICharSequence* label);

        /**
         * Creates an instance of the {@link PhoneAccount.Builder} from an existing
         * {@link PhoneAccount}.
         *
         * @param phoneAccount The {@link PhoneAccount} used to initialize the builder.
         */
        CARAPI constructor(
            /* [in] */ IPhoneAccount* phoneAccount);

        CARAPI SetAddress(
            /* [in] */ IUri* value);

        CARAPI SetSubscriptionAddress(
            /* [in] */ IUri* value);

        CARAPI SetCapabilities(
            /* [in] */ Int32 value);

        CARAPI SetIconResId(
            /* [in] */ Int32 value);

        CARAPI SetShortDescription(
            /* [in] */ ICharSequence* value);

        /**
         * Specifies an additional URI scheme supported by the {@link PhoneAccount}.
         *
         * @param uriScheme The URI scheme.
         * @return The Builder.
         * @hide
         */
        CARAPI AddSupportedUriScheme(
            /* [in] */ const String& uriScheme);

        /**
         * Specifies the URI schemes supported by the {@link PhoneAccount}.
         *
         * @param uriSchemes The URI schemes.
         * @return The Builder.
         */
        CARAPI SetSupportedUriSchemes(
            /* [in] */ IList* uriSchemes);

        /**
         * Creates an instance of a {@link PhoneAccount} based on the current builder settings.
         *
         * @return The {@link PhoneAccount}.
         */
        CARAPI Build(
            /* [in] */ IPhoneAccount** result);

    private:
        AutoPtr<IPhoneAccountHandle> mAccountHandle;
        AutoPtr<IUri> mAddress;
        AutoPtr<IUri> mSubscriptionAddress;
        Int32 mCapabilities;
        Int32 mIconResId;
        AutoPtr<ICharSequence> mLabel;
        AutoPtr<ICharSequence> mShortDescription;
        AutoPtr<IList> mSupportedUriSchemes;
    };

public:
    CAR_INTERFACE_DECL()

    PhoneAccount();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IPhoneAccountHandle* account,
        /* [in] */ IUri* address,
        /* [in] */ IUri* subscriptionAddress,
        /* [in] */ Int32 capabilities,
        /* [in] */ Int32 iconResId,
        /* [in] */ ICharSequence* label,
        /* [in] */ ICharSequence* shortDescription,
        /* [in] */ IList* supportedUriSchemes);

    static AutoPtr<IPhoneAccountBuilder> _Builder(
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [in] */ ICharSequence* label);

    /**
     * {@hide}
     */
    CARAPI SetBit(
        /* [in] */ Int32 bit);

    /**
     * {@hide}
     */
    CARAPI UnSetBit(
        /* [in] */ Int32 bit);

    /**
     * {@hide}
     */
    CARAPI IsSet(
        /* [in] */ Int32 bit,
        /* [out] */ Boolean* result);

    /**
     * Returns a builder initialized with the current {@link PhoneAccount} instance.
     *
     * @return The builder.
     * @hide
     */
    CARAPI ToBuilder(
        /* [out] */ IPhoneAccountBuilder** result);

    /**
     * The unique identifier of this {@code PhoneAccount}.
     *
     * @return A {@code PhoneAccountHandle}.
     */
    CARAPI GetAccountHandle(
        /* [out] */ IPhoneAccountHandle** result);

    /**
     * The address (e.g., a phone number) associated with this {@code PhoneAccount}. This
     * represents the destination from which outgoing calls using this {@code PhoneAccount}
     * will appear to come, if applicable, and the destination to which incoming calls using this
     * {@code PhoneAccount} may be addressed.
     *
     * @return A address expressed as a {@code Uri}, for example, a phone number.
     */
    CARAPI GetAddress(
        /* [out] */ IUri** result);

    /**
     * The raw callback number used for this {@code PhoneAccount}, as distinct from
     * {@link #getAddress()}. For the majority of {@code PhoneAccount}s this should be registered
     * as {@code null}.  It is used by the system for SIM-based {@code PhoneAccount} registration
     *
     * @return The subscription number, suitable for display to the user.
     */
    CARAPI GetSubscriptionAddress(
        /* [out] */ IUri** result);

    /**
     * The capabilities of this {@code PhoneAccount}.
     *
     * @return A bit field of flags describing this {@code PhoneAccount}'s capabilities.
     */
    CARAPI GetCapabilities(
        /* [out] */ Int32* result);

    /**
     * Determines if this {@code PhoneAccount} has a capabilities specified by the passed in
     * bit mask.
     *
     * @param capability The capabilities to check.
     * @return {@code True} if the phone account has the capability.
     */
    CARAPI HasCapabilities(
        /* [in] */ Int32 capability,
        /* [out] */ Boolean* result);

    /**
     * A short label describing a {@code PhoneAccount}.
     *
     * @return A label for this {@code PhoneAccount}.
     */
    CARAPI GetLabel(
        /* [out] */ ICharSequence** result);

    /**
     * A short paragraph describing this {@code PhoneAccount}.
     *
     * @return A description for this {@code PhoneAccount}.
     */
    CARAPI GetShortDescription(
        /* [out] */ ICharSequence** result);

    /**
     * The URI schemes supported by this {@code PhoneAccount}.
     *
     * @return The URI schemes.
     */
    CARAPI GetSupportedUriSchemes(
        /* [out] */ IList** result);

    /**
     * Determines if the {@link PhoneAccount} supports calls to/from addresses with a specified URI
     * scheme.
     *
     * @param uriScheme The URI scheme to check.
     * @return {@code True} if the {@code PhoneAccount} supports calls to/from addresses with the
     * specified URI scheme.
     */
    CARAPI SupportsUriScheme(
        /* [in] */ const String& uriScheme,
        /* [out] */ Boolean* result);

    /**
     * The icon resource ID for the icon of this {@code PhoneAccount}.
     *
     * @return A resource ID.
     */
    CARAPI GetIconResId(
        /* [out] */ Int32* result);

    /**
     * An icon to represent this {@code PhoneAccount} in a user interface.
     *
     * @return An icon for this {@code PhoneAccount}.
     */
    CARAPI GetIcon(
        /* [in] */ IContext* context,
        /* [out] */ IDrawable** result);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

private:
    AutoPtr<IDrawable> GetIcon(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resId);

private:
    AutoPtr<IPhoneAccountHandle> mAccountHandle;
    AutoPtr<IUri> mAddress;
    AutoPtr<IUri> mSubscriptionAddress;
    Int32 mCapabilities;
    Int32 mIconResId;
    AutoPtr<ICharSequence> mLabel;
    AutoPtr<ICharSequence> mShortDescription;
    AutoPtr<IList> mSupportedUriSchemes;
    AutoPtr<IBitSet> mDsda;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_PHONEACCOUNT_H__