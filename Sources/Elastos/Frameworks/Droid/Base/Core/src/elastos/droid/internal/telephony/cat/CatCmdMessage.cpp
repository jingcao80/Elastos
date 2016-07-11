
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/CatCmdMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                CatCmdMessage::BrowserSettings
//=====================================================================
CAR_INTERFACE_IMPL(CatCmdMessage::BrowserSettings, Object, IBrowserSettings);

ECode CatCmdMessage::BrowserSettings::SetUrl(
    /* [in] */ const String& _url)
{
    mUrl = _url;
    return NOERROR;
}

ECode CatCmdMessage::BrowserSettings::GetUrl(
    /* [out] */ String* _url)
{
    VALIDATE_NOT_NULL(_url);
    *_url = mUrl;
    return NOERROR;
}

ECode CatCmdMessage::BrowserSettings::SetMode(
    /* [in] */ LaunchBrowserMode _mode)
{
    mMode = _mode;
    return NOERROR;
}

ECode CatCmdMessage::BrowserSettings::GetMode(
    /* [out] */ LaunchBrowserMode* _mode)
{
    VALIDATE_NOT_NULL(_mode);
    *_mode = mMode;
    return NOERROR;
}

//=====================================================================
//                CatCmdMessage::CallSettings
//=====================================================================
CAR_INTERFACE_IMPL(CatCmdMessage::CallSettings, Object, ICallSettings);

ECode CatCmdMessage::CallSettings::SetConfirmMsg(
    /* [in] */ ITextMessage* _confirmMsg)
{
    mConfirmMsg = _confirmMsg;
    return NOERROR;
}

