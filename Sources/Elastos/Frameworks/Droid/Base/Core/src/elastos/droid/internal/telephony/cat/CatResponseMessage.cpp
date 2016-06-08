
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CatResponseMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                          CatResponseMessage
//=====================================================================
CAR_INTERFACE_IMPL(CatResponseMessage, Object, ICatResponseMessage);

CatResponseMessage::CatResponseMessage()
{
}

ECode CatResponseMessage::constructor(
    /* [in] */ ICatCmdMessage* cmdMsg)
{
    // ==================before translated======================
    // mCmdDet = cmdMsg.mCmdDet;
    return NOERROR;
}

ECode CatResponseMessage::SetResultCode(
    /* [in] */ ResultCode resCode)
{
    // ==================before translated======================
    // mResCode = resCode;
    assert(0);
    return NOERROR;
}

ECode CatResponseMessage::SetMenuSelection(
    /* [in] */ Int32 selection)
{
    // ==================before translated======================
    // mUsersMenuSelection = selection;
    assert(0);
    return NOERROR;
}

ECode CatResponseMessage::SetInput(
    /* [in] */ const String& input)
{
    // ==================before translated======================
    // mUsersInput = input;
    assert(0);
    return NOERROR;
}

ECode CatResponseMessage::SetEventDownload(
    /* [in] */ Int32 event,
    /* [in] */ ArrayOf<Byte>* addedInfo)
{
    // ==================before translated======================
    // this.mEventValue = event;
    // this.mAddedInfo = addedInfo;
    assert(0);
    return NOERROR;
}

ECode CatResponseMessage::SetYesNo(
    /* [in] */ Boolean yesNo)
{
    // ==================before translated======================
    // mUsersYesNoSelection = yesNo;
    assert(0);
    return NOERROR;
}

ECode CatResponseMessage::SetConfirmation(
    /* [in] */ Boolean confirm)
{
    // ==================before translated======================
    // mUsersConfirm = confirm;
    assert(0);
    return NOERROR;
}

ECode CatResponseMessage::SetAdditionalInfo(
    /* [in] */ Int32 info)
{
    // ==================before translated======================
    // mIncludeAdditionalInfo = true;
    // mAdditionalInfo = info;
    assert(0);
    return NOERROR;
}

ECode CatResponseMessage::GetCmdDetails(
    /* [out] */ CommandDetails** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCmdDet;
    assert(0);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
