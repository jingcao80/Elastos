#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ADNRECORDLOADER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ADNRECORDLOADER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"

// import java.util.ArrayList;
// import android.os.AsyncResult;
// import android.os.Handler;
// import android.os.Looper;
// import android.telephony.Rlog;
// import com.android.internal.telephony.uicc.IccConstants;

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

class AdnRecordLoader
    : public Handler
    , public IAdnRecordLoader
{
public:
    CAR_INTERFACE_DECL();

    AdnRecordLoader();
    //***** Constructor
    CARAPI constructor(
        /* [in] */ IIccFileHandler* fh);

    /**
      * Resulting AdnRecord is placed in response.obj.result
      * or response.obj.exception is set
      */
    virtual CARAPI LoadFromEF(
        /* [in] */ Int32 ef,
        /* [in] */ Int32 extensionEF,
        /* [in] */ Int32 recordNumber,
        /* [in] */ IMessage* response);

    /**
      * Resulting ArrayList&lt;adnRecord> is placed in response.obj.result
      * or response.obj.exception is set
      */
    virtual CARAPI LoadAllFromEF(
        /* [in] */ Int32 ef,
        /* [in] */ Int32 extensionEF,
        /* [in] */ IMessage* response);

    /**
      * Write adn to a EF SIM record
      * It will get the record size of EF record and compose hex adn array
      * then write the hex array to EF record
      *
      * @param adn is set with alphaTag and phone number
      * @param ef EF fileid
      * @param extensionEF extension EF fileid
      * @param recordNumber 1-based record index
      * @param pin2 for CHV2 operations, must be null if pin2 is not needed
      * @param response will be sent to its handler when completed
      */
    virtual CARAPI UpdateEF(
        /* [in] */ IAdnRecord* adn,
        /* [in] */ Int32 ef,
        /* [in] */ Int32 extensionEF,
        /* [in] */ Int32 recordNumber,
        /* [in] */ const String& pin2,
        /* [in] */ IMessage* response);

    //***** Overridden from Handler
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

private:
    CARAPI_(String) GetEFPath(
        /* [in] */ Int32 efid);

public:
    static const String LOGTAG;
    static const Boolean VDBG;
    Int32 mEf;
    Int32 mExtensionEF;
    Int32 mPendingExtLoads;
    AutoPtr<IMessage> mUserResponse;
    String mPin2;
    // For "load one"
    Int32 mRecordNumber;
    // for "load all"
    AutoPtr<IArrayList/*<AdnRecord> */> mAdns;
    // only valid after EVENT_ADN_LOAD_ALL_DONE
    // Either an AdnRecord or a reference to adns depending
    // if this is a load one or load all operation
    AutoPtr<IInterface> mResult;
    //***** Event Constants
    static const Int32 EVENT_ADN_LOAD_DONE = 1;
    static const Int32 EVENT_EXT_RECORD_LOAD_DONE = 2;
    static const Int32 EVENT_ADN_LOAD_ALL_DONE = 3;
    static const Int32 EVENT_EF_LINEAR_RECORD_SIZE_DONE = 4;
    static const Int32 EVENT_UPDATE_RECORD_DONE = 5;

private:
    //***** Instance Variables
    AutoPtr<IIccFileHandler> mFh;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ADNRECORDLOADER_H__

