
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CCONTACTLISTFILTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CCONTACTLISTFILTER_H__

#include "_Elastos_Droid_Contacts_Common_List_CContactListFilter.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

CarClass(CContactListFilter)
    , public Object
    , public IContactListFilter
    , public IComparable
    , public IParcelable
{
public:
    CContactListFilter()
        : mFilterType(0)
    {}

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 filterType,
        /* [in] */ const String& accountType,
        /* [in] */ const String& accountName,
        /* [in] */ const String& dataSet,
        /* [in] */ IDrawable* icon);

    static CARAPI_(AutoPtr<IContactListFilter>) CreateFilterWithType(
        /* [in] */ Int32 filterType);

    static CARAPI_(AutoPtr<IContactListFilter>) CreateAccountFilter(
        /* [in] */ const String& accountType,
        /* [in] */ const String& accountName,
        /* [in] */ const String& dataSet,
        /* [in] */ IDrawable* icon);

    /**
     * Returns true if this filter is based on data and may become invalid over time.
     */
    CARAPI IsValidationRequired(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI CompareTo(
        /* [in] */ IInterface* that,
        /* [out] */ Int32*  result);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    /**
     * Store the given {@link ContactListFilter} to preferences. If the requested filter is
     * of type {@link #FILTER_TYPE_SINGLE_CONTACT} then do not save it to preferences because
     * it is a temporary state.
     */
    static CARAPI_(void) StoreToPreferences(
        /* [in] */ ISharedPreferences* prefs,
        /* [in] */ IContactListFilter* filter);

    /**
     * Try to obtain ContactListFilter object saved in SharedPreference.
     * If there's no info there, return ALL filter instead.
     */
    static CARAPI_(AutoPtr<IContactListFilter>) RestoreDefaultPreferences(
        /* [in] */ ISharedPreferences* prefs);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Returns a string that can be used as a stable persistent identifier for this filter.
     */
    CARAPI GetId(
        /* [out] */ String* id);

    /**
     * Adds the account query parameters to the given {@code uriBuilder}.
     *
     * @throws IllegalStateException if the filter type is not {@link #FILTER_TYPE_ACCOUNT}.
     */
    CARAPI AddAccountQueryParameterToUrl(
        /* [in] */ IUriBuilder* uriBuilder,
        /* [out] */ IUriBuilder** result);

    CARAPI ToDebugString(
        /* [out] */ String* debugStr);

    static CARAPI_(String) FilterTypeToString(
        /* [in] */ Int32 filterType);

    CARAPI GetFilterType(
        /* [out] */ Int32* filterType);

    CARAPI SetFilterType(
        /* [in] */ Int32 filterType);

    CARAPI GetAccountType(
        /* [out] */ String* accountType);

    CARAPI SetAccountType(
        /* [in] */ const String& accountType);

    CARAPI GetAccountName(
        /* [out] */ String* accountName);

    CARAPI SetAccountName(
        /* [in] */ const String& accountName);

    CARAPI GetDataSet(
        /* [out] */ String* dataSet);

    CARAPI SetDataSet(
        /* [in] */ const String& dataSet);

    CARAPI GetIcon(
        /* [out] */ IDrawable** icon);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

private:
    static CARAPI_(AutoPtr<IContactListFilter>) RestoreFromPreferences(
        /* [in] */ ISharedPreferences* prefs);

public:
    Int32 mFilterType;
    String mAccountType;
    String mAccountName;
    String mDataSet;
    AutoPtr<IDrawable> mIcon;

private:
    /**
     * Obsolete filter which had been used in Honeycomb. This may be stored in
     * {@link SharedPreferences}, but should be replaced with ALL filter when it is found.
     *
     * TODO: "group" filter and relevant variables are all obsolete. Remove them.
     */
    static const Int32 FILTER_TYPE_GROUP = 1;

    static const String KEY_FILTER_TYPE;
    static const String KEY_ACCOUNT_NAME;
    static const String KEY_ACCOUNT_TYPE;
    static const String KEY_DATA_SET;

    String mId;
};

} // namespace List
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTACTS_COMMON_LIST_CCONTACTLISTFILTER_H__
