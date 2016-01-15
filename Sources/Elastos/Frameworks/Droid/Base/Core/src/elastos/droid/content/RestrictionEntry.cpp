#include "elastos/droid/content/RestrictionEntry.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(RestrictionEntry, Object, IRestrictionEntry, IParcelable)

RestrictionEntry::RestrictionEntry()
    : mType(0)
{
}

RestrictionEntry::~RestrictionEntry()
{}

ECode RestrictionEntry::constructor()
{
    return NOERROR;
}

ECode RestrictionEntry::constructor(
    /* [in] */ Int32 type,
    /* [in] */ const String& key)
{
    mType = type;
    mKey = key;
    return NOERROR;
}

ECode RestrictionEntry::constructor(
    /* [in] */ const String& key,
    /* [in] */ const String& selectedString)
{
    mKey = key;
    mType = IRestrictionEntry::TYPE_CHOICE;
    mCurrentValue = selectedString;
    return NOERROR;
}

ECode RestrictionEntry::constructor(
    /* [in] */ const String& key,
    /* [in] */ Boolean selectedState)
{
    mKey = key;
    mType = IRestrictionEntry::TYPE_BOOLEAN;
    SetSelectedState(selectedState);
    return NOERROR;
}

ECode RestrictionEntry::constructor(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<String>* selectedStrings)
{
    mKey = key;
    mType = IRestrictionEntry::TYPE_MULTI_SELECT;
    mCurrentValues = selectedStrings;
    return NOERROR;
}

ECode RestrictionEntry::constructor(
    /* [in] */ const String& key,
    /* [in] */ Int32 selectedInt)
{
    mKey = key;
    mType = IRestrictionEntry::TYPE_INTEGER;
    SetInt32Value(selectedInt);
    return NOERROR;
}

ECode RestrictionEntry::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode RestrictionEntry::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode RestrictionEntry::GetSelectedString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mCurrentValue;
    return NOERROR;
}

ECode RestrictionEntry::GetAllSelectedStrings(
    /* [out, callee] */ ArrayOf<String>** strs)
{
    VALIDATE_NOT_NULL(strs)
    *strs = mCurrentValues;
    REFCOUNT_ADD(*strs)
    return NOERROR;
}

ECode RestrictionEntry::GetSelectedState(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state)
    *state = mCurrentValue.Equals("TRUE") || mCurrentValue.Equals("true");
    return NOERROR;
}

ECode RestrictionEntry::GetInt32Value(
    /* [out] */ Int32* value)
{
    return StringUtils::Parse(mCurrentValue, value);
}

ECode RestrictionEntry::SetInt32Value(
    /* [in] */ Int32 value)
{
    mCurrentValue = StringUtils::ToString(value);
    return NOERROR;
}

ECode RestrictionEntry::SetSelectedString(
    /* [in] */ const String& selectedString)
{
    mCurrentValue = selectedString;
    return NOERROR;
}

ECode RestrictionEntry::SetSelectedState(
    /* [in] */ Boolean state)
{
    mCurrentValue = StringUtils::BooleanToString(state);
    return NOERROR;
}

ECode RestrictionEntry::SetAllSelectedStrings(
    /* [in] */ ArrayOf<String>* allSelectedStrings)
{
    mCurrentValues = allSelectedStrings;
    return NOERROR;
}

ECode RestrictionEntry::SetChoiceValues(
    /* [in] */ ArrayOf<String>* choiceValues)
{
    mChoiceValues = choiceValues;
    return NOERROR;
}

ECode RestrictionEntry::SetChoiceValues(
    /* [in] */ IContext* context,
    /* [in] */ Int32 stringArrayResId)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    return res->GetStringArray(stringArrayResId, (ArrayOf<String>**)&mChoiceValues);
}

ECode RestrictionEntry::GetChoiceValues(
    /* [out, callee] */ ArrayOf<String>** choiceValues)
{
    VALIDATE_NOT_NULL(choiceValues)
    *choiceValues = mChoiceValues;
    REFCOUNT_ADD(*choiceValues);
    return NOERROR;
}

ECode RestrictionEntry::SetChoiceEntries(
    /* [in] */ ArrayOf<String>* choiceEntries)
{
    mChoiceEntries = choiceEntries;
    return NOERROR;
}

ECode RestrictionEntry::SetChoiceEntries(
    /* [in] */ IContext* context,
    /* [in] */ Int32 stringArrayResId)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    return res->GetStringArray(stringArrayResId, (ArrayOf<String>**)&mChoiceEntries);
}

