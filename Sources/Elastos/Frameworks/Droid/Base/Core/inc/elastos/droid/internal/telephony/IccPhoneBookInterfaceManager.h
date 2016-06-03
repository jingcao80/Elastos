
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCPHONEBOOKINTERFACEMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCPHONEBOOKINTERFACEMANAGER_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecordCache;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Handler;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * SimPhoneBookInterfaceManager to provide an inter-process communication to
 * access ADN-like SIM records.
 */
class IccPhoneBookInterfaceManager
    : public Object
    , public IIccPhoneBookInterfaceManager
{
protected:
    class BaseHandler: public Handler
    {
    public:
        BaseHandler(
            /* [in] */ IccPhoneBookInterfaceManager* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) NotifyPending(
            /* [in] */ AsyncResult* ar);

    private:
        IccPhoneBookInterfaceManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    IccPhoneBookInterfaceManager(
        /* [in] */ IPhoneBase* phone);

    CARAPI Dispose();

    CARAPI SetIccCard(
        /* [in] */ IUiccCard* card);

    /**
     * Replace oldAdn with newAdn in ADN-like record in EF
     *
     * getAdnRecordsInEf must be called at least once before this function,
     * otherwise an error will be returned. Currently the email field
     * if set in the ADN record is ignored.
     * throws SecurityException if no WRITE_CONTACTS permission
     *
     * @param efid must be one among EF_ADN, EF_FDN, and EF_SDN
     * @param oldTag adn tag to be replaced
     * @param oldPhoneNumber adn number to be replaced
     *        Set both oldTag and oldPhoneNubmer to "" means to replace an
     *        empty record, aka, insert new record
     * @param newTag adn tag to be stored
     * @param newPhoneNumber adn number ot be stored
     *        Set both newTag and newPhoneNubmer to "" means to replace the old
     *        record with empty one, aka, delete old record
     * @param pin2 required to update EF_FDN, otherwise must be NULL
     * @return TRUE for success
     */
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
        /* [out] */ Boolean* result);

    /**
     * Update an ADN-like EF record by record index
     *
     * This is useful for iteration the whole ADN file, such as write the whole
     * phone book or erase/format the whole phonebook. Currently the email field
     * if set in the ADN record is ignored.
     * throws SecurityException if no WRITE_CONTACTS permission
     *
     * @param efid must be one among EF_ADN, EF_FDN, and EF_SDN
     * @param newTag adn tag to be stored
     * @param newPhoneNumber adn number to be stored
     *        Set both newTag and newPhoneNubmer to "" means to replace the old
     *        record with empty one, aka, delete old record
     * @param index is 1-based adn record index to be updated
     * @param pin2 required to update EF_FDN, otherwise must be NULL
     * @return TRUE for success
     */
    CARAPI UpdateAdnRecordsInEfByIndex(
        /* [in] */ Int32 efid,
        /* [in] */ const String& newTag,
        /* [in] */ const String& newPhoneNumber,
        /* [in] */ Int32 index,
        /* [in] */ const String& pin2,
        /* [out] */ Boolean* result);

    /**
     * Loads the AdnRecords in efid and returns them as a
     * List of AdnRecords
     *
     * throws SecurityException if no READ_CONTACTS permission
     *
     * @param efid the EF id of a ADN-like ICC
     * @return List of AdnRecord
     */
    CARAPI GetAdnRecordsInEf(
        /* [in] */ Int32 efid,
        /* [out] */ IList** result);

    CARAPI GetAdnCount(
        /* [out] */ Int32* result);

    CARAPI GetAnrCount(
        /* [out] */ Int32* result);

    CARAPI GetEmailCount(
        /* [out] */ Int32* result);

    CARAPI GetSpareAnrCount(
        /* [out] */ Int32* result);

    CARAPI GetSpareEmailCount(
        /* [out] */ Int32* result);

protected:
    virtual CARAPI_(void) Logd(
        /* [in] */ const String& msg) = 0;

    virtual CARAPI_(void) Loge(
        /* [in] */ const String& msg) = 0;

    virtual CARAPI CheckThread();

    virtual CARAPI_(void) WaitForResult(
        /* [in] */ IAtomicBoolean* status);

private:
    CARAPI_(void) CleanUp();

    CARAPI_(AutoPtr<ArrayOf<String> >) GetStringArray(
        /* [in] */ const String& str);

    CARAPI_(Int32) UpdateEfForIccType(
        /* [in] */ Int32 efid);

protected:
    static const Boolean DBG;

    AutoPtr<IPhoneBase> mPhone;
    AutoPtr<IAdnRecordCache> mAdnCache;
    Object mLock; // = new Object();
    AutoPtr<ArrayOf<Int32> > mRecordSize;
    Boolean mSuccess;
    AutoPtr<IList> mRecords; // List<AdnRecord>


    static const Boolean ALLOW_SIM_OP_IN_UI_THREAD;

    static const Int32 EVENT_GET_SIZE_DONE;
    static const Int32 EVENT_LOAD_DONE;
    static const Int32 EVENT_UPDATE_DONE;
    AutoPtr<IHandler> mBaseHandler;

private:
    AutoPtr<IUiccCardApplication> mCurrentApp;
    Boolean mIs3gCard;  // flag to determine if card is 3G or 2G
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_ICCPHONEBOOKINTERFACEMANAGER_H__
