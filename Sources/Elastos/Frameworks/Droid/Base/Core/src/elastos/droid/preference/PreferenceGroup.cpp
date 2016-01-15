
#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/preference/PreferenceGroup.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::R;
using Elastos::Core::AutoLock;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL_2(PreferenceGroup, Preference, IGenericInflaterParent, IPreferenceGroup)

PreferenceGroup::PreferenceGroup()
    : mOrderingAsAdded(TRUE)
    , mCurrentPreferenceOrder(0)
    , mAttachedToActivity(FALSE)
{
}

ECode PreferenceGroup::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(Preference::constructor(context, attrs, defStyleAttr, defStyleRes));
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::PreferenceGroup),
            ArraySize(R::styleable::PreferenceGroup));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    a->GetBoolean(R::styleable::PreferenceGroup_orderingFromXml,
            mOrderingAsAdded, &mOrderingAsAdded);
    a->Recycle();
    return NOERROR;
}

ECode PreferenceGroup::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode PreferenceGroup::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0, 0);
}

ECode PreferenceGroup::SetOrderingAsAdded(
    /* [in] */ Boolean orderingAsAdded)
{
    mOrderingAsAdded = orderingAsAdded;
    return NOERROR;
}

ECode PreferenceGroup::IsOrderingAsAdded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOrderingAsAdded;
    return NOERROR;
}

ECode PreferenceGroup::AddItemFromInflater(
    /* [in] */ IInterface* child)
{
    Boolean result;
    AutoPtr<IPreference> preference = IPreference::Probe(child);
    return AddPreference(preference, &result);
}

ECode PreferenceGroup::GetPreferenceCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mPreferenceList->GetSize(count);
}

ECode PreferenceGroup::GetPreference(
    /* [in] */ Int32 index,
    /* [out] */ IPreference** preference)
{
    VALIDATE_NOT_NULL(preference)
    return mPreferenceList->Get(index, (IInterface**)preference);
}

ECode PreferenceGroup::AddPreference(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mPreferenceList->Contains(preference, result), *result) {
        // Exists
        return NOERROR;
    }

    Int32 order;
    if (preference->GetOrder((Int32*)&order), order == IPreference::DEFAULT_ORDER) {
        if (mOrderingAsAdded) {
            preference->SetOrder(mCurrentPreferenceOrder++);
        }

        AutoPtr<IPreferenceGroup> group;
        if(IPreferenceGroup::Probe(preference) != NULL){
            // TODO: fix (method is called tail recursively when inflating,
            // so we won't end up properly passing this flag down to children
            AutoPtr<IPreferenceGroup> group = IPreferenceGroup::Probe(preference);
            group->SetOrderingAsAdded(mOrderingAsAdded);
        }

    }

    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    Int32 insertionIndex;
    coll->BinarySearch(mPreferenceList, preference, &insertionIndex);
    if (insertionIndex < 0) {
        insertionIndex = insertionIndex * -1 - 1;
    }

    Boolean value;
    if (OnPrepareAddPreference(preference, &value), !value) {
        *result = FALSE;
        return NOERROR;
    }

    synchronized(this) {
        mPreferenceList->Add(insertionIndex, preference);
    }

    AutoPtr<IPreferenceManager> preferencemanager;
    GetPreferenceManager((IPreferenceManager**)&preferencemanager);
    preference->OnAttachedToHierarchy(preferencemanager);

    if (mAttachedToActivity) {
        preference->OnAttachedToActivity();
    }

    NotifyHierarchyChanged();
    *result = TRUE;
    return NOERROR;
}

ECode PreferenceGroup::RemovePreference(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean returnValue = RemovePreferenceInt(preference);
    NotifyHierarchyChanged();
    *result = returnValue;
    return NOERROR;
}

Boolean PreferenceGroup::RemovePreferenceInt(
    /* [in] */ IPreference* preference)
{
    Boolean modified;
    synchronized(this) {
        preference->OnPrepareForRemoval();
        mPreferenceList->Remove(preference, &modified);
    }
    return modified;
}

ECode PreferenceGroup::RemoveAll()
{
    synchronized(this) {
        AutoPtr<IList> preferenceList = mPreferenceList;
        Int32 i;
        preferenceList->GetSize(&i);
        for (i = i - 1; i >= 0; i--) {
            AutoPtr<IPreference> preference;
            preferenceList->Get(0, (IInterface**)&preference);
            RemovePreferenceInt(preference);
        }
    }
    NotifyHierarchyChanged();
    return NOERROR;
}

ECode PreferenceGroup::OnPrepareAddPreference(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean enable = FALSE;
    ShouldDisableDependents(&enable);
    preference->OnParentChanged(this, enable);
    *result = TRUE;
    return NOERROR;
}

ECode PreferenceGroup::FindPreference(
    /* [in] */ ICharSequence* key,
    /* [out] */ IPreference** preferencevalue)
{
    VALIDATE_NOT_NULL(preferencevalue)

    String keyStr1, keyStr2;
    GetKey(&keyStr1);
    key->ToString(&keyStr2);
    if (TextUtils::Equals(keyStr1, keyStr2)) {
        *preferencevalue = this;
        REFCOUNT_ADD(*preferencevalue)
        return NOERROR;
    }
    Int32 preferenceCount;
    GetPreferenceCount(&preferenceCount);
    for (Int32 i = 0; i < preferenceCount; i++) {
        AutoPtr<IPreference> preference;
        GetPreference(i, (IPreference**)&preference);
        String curKey;
        preference->GetKey(&curKey);

        if (!curKey.IsNull() && curKey.Equals(keyStr2)) {
            *preferencevalue = preference;
            REFCOUNT_ADD(*preferencevalue)
            return NOERROR;
        }

        if (IPreferenceGroup::Probe(preference) != NULL) {
            AutoPtr<IPreferenceGroup> group = IPreferenceGroup::Probe(preference);
            AutoPtr<IPreference> returnedPreference;
            group->FindPreference(key, (IPreference**)&returnedPreference);
            if (returnedPreference != NULL) {
                *preferencevalue = returnedPreference;
                REFCOUNT_ADD(*preferencevalue)
                return NOERROR;
            }
        }
    }

    *preferencevalue = NULL;
    return NOERROR;
}

ECode PreferenceGroup::IsOnSameScreenAsChildren(
    /* [out] */ Boolean* isOnSameScreenAsChildren)
{
    VALIDATE_NOT_NULL(isOnSameScreenAsChildren)
    *isOnSameScreenAsChildren = TRUE;
    return NOERROR;
}

ECode PreferenceGroup::OnAttachedToActivity()
{
    FAIL_RETURN(Preference::OnAttachedToActivity())

    // Mark as attached so if a preference is later added to this group, we
    // can tell it we are already attached
    mAttachedToActivity = TRUE;

    // Dispatch to all contained preferences
    Int32 preferenceCount;
    GetPreferenceCount(&preferenceCount);
    for (Int32 i = 0; i < preferenceCount; i++) {
        AutoPtr<IPreference> p;
        GetPreference(i, (IPreference**)&p);
        p->OnAttachedToActivity();
    }
    return NOERROR;
}

ECode PreferenceGroup::OnPrepareForRemoval()
{
    FAIL_RETURN(Preference::OnPrepareForRemoval())

    // We won't be attached to the activity anymore
    mAttachedToActivity = FALSE;
    return NOERROR;
}

ECode PreferenceGroup::NotifyDependencyChange(
    /* [in] */ Boolean disableDependents)
{
    FAIL_RETURN(Preference::NotifyDependencyChange(disableDependents));

    // Child preferences have an implicit dependency on their containing
    // group. Dispatch dependency change to all contained preferences.
    Int32 preferenceCount;
    GetPreferenceCount(&preferenceCount);
    for (Int32 i = 0; i < preferenceCount; i++) {
        AutoPtr<IPreference> p;
        GetPreference(i, (IPreference**)&p);
        p->OnParentChanged(IPreference::Probe(this), disableDependents);
    }
    return NOERROR;
}

ECode PreferenceGroup::SortPreferences()
{
    synchronized(this) {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        coll->Sort(mPreferenceList);
    }
    return NOERROR;
}

ECode PreferenceGroup::DispatchSaveInstanceState(
    /* [in] */ IBundle* container)
{
    FAIL_RETURN(Preference::DispatchSaveInstanceState(container))

    // Dispatch to all contained preferences
    Int32 preferenceCount;
    GetPreferenceCount(&preferenceCount);
    for (Int32 i = 0; i < preferenceCount; i++) {
        AutoPtr<IPreference> p;
        GetPreference(i, (IPreference**)&p);
        p->DispatchSaveInstanceState(container);
    }
    return NOERROR;
}

ECode PreferenceGroup::DispatchRestoreInstanceState(
    /* [in] */ IBundle* container)
{
    FAIL_RETURN(Preference::DispatchRestoreInstanceState(container))

    // Dispatch to all contained preferences
    Int32 preferenceCount;
    GetPreferenceCount(&preferenceCount);
    for (Int32 i = 0; i < preferenceCount; i++) {
        AutoPtr<IPreference> p;
        GetPreference(i, (IPreference**)&p);
        p->DispatchRestoreInstanceState(container);
    }
    return NOERROR;
}

} // namespace Elastos
} // namespace Droid
} // namespace Preference