ECode RestrictionEntry::GetChoiceEntries(
    /* [out, callee] */ ArrayOf<String>** choiceEntries)
{
    VALIDATE_NOT_NULL(choiceEntries)
    *choiceEntries = mChoiceEntries;
    REFCOUNT_ADD(*choiceEntries)
    return NOERROR;
}

ECode RestrictionEntry::GetDescription(
    /* [out] */ String* description)
{
    VALIDATE_NOT_NULL(description)
    *description = mDescription;
    return NOERROR;
}

ECode RestrictionEntry::SetDescription(
    /* [in] */ const String& description)
{
    mDescription = description;
    return NOERROR;
}

ECode RestrictionEntry::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key)
    *key = mKey;
    return NOERROR;
}

ECode RestrictionEntry::GetTitle(
    /* [out] */ String* title)
{
    VALIDATE_NOT_NULL(title)
    *title = mTitle;
    return NOERROR;
}

ECode RestrictionEntry::SetTitle(
    /* [in] */ const String& title)
{
    mTitle = title;
    return NOERROR;
}

ECode RestrictionEntry::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (o == TO_IINTERFACE(this)) {
        *result = TRUE;
        return NOERROR;
    }
    // if (!(o instanceof RestrictionEntry)) return false;
    // final RestrictionEntry other = (RestrictionEntry) o;
    // // Make sure that either currentValue matches or currentValues matches.
    // return mType == other->mType && mKey.equals(other->mKey)
    //         &&
    //         ((mCurrentValues == null && other->mCurrentValues == null
    //           && mCurrentValue != null && mCurrentValue.equals(other->mCurrentValue))
    //          ||
    //          (mCurrentValue == null && other->mCurrentValue == null
    //           && mCurrentValues != null && equalArrays(mCurrentValues, other->mCurrentValues)));
    return NOERROR;
}

ECode RestrictionEntry::GetHashCode(
    /* [out] */ Int32* code)
{
    // int result = 17;
    // result = 31 * result + mKey.hashCode();
    // if (mCurrentValue != null) {
    //     result = 31 * result + mCurrentValue.hashCode();
    // } else if (mCurrentValues != null) {
    //     for (String value : mCurrentValues) {
    //         if (value != null) {
    //             result = 31 * result + value.hashCode();
    //         }
    //     }
    // }
    // return result;
    return NOERROR;
}

ECode RestrictionEntry::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("RestrictionsEntry {type=");;
    sb += mType;
    sb += ", key=";
    sb += mKey;
    sb += ", value=";
    sb += mCurrentValue;
    *str = sb.ToString();
    return NOERROR;
}

ECode RestrictionEntry::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mType);
    dest->WriteString(mKey);
    dest->WriteString(mTitle);
    dest->WriteString(mDescription);
    WriteArray(dest, mChoiceEntries);
    WriteArray(dest, mChoiceValues);
    dest->WriteString(mCurrentValue);
    WriteArray(dest, mCurrentValues);
    return NOERROR;
}

ECode RestrictionEntry::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mType);
    in->ReadString(&mKey);
    in->ReadString(&mTitle);
    in->ReadString(&mDescription);
    mChoiceEntries = ReadArray(in);
    mChoiceValues = ReadArray(in);
    in->ReadString(&mCurrentValue);
    mCurrentValues = ReadArray(in);
    return NOERROR;
}

Boolean RestrictionEntry::EqualArrays(
    /* [in] */ ArrayOf<String>* one,
    /* [in] */ ArrayOf<String>* other)
{
    if (one->GetLength() != other->GetLength()) return FALSE;
    for (Int32 i = 0; i < one->GetLength(); i++) {
        if (!(*one)[i].Equals((*other)[i])) return FALSE;
    }
    return TRUE;
}

AutoPtr<ArrayOf<String> > RestrictionEntry::ReadArray(
    /* [in] */ IParcel* in)
{
    Int32 count;
    in->ReadInt32(&count);
    AutoPtr<ArrayOf<String> > values = ArrayOf<String>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        in->ReadString(&(*values)[i]);
    }
    return values;
}

ECode RestrictionEntry::WriteArray(
    /* [in] */ IParcel* dest,
    /* [in] */ ArrayOf<String>* values)
{
    if (values == NULL) {
        dest->WriteInt32(0);
    } else {
        dest->WriteInt32(values->GetLength());
        for (Int32 i = 0; i < values->GetLength(); i++) {
            dest->WriteString((*values)[i]);
        }
    }
    return NOERROR;
}


} // Content
} // Droid
} // Elastos