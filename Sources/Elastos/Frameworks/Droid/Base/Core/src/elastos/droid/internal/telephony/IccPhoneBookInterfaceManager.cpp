
#include "elastos/droid/internal/telephony/IccPhoneBookInterfaceManager.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Telephony::Uicc::AppType;
using Elastos::Droid::Internal::Telephony::Uicc::APPTYPE_CSIM;
using Elastos::Droid::Internal::Telephony::Uicc::APPTYPE_ISIM;
using Elastos::Droid::Internal::Telephony::Uicc::APPTYPE_UNKNOWN;
using Elastos::Droid::Internal::Telephony::Uicc::APPTYPE_USIM;
using Elastos::Droid::Internal::Telephony::Uicc::IAdnRecord;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const Boolean IccPhoneBookInterfaceManager::DBG = TRUE;
const Boolean IccPhoneBookInterfaceManager::ALLOW_SIM_OP_IN_UI_THREAD = FALSE;
const Int32 IccPhoneBookInterfaceManager::EVENT_GET_SIZE_DONE = 1;
const Int32 IccPhoneBookInterfaceManager::EVENT_LOAD_DONE = 2;
const Int32 IccPhoneBookInterfaceManager::EVENT_UPDATE_DONE = 3;

IccPhoneBookInterfaceManager::BaseHandler::BaseHandler(
    /* [in] */ IccPhoneBookInterfaceManager* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode IccPhoneBookInterfaceManager::BaseHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_GET_SIZE_DONE: {
            ar = (AsyncResult*) IObject::Probe(obj);
            {
                AutoLock lock(mHost->mLock);
                if (ar->mException == NULL) {
                    assert(0 && "TODO");
                    // mHost->mRecordSize = (Int32[])ar->mResult;

                    // recordSize[0]  is the record length
                    // recordSize[1]  is the total length of the EF file
                    // recordSize[2]  is the number of records in the EF file
                    String str;
                    str.AppendFormat("GET_RECORD_SIZE Size %d total %d #record %d"
                        , (*mHost->mRecordSize)[0], (*mHost->mRecordSize)[1], (*mHost->mRecordSize)[2]);
                    mHost->Logd(str);
                }
                NotifyPending(ar);
            }
            break;
        }
        case EVENT_UPDATE_DONE: {
            ar = (AsyncResult*) IObject::Probe(obj);
            {
                AutoLock lock(mHost->mLock);
                mHost->mSuccess = (ar->mException == NULL);
                NotifyPending(ar);
            }
            break;
        }
        case EVENT_LOAD_DONE: {
            ar = (AsyncResult*)IObject::Probe(obj);
            {
                AutoLock lock(mHost->mLock);
                if (ar->mException == NULL) {
                    mHost->mRecords = IList::Probe(ar->mResult);
                }
                else {
                    if(DBG) mHost->Logd(String("Cannot load ADN records"));
                    if (mHost->mRecords != NULL) {
                        mHost->mRecords->Clear();
                    }
                }
                NotifyPending(ar);
            }
            break;
        }
    }
    return NOERROR;
}

void IccPhoneBookInterfaceManager::BaseHandler::NotifyPending(
    /* [in] */ AsyncResult* ar)
{
    if (ar->mUserObj != NULL) {
        AutoPtr<IAtomicBoolean> status = IAtomicBoolean::Probe(ar->mUserObj);
        status->Set(TRUE);
    }
    mHost->mLock.NotifyAll();
}

CAR_INTERFACE_IMPL(IccPhoneBookInterfaceManager, Object, IIccPhoneBookInterfaceManager)
IccPhoneBookInterfaceManager::IccPhoneBookInterfaceManager()
{
}

ECode IccPhoneBookInterfaceManager::constructor(
    /* [in] */ IPhoneBase* phone)
{
    mPhone = phone;
    mSuccess = FALSE;
    mIs3gCard = FALSE;
    mBaseHandler = new BaseHandler(this);
    return NOERROR;
}

void IccPhoneBookInterfaceManager::CleanUp()
{
    if (mAdnCache != NULL) {
        mAdnCache->Reset();
        mAdnCache = NULL;
    }
    mIs3gCard = FALSE;
    mCurrentApp = NULL;
}

ECode IccPhoneBookInterfaceManager::Dispose()
{
    if (mRecords != NULL) {
        mRecords->Clear();
    }
    CleanUp();
    return NOERROR;
}

