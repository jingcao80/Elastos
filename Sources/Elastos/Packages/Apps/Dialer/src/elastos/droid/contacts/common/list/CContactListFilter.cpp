
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/contacts/common/list/CContactListFilter.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Contacts::Common::List::EIID_IContactListFilter;
using Elastos::Droid::Provider::IContactsContractSyncColumns;
using Elastos::Droid::Provider::IContactsContractRawContactsColumns;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

const Int32 CContactListFilter::FILTER_TYPE_GROUP;
const String CContactListFilter::KEY_FILTER_TYPE("filter.type");
const String CContactListFilter::KEY_ACCOUNT_NAME("filter.accountName");
const String CContactListFilter::KEY_ACCOUNT_TYPE("filter.accountType");
const String CContactListFilter::KEY_DATA_SET("filter.dataSet");

CAR_OBJECT_IMPL(CContactListFilter)

CAR_INTERFACE_IMPL_3(CContactListFilter, Object, IContactListFilter, IComparable, IParcelable)

ECode CContactListFilter::constructor()
{
    return NOERROR;
}

ECode CContactListFilter::constructor(
    /* [in] */ Int32 filterType,
    /* [in] */ const String& accountType,
    /* [in] */ const String& accountName,
    /* [in] */ const String& dataSet,
    /* [in] */ IDrawable* icon)
{
    mFilterType = filterType;
    mAccountType = accountType;
    mAccountName = accountName;
    mDataSet = dataSet;
    mIcon = icon;
    return NOERROR;
}

AutoPtr<IContactListFilter> CContactListFilter::CreateFilterWithType(
    /* [in] */ Int32 filterType)
{
    AutoPtr<IContactListFilter> filter;
    CContactListFilter::New(filterType, String(NULL), String(NULL), String(NULL),
            NULL, (IContactListFilter**)&filter);
    return filter;
}

AutoPtr<IContactListFilter> CContactListFilter::CreateAccountFilter(
    /* [in] */ const String& accountType,
    /* [in] */ const String& accountName,
    /* [in] */ const String& dataSet,
    /* [in] */ IDrawable* icon)
{
    AutoPtr<IContactListFilter> filter;
    CContactListFilter::New(FILTER_TYPE_ACCOUNT, accountType, accountName, dataSet,
            icon, (IContactListFilter**)&filter);
    return filter;
}

ECode CContactListFilter::IsValidationRequired(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFilterType == FILTER_TYPE_ACCOUNT;
    return NOERROR;
}

ECode CContactListFilter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    switch (filterType) {
        case FILTER_TYPE_DEFAULT:
            *str = String("default");
        case FILTER_TYPE_ALL_ACCOUNTS:
            *str = String("all_accounts");
        case FILTER_TYPE_CUSTOM:
            *str = String("custom");
        case FILTER_TYPE_STARRED:
            *str = String("starred");
        case FILTER_TYPE_WITH_PHONE_NUMBERS_ONLY:
            return "with_phones");
        case FILTER_TYPE_SINGLE_CONTACT:
            return "single");
        case FILTER_TYPE_ACCOUNT:
            *str = String("account: ") + mAccountType + (!mDataSet.IsNull()? "/" + mDataSet : "")
                    + " " + mAccountName;
    }
    return NOERROR;
}

ECode CContactListFilter::CompareTo(
    /* [in] */ IInterface* that,
    /* [out] */ Int32*  result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContactListFilter> filter = IContactListFilter::Probe(that);
    String accountName;
    filter->GetAccountName(&accountName);
    Int32 res = mAccountName.Compare(accountName);
    if (res != 0) {
        *result = res;
        return NOERROR;
    }

    String accountType;
    filter->GetAccountType(&accountType);
    res = mAccountType.Compare(accountType);
    if (res != 0) {
        *result = res;
        return NOERROR;
    }

    Int32 filterType;
    filter->GetFilterType(&filterType);
    *result = mFilterType - filterType;
    return NOERROR;
}

ECode CContactListFilter::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 code = mFilterType;
    if (!mAccountType.IsNull()) {
        code = code * 31 + mAccountType.GetHashCode();
        code = code * 31 + mAccountName.GetHashCode();
    }
    if (!mDataSet.IsNull()) {
        code = code * 31 + mDataSet.GetHashCode();
    }
    *hashCode = code;
}

ECode CContactListFilter::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    if (IInterface::Probe(this) == other) {
        *isEqual = TRUE;
        return NOERROR;
    }

    AutoPtr<IContactListFilter> otherFilter = IContactListFilter::Probe(obj);
    if (filter == NULL) {
        *isEqual = FALSE;
        return NOERROR;
    }

    Int32 filterType;
    String accountName, accountType, dataSet;
    if ((otherFilter->GetFilterType(&filterType), mFilterType != filterType)
            || (otherFilter->GetAccountName(&accountName), !TextUtils::Equals(mAccountName, accountName))
            || (otherFilter->GetAccountType(&accountType), !TextUtils::Equals(mAccountType, accountType))
            || (otherFilter->GetDataSet(&dataSet), !TextUtils::Equals(mDataSet, dataSet))) {
        *isEqual = FALSE;
        return NOERROR;
    }

    *isEqual = TRUE;
    return NOERROR;
}

void CContactListFilter::StoreToPreferences(
    /* [in] */ ISharedPreferences* prefs,
    /* [in] */ IContactListFilter* filter)
{
    Int32 filterType;
    if (filter != NULL && (filter->GetFilterType(&filterType), filterType == FILTER_TYPE_SINGLE_CONTACT)) {
        return;
    }
    prefs->Edit();
    prefs->PutInt32(KEY_FILTER_TYPE, filter == NULL ? FILTER_TYPE_DEFAULT : filterType);
    String accountName;
    prefs->PutString(KEY_ACCOUNT_NAME,
            filter == NULL ? String(NULL) : (filter->GetAccountName(&accountName), accountName));
    String accountType;
    prefs->PutString(KEY_ACCOUNT_TYPE,
            filter == NULL ? String(NULL) : (filter->GetAccountType(&accountType), accountType));
    String dataSet;
    prefs->PutString(KEY_DATA_SET,
            filter == NULL ? String(NULL) : (filter->GetDataSet(&dataSet), dataSet));
    prefs->Apply();
}

AutoPtr<IContactListFilter> CContactListFilter::RestoreDefaultPreferences(
    /* [in] */ ISharedPreferences* prefs)
{
    AutoPtr<IContactListFilter> filter = RestoreFromPreferences(prefs);
    if (filter == NULL) {
        filter = CreateFilterWithType(FILTER_TYPE_ALL_ACCOUNTS);
    }
    // "Group" filter is obsolete and thus is not exposed anymore. The "single contact mode"
    // should also not be stored in preferences anymore since it is a temporary state.
    Int32 filterType;
    filter->GetFilterType(&filterType);
    if (filterType == FILTER_TYPE_GROUP ||
            filterType == FILTER_TYPE_SINGLE_CONTACT) {
        filter = CreateFilterWithType(FILTER_TYPE_ALL_ACCOUNTS);
    }
    return filter;
}

AutoPtr<IContactListFilter> CContactListFilter::RestoreFromPreferences(
    /* [in] */ ISharedPreferences* prefs)
{
    Int32 filterType;
    prefs->GetInt(KEY_FILTER_TYPE, FILTER_TYPE_DEFAULT, &filterType);
    if (filterType == FILTER_TYPE_DEFAULT) {
        return NULL;
    }

    String accountName;
    prefs->GetString(KEY_ACCOUNT_NAME, String(NULL), &accountName);
    String accountType;
    prefs->GetString(KEY_ACCOUNT_TYPE, String(NULL), &accountType);
    String dataSet;
    prefs->GetString(KEY_DATA_SET, String(NULL), &dataSet);
    AutoPtr<IContactListFilter> filter;
    CContactListFilter::New(filterType, accountType, accountName, dataSet,
            NULL, (IContactListFilter**)&filter);
    return filter;
}

ECode CContactListFilter::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mFilterType);
    dest->WriteString(mAccountName);
    dest->WriteString(mAccountType);
    dest->WriteString(mDataSet);
    return NOERROR;
}

ECode CContactListFilter::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mFilterType);
    source->ReadString(&mAccountName);
    source->ReadString(&mAccountType);
    source->ReadString(&mDataSet);
    return NOERROR;
}

ECode CContactListFilter::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id)
    if (mId.IsNull()) {
        StringBuilder sb = new StringBuilder();
        sb.Append(mFilterType);
        if (!mAccountType.IsNull()) {
            sb.AppendChar('-');
            sb.Append(mAccountType);
        }
        if (!mDataSet.IsNull()) {
            sb.AppendChar('/');
            sb.Append(mDataSet);
        }
        if (!mAccountName.IsNull()) {
            sb.AppendChar('-');
            sb.Append(mAccountName.Replace('-', '_'));
        }
        mId = sb.ToString();
    }
    *id = mId;
    return NOERROR;
}

ECode CContactListFilter::AddAccountQueryParameterToUrl(
    /* [in] */ IUriBuilder* uriBuilder,
    /* [out] */ IUriBuilder** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mFilterType != FILTER_TYPE_ACCOUNT) {
        Logger::E("CContactListFilter", "filterType must be FILTER_TYPE_ACCOUNT");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    uriBuilder->AppendQueryParameter(IContactsContractSyncColumns::ACCOUNT_NAME, mAccountName);
    uriBuilder->AppendQueryParameter(IContactsContractSyncColumns::ACCOUNT_TYPE, mAccountType);
    if (!TextUtils::IsEmpty(mDataSet)) {
        uriBuilder->AppendQueryParameter(IContactsContractRawContactsColumns::DATA_SET, mDataSet);
    }
    *result = uriBuilder;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CContactListFilter::ToDebugString(
    /* [out] */ String* debugStr)
{
    VALIDATE_NOT_NULL(debugStr)
    StringBuilder builder;
    builder.Append("[filter type: ");
    builder.Append(mFilterType);
    builder.Append(" (");
    builder.Append(FilterTypeToString(mFilterType) + ")");
    if (mFilterType == FILTER_TYPE_ACCOUNT) {
        builder.Append(", accountType: ");
        builder.Append(mAccountType);
        builder.Append(", accountName: ");
        builder.Append(mAccountName);
        builder.Append(", dataSet: ");
        builder.Append(mDataSet);
    }
    builder.Append(", icon: ");
    builder.Append(TO_CSTR(mIcon) + "]");
    *debugStr = builder.ToString();
    return NOERROR;
}

String CContactListFilter::FilterTypeToString(
    /* [in] */ Int32 filterType)
{
    switch (filterType) {
        case FILTER_TYPE_DEFAULT:
            return String("FILTER_TYPE_DEFAULT");
        case FILTER_TYPE_ALL_ACCOUNTS:
            return String("FILTER_TYPE_ALL_ACCOUNTS");
        case FILTER_TYPE_CUSTOM:
            return String("FILTER_TYPE_CUSTOM");
        case FILTER_TYPE_STARRED:
            return String("FILTER_TYPE_STARRED");
        case FILTER_TYPE_WITH_PHONE_NUMBERS_ONLY:
            return String("FILTER_TYPE_WITH_PHONE_NUMBERS_ONLY");
        case FILTER_TYPE_SINGLE_CONTACT:
            return String("FILTER_TYPE_SINGLE_CONTACT");
        case FILTER_TYPE_ACCOUNT:
            return String("FILTER_TYPE_ACCOUNT");
        default:
            return String("(unknown)");
    }
}

ECode CContactListFilter::GetFilterType(
    /* [out] */ Int32* filterType)
{
    VALIDATE_NOT_NULL(filterType)
    *filterType = mFilterType;
    return NOERROR;
}

ECode CContactListFilter::SetFilterType(
    /* [in] */ Int32 filterType)
{
    mFilterType = filterType;
    return NOERROR;
}

ECode CContactListFilter::GetAccountType(
    /* [out] */ String* accountType)
{
    VALIDATE_NOT_NULL(accountType)
    *accountType = mAccountType;
    return NOERROR;
}

ECode CContactListFilter::SetAccountType(
    /* [in] */ const String& accountType)
{
    mAccountType = accountType;
    return NOERROR;
}

ECode CContactListFilter::GetAccountName(
    /* [out] */ String* accountName)
{
    VALIDATE_NOT_NULL(accountName)
    *accountName = mAccountName;
    return NOERROR;
}

ECode CContactListFilter::SetAccountName(
    /* [in] */ const String& accountName)
{
    mAccountName = accountName;
    return NOERROR;
}

ECode CContactListFilter::GetDataSet(
    /* [out] */ String* dataSet)
{
    VALIDATE_NOT_NULL(dataSet)
    *dataSet = mDataSet;
    return NOERROR;
}

ECode CContactListFilter::SetDataSet(
    /* [in] */ const String& dataSet)
{
    mDataSet = dataSet;
    return NOERROR;
}

ECode CContactListFilter::GetIcon(
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = mIcon;
    REFCOUNT_ADD(*icon)
    return NOERROR;
}

ECode CContactListFilter::SetIcon(
    /* [in] */ IDrawable* icon)
{
    mIcon = icon;
    return NOERROR;
}

} // namespace List
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
