#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCFILEHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCFILEHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"

// import android.os.*;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class IccFileHandler
    : public Handler
    , public IIccConstants
{
public:
    class LoadLinearFixedContext
        : public Object
    {
    public:
        LoadLinearFixedContext(
            /* [in] */ Int32 efid,
            /* [in] */ Int32 recordNum,
            /* [in] */ IMessage* onLoaded);

        LoadLinearFixedContext(
            /* [in] */ Int32 efid,
            /* [in] */ Int32 recordNum,
            /* [in] */ const String& path,
            /* [in] */ IMessage* onLoaded);

        LoadLinearFixedContext(
            /* [in] */ Int32 efid,
            /* [in] */ const String& path,
            /* [in] */ IMessage* onLoaded);

        LoadLinearFixedContext(
            /* [in] */ Int32 efid,
            /* [in] */ IMessage* onLoaded);

        LoadLinearFixedContext(
            /* [in] */ Int32 efid,
            /* [in] */ IArrayList/*<Integer*>*/* recordNums,
            /* [in] */ const String& path,
            /* [in] */ IMessage* onLoaded);

    private:
        CARAPI_(void) InitLCResults(
            /* [in] */ Int32 size);

    public:
        Int32 mEfid;
        Int32 mRecordNum;
        Int32 mRecordSize;
        Int32 mCountRecords;
        Boolean mLoadAll;
        String mPath;
        // Variables used to load part records
        Boolean mLoadPart;
        AutoPtr<IArrayList/*<Integer>*/> mRecordNums;
        Int32 mCountLoadrecords;
        Int32 mCount;
        AutoPtr<IMessage> mOnLoaded;
        AutoPtr<IArrayList/*< AutoPtr< ArrayOf<Byte> > >*/> results;
    };

public:
    CAR_INTERFACE_DECL();

    virtual CARAPI Dispose();

    //***** Public Methods
    /**
      * Load a record from a SIM Linear Fixed EF
      *
      * @param fileid EF id
      * @param path Path of the EF on the card
      * @param recordNum 1-based (not 0-based) record number
      * @param onLoaded
      *
      * ((AsyncResult)(onLoaded.obj)).result is the byte[]
      *
      */
    virtual CARAPI LoadEFLinearFixed(
        /* [in] */ Int32 fileid,
        /* [in] */ const String& path,
        /* [in] */ Int32 recordNum,
        /* [in] */ IMessage* onLoaded);

    /**
      * Load a record from a SIM Linear Fixed EF
      *
      * @param fileid EF id
      * @param recordNum 1-based (not 0-based) record number
      * @param onLoaded
      *
      * ((AsyncResult)(onLoaded.obj)).result is the byte[]
      *
      */
    virtual CARAPI LoadEFLinearFixed(
        /* [in] */ Int32 fileid,
        /* [in] */ Int32 recordNum,
        /* [in] */ IMessage* onLoaded);

    /**
      * Load a image instance record from a SIM Linear Fixed EF-IMG
      *
      * @param recordNum 1-based (not 0-based) record number
      * @param onLoaded
      *
      * ((AsyncResult)(onLoaded.obj)).result is the byte[]
      *
      */
    virtual CARAPI LoadEFImgLinearFixed(
        /* [in] */ Int32 recordNum,
        /* [in] */ IMessage* onLoaded);

    /**
      * get record size for a linear fixed EF
      *
      * @param fileid EF id
      * @param path Path of the EF on the card
      * @param onLoaded ((AsnyncResult)(onLoaded.obj)).result is the recordSize[]
      *        int[0] is the record length int[1] is the total length of the EF
      *        file int[3] is the number of records in the EF file So int[0] *
      *        int[3] = int[1]
      */
    virtual CARAPI GetEFLinearRecordSize(
        /* [in] */ Int32 fileid,
        /* [in] */ const String& path,
        /* [in] */ IMessage* onLoaded);

    /**
      * get record size for a linear fixed EF
      *
      * @param fileid EF id
      * @param onLoaded ((AsnyncResult)(onLoaded.obj)).result is the recordSize[]
      *        int[0] is the record length int[1] is the total length of the EF
      *        file int[3] is the number of records in the EF file So int[0] *
      *        int[3] = int[1]
      */
    virtual CARAPI GetEFLinearRecordSize(
        /* [in] */ Int32 fileid,
        /* [in] */ IMessage* onLoaded);

    /**
      * Load all records from a SIM Linear Fixed EF
      *
      * @param fileid EF id
      * @param onLoaded
      *
      * ((AsyncResult)(onLoaded.obj)).result is an ArrayList<byte[]>
      *
      */
    virtual CARAPI LoadEFLinearFixedAll(
        /* [in] */ Int32 fileid,
        /* [in] */ IMessage* onLoaded);

    /**
      * Load all records from a SIM Linear Fixed EF
      *
      * @param fileid EF id
      * @param path Path of the EF on the card
      * @param onLoaded
      *
      * ((AsyncResult)(onLoaded.obj)).result is an ArrayList<byte[]>
      *
      */
    virtual CARAPI LoadEFLinearFixedAll(
        /* [in] */ Int32 fileid,
        /* [in] */ const String& path,
        /* [in] */ IMessage* onLoaded);

    /**
      * Load several records from a SIM Linear Fixed EF
      *
      * @param fileid EF id
      * @param onLoaded
      *
      * ((AsyncResult)(onLoaded.obj)).result is an ArrayList<byte[]>
      *
      */
    virtual CARAPI LoadEFLinearFixedPart(
        /* [in] */ Int32 fileid,
        /* [in] */ IArrayList/*<Integer*>*/* recordNums,
        /* [in] */ IMessage* onLoaded);

    /**
      * Load a SIM Transparent EF
      *
      * @param fileid EF id
      * @param onLoaded
      *
      * ((AsyncResult)(onLoaded.obj)).result is the byte[]
      *
      */
    virtual CARAPI LoadEFTransparent(
        /* [in] */ Int32 fileid,
        /* [in] */ IMessage* onLoaded);

    /**
      * Load first @size bytes from SIM Transparent EF
      *
      * @param fileid EF id
      * @param size
      * @param onLoaded
      *
      * ((AsyncResult)(onLoaded.obj)).result is the byte[]
      *
      */
    virtual CARAPI LoadEFTransparent(
        /* [in] */ Int32 fileid,
        /* [in] */ Int32 size,
        /* [in] */ IMessage* onLoaded);

    /**
      * Load a SIM Transparent EF-IMG. Used right after loadEFImgLinearFixed to
      * retrive STK's icon data.
      *
      * @param fileid EF id
      * @param onLoaded
      *
      * ((AsyncResult)(onLoaded.obj)).result is the byte[]
      *
      */
    virtual CARAPI LoadEFImgTransparent(
        /* [in] */ Int32 fileid,
        /* [in] */ Int32 highOffset,
        /* [in] */ Int32 lowOffset,
        /* [in] */ Int32 length,
        /* [in] */ IMessage* onLoaded);

    /**
      * Update a record in a linear fixed EF
      * @param fileid EF id
      * @param path Path of the EF on the card
      * @param recordNum 1-based (not 0-based) record number
      * @param data must be exactly as long as the record in the EF
      * @param pin2 for CHV2 operations, otherwist must be null
      * @param onComplete onComplete.obj will be an AsyncResult
      *                   onComplete.obj.userObj will be a IccIoResult on success
      */
    virtual CARAPI UpdateEFLinearFixed(
        /* [in] */ Int32 fileid,
        /* [in] */ const String& path,
        /* [in] */ Int32 recordNum,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* onComplete);

    /**
      * Update a record in a linear fixed EF
      * @param fileid EF id
      * @param recordNum 1-based (not 0-based) record number
      * @param data must be exactly as long as the record in the EF
      * @param pin2 for CHV2 operations, otherwist must be null
      * @param onComplete onComplete.obj will be an AsyncResult
      *                   onComplete.obj.userObj will be a IccIoResult on success
      */
    virtual CARAPI UpdateEFLinearFixed(
        /* [in] */ Int32 fileid,
        /* [in] */ Int32 recordNum,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* onComplete);

    /**
      * Update a transparent EF
      * @param fileid EF id
      * @param data must be exactly as long as the EF
      */
    virtual CARAPI UpdateEFTransparent(
        /* [in] */ Int32 fileid,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ IMessage* onComplete);

    //***** Overridden from Handler
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

protected:
    /**
      * Default constructor
      */
    CARAPI constructor(
        /* [in] */ IUiccCardApplication* app,
        /* [in] */ const String& aid,
        /* [in] */ ICommandsInterface* ci);

    /**
      * Returns the root path of the EF file.
      * i.e returns MasterFile + DFfile as a string.
      * Ex: For EF_ADN on a SIM, it will return "3F007F10"
      * This function handles only EFids that are common to
      * RUIM, SIM, USIM and other types of Icc cards.
      *
      * @param efid of path to retrieve
      * @return root path of the file.
      */
    virtual CARAPI_(String) GetCommonIccEFPath(
        /* [in] */ Int32 efid);

    virtual CARAPI_(String) GetEFPath(
        /* [in] */ Int32 efid) = 0;

    virtual CARAPI Logd(
        /* [in] */ const String& s) = 0;

    virtual CARAPI Loge(
        /* [in] */ const String& s) = 0;

private:
    //***** Abstract Methods
    //***** Private Methods
    CARAPI_(void) SendResult(
        /* [in] */ IMessage* response,
        /* [in] */ IInterface* result);
        ///* [in] */ Throwable* ex);

    CARAPI_(Boolean) ProcessException(
        /* [in] */ IMessage* response,
        /* [in] */ AsyncResult* ar);

protected:
    //from TS 11.11 9.1 or elsewhere
    static const Int32 COMMAND_READ_BINARY = 0xb0;
    static const Int32 COMMAND_UPDATE_BINARY = 0xd6;
    static const Int32 COMMAND_READ_RECORD = 0xb2;
    static const Int32 COMMAND_UPDATE_RECORD = 0xdc;
    static const Int32 COMMAND_SEEK = 0xa2;
    static const Int32 COMMAND_GET_RESPONSE = 0xc0;
    // from TS 11.11 9.2.5
    static const Int32 READ_RECORD_MODE_ABSOLUTE = 4;
    //***** types of files  TS 11.11 9.3
    static const Int32 EF_TYPE_TRANSPARENT = 0;
    static const Int32 EF_TYPE_LINEAR_FIXED = 1;
    static const Int32 EF_TYPE_CYCLIC = 3;
    //***** types of files  TS 11.11 9.3
    static const Int32 TYPE_RFU = 0;
    static const Int32 TYPE_MF = 1;
    static const Int32 TYPE_DF = 2;
    static const Int32 TYPE_EF = 4;
    // size of GET_RESPONSE for EF's
    static const Int32 GET_RESPONSE_EF_SIZE_BYTES = 15;
    static const Int32 GET_RESPONSE_EF_IMG_SIZE_BYTES = 10;
    // Byte order received in response to COMMAND_GET_RESPONSE
    // Refer TS 51.011 Section 9.2.1
    static const Int32 RESPONSE_DATA_RFU_1 = 0;
    static const Int32 RESPONSE_DATA_RFU_2 = 1;
    static const Int32 RESPONSE_DATA_FILE_SIZE_1 = 2;
    static const Int32 RESPONSE_DATA_FILE_SIZE_2 = 3;
    static const Int32 RESPONSE_DATA_FILE_ID_1 = 4;
    static const Int32 RESPONSE_DATA_FILE_ID_2 = 5;
    static const Int32 RESPONSE_DATA_FILE_TYPE = 6;
    static const Int32 RESPONSE_DATA_RFU_3 = 7;
    static const Int32 RESPONSE_DATA_ACCESS_CONDITION_1 = 8;
    static const Int32 RESPONSE_DATA_ACCESS_CONDITION_2 = 9;
    static const Int32 RESPONSE_DATA_ACCESS_CONDITION_3 = 10;
    static const Int32 RESPONSE_DATA_FILE_STATUS = 11;
    static const Int32 RESPONSE_DATA_LENGTH = 12;
    static const Int32 RESPONSE_DATA_STRUCTURE = 13;
    static const Int32 RESPONSE_DATA_RECORD_LENGTH = 14;
    //***** Events
    /** Finished retrieving size of transparent EF; start loading. */
    static const Int32 EVENT_GET_BINARY_SIZE_DONE = 4;
    /** Finished loading contents of transparent EF; post result. */
    static const Int32 EVENT_READ_BINARY_DONE = 5;
    /** Finished retrieving size of records for linear-fixed EF; now load. */
    static const Int32 EVENT_GET_RECORD_SIZE_DONE = 6;
    /** Finished loading single record from a linear-fixed EF; post result. */
    static const Int32 EVENT_READ_RECORD_DONE = 7;
    /** Finished retrieving record size; post result. */
    static const Int32 EVENT_GET_EF_LINEAR_RECORD_SIZE_DONE = 8;
    /** Finished retrieving image instance record; post result. */
    static const Int32 EVENT_READ_IMG_DONE = 9;
    /** Finished retrieving icon data; post result. */
    static const Int32 EVENT_READ_ICON_DONE = 10;
    /** Finished retrieving size of record for EFimg now. */
    static const Int32 EVENT_GET_RECORD_SIZE_IMG_DONE = 11;
    // member variables
    AutoPtr<ICommandsInterface> mCi;
    AutoPtr<IUiccCardApplication> mParentApp;
    String mAid;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCFILEHANDLER_H__

