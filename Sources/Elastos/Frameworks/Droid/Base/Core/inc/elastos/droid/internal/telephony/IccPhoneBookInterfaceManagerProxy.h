
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCPHONEBOOKINTERFACEMANAGERPROXY_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCPHONEBOOKINTERFACEMANAGERPROXY_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * SimPhoneBookInterfaceManager to provide an inter-process communication to
 * access ADN-like SIM records.
 */
class IccPhoneBookInterfaceManagerProxy
    : public Object
    , public IIccPhoneBookInterfaceManagerProxy
{
public:
    CAR_INTERFACE_DECL()

    IccPhoneBookInterfaceManagerProxy(
        /* [in] */ IIccPhoneBookInterfaceManager* iccPhoneBookInterfaceManager);

    CARAPI SetmIccPhoneBookInterfaceManager(
        /* [in] */ IIccPhoneBookInterfaceManager* iccPhoneBookInterfaceManager);

    CARAPI UpdateAdnRecordsInEfBySearch(
        /* [in] */ Int32 efid,
        /* [in] */ const String& oldTag,
        /* [in] */ const String& oldPhoneNumber,
        /* [in] */ const String& newTag,
        /* [in] */ const String& newPhoneNumber,
        /* [in] */ const String& pin2,
        /* [out] */ Boolean* result);

    CARAPI UpdateAdnRecordsWithContentValuesInEfBySearch(
        /* [in] */ Int32 efid,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& pin2,
        /* [out] */ Boolean* result) /*throws android.os.RemoteException*/;

    CARAPI UpdateAdnRecordsInEfByIndex(
        /* [in] */ Int32 efid,
        /* [in] */ const String& newTag,
        /* [in] */ const String& newPhoneNumber,
        /* [in] */ Int32 index,
        /* [in] */ const String& pin2,
        /* [out] */ Boolean* result);

    CARAPI GetAdnRecordsSize(
        /* [in] */ Int32 efid,
        /* [out] */ ArrayOf<Int32>** result);

    CARAPI GetAdnRecordsInEf(
        /* [in] */ Int32 efid,
        /* [out] */ IList** result);

    CARAPI GetAdnCount(
        /* [out] */ Int32* count);

    CARAPI GetAnrCount(
        /* [out] */ Int32* count);

    CARAPI GetEmailCount(
        /* [out] */ Int32* count);

    CARAPI GetSpareAnrCount(
        /* [out] */ Int32* count);

    CARAPI GetSpareEmailCount(
        /* [out] */ Int32* count);

private:
    AutoPtr<IIccPhoneBookInterfaceManager> mIccPhoneBookInterfaceManager;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCPHONEBOOKINTERFACEMANAGERPROXY_H__