ECode IccPhoneBookInterfaceManager::SetIccCard(
    /* [in] */ IUiccCard* card)
{
    Logd(String("Card update received: ") + (card == NULL ? "NULL" : TO_CSTR(card)));

    if (card == NULL) {
        Logd(String("Card is NULL. Cleanup"));
        CleanUp();
        return NOERROR;
    }

    AutoPtr<IUiccCardApplication> validApp;
    Int32 numApps = 0;
    card->GetNumApplications(&numApps);
    Boolean isCurrentAppFound = FALSE;
    mIs3gCard = FALSE;

    for (Int32 i = 0; i < numApps; i++) {
        AutoPtr<IUiccCardApplication> app;
        card->GetApplicationIndex(i, (IUiccCardApplication**)&app);
        if (app != NULL) {
            // Determine if the card is a 3G card by looking
            // for a CSIM/USIM/ISIM app on the card
            AppType type;
            app->GetType(&type);
            if (type == APPTYPE_CSIM || type == APPTYPE_USIM
                    || type == APPTYPE_ISIM) {
                Logd(String("Card is 3G"));
                mIs3gCard = TRUE;
            }
            // Check if the app we have is present.
            // If yes, then continue using that.
            if (!isCurrentAppFound) {
                // if not, then find a valid app.
                // It does not matter which app, since we are
                // accessing non-app specific files
                if (validApp == NULL && type != APPTYPE_UNKNOWN) {
                    validApp = app;
                }

                if (mCurrentApp == app) {
                    Logd(String("Existing app found"));
                    isCurrentAppFound = TRUE;
                }
            }

            // We have determined that this is 3g card
            // and we also found the current app
            // We are done
            if (mIs3gCard && isCurrentAppFound) {
                break;
            }
        }
    }

    //Set a new currentApp if
    // - one was not set before
    // OR
    // - the previously set app no longer exists
    if (mCurrentApp == NULL || !isCurrentAppFound) {
        if (validApp != NULL) {
            Logd(String("Setting currentApp: ") + TO_CSTR(validApp));
            mCurrentApp = validApp;
            AutoPtr<IIccRecords> ir;
            mCurrentApp->GetIccRecords((IIccRecords**)&ir);
            ir->GetAdnCache((IAdnRecordCache**)&mAdnCache);
        }
    }
    return NOERROR;
}

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
ECode IccPhoneBookInterfaceManager::UpdateAdnRecordsInEfBySearch(
    /* [in] */ Int32 efid,
    /* [in] */ const String& oldTag,
    /* [in] */ const String& oldPhoneNumber,
    /* [in] */ const String& newTag,
    /* [in] */ const String& newPhoneNumber,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    Int32 v = 0;
    ctx->CheckCallingOrSelfPermission(Manifest::permission::WRITE_CONTACTS, &v);
    if (v != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException(
        //         "Requires android.permission.WRITE_CONTACTS permission");
        *result = FALSE;
        return E_SECURITY_EXCEPTION;
    }

    if (DBG) {
        String vv;
        vv.AppendFormat("updateAdnRecordsInEfBySearch: efid=%d (%s,%s)==> (%s,%s) pin2=%s"
            , efid, oldTag.string(), oldPhoneNumber.string()
            , newTag.string(), newPhoneNumber.string(), pin2.string());
        Logd(vv);
    }

    efid = UpdateEfForIccType(efid);

    {
        AutoLock lock(mLock);
        FAILED(CheckThread());
        mSuccess = FALSE;
        AutoPtr<IAtomicBoolean> status;
        CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&status);
        AutoPtr<IMessage> response;
        mBaseHandler->ObtainMessage(EVENT_UPDATE_DONE, status, (IMessage**)&response);
        AutoPtr<IAdnRecord> oldAdn;
        AutoPtr<IAdnRecord> newAdn;
        assert(0 && "TODO");
        // CAdnRecord::New(oldTag, oldPhoneNumber, (IAdnRecord**)&oldAdn);
        // CAdnRecord::New(newTag, newPhoneNumber, (IAdnRecord**)&newAdn);
        if (mAdnCache != NULL) {
            mAdnCache->UpdateAdnBySearch(efid, oldAdn, newAdn, pin2, response);
            WaitForResult(status);
        }
        else {
            Loge(String("Failure while trying to update by search due to uninitialised adncache"));
        }
    }
    *result = mSuccess;
    return NOERROR;
}

