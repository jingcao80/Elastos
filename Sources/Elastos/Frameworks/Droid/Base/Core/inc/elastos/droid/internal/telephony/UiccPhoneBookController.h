
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICCPHONEBOOKCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICCPHONEBOOKCONTROLLER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Internal::Telephony::IIccPhoneBookInterfaceManagerProxy;
using Elastos::Droid::Internal::Telephony::IIIccPhoneBook;
using Elastos::Droid::Internal::Telephony::IPhone;

using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class UiccPhoneBookController
    : public Object
    , public IIIccPhoneBook
    , public IUiccPhoneBookController
{
public:
    CAR_INTERFACE_DECL()

    /* only one UiccPhoneBookController exists */
    CARAPI constructor(
        /* [in] */ ArrayOf<IPhone*>* phone);

    CARAPI UpdateAdnRecordsInEfBySearch(
        /* [in] */ Int32 efid,
        /* [in] */ const String& oldTag,
        /* [in] */ const String& oldPhoneNumber,
        /* [in] */ const String& newTag,
        /* [in] */ const String& newPhoneNumber,
        /* [in] */ const String& pin2,
        /* [out] */ Boolean* result);

    CARAPI UpdateAdnRecordsInEfBySearchForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 efid,
        /* [in] */ const String& oldTag,
        /* [in] */ const String& oldPhoneNumber,
        /* [in] */ const String& newTag,
        /* [in] */ const String& newPhoneNumber,
        /* [in] */ const String& pin2,
        /* [out] */ Boolean* result);

    CARAPI UpdateAdnRecordsInEfByIndex(
        /* [in] */ Int32 efid,
        /* [in] */ const String& newTag,
        /* [in] */ const String& newPhoneNumber,
        /* [in] */ Int32 index,
        /* [in] */ const String& pin2,
        /* [out] */ Boolean* result);

    CARAPI UpdateAdnRecordsInEfByIndexForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 efid,
        /* [in] */ const String& newTag,
        /* [in] */ const String& newPhoneNumber,
        /* [in] */ Int32 index,
        /* [in] */ const String& pin2,
        /* [out] */ Boolean* result);

    CARAPI GetAdnRecordsSize(
        /* [in] */ Int32 efid,
        /* [out] */ ArrayOf<Int32>** result);

    CARAPI GetAdnRecordsSizeForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 efid,
        /* [out] */ ArrayOf<Int32>** result);

    CARAPI GetAdnRecordsInEf(
        /* [in] */ Int32 efid,
        /* [out] */ IList** result);

    CARAPI GetAdnRecordsInEfForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 efid,
        /* [out] */ IList** result);

    CARAPI UpdateAdnRecordsWithContentValuesInEfBySearch(
        /* [in] */ Int32 efid,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& pin2,
        /* [out] */ Boolean* result);

    CARAPI UpdateAdnRecordsWithContentValuesInEfBySearchUsingSubId(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 efid,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& pin2,
        /* [out] */ Boolean* result);

    CARAPI GetAdnCount(
        /* [out] */ Int32* result);

    CARAPI GetAdnCountUsingSubId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    CARAPI GetAnrCount(
        /* [out] */ Int32* result);

    CARAPI GetAnrCountUsingSubId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    CARAPI GetEmailCount(
        /* [out] */ Int32* result);

    CARAPI GetEmailCountUsingSubId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    CARAPI GetSpareAnrCount(
        /* [out] */ Int32* result);

    CARAPI GetSpareAnrCountUsingSubId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    CARAPI GetSpareEmailCount(
        /* [out] */ Int32* result);

    CARAPI GetSpareEmailCountUsingSubId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

private:
    /**
     * get phone book interface manager proxy object based on subscription.
     **/
    CARAPI_(AutoPtr<IIccPhoneBookInterfaceManagerProxy>) GetIccPhoneBookInterfaceManagerProxy(
        /* [in] */ Int64 subId);

    CARAPI_(Int64) GetDefaultSubId();

private:
    static const String TAG;
    AutoPtr<ArrayOf<IPhone*> > mPhone;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICCPHONEBOOKCONTROLLER_H__
