//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    Boolean flag;
    in->ReadBoolean(&flag);
    mCmdDet = NULL;
    if (flag) {
        AutoPtr<IInterface> obj;
        in->ReadInterfacePtr((HANDLE*)&obj);
        mCmdDet = (CommandDetails*)IParcelable::Probe(obj);
    }

    in->ReadBoolean(&flag);
    mTextMsg = NULL;
    if (flag) {
        AutoPtr<IInterface> obj;
        in->ReadInterfacePtr((HANDLE*)&obj);
        mTextMsg = ITextMessage::Probe(obj);
    }

    in->ReadBoolean(&flag);
    mMenu = NULL;
    if (flag) {
        AutoPtr<IInterface> obj;
        in->ReadInterfacePtr((HANDLE*)&obj);
        mMenu = IMenu::Probe(obj);
    }

    in->ReadBoolean(&flag);
    mInput = NULL;
    if (flag) {
        AutoPtr<IInterface> obj;
        in->ReadInterfacePtr((HANDLE*)&obj);
        mInput = IInput::Probe(obj);
    }

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
        in->ReadBoolean(&flag);
        mToneSettings = NULL;
        if (flag) {
            AutoPtr<IInterface> obj;
            in->ReadInterfacePtr((HANDLE*)&obj);
            mToneSettings = IToneSettings::Probe(obj);
        }
        break;
    }
    case SET_UP_CALL: {
        mCallSettings = new CallSettings();
        in->ReadBoolean(&flag);
        AutoPtr<ITextMessage> cfm;
        if (flag) {
            AutoPtr<IInterface> obj;
            in->ReadInterfacePtr((HANDLE*)&obj);
            cfm = ITextMessage::Probe(obj);
        }
        mCallSettings->mConfirmMsg = cfm;

        AutoPtr<ITextMessage> cm;
        in->ReadBoolean(&flag);
        if (flag) {
            AutoPtr<IInterface> obj;
            in->ReadInterfacePtr((HANDLE*)&obj);
            cm = ITextMessage::Probe(obj);
        }
        mCallSettings->mCallMsg = cm;
        break;
    }
    case SET_UP_EVENT_LIST: {
        mSetupEventListSettings = new SetupEventListSettings();
        AutoPtr<ArrayOf<Int32> > eventList;
        in->ReadArrayOf((HANDLE*)(&eventList));
        mSetupEventListSettings->mEventList = eventList;
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
    if (mCmdDet == NULL) {
        dest->WriteBoolean(FALSE);
    }
    else {
        dest->WriteBoolean(TRUE);
        IParcelable::Probe(mCmdDet)->WriteToParcel(dest);
    }

    if (mTextMsg == NULL) {
        dest->WriteBoolean(FALSE);
    }
    else {
        dest->WriteBoolean(TRUE);
        IParcelable::Probe(mTextMsg)->WriteToParcel(dest);
    }

    if (mMenu == NULL) {
        dest->WriteBoolean(FALSE);
    }
    else {
        dest->WriteBoolean(TRUE);
        IParcelable::Probe(mMenu)->WriteToParcel(dest);
    }

    if (mInput == NULL) {
        dest->WriteBoolean(FALSE);
    }
    else {
        dest->WriteBoolean(TRUE);
        IParcelable::Probe(mInput)->WriteToParcel(dest);
    }
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
        if (mToneSettings == NULL) {
            dest->WriteBoolean(FALSE);
        }
        else {
            dest->WriteBoolean(TRUE);
            IParcelable::Probe(mToneSettings)->WriteToParcel(dest);
        }
        break;
    }
    case SET_UP_CALL: {
        AutoPtr<ITextMessage> confirmMsg;
        mCallSettings->GetConfirmMsg((ITextMessage**)&confirmMsg);
        if (confirmMsg == NULL) {
            dest->WriteBoolean(FALSE);
        }
        else {
            dest->WriteBoolean(TRUE);
            IParcelable::Probe(confirmMsg)->WriteToParcel(dest);
        }
        AutoPtr<ITextMessage> callMsg;
        mCallSettings->GetConfirmMsg((ITextMessage**)&callMsg);
        if (callMsg == NULL) {
            dest->WriteBoolean(FALSE);
        }
        else {
            dest->WriteBoolean(TRUE);
            IParcelable::Probe(callMsg)->WriteToParcel(dest);
        }
        break;
    }
    case SET_UP_EVENT_LIST: {
        AutoPtr<ArrayOf<Int32> > eventList;
        mSetupEventListSettings->GetEventList((ArrayOf<Int32>**)&eventList);
        dest->WriteArrayOf((HANDLE)eventList.Get());
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
