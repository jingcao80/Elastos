
#include "elastos/droid/teleservice/phone/settings/CAccountSelectionPreference.h"
#include <elastos/core/Object.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include "elastos/utility/Objects.h"

using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Core::Object;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {
namespace Settings {

CAR_INTERFACE_IMPL(CAccountSelectionPreference::InnerListener, Object,
        IPreferenceOnPreferenceChangeListener)

CAccountSelectionPreference::InnerListener::InnerListener(
    /* [in] */ CAccountSelectionPreference* host)
    : mHost(host)
{}

ECode CAccountSelectionPreference::InnerListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* res)
{
    return mHost->OnPreferenceChange(preference, newValue, res);
}

CAR_OBJECT_IMPL(CAccountSelectionPreference)

CAR_INTERFACE_IMPL(CAccountSelectionPreference, ListPreference, IAccountSelectionPreference)

CAccountSelectionPreference::CAccountSelectionPreference()
    : mShowSelectionInSummary(TRUE)
{}

ECode CAccountSelectionPreference::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CAccountSelectionPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ListPreference::constructor(context, attrs);
    mContext = context;
    AutoPtr<InnerListener> listener = new InnerListener(this);
    return SetOnPreferenceChangeListener(listener);
}

ECode CAccountSelectionPreference::SetListener(
    /* [in] */ IAccountSelectionListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode CAccountSelectionPreference::SetShowSelectionInSummary(
    /* [in] */ Boolean value)
{
    mShowSelectionInSummary = value;
    return NOERROR;
}

ECode CAccountSelectionPreference::SetModel(
    /* [in] */ ITelecomManager* telecomManager,
    /* [in] */ IList* accountsList,
    /* [in] */ IPhoneAccountHandle* currentSelection,
    /* [in] */ ICharSequence* nullSelectionString)
{
    AutoPtr<ArrayOf<IInterface*> > array;
    accountsList->ToArray((ArrayOf<IInterface*>**)&mAccounts);

    mEntryValues = ArrayOf<ICharSequence*>::Alloc(mAccounts->GetLength() + 1);
    mEntries = ArrayOf<ICharSequence*>::Alloc(mAccounts->GetLength() + 1);

    Int32 selectedIndex = mAccounts->GetLength();  // Points to nullSelectionString by default
    Int32 i = 0;
    for ( ; i < mAccounts->GetLength(); i++) {
        AutoPtr<IPhoneAccount> account;
        telecomManager->GetPhoneAccount(IPhoneAccountHandle::Probe((*mAccounts)[i]), (IPhoneAccount**)&account);
        AutoPtr<ICharSequence> label;
        account->GetLabel((ICharSequence**)&label);

        if (label == NULL) {
            mEntries->Set(i, NULL);
        }
        else {
            mEntries->Set(i, label);
        }

        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(StringUtils::ToString(i));
        mEntryValues->Set(i,  cchar);
        if (Objects::Equals(TO_IINTERFACE(currentSelection), (*mAccounts)[i])) {
            selectedIndex = i;
        }
    }
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(StringUtils::ToString(i));
    mEntryValues->Set(i,  cchar);
    mEntries->Set(i, nullSelectionString);

    SetEntryValues(mEntryValues);
    SetEntries(mEntries);
    SetValueIndex(selectedIndex);
    if (mShowSelectionInSummary) {
        SetSummary((*mEntries)[selectedIndex]);
    }
    return NOERROR;
}

ECode CAccountSelectionPreference::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mListener != NULL) {
        AutoPtr<ICharSequence> cchar = ICharSequence::Probe(newValue);
        String str;
        cchar->ToString(&str);
        Int32 index = StringUtils::ParseInt32(str);

        AutoPtr<IPhoneAccountHandle> account;
        if (index < mAccounts->GetLength()) {
            account = IPhoneAccountHandle::Probe((*mAccounts)[index]);
        }
        else {
            account = NULL;
        }
        Boolean res;
        if ((mListener->OnAccountSelected(this, account, &res), res)) {
            if (mShowSelectionInSummary) {
                SetSummary((*mEntries)[index]);
            }
            String str;
            GetValue(&str);
            Int32 _index;
            FindIndexOfValue(str, &_index);
            if (index != _index) {
                SetValueIndex(index);
                mListener->OnAccountChanged(this);
            }
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CAccountSelectionPreference::OnPrepareDialogBuilder(
    /* [in] */ IAlertDialogBuilder* builder)
{
    // Notify the listener that the dialog is about to be built.  This is important so that the
    // list of enabled accounts can be updated prior to showing the dialog.
    mListener->OnAccountSelectionDialogShow(this);

    return ListPreference::OnPrepareDialogBuilder(builder);
}

} // namespace Settings
} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos