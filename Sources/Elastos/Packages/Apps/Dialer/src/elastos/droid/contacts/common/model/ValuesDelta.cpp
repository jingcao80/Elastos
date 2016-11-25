
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/contacts/common/model/ValuesDelta.h"
#include "elastos/droid/contacts/common/model/CValuesDelta.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentProviderOperationHelper;
using Elastos::Droid::Content::CContentProviderOperationHelper;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractDataColumns;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsGroupMembership;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhoto;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsStructuredName;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsCommonColumns;
using Elastos::Core::ICharSequence;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IIterator;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Model {

Int32 ValuesDelta::sNextInsertId = -1;

ValuesDelta::ValuesDelta()
    : mIdColumn(IBaseColumns::ID)
    , mFromTemplate(FALSE)
{}

CAR_INTERFACE_IMPL_2(ValuesDelta, Object, IValuesDelta, IParcelable)

ECode ValuesDelta::constructor()
{
    return NOERROR;
}

AutoPtr<IValuesDelta> ValuesDelta::FromBefore(
    /* [in] */ IContentValues* before)
{
    AutoPtr<CValuesDelta> entry;
    CValuesDelta::NewByFriend((CValuesDelta**)&entry);
    entry->mBefore = before;
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    entry->mAfter = values;
    return IValuesDelta::Probe(entry);
}

AutoPtr<IValuesDelta> ValuesDelta::FromAfter(
    /* [in] */ IContentValues* after)
{
    AutoPtr<CValuesDelta> entry;
    CValuesDelta::NewByFriend((CValuesDelta**)&entry);
    entry->mBefore = NULL;
    entry->mAfter = after;

    // Assign temporary id which is dropped before insert.
    entry->mAfter->Put(entry->mIdColumn, sNextInsertId--);
    return IValuesDelta::Probe(entry);
}

ECode ValuesDelta::GetAfter(
    /* [out] */ IContentValues** after)
{
    VALIDATE_NOT_NULL(after)
    *after = mAfter;
    REFCOUNT_ADD(*after)
    return NOERROR;
}

ECode ValuesDelta::SetAfter(
    /* [in] */ IContentValues* after)
{
    mAfter = after;
    return NOERROR;
}

ECode ValuesDelta::ContainsKey(
    /* [in] */ const String& key,
    /* [out] */ Boolean* contains)
{
    VALIDATE_NOT_NULL(contains)
    Boolean afterContains, beforeContains;
    *contains = ((mAfter != NULL && (mAfter->ContainsKey(key, &afterContains), afterContains)) ||
            (mBefore != NULL && (mBefore->ContainsKey(key, &beforeContains), beforeContains)));
    return NOERROR;
}

ECode ValuesDelta::GetAsString(
    /* [in] */ const String& key,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Boolean afterContains, beforeContains;
    if (mAfter != NULL && (mAfter->ContainsKey(key, &afterContains), afterContains)) {
        return mAfter->GetAsString(key, str);
    }
    else if (mBefore != NULL && (mBefore->ContainsKey(key, &beforeContains), beforeContains)) {
        return mBefore->GetAsString(key, str);
    }
    else {
        *str = String(NULL);
        return NOERROR;
    }
}

ECode ValuesDelta::GetAsByteArray(
    /* [in] */ const String& key,
    /* [out, callee] */ ArrayOf<Byte>** byteArray)
{
    VALIDATE_NOT_NULL(byteArray)
    Boolean afterContains, beforeContains;
    if (mAfter != NULL && (mAfter->ContainsKey(key, &afterContains), afterContains)) {
        return mAfter->GetAsByteArray(key, byteArray);
    }
    else if (mBefore != NULL && (mBefore->ContainsKey(key, &beforeContains), beforeContains)) {
        return mBefore->GetAsByteArray(key, byteArray);
    }
    else {
        *byteArray = NULL;
        return NOERROR;
    }
}

ECode ValuesDelta::GetAsInt64(
    /* [in] */ const String& key,
    /* [out] */ IInteger64** value)
{
    VALIDATE_NOT_NULL(value)
    Boolean afterContains, beforeContains;
    if (mAfter != NULL && (mAfter->ContainsKey(key, &afterContains), afterContains)) {
        return mAfter->GetAsInteger64(key, value);
    }
    else if (mBefore != NULL && (mBefore->ContainsKey(key, &beforeContains), beforeContains)) {
        return mBefore->GetAsInteger64(key, value);
    }
    else {
        *value = NULL;
        return NOERROR;
    }
}

ECode ValuesDelta::GetAsInteger(
    /* [in] */ const String& key,
    /* [out] */ IInteger32** value)
{
    VALIDATE_NOT_NULL(value)
    return GetAsInteger(key, NULL, value);
}

ECode ValuesDelta::GetAsInteger(
    /* [in] */ const String& key,
    /* [in] */ IInteger32* defaultValue,
    /* [out] */ IInteger32** value)
{
    VALIDATE_NOT_NULL(value)
    Boolean afterContains, beforeContains;
    if (mAfter != NULL && (mAfter->ContainsKey(key, &afterContains), afterContains)) {
        return mAfter->GetAsInteger32(key, value);
    }
    else if (mBefore != NULL && (mBefore->ContainsKey(key, &beforeContains), beforeContains)) {
        return mBefore->GetAsInteger32(key, value);
    }
    else {
        *value = defaultValue;
        REFCOUNT_ADD(*value)
        return NOERROR;
    }
}

ECode ValuesDelta::IsChanged(
    /* [in] */ const String& key,
    /* [out] */ Boolean* isChanged)
{
    VALIDATE_NOT_NULL(isChanged)
    Boolean afterContains;
    if (mAfter == NULL || (mAfter->ContainsKey(key, &afterContains), afterContains)) {
        *isChanged = FALSE;
    }

    AutoPtr<IInterface> newValue;
    mAfter->Get(key, (IInterface**)&newValue);
    AutoPtr<IInterface> oldValue;
    mBefore->Get(key, (IInterface**)&oldValue);

    if (oldValue == NULL) {
        *isChanged = newValue != NULL;
        return NOERROR;
    }

    Boolean equals;
    IObject::Probe(oldValue)->Equals(newValue, &equals);
    *isChanged = !equals;
    return NOERROR;
}

ECode ValuesDelta::GetMimetype(
    /* [out] */ String* mimetype)
{
    VALIDATE_NOT_NULL(mimetype)
    return GetAsString(IContactsContractDataColumns::MIMETYPE, mimetype);
}

ECode ValuesDelta::GetId(
    /* [out] */ IInteger64** id)
{
    VALIDATE_NOT_NULL(id)
    return GetAsInt64(mIdColumn, id);
}

ECode ValuesDelta::SetIdColumn(
    /* [in] */ const String& idColumn)
{
    mIdColumn = idColumn;
    return NOERROR;
}

ECode ValuesDelta::IsPrimary(
    /* [out] */ Boolean* isPrimary)
{
    VALIDATE_NOT_NULL(isPrimary)
    AutoPtr<IInteger64> primary;
    GetAsInt64(IContactsContractDataColumns::IS_PRIMARY, (IInteger64**)&primary);
    Int64 value;
    *isPrimary = primary == NULL ? FALSE : (primary->GetValue(&value), value != 0);
    return NOERROR;
}

ECode ValuesDelta::SetFromTemplate(
    /* [in] */ Boolean isFromTemplate)
{
    mFromTemplate = isFromTemplate;
    return NOERROR;
}

ECode ValuesDelta::IsFromTemplate(
    /* [out] */ Boolean* isFromTemplate)
{
    VALIDATE_NOT_NULL(isFromTemplate)
    *isFromTemplate = mFromTemplate;
    return NOERROR;
}

ECode ValuesDelta::IsSuperPrimary(
    /* [out] */ Boolean* _isSuperPrimary)
{
    VALIDATE_NOT_NULL(_isSuperPrimary)
    AutoPtr<IInteger64> isSuperPrimary;
    GetAsInt64(IContactsContractDataColumns::IS_SUPER_PRIMARY, (IInteger64**)&isSuperPrimary);
    Int64 value;
    *_isSuperPrimary = isSuperPrimary == NULL ? FALSE : (isSuperPrimary->GetValue(&value), value != 0);
    return NOERROR;
}

ECode ValuesDelta::BeforeExists(
    /* [out] */ Boolean* isBefore)
{
    VALIDATE_NOT_NULL(isBefore)
    Boolean contains;
    *isBefore = (mBefore != NULL && (mBefore->ContainsKey(mIdColumn, &contains), contains));
    return NOERROR;
}

ECode ValuesDelta::IsVisible(
    /* [out] */ Boolean* isVisible)
{
    VALIDATE_NOT_NULL(isVisible)
    *isVisible = (mAfter != NULL);
    return NOERROR;
}

ECode ValuesDelta::IsDelete(
    /* [out] */ Boolean* isDelete)
{
    VALIDATE_NOT_NULL(isDelete)
    Boolean isBefore;
    *isDelete = (BeforeExists(&isBefore), isBefore) && (mAfter == NULL);
    return NOERROR;
}

ECode ValuesDelta::IsTransient(
    /* [out] */ Boolean* isTransient)
{
    VALIDATE_NOT_NULL(isTransient)
    *isTransient = (mBefore == NULL) && (mAfter == NULL);
    return NOERROR;
}

ECode ValuesDelta::IsUpdate(
    /* [out] */ Boolean* isUpdate)
{
    VALIDATE_NOT_NULL(isUpdate)
    Boolean isBefore;
    Int32 size;
    if ((BeforeExists(&isBefore), !isBefore) || mAfter == NULL || (mAfter->GetSize(&size), size == 0)) {
        *isUpdate = FALSE;
    }
    AutoPtr<ISet> keySet;
    mAfter->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        String key;
        ICharSequence::Probe(next)->ToString(&key);
        AutoPtr<IInterface> newValue;
        mAfter->Get(key, (IInterface**)&newValue);
        AutoPtr<IInterface> oldValue;
        mBefore->Get(key, (IInterface**)&oldValue);
        Boolean equals;
        if (oldValue == NULL) {
            if (newValue != NULL) {
                *isUpdate = TRUE;
                return NOERROR;
            }
        }
        else if (IObject::Probe(oldValue)->Equals(newValue, &equals), !equals) {
            *isUpdate = TRUE;
            return NOERROR;
        }
    }
    *isUpdate = FALSE;
    return NOERROR;
}