ECode CatCmdMessage::CallSettings::GetConfirmMsg(
    /* [out] */ ITextMessage** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mConfirmMsg;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CatCmdMessage::CallSettings::SetCallMsg(
    /* [in] */ ITextMessage* _callMsg)
{
    mCallMsg = _callMsg;
    return NOERROR;
}

ECode CatCmdMessage::CallSettings::GetCallMsg(
    /* [out] */ ITextMessage** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCallMsg;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//=====================================================================
//                CatCmdMessage::SetupEventListSettings
//=====================================================================
CAR_INTERFACE_IMPL(CatCmdMessage::SetupEventListSettings, Object, ISetupEventListSettings);

ECode CatCmdMessage::SetupEventListSettings::SetEventList(
    /* [in] */ ArrayOf<Int32>* _eventList)
{
    mEventList = _eventList;
    return NOERROR;
}

ECode CatCmdMessage::SetupEventListSettings::GetEventList(
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEventList;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

////=====================================================================
////                CatCmdMessage::InnerParcelableCreator
////=====================================================================
//CatCmdMessage::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ CatCmdMessage* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode CatCmdMessage::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ CatCmdMessage** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new CatCmdMessage(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode CatCmdMessage::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ CatCmdMessage[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new CatCmdMessage[size];
//    assert(0);
//    return NOERROR;
//}

//=====================================================================
//                            CatCmdMessage
//=====================================================================
CAR_INTERFACE_IMPL_2(CatCmdMessage, Object, ICatCmdMessage, IParcelable);

const Int32 CatCmdMessage::REFRESH_NAA_INIT_AND_FULL_FILE_CHANGE;
const Int32 CatCmdMessage::REFRESH_NAA_INIT_AND_FILE_CHANGE;
const Int32 CatCmdMessage::REFRESH_NAA_INIT;
const Int32 CatCmdMessage::REFRESH_UICC_RESET;
//const AutoPtr<IParcelable> AutoPtr< ::Creator<CatCmdMessage> > CatCmdMessage::CREATOR = new InnerParcelableCreator(this);

CatCmdMessage::CatCmdMessage()
{
}

ECode CatCmdMessage::constructor()
{
    return NOERROR;
}

ECode CatCmdMessage::constructor(
    /* [in] */ /*CommandParams*/IInterface* _cmdParams)
{
    AutoPtr<CommandParams> cmdParams = (CommandParams*)(IObject::Probe(_cmdParams));
    mCmdDet = cmdParams->mCmdDet;
    mLoadIconFailed = cmdParams->mLoadIconFailed;
    CommandType type;
    GetCmdType(&type);
    switch(type) {
    case SET_UP_MENU:
    case SELECT_ITEM: {
        mMenu = ((SelectItemParams*) cmdParams.Get())->mMenu;
        break;
    }
    case DISPLAY_TEXT:
    case SET_UP_IDLE_MODE_TEXT:
    case SEND_DTMF:
    case SEND_SMS:
    case REFRESH:
    case SEND_SS:
    case SEND_USSD: {
        mTextMsg = ((DisplayTextParams*) cmdParams.Get())->mTextMsg;
        break;
    }
    case GET_INPUT:
    case GET_INKEY: {
        mInput = ((GetInputParams*) cmdParams.Get())->mInput;
        break;
    }
    case LAUNCH_BROWSER: {
        mTextMsg = ((LaunchBrowserParams*) cmdParams.Get())->mConfirmMsg;
        mBrowserSettings = new BrowserSettings();
        mBrowserSettings->SetUrl(((LaunchBrowserParams*) cmdParams.Get())->mUrl);
        mBrowserSettings->SetMode(((LaunchBrowserParams*) cmdParams.Get())->mMode);
        break;
    }
    case PLAY_TONE: {
        AutoPtr<PlayToneParams> params = (PlayToneParams*) cmdParams.Get();
        mToneSettings = params->mSettings;
        mTextMsg = params->mTextMsg;
        break;
    }
    case GET_CHANNEL_STATUS: {
        mTextMsg = ((CallSetupParams*) cmdParams.Get())->mConfirmMsg;
        break;
    }
    case SET_UP_CALL: {
        mCallSettings = new CallSettings();
        mCallSettings->SetConfirmMsg(((CallSetupParams*) cmdParams.Get())->mConfirmMsg);
        mCallSettings->SetCallMsg(((CallSetupParams*) cmdParams.Get())->mCallMsg);
        break;
    }
    case OPEN_CHANNEL:
    case CLOSE_CHANNEL:
    case RECEIVE_DATA:
    case SEND_DATA: {
        AutoPtr<BIPClientParams> param = (BIPClientParams*) cmdParams.Get();
        mTextMsg = param->mTextMsg;
        break;
    }
    case SET_UP_EVENT_LIST: {
        mSetupEventListSettings = new SetupEventListSettings();
        mSetupEventListSettings->SetEventList(((SetEventListParams*) cmdParams.Get())->mEventInfo);
        break;
    }
    case ACTIVATE:
    case PROVIDE_LOCAL_INFORMATION:
    default:
        break;
    }
    return NOERROR;
}

ECode CatCmdMessage::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    assert(0 && "TODO");
    // in->ReadParcelable(NULL, &mCmdDet);
    // in->ReadParcelable(NULL, &mTextMsg);
    // in->ReadParcelable(NULL, &mMenu);
    // in->ReadParcelable(NULL, &mInput);
    Byte num = 0;
    in->ReadByte(&num);
    mLoadIconFailed = (num == 1);
    Int32 type = 0;
    GetCmdType(&type);
    switch (type) {
    case LAUNCH_BROWSER: {
        mBrowserSettings = new BrowserSettings();
        String str;
        in->ReadString(&str);
        mBrowserSettings->SetUrl(str);
        Int32 mode = 0;
        in->ReadInt32(&mode);
        mBrowserSettings->SetMode(mode);
        break;
    }
    case PLAY_TONE: {
        // mToneSettings = in->ReadParcelable(NULL);
        break;
    }
    case SET_UP_CALL: {
        mCallSettings = new CallSettings();
        // in->ReadParcelable(NULL, &(mCallSettings->mConfirmMsg));
        // in->ReadParcelable(NULL, &(mCallSettings->mCallMsg));
        break;
    }
    case SET_UP_EVENT_LIST: {
        mSetupEventListSettings = new SetupEventListSettings();
        Int32 length = 0;
        in->ReadInt32(&length);
        AutoPtr<ArrayOf<Int32> > eventList;
        mSetupEventListSettings->GetEventList((ArrayOf<Int32>**)&eventList);
        eventList = ArrayOf<Int32>::Alloc(length);
        for (Int32 i = 0; i < length; i++) {
            Int32 r = 0;
            in->ReadInt32(&r);
            (*eventList)[i] = r;
        }
        break;
    }
    default:
        break;
    }
    return NOERROR;
}

ECode CatCmdMessage::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    IParcelable::Probe(mCmdDet)->WriteToParcel(dest);
    IParcelable::Probe(mTextMsg)->WriteToParcel(dest);
    IParcelable::Probe(mMenu)->WriteToParcel(dest);
    IParcelable::Probe(mInput)->WriteToParcel(dest);
    dest->WriteByte((Byte) (mLoadIconFailed ? 1 : 0));
    Int32 type = 0;
    GetCmdType(&type);
    switch(type) {
    case LAUNCH_BROWSER: {
        String url;
        mBrowserSettings->GetUrl(&url);
        dest->WriteString(url);
        LaunchBrowserMode mode;
        mBrowserSettings->GetMode(&mode);
        dest->WriteInt32(mode);
        break;
    }
    case PLAY_TONE: {
        IParcelable::Probe(mToneSettings)->WriteToParcel(dest);
        break;
    }
    case SET_UP_CALL: {
        AutoPtr<ITextMessage> confirmMsg;
        mCallSettings->GetConfirmMsg((ITextMessage**)&confirmMsg);
        IParcelable::Probe(confirmMsg)->WriteToParcel(dest);
        AutoPtr<ITextMessage> callMsg;
        mCallSettings->GetConfirmMsg((ITextMessage**)&callMsg);
        IParcelable::Probe(callMsg)->WriteToParcel(dest);
        break;
    }
    case SET_UP_EVENT_LIST: {
        AutoPtr<ArrayOf<Int32> > eventList;
        mSetupEventListSettings->GetEventList((ArrayOf<Int32>**)&eventList);
        assert(0 && "TODO");
        // dest->Write(eventList);
        break;
    }
    default:
        break;
    }
    return NOERROR;
}

ECode CatCmdMessage::GetCmdType(
    /* [out] */ CommandType* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCmdDet->mTypeOfCommand;
    return NOERROR;
}

ECode CatCmdMessage::GetMenu(
    /* [out] */ IMenu** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMenu;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CatCmdMessage::GeInput(
    /* [out] */ IInput** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInput;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CatCmdMessage::GeTextMessage(
    /* [out] */ ITextMessage** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTextMsg;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CatCmdMessage::GetBrowserSettings(
    /* [out] */ IBrowserSettings** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBrowserSettings;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CatCmdMessage::GetToneSettings(
    /* [out] */ IToneSettings** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mToneSettings;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CatCmdMessage::GetCallSettings(
    /* [out] */ ICallSettings** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCallSettings;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CatCmdMessage::HasIconLoadFailed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLoadIconFailed;
    return NOERROR;
}

ECode CatCmdMessage::IsRefreshResetOrInit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if ((mCmdDet->mCommandQualifier == REFRESH_NAA_INIT_AND_FULL_FILE_CHANGE)
        || (mCmdDet->mCommandQualifier == REFRESH_NAA_INIT_AND_FILE_CHANGE )
        || (mCmdDet->mCommandQualifier == REFRESH_NAA_INIT)
        || (mCmdDet->mCommandQualifier == REFRESH_UICC_RESET)) {
        *result = TRUE;
        return NOERROR;
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
}

ECode CatCmdMessage::GetSetEventList(
    /* [out] */ ISetupEventListSettings** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSetupEventListSettings;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