ECode IccPhoneBookInterfaceManager::UpdateAdnRecordsWithContentValuesInEfBySearch(
    /* [in] */ Int32 efid,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    Int32 v = 0;
    ctx->CheckCallingOrSelfPermission(Manifest::permission::WRITE_CONTACTS, &v);
    if (v != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires android.permission.WRITE_CONTACTS permission");
        *result = FALSE;
        return E_SECURITY_EXCEPTION;
    }

    String oldTag;
    values->GetAsString(IIccProvider::STR_TAG, &oldTag);
    String newTag;
    values->GetAsString(IIccProvider::STR_NEW_TAG, &newTag);
    String oldPhoneNumber;
    values->GetAsString(IIccProvider::STR_NUMBER, &oldPhoneNumber);
    String newPhoneNumber;
    values->GetAsString(IIccProvider::STR_NEW_NUMBER, &newPhoneNumber);
    String oldEmail;
    values->GetAsString(IIccProvider::STR_EMAILS, &oldEmail);
    String newEmail;
    values->GetAsString(IIccProvider::STR_NEW_EMAILS, &newEmail);
    String oldAnr;
    values->GetAsString(IIccProvider::STR_ANRS, &oldAnr);
    String newAnr;
    values->GetAsString(IIccProvider::STR_NEW_ANRS, &newAnr);
    AutoPtr<ArrayOf<String> > oldEmailArray = TextUtils::IsEmpty(oldEmail) ? NULL : GetStringArray(oldEmail);
    AutoPtr<ArrayOf<String> > newEmailArray = TextUtils::IsEmpty(newEmail) ? NULL : GetStringArray(newEmail);
    AutoPtr<ArrayOf<String> > oldAnrArray = TextUtils::IsEmpty(oldAnr) ? NULL : GetStringArray(oldAnr);
    AutoPtr<ArrayOf<String> > newAnrArray = TextUtils::IsEmpty(newAnr) ? NULL : GetStringArray(newAnr);
    efid = UpdateEfForIccType(efid);

    if (DBG) {
        Logd(String("updateAdnRecordsInEfBySearch: efid=")
             + StringUtils::ToString(efid) + ", values = "
             + TO_CSTR(values) + ", pin2=" + pin2);
    }

    {
        AutoLock lock(mLock);
        FAILED(CheckThread());
        mSuccess = FALSE;
        AutoPtr<IAtomicBoolean> status;
        CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&status);
        AutoPtr<IMessage> response;
        mBaseHandler->ObtainMessage(EVENT_UPDATE_DONE, status, (IMessage**)&response);
        AutoPtr<IAdnRecord> oldAdn;
        AutoPtr<IAdnRecord> newAdn;
        assert(0 && "TODO");
        // CAdnRecord::New(oldTag, oldPhoneNumber, oldEmailArray, oldAnrArray, (IAdnRecord**)&oldAdn);
        // CAdnRecord::New(newTag, newPhoneNumber, newEmailArray, newAnrArray, (IAdnRecord**)&newAdn);
        if (mAdnCache != NULL) {
            mAdnCache->UpdateAdnBySearch(efid, oldAdn, newAdn, pin2, response);
            WaitForResult(status);
        }
        else {
            Loge(String("Failure while trying to update by search due to uninitialised adncache"));
        }
    }
    *result = mSuccess;
    return NOERROR;
}

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
ECode IccPhoneBookInterfaceManager::UpdateAdnRecordsInEfByIndex(
    /* [in] */ Int32 efid,
    /* [in] */ const String& newTag,
    /* [in] */ const String& newPhoneNumber,
    /* [in] */ Int32 index,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    Int32 v = 0;
    ctx->CheckCallingOrSelfPermission(Manifest::permission::WRITE_CONTACTS, &v);

    if (v != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException(
        //         "Requires android.permission.WRITE_CONTACTS permission");
        *result = FALSE;
        return E_SECURITY_EXCEPTION;
    }

    if (DBG) {
        Logd(String("updateAdnRecordsInEfByIndex: efid=") + StringUtils::ToString(efid) +
            " Index=" + StringUtils::ToString(index) + " ==> " +
            "("+ newTag + "," + newPhoneNumber + ")"+ " pin2=" + pin2);
    }
    {
        AutoLock lock(mLock);
        FAILED(CheckThread());
        mSuccess = FALSE;
        AutoPtr<IAtomicBoolean> status;
        CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&status);
        AutoPtr<IMessage> response;
        mBaseHandler->ObtainMessage(EVENT_UPDATE_DONE, status, (IMessage**)&response);
        AutoPtr<IAdnRecord> newAdn;
        assert(0 && "TODO");
        // CAdnRecord::New(newTag, newPhoneNumber, (IAdnRecord**)&newAdn);
        if (mAdnCache != NULL) {
            mAdnCache->UpdateAdnByIndex(efid, newAdn, index, pin2, response);
            WaitForResult(status);
        }
        else {
            Loge(String("Failure while trying to update by index due to uninitialised adncache"));
        }
    }
    *result = mSuccess;
    return NOERROR;
}

/**
 * Loads the AdnRecords in efid and returns them as a
 * List of AdnRecords
 *
 * throws SecurityException if no READ_CONTACTS permission
 *
 * @param efid the EF id of a ADN-like ICC
 * @return List of AdnRecord
 */
ECode IccPhoneBookInterfaceManager::GetAdnRecordsInEf(
    /* [in] */ Int32 efid,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    Int32 v = 0;
    ctx->CheckCallingOrSelfPermission(Manifest::permission::READ_CONTACTS, &v);

    if (v != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException(
        //         "Requires android.permission.READ_CONTACTS permission");
        return E_SECURITY_EXCEPTION;
    }

    efid = UpdateEfForIccType(efid);
    if (DBG) Logd(String("getAdnRecordsInEF: efid=") + StringUtils::ToString(efid));

    {
        AutoLock lock(mLock);
        FAILED(CheckThread());
        AutoPtr<IAtomicBoolean> status;
        CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&status);
        AutoPtr<IMessage> response;
        mBaseHandler->ObtainMessage(EVENT_LOAD_DONE, status, (IMessage**)&response);
        if (mAdnCache != NULL) {
            Int32 iv = 0;
            mAdnCache->ExtensionEfForEf(efid, &iv);
            mAdnCache->RequestLoadAllAdnLike(efid, iv, response);
            WaitForResult(status);
        }
        else {
            Loge(String("Failure while trying to load from SIM due to uninitialised adncache"));
        }
    }
    *result = mRecords;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode IccPhoneBookInterfaceManager::CheckThread()
{
    if (!ALLOW_SIM_OP_IN_UI_THREAD) {
        // Make sure this isn't the UI thread, since it will block
        AutoPtr<ILooper> l;
        mBaseHandler->GetLooper((ILooper**)&l);
        Boolean e = FALSE;
        if (IObject::Probe(l)->Equals(Looper::GetMyLooper(), &e), e) {
            Loge(String("query() called on the main UI thread!"));
            // throw new IllegalStateException(
            //         "You cannot call query on this provder from the main UI thread.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    return NOERROR;
}

AutoPtr<ArrayOf<String> > IccPhoneBookInterfaceManager::GetStringArray(
    /* [in] */ const String& str)
{
    if (str != NULL) {
        AutoPtr<ArrayOf<String> > vs;
        StringUtils::Split(str, String(","), (ArrayOf<String>**)&vs);
        return vs;
    }
    return NULL;
}

void IccPhoneBookInterfaceManager::WaitForResult(
    /* [in] */ IAtomicBoolean* status)
{
    Boolean v = FALSE;
    while (status->Get(&v), !v) {
        // try {
        if (FAILED(mLock.Wait())) {
            Logd(String("interrupted while trying to update by search"));
        }
        // } catch (InterruptedException e) {
        //     Logd("interrupted while trying to update by search");
        // }
    }
}

Int32 IccPhoneBookInterfaceManager::UpdateEfForIccType(
    /* [in] */ Int32 efid)
{
    // If we are trying to read ADN records on a 3G card
    // use EF_PBR
    if (efid == IIccConstants::EF_ADN && mIs3gCard) {
        Logd(String("Translate EF_ADN to EF_PBR"));
        return IIccConstants::EF_PBR;
    }
    return efid;
}

ECode IccPhoneBookInterfaceManager::GetAdnCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 adnCount = 0;
    if (mAdnCache != NULL) {
        AppType at;
        mPhone->GetCurrentUiccAppType(&at);
        if (at == APPTYPE_USIM) {
            mAdnCache->GetUsimAdnCount(&adnCount);
        }
        else {
            mAdnCache->GetAdnCount(&adnCount);
        }
    }
    else {
        Loge(String("mAdnCache is NULL when getAdnCount."));
    }
    *result = adnCount;
    return NOERROR;
}

ECode IccPhoneBookInterfaceManager::GetAnrCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 anrCount = 0;
    if (mAdnCache != NULL) {
        mAdnCache->GetAnrCount(&anrCount);
    }
    else {
        Loge(String("mAdnCache is NULL when getAnrCount."));
    }
    *result = anrCount;
    return NOERROR;
}

ECode IccPhoneBookInterfaceManager::GetEmailCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 emailCount = 0;
    if (mAdnCache != NULL) {
        mAdnCache->GetEmailCount(&emailCount);
    }
    else {
        Loge(String("mAdnCache is NULL when getEmailCount."));
    }
    *result = emailCount;
    return NOERROR;
}

ECode IccPhoneBookInterfaceManager::GetSpareAnrCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 spareAnrCount = 0;
    if (mAdnCache != NULL) {
        mAdnCache->GetSpareAnrCount(&spareAnrCount);
    }
    else {
        Loge(String("mAdnCache is NULL when getSpareAnrCount."));
    }
    *result = spareAnrCount;
    return NOERROR;
}

ECode IccPhoneBookInterfaceManager::GetSpareEmailCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 spareEmailCount = 0;
    if (mAdnCache != NULL) {
        mAdnCache->GetSpareEmailCount(&spareEmailCount);
    }
    else {
        Loge(String("mAdnCache is NULL when getSpareEmailCount."));
    }
    *result = spareEmailCount;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
