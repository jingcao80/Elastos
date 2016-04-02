
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/PhoneAccount.h"
#include "elastos/droid/telecomm/telecom/CPhoneAccount.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// PhoneAccount::Builder::
//===============================================================
CAR_INTERFACE_IMPL(PhoneAccount::Builder, Object, IPhoneAccountBuilder)

PhoneAccount::Builder::Builder()
    : mCapabilities(0)
    , mIconResId(0)
{
    CArrayList::New((IList**)&mSupportedUriSchemes);
}

ECode PhoneAccount::Builder::constructor(
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ ICharSequence* label)
{
    mAccountHandle = accountHandle;
    mLabel = label;
    return NOERROR;
}

ECode PhoneAccount::Builder::constructor(
    /* [in] */ IPhoneAccount* phoneAccount)
{
    phoneAccount->GetAccountHandle((IPhoneAccountHandle**)&mAccountHandle);
    phoneAccount->GetAddress((IUri**)&mAddress);
    phoneAccount->GetSubscriptionAddress((IUri**)&mSubscriptionAddress);
    phoneAccount->GetCapabilities(&mCapabilities);
    phoneAccount->GetIconResId(&mIconResId);
    phoneAccount->GetLabel((ICharSequence**)&mLabel);
    phoneAccount->GetShortDescription((ICharSequence**)&mShortDescription);
    AutoPtr<IList> l;
    phoneAccount->GetSupportedUriSchemes((IList**)&l);
    mSupportedUriSchemes->AddAll(ICollection::Probe(l));
    return NOERROR;
}

ECode PhoneAccount::Builder::SetAddress(
    /* [in] */ IUri* value)
{
    mAddress = value;
    return NOERROR;
}

ECode PhoneAccount::Builder::SetSubscriptionAddress(
    /* [in] */ IUri* value)
{
    mSubscriptionAddress = value;
    return NOERROR;
}

ECode PhoneAccount::Builder::SetCapabilities(
    /* [in] */ Int32 value)
{
    mCapabilities = value;
    return NOERROR;
}

ECode PhoneAccount::Builder::SetIconResId(
    /* [in] */ Int32 value)
{
    mIconResId = value;
    return NOERROR;
}

ECode PhoneAccount::Builder::SetShortDescription(
    /* [in] */ ICharSequence* value)
{
    mShortDescription = value;
    return NOERROR;
}

ECode PhoneAccount::Builder::AddSupportedUriScheme(
    /* [in] */ const String& uriScheme)
{
    AutoPtr<ICharSequence> pUriScheme;
    CString::New(uriScheme, (ICharSequence**)&pUriScheme);
    Boolean bContain = FALSE;
    if (!TextUtils::IsEmpty(uriScheme) &&
        !(mSupportedUriSchemes->Contains(pUriScheme, &bContain), bContain)) {
        mSupportedUriSchemes->Add(pUriScheme);
    }
    return NOERROR;
}

ECode PhoneAccount::Builder::SetSupportedUriSchemes(
    /* [in] */ IList* uriSchemes)
{
    mSupportedUriSchemes->Clear();

    Boolean bEmp = FALSE;
    if (uriSchemes != NULL && !(uriSchemes->IsEmpty(&bEmp), bEmp)) {
        AutoPtr<IIterator> it;
        uriSchemes->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<ICharSequence> pUriScheme = ICharSequence::Probe(p);
            String uriScheme;
            pUriScheme->ToString(&uriScheme);
            AddSupportedUriScheme(uriScheme);
        }
    }
    return NOERROR;
}

