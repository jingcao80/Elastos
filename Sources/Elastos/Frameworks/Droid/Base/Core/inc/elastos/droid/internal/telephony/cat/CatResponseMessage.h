#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATRESPONSEMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATRESPONSEMESSAGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/internal/telephony/cat/CommandDetails.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

class CatResponseMessage
    : public Object
    , public ICatResponseMessage
{
public:
    CAR_INTERFACE_DECL();

    CatResponseMessage();

    CARAPI constructor(
        /* [in] */ ICatCmdMessage* cmdMsg);

    virtual CARAPI SetResultCode(
        /* [in] */ ResultCode resCode);

    virtual CARAPI SetMenuSelection(
        /* [in] */ Int32 selection);

    virtual CARAPI SetInput(
        /* [in] */ const String& input);

    virtual CARAPI SetEventDownload(
        /* [in] */ Int32 event,
        /* [in] */ ArrayOf<Byte>* addedInfo);

    virtual CARAPI SetYesNo(
        /* [in] */ Boolean yesNo);

    virtual CARAPI SetConfirmation(
        /* [in] */ Boolean confirm);

    virtual CARAPI SetAdditionalInfo(
        /* [in] */ Int32 info);

    virtual CARAPI GetCmdDetails(
        /* [out] */ CommandDetails** result);

public:
    AutoPtr<CommandDetails> mCmdDet;
    ResultCode mResCode;
    Int32 mUsersMenuSelection;
    String mUsersInput;
    Boolean mUsersYesNoSelection;
    Boolean mUsersConfirm;
    Boolean mIncludeAdditionalInfo;
    Int32 mAdditionalInfo;
    Int32 mEventValue;
    AutoPtr<ArrayOf<Byte> > mAddedInfo;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_CATRESPONSEMESSAGE_H__