ECode ValuesDelta::IsNoop(
    /* [out] */ Boolean* isNoop)
{
    VALIDATE_NOT_NULL(isNoop)
    Boolean isBefore;
    Int32 size;
    *isNoop = (BeforeExists(&isBefore), isBefore) && (mAfter != NULL && (mAfter->GetSize(&size), size == 0));
    return NOERROR;
}

ECode ValuesDelta::IsInsert(
    /* [out] */ Boolean* isInsert)
{
    VALIDATE_NOT_NULL(isInsert)
    Boolean isBefore;
    *isInsert = (BeforeExists(&isBefore), !isBefore) && (mAfter != NULL);
    return NOERROR;
}

ECode ValuesDelta::MarkDeleted()
{
    mAfter = NULL;
    return NOERROR;
}

void ValuesDelta::EnsureUpdate()
{
    if (mAfter == NULL) {
        CContentValues::New((IContentValues**)&mAfter);
    }
}

ECode ValuesDelta::Put(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    EnsureUpdate();
    return mAfter->Put(key, value);
}

ECode ValuesDelta::Put(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* value)
{
    EnsureUpdate();
    return mAfter->Put(key, value);
}

ECode ValuesDelta::Put(
    /* [in] */ const String& key,
    /* [in] */ Int32 value)
{
    EnsureUpdate();
    return mAfter->Put(key, value);
}

ECode ValuesDelta::Put(
    /* [in] */ const String& key,
    /* [in] */ Int64 value)
{
    EnsureUpdate();
    return mAfter->Put(key, value);
}

ECode ValuesDelta::PutNull(
    /* [in] */ const String& key)
{
    EnsureUpdate();
    return mAfter->PutNull(key);
}

ECode ValuesDelta::CopyStringFrom(
    /* [in] */ IValuesDelta* from,
    /* [in] */ const String& key)
{
    EnsureUpdate();
    String str;
    from->GetAsString(key, &str);
    return Put(key, str);
}

ECode ValuesDelta::GetKeySet(
    /* [out] */ ISet** set)
{
    VALIDATE_NOT_NULL(set)
    AutoPtr<ISet> keys;
    CHashSet::New((ISet**)&keys);

    if (mBefore != NULL) {
        AutoPtr<ISet> valueSet;
        mBefore->GetValueSet((ISet**)&valueSet);
        AutoPtr<IIterator> it;
        valueSet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            keys->Add(next);
        }
    }

    if (mAfter != NULL) {
        AutoPtr<ISet> valueSet;
        mAfter->GetValueSet((ISet**)&valueSet);
        AutoPtr<IIterator> it;
        valueSet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            keys->Add(next);
        }
    }

    *set = keys;
    REFCOUNT_ADD(*set)
    return NOERROR;
}

ECode ValuesDelta::GetCompleteValues(
    /* [out] */ IContentValues** _values)
{
    VALIDATE_NOT_NULL(_values)
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    if (mBefore != NULL) {
        values->PutAll(mBefore);
    }
    if (mAfter != NULL) {
        values->PutAll(mAfter);
    }
    Boolean contains;
    if (values->ContainsKey(IContactsContractCommonDataKindsGroupMembership::GROUP_ROW_ID, &contains), contains) {
        // Clear to avoid double-definitions, and prefer rows
        values->Remove(IContactsContractCommonDataKindsGroupMembership::GROUP_SOURCE_ID);
    }

    *_values = values;
    REFCOUNT_ADD(*_values)
    return NOERROR;
}

AutoPtr<IValuesDelta> ValuesDelta::MergeAfter(
    /* [in] */ IValuesDelta* inLocal,
    /* [in] */ IValuesDelta* remote)
{
    AutoPtr<IValuesDelta> local = inLocal;
    // Bail early if trying to merge delete with missing local
    Boolean isDelete, isTransient;
    if (local == NULL && ((remote->IsDelete(&isDelete), isDelete) || (remote->IsTransient(&isTransient), isTransient))) return NULL;

    // Create local version if none exists yet
    if (local == NULL) {
        CValuesDelta::New((IValuesDelta**)&local);
    }

    Boolean isBefore;
    if (local->BeforeExists(&isBefore), !isBefore) {
        // Any "before" record is missing, so take all values as "insert"
        AutoPtr<IContentValues> temp;
        remote->GetCompleteValues((IContentValues**)&temp);
        local->SetAfter(temp);
    }
    else {
        // Existing "update" with only "after" values
        AutoPtr<IContentValues> temp;
        remote->GetAfter((IContentValues**)&temp);
        local->SetAfter(temp);
    }

    return local;
}

ECode ValuesDelta::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    AutoPtr<IValuesDelta> other = IValuesDelta::Probe(object);
    if (other != NULL) {
        // Only exactly equal with both are identical subsets
        Boolean equals1, equals2;
        *result = (SubsetEquals(other, &equals1), equals1) && (other->SubsetEquals(IValuesDelta::Probe(this), &equals2), equals2);
    }
    return NOERROR;
}

ECode ValuesDelta::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder builder;
    builder.Append("{ ");
    builder.Append("IdColumn=");
    builder.Append(mIdColumn);
    builder.Append(", FromTemplate=");
    builder.Append(mFromTemplate);
    builder.Append(", ");
    AutoPtr<ISet> keySet;
    GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        String key;
        ICharSequence::Probe(value)->ToString(&key);
        builder.Append(key);
        builder.Append("=");
        String s;
        GetAsString(key, &s);
        builder.Append(s);
        builder.Append(", ");
    }
    builder.Append("}");
    *str = builder.ToString();
    return NOERROR;
}

ECode ValuesDelta::SubsetEquals(
    /* [in] */ IValuesDelta* other,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals)
    AutoPtr<ISet> keySet;
    GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        String key;
        ICharSequence::Probe(value)->ToString(&key);
        String ourValue;
        GetAsString(key, &ourValue);
        String theirValue;
        other->GetAsString(key, &theirValue);
        if (ourValue.IsNull()) {
            // If they have value when we're null, no match
            if (!theirValue.IsNull()) {
                *equals = FALSE;
                return NOERROR;
            }
        }
        else {
            // If both values defined and aren't equal, no match
            if (!ourValue.Equals(theirValue)) {
                *equals = FALSE;
                return NOERROR;
            }
        }
    }
    // All values compared and matched
    *equals = TRUE;
    return NOERROR;
}

ECode ValuesDelta::BuildDiff(
    /* [in] */ IUri* targetUri,
    /* [out] */ IContentProviderOperationBuilder** _builder)
{
    VALIDATE_NOT_NULL(_builder)
    AutoPtr<IContentProviderOperationBuilder> builder;
    Boolean isInsert, isDelete, isUpdate;
    if (IsInsert(&isInsert), isInsert) {
        // Changed values are "insert" back-referenced to Contact
        mAfter->Remove(mIdColumn);
        AutoPtr<IContentProviderOperationHelper> helper;
        CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&helper);
        helper->NewInsert(targetUri, (IContentProviderOperationBuilder**)&builder);
        builder->WithValues(mAfter);
    }
    else if (IsDelete(&isDelete), isDelete) {
        // When marked for deletion and "before" exists, then "delete"
        AutoPtr<IContentProviderOperationHelper> helper;
        CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&helper);
        helper->NewInsert(targetUri, (IContentProviderOperationBuilder**)&builder);
        AutoPtr<IInteger64> id;
        GetId((IInteger64**)&id);
        Int64 value;
        id->GetValue(&value);
        builder->WithSelection(mIdColumn + "=" + StringUtils::ToString(value), NULL);
    }
    else if (IsUpdate(&isUpdate), isUpdate) {
        // When has changes and "before" exists, then "update"

        AutoPtr<IContentProviderOperationHelper> helper;
        CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&helper);
        helper->NewInsert(targetUri, (IContentProviderOperationBuilder**)&builder);
        AutoPtr<IInteger64> id;
        GetId((IInteger64**)&id);
        Int64 value;
        id->GetValue(&value);
        builder->WithSelection(mIdColumn + "=" + StringUtils::ToString(value), NULL);
        builder->WithValues(mAfter);
    }
    *_builder = builder;
    REFCOUNT_ADD(*_builder)
    return NOERROR;
}

ECode ValuesDelta::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mBefore);
    dest->WriteInterfacePtr(mAfter);
    dest->WriteString(mIdColumn);
    return NOERROR;
}

ECode ValuesDelta::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> before;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&before);
    mBefore = IContentValues::Probe(before);
    AutoPtr<IInterface> after;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&after);
    mAfter = IContentValues::Probe(after);
    source->ReadString(&mIdColumn);
    return NOERROR;
}

ECode ValuesDelta::SetGroupRowId(
    /* [in] */ Int64 groupId)
{
    return Put(IContactsContractCommonDataKindsGroupMembership::GROUP_ROW_ID, groupId);
}

ECode ValuesDelta::GetGroupRowId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    AutoPtr<IInteger64> integer;
    GetAsInt64(IContactsContractCommonDataKindsGroupMembership::GROUP_ROW_ID, (IInteger64**)&integer);
    return integer->GetValue(id);
}

ECode ValuesDelta::SetPhoto(
    /* [in] */ ArrayOf<Byte>* value)
{
    return Put(IContactsContractCommonDataKindsPhoto::PHOTO, value);
}

ECode ValuesDelta::GetPhoto(
    /* [out, callee] */ ArrayOf<Byte>** photo)
{
    VALIDATE_NOT_NULL(photo)
    return GetAsByteArray(IContactsContractCommonDataKindsPhoto::PHOTO, photo);
}

ECode ValuesDelta::SetSuperPrimary(
    /* [in] */ Boolean val)
{
    if (val) {
        Put(IContactsContractDataColumns::IS_SUPER_PRIMARY, 1);
    }
    else {
        Put(IContactsContractDataColumns::IS_SUPER_PRIMARY, 0);
    }
    return NOERROR;
}

ECode ValuesDelta::SetPhoneticFamilyName(
    /* [in] */ const String& value)
{
    return Put(IContactsContractCommonDataKindsStructuredName::PHONETIC_FAMILY_NAME, value);
}

ECode ValuesDelta::SetPhoneticMiddleName(
    /* [in] */ const String& value)
{
    return Put(IContactsContractCommonDataKindsStructuredName::PHONETIC_MIDDLE_NAME, value);
}

ECode ValuesDelta::SetPhoneticGivenName(
    /* [in] */ const String& value)
{
    return Put(IContactsContractCommonDataKindsStructuredName::PHONETIC_GIVEN_NAME, value);
}

ECode ValuesDelta::GetPhoneticFamilyName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    return GetAsString(IContactsContractCommonDataKindsStructuredName::PHONETIC_FAMILY_NAME, name);
}

ECode ValuesDelta::GetPhoneticMiddleName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    return GetAsString(IContactsContractCommonDataKindsStructuredName::PHONETIC_MIDDLE_NAME, name);
}

ECode ValuesDelta::GetPhoneticGivenName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    return GetAsString(IContactsContractCommonDataKindsStructuredName::PHONETIC_GIVEN_NAME, name);
}

ECode ValuesDelta::GetDisplayName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    return GetAsString(IContactsContractCommonDataKindsStructuredName::DISPLAY_NAME, name);
}

ECode ValuesDelta::SetDisplayName(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        PutNull(IContactsContractCommonDataKindsStructuredName::DISPLAY_NAME);
    }
    else {
        Put(IContactsContractCommonDataKindsStructuredName::DISPLAY_NAME, name);
    }
    return NOERROR;
}

ECode ValuesDelta::CopyStructuredNameFieldsFrom(
    /* [in] */ IValuesDelta* name)
{
    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::DISPLAY_NAME);

    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::GIVEN_NAME);
    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::FAMILY_NAME);
    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::PREFIX);
    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::MIDDLE_NAME);
    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::SUFFIX);

    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::PHONETIC_GIVEN_NAME);
    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::PHONETIC_MIDDLE_NAME);
    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::PHONETIC_FAMILY_NAME);

    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::FULL_NAME_STYLE);
    CopyStringFrom(name, IContactsContractCommonDataKindsStructuredName::PHONETIC_NAME_STYLE);
    return NOERROR;
}

ECode ValuesDelta::GetPhoneNumber(
    /* [out] */ String* number)
{
    VALIDATE_NOT_NULL(number)
    return GetAsString(IContactsContractCommonDataKindsPhone::NUMBER, number);
}

ECode ValuesDelta::GetPhoneNormalizedNumber(
    /* [out] */ String* number)
{
    VALIDATE_NOT_NULL(number)
    return GetAsString(IContactsContractCommonDataKindsPhone::NORMALIZED_NUMBER, number);
}

ECode ValuesDelta::PhoneHasType(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return ContainsKey(IContactsContractCommonDataKindsCommonColumns::TYPE, result);
}

ECode ValuesDelta::GetPhoneType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    AutoPtr<IInteger32> integer;
    GetAsInteger(IContactsContractCommonDataKindsCommonColumns::TYPE, (IInteger32**)&integer);
    return integer->GetValue(type);
}

ECode ValuesDelta::GetPhoneLabel(
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label)
    return GetAsString(IContactsContractCommonDataKindsCommonColumns::LABEL, label);
}

ECode ValuesDelta::GetEmailData(
    /* [out] */ String* data)
{
    VALIDATE_NOT_NULL(data)
    return GetAsString(IContactsContractCommonDataKindsCommonColumns::DATA, data);
}

ECode ValuesDelta::EmailHasType(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return ContainsKey(IContactsContractCommonDataKindsCommonColumns::TYPE, result);
}

ECode ValuesDelta::GetEmailType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    AutoPtr<IInteger32> integer;
    GetAsInteger(IContactsContractCommonDataKindsCommonColumns::TYPE, (IInteger32**)&integer);
    return integer->GetValue(type);
}

ECode ValuesDelta::GetEmailLabel(
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label)
    return GetAsString(IContactsContractCommonDataKindsCommonColumns::LABEL, label);
}

} // Model
} // Common
} // Contacts
} // Droid
} // Elastos