ECode PhoneAccount::Builder::Build(
    /* [in] */ IPhoneAccount** result)
{
    // If no supported URI schemes were defined, assume "tel" is supported.
    Boolean bEmp = FALSE;
    if ((mSupportedUriSchemes->IsEmpty(&bEmp), bEmp)) {
        AddSupportedUriScheme(SCHEME_TEL);
    }

    AutoPtr<IPhoneAccount> res;
    CPhoneAccount::New(
                    mAccountHandle,
                    mAddress,
                    mSubscriptionAddress,
                    mCapabilities,
                    mIconResId,
                    mLabel,
                    mShortDescription,
                    mSupportedUriSchemes,
                    (IPhoneAccount**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================
// PhoneAccount::
//===============================================================
CAR_INTERFACE_IMPL_2(PhoneAccount, Object, IPhoneAccount, IParcelable)

ECode PhoneAccount::constructor(
    /* [in] */ IPhoneAccountHandle* account,
    /* [in] */ IUri* address,
    /* [in] */ IUri* subscriptionAddress,
    /* [in] */ Int32 capabilities,
    /* [in] */ Int32 iconResId,
    /* [in] */ ICharSequence* label,
    /* [in] */ ICharSequence* shortDescription,
    /* [in] */ IList* supportedUriSchemes)
{
    mAccountHandle = account;
    mAddress = address;
    mSubscriptionAddress = subscriptionAddress;
    mCapabilities = capabilities;
    mIconResId = iconResId;
    mLabel = label;
    mShortDescription = shortDescription;
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->UnmodifiableList(supportedUriSchemes, (IList**)&mSupportedUriSchemes);
    return NOERROR;
}

AutoPtr<IPhoneAccountBuilder> PhoneAccount::_Builder(
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ ICharSequence* label)
{
    AutoPtr<Builder> res = new Builder();
    res->constructor(accountHandle, label);
    return IPhoneAccountBuilder::Probe(res);
}

ECode PhoneAccount::ToBuilder(
    /* [out] */ IPhoneAccountBuilder** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<Builder> res = new Builder();
    res->constructor(this);
    *result = IPhoneAccountBuilder::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccount::GetAccountHandle(
    /* [out] */ IPhoneAccountHandle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAccountHandle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccount::GetAddress(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccount::GetSubscriptionAddress(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSubscriptionAddress;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccount::GetCapabilities(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCapabilities;
    return NOERROR;
}

ECode PhoneAccount::HasCapabilities(
    /* [in] */ int capability,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mCapabilities & capability) == capability;
    return NOERROR;
}

ECode PhoneAccount::GetLabel(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLabel;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccount::GetShortDescription(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mShortDescription;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccount::GetSupportedUriSchemes(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSupportedUriSchemes;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneAccount::SupportsUriScheme(
    /* [in] */ const String& uriScheme,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mSupportedUriSchemes == NULL || uriScheme.IsNull()) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IIterator> it;
    mSupportedUriSchemes->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ICharSequence> pScheme = ICharSequence::Probe(p);
        String scheme;
        pScheme->ToString(&scheme);
        if (!scheme.IsNull() && scheme.Equals(uriScheme)) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode PhoneAccount::GetIconResId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIconResId;
    return NOERROR;
}

ECode PhoneAccount::GetIcon(
    /* [in] */ IContext* context,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDrawable> d = GetIcon(context, mIconResId);
    *result = d.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IDrawable> PhoneAccount::GetIcon(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> packageContext;

    ECode ec = context->CreatePackageContext(
            String("Telecom"), 0, (IContext**)&packageContext); // mAccountHandle.getComponentName().getPackageName()
    if (FAILED(ec)) {
        Logger::E("PhoneAccount", "Cannot find package Telecom");
        return NULL;
    }

    AutoPtr<IDrawable> res;
    packageContext->GetDrawable(resId, (IDrawable**)&res);
    return res;
}

ECode PhoneAccount::WriteToParcel(
    /* [in] */ IParcel* out)
{
    IParcelable::Probe(mAccountHandle)->WriteToParcel(out);
    IParcelable::Probe(mAddress)->WriteToParcel(out);
    IParcelable::Probe(mSubscriptionAddress)->WriteToParcel(out);
    out->WriteInt32(mCapabilities);
    out->WriteInt32(mIconResId);
    IParcelable::Probe(mLabel)->WriteToParcel(out);
    IParcelable::Probe(mShortDescription)->WriteToParcel(out);
    IParcelable::Probe(mSupportedUriSchemes)->WriteToParcel(out);
    return NOERROR;
}

ECode PhoneAccount::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    IParcelable::Probe(mAccountHandle)->ReadFromParcel(in);
    IParcelable::Probe(mAddress)->ReadFromParcel(in);
    IParcelable::Probe(mSubscriptionAddress)->ReadFromParcel(in);
    in->ReadInt32(&mCapabilities);
    in->ReadInt32(&mIconResId);
    IParcelable::Probe(mLabel)->ReadFromParcel(in);
    IParcelable::Probe(mShortDescription)->ReadFromParcel(in);

    AutoPtr<IList> supportedUriSchemes;
    CArrayList::New((IList**)&supportedUriSchemes);
    IParcelable::Probe(supportedUriSchemes)->ReadFromParcel(in);
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    cls->UnmodifiableList(supportedUriSchemes, (IList**)&mSupportedUriSchemes);
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos