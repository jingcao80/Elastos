
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_MODEL_VALUESDELTA_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_MODEL_VALUESDELTA_H__

#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContentProviderOperationBuilder;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Model {

/**
 * Type of {@link android.content.ContentValues} that maintains both an original state and a
 * modified version of that state. This allows us to build insert, update,
 * or delete operations based on a "before" {@link Entity} snapshot.
 */
class ValuesDelta
    : public Object
    , public IValuesDelta
    , public IParcelable
{
public:
    ValuesDelta();

    virtual ~ValuesDelta() {}

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Create {@link ValuesDelta}, using the given object as the
     * "before" state, usually from an {@link Entity}.
     */
    static CARAPI_(AutoPtr<IValuesDelta>) FromBefore(
        /* [in] */ IContentValues* before);

    /**
     * Create {@link ValuesDelta}, using the given object as the "after"
     * state, usually when we are inserting a row instead of updating.
     */
    static CARAPI_(AutoPtr<IValuesDelta>) FromAfter(
        /* [in] */ IContentValues* after);

    // @NeededForTesting
    CARAPI GetAfter(
        /* [out] */ IContentValues** after);

    CARAPI SetAfter(
        /* [in] */ IContentValues* after);

    CARAPI ContainsKey(
        /* [in] */ const String& key,
        /* [out] */ Boolean* contains);

    CARAPI GetAsString(
        /* [in] */ const String& key,
        /* [out] */ String* str);

    CARAPI GetAsByteArray(
        /* [in] */ const String& key,
        /* [out, callee] */ ArrayOf<Byte>** byteArray);

    CARAPI GetAsInt64(
        /* [in] */ const String& key,
        /* [out] */ IInteger64** value);

    CARAPI GetAsInteger(
        /* [in] */ const String& key,
        /* [out] */ IInteger32** value);

    CARAPI GetAsInteger(
        /* [in] */ const String& key,
        /* [in] */ IInteger32* defaultValue,
        /* [out] */ IInteger32** value);

    CARAPI IsChanged(
        /* [in] */ const String& key,
        /* [out] */ Boolean* isChanged);

    CARAPI GetMimetype(
        /* [out] */ String* mimetype);

    CARAPI GetId(
        /* [out] */ IInteger64** id);

    CARAPI SetIdColumn(
        /* [in] */ const String& idColumn);

    CARAPI IsPrimary(
        /* [out] */ Boolean* isPrimary);

    CARAPI SetFromTemplate(
        /* [in] */ Boolean isFromTemplate);

    CARAPI IsFromTemplate(
        /* [out] */ Boolean* isFromTemplate);

    CARAPI IsSuperPrimary(
        /* [out] */ Boolean* isSuperPrimary);

    CARAPI BeforeExists(
        /* [out] */ Boolean* isBefore);

    /**
     * When "after" is present, then visible
     */
    CARAPI IsVisible(
        /* [out] */ Boolean* isVisible);

    /**
     * When "after" is wiped, action is "delete"
     */
    CARAPI IsDelete(
        /* [out] */ Boolean* isDelete);

    /**
     * When no "before" or "after", is transient
     */
    CARAPI IsTransient(
        /* [out] */ Boolean* isTransient);

    /**
     * When "after" has some changes, action is "update"
     */
    CARAPI IsUpdate(
        /* [out] */ Boolean* isUpdate);

    /**
     * When "after" has no changes, action is no-op
     */
    CARAPI IsNoop(
        /* [out] */ Boolean* isNoop);

    /**
     * When no "before" id, and has "after", action is "insert"
     */
    CARAPI IsInsert(
        /* [out] */ Boolean* isInsert);

    CARAPI MarkDeleted();

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ Int32 value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ Int64 value);

    CARAPI PutNull(
        /* [in] */ const String& key);

    CARAPI CopyStringFrom(
        /* [in] */ IValuesDelta* from,
        /* [in] */ const String& key);

    /**
     * Return set of all keys defined through this object.
     */
    CARAPI GetKeySet(
        /* [out] */ ISet** set);

    /**
     * Return complete set of "before" and "after" values mixed together,
     * giving full state regardless of edits.
     */
    CARAPI GetCompleteValues(
        /* [out] */ IContentValues** values);

    /**
     * Merge the "after" values from the given {@link ValuesDelta},
     * discarding any existing "after" state. This is typically used when
     * re-parenting changes onto an updated {@link Entity}.
     */
    static CARAPI_(AutoPtr<IValuesDelta>) MergeAfter(
        /* [in] */ IValuesDelta* local,
        /* [in] */ IValuesDelta* remote);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

    /**
     * Check if the given {@link ValuesDelta} is both a subset of this
     * object, and any defined keys have equal values.
     */
    CARAPI SubsetEquals(
        /* [in] */ IValuesDelta* other,
        /* [out] */ Boolean* equals);

    /**
     * Build a {@link android.content.ContentProviderOperation} that will transform our
     * "before" state into our "after" state, using insert, update, or
     * delete as needed.
     */
    CARAPI BuildDiff(
        /* [in] */ IUri* targetUri,
        /* [out] */ IContentProviderOperationBuilder** builder);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI SetGroupRowId(
        /* [in] */ Int64 groupId);

    CARAPI GetGroupRowId(
        /* [out] */ Int64* id);

    CARAPI SetPhoto(
        /* [in] */ ArrayOf<Byte>* value);

    CARAPI GetPhoto(
        /* [out, callee] */ ArrayOf<Byte>** photo);

    CARAPI SetSuperPrimary(
        /* [in] */ Boolean val);

    CARAPI SetPhoneticFamilyName(
        /* [in] */ const String& value);

    CARAPI SetPhoneticMiddleName(
        /* [in] */ const String& value);

    CARAPI SetPhoneticGivenName(
        /* [in] */ const String& value);

    CARAPI GetPhoneticFamilyName(
        /* [out] */ String* name);

    CARAPI GetPhoneticMiddleName(
        /* [out] */ String* name);

    CARAPI GetPhoneticGivenName(
        /* [out] */ String* name);

    CARAPI GetDisplayName(
        /* [out] */ String* name);

    CARAPI SetDisplayName(
        /* [in] */ const String& name);

    CARAPI CopyStructuredNameFieldsFrom(
        /* [in] */ IValuesDelta* name);

    CARAPI GetPhoneNumber(
        /* [out] */ String* number);

    CARAPI GetPhoneNormalizedNumber(
        /* [out] */ String* number);

    CARAPI PhoneHasType(
        /* [out] */ Boolean* result);

    CARAPI GetPhoneType(
        /* [out] */ Int32* type);

    CARAPI GetPhoneLabel(
        /* [out] */ String* label);

    CARAPI GetEmailData(
        /* [out] */ String* data);

    CARAPI EmailHasType(
        /* [out] */ Boolean* result);

    CARAPI GetEmailType(
        /* [out] */ Int32* type);

    CARAPI GetEmailLabel(
        /* [out] */ String* label);

private:
    /**
     * Ensure that our internal structure is ready for storing updates.
     */
    CARAPI_(void) EnsureUpdate();

protected:
    AutoPtr<IContentValues> mBefore;
    AutoPtr<IContentValues> mAfter;
    String mIdColumn;
    Boolean mFromTemplate;

    /**
     * Next value to assign to {@link #mIdColumn} when building an insert
     * operation through {@link #fromAfter(android.content.ContentValues)}. This is used so
     * we can concretely reference this {@link ValuesDelta} before it has
     * been persisted.
     */
    static Int32 sNextInsertId;
};

} // Model
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_MODEL_VALUESDELTA_H__
