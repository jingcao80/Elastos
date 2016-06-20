#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ADNRECORDCACHE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ADNRECORDCACHE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"

// import android.text.TextUtils;
// import android.util.Log;
// import java.util.Iterator;

using Elastos::Droid::Internal::Telephony::Gsm::IUsimPhoneBookManager;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class AdnRecordCache
    : public Handler
    , public IAdnRecordCache
    , public IIccConstants
{
public:
    CAR_INTERFACE_DECL();

    AdnRecordCache();

    //***** Constructor
    CARAPI constructor(
        /* [in] */ IIccFileHandler* fh);

    //***** Called from SIMRecords
    /**
      * Called from SIMRecords.onRadioNotAvailable and SIMRecords.handleSimRefresh.
      */
    virtual CARAPI Reset();

    /**
      * @return List of AdnRecords for efid if we've already loaded them this
      * radio session, or null if we haven't
      */
    virtual CARAPI GetRecordsIfLoaded(
        /* [in] */ Int32 efid,
        /* [out] */ IArrayList/*<AdnRecord>*/** result);

    /**
      * Returns extension ef associated with ADN-like EF or -1 if
      * we don't know.
      *
      * See 3GPP TS 51.011 for this mapping
      */
    virtual CARAPI ExtensionEfForEf(
        /* [in] */ Int32 efid,
        /* [out] */ Int32* result);

    /**
      * Update an ADN-like record in EF by record index
      *
      * @param efid must be one among EF_ADN, EF_FDN, and EF_SDN
      * @param adn is the new adn to be stored
      * @param recordIndex is the 1-based adn record index
      * @param pin2 is required to update EF_FDN, otherwise must be null
      * @param response message to be posted when done
      *        response.exception hold the exception in error
      */
    virtual CARAPI UpdateAdnByIndex(
        /* [in] */ Int32 efid,
        /* [in] */ IAdnRecord* adn,
        /* [in] */ Int32 recordIndex,
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* response);

    /**
      * Replace oldAdn with newAdn in ADN-like record in EF
      *
      * The ADN-like records must be read through requestLoadAllAdnLike() before
      *
      * @param efid must be one of EF_ADN, EF_FDN, and EF_SDN
      * @param oldAdn is the adn to be replaced
      *        If oldAdn.isEmpty() is ture, it insert the newAdn
      * @param newAdn is the adn to be stored
      *        If newAdn.isEmpty() is true, it delete the oldAdn
      * @param pin2 is required to update EF_FDN, otherwise must be null
      * @param response message to be posted when done
      *        response.exception hold the exception in error
      */
    virtual CARAPI UpdateAdnBySearch(
        /* [in] */ Int32 efid,
        /* [in] */ IAdnRecord* oldAdn,
        /* [in] */ IAdnRecord* newAdn,
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* response);

    /**
      * Responds with exception (in response) if efid is not a known ADN-like
      * record
      */
    virtual CARAPI RequestLoadAllAdnLike(
        /* [in] */ Int32 efid,
        /* [in] */ Int32 extensionEf,
        /* [in] */ IMessage* response);

    //***** Overridden from Handler
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    virtual CARAPI UpdateUsimAdnByIndex(
        /* [in] */ Int32 efid,
        /* [in] */ IAdnRecord* newAdn,
        /* [in] */ Int32 recordIndex,
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* response);

    virtual CARAPI GetAnrCount(
        /* [out] */ Int32* result);

    virtual CARAPI GetEmailCount(
        /* [out] */ Int32* result);

    virtual CARAPI GetSpareAnrCount(
        /* [out] */ Int32* result);

    virtual CARAPI GetSpareEmailCount(
        /* [out] */ Int32* result);

    virtual CARAPI GetAdnCount(
        /* [out] */ Int32* result);

    virtual CARAPI SetAdnCount(
        /* [in] */ Int32 count);

    virtual CARAPI GetUsimAdnCount(
        /* [out] */ Int32* result);

private:
    CARAPI_(void) ClearWaiters();

    CARAPI_(void) ClearUserWriters();

    CARAPI_(void) SendErrorResponse(
        /* [in] */ IMessage* response,
        /* [in] */ const String& errString);

    //***** Private methods
    CARAPI_(void) NotifyWaiters(
        /* [in] */ IArrayList/*<IMessage>*/* waiters,
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) UpdateEmailAndAnr(
        /* [in] */ Int32 efid,
        /* [in] */ IAdnRecord* oldAdn,
        /* [in] */ IAdnRecord* newAdn,
        /* [in] */ Int32 index,
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* response);

    CARAPI_(Boolean) UpdateAnrEmailFile(
        /* [in] */ const String& oldRecord,
        /* [in] */ const String& newRecord,
        /* [in] */ Int32 index,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 efidIndex);

    CARAPI_(Boolean) UpdateUsimRecord(
        /* [in] */ IAdnRecord* oldAdn,
        /* [in] */ IAdnRecord* newAdn,
        /* [in] */ Int32 index,
        /* [in] */ Int32 tag);

public:
    // Indexed by EF ID
    AutoPtr<ISparseArray/*< AutoPtr< IArrayList<AdnRecord> > >*/> mAdnLikeFiles;
    // People waiting for ADN-like files to be loaded
    AutoPtr<ISparseArray/*<AutoPtr< IArrayList<IMessage> > >*/> mAdnLikeWaiters;
    // People waiting for adn record to be updated
    AutoPtr<ISparseArray/*<IMessage> */> mUserWriteResponse;
    //***** Event Constants
    static const Int32 EVENT_LOAD_ALL_ADN_LIKE_DONE = 1;
    static const Int32 EVENT_UPDATE_ADN_DONE = 2;

private:
    //***** Instance Variables
    AutoPtr<IIccFileHandler> mFh;
    AutoPtr<IUsimPhoneBookManager> mUsimPhoneBookManager;
    Int32 mAdncountofIcc;
    // *****USIM TAG Constants
    static const Int32 USIM_EFANR_TAG = 0xC4;
    static const Int32 USIM_EFEMAIL_TAG = 0xCA;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ADNRECORDCACHE_H__

