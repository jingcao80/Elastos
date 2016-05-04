/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.cat;

using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;

/**
 * Class used to pass CAT messages from telephony to application. Application
 * should call GetXXX() to get commands's specific values.
 *
 */
public class CatCmdMessage implements Parcelable {
    // members
    CommandDetails mCmdDet;
    private TextMessage mTextMsg;
    private Menu mMenu;
    private Input mInput;
    private BrowserSettings mBrowserSettings = NULL;
    private ToneSettings mToneSettings = NULL;
    private CallSettings mCallSettings = NULL;
    private SetupEventListSettings mSetupEventListSettings = NULL;
    private Boolean mLoadIconFailed = FALSE;

    // Command Qualifier values for refresh command
    static const Int32 REFRESH_NAA_INIT_AND_FULL_FILE_CHANGE  = 0x00;
    static const Int32 REFRESH_NAA_INIT_AND_FILE_CHANGE       = 0x02;
    static const Int32 REFRESH_NAA_INIT                       = 0x03;
    static const Int32 REFRESH_UICC_RESET                     = 0x04;
    /*
     * Container for Launch Browser command settings.
     */
    public class BrowserSettings {
        public String url;
        public LaunchBrowserMode mode;
    }

    /*
     * Container for Call Setup command settings.
     */
    public class CallSettings {
        public TextMessage confirmMsg;
        public TextMessage callMsg;
    }

    public class SetupEventListSettings {
        public Int32[] eventList;
    }

    public class SetupEventListConstants {
        // Event values in SETUP_EVENT_LIST Proactive Command as per ETSI 102.223
        public static const Int32 USER_ACTIVITY_EVENT          = 0x04;
        public static const Int32 IDLE_SCREEN_AVAILABLE_EVENT  = 0x05;
        public static const Int32 LANGUAGE_SELECTION_EVENT     = 0x07;
        public static const Int32 BROWSER_TERMINATION_EVENT    = 0x08;
        public static const Int32 BROWSING_STATUS_EVENT        = 0x0F;
        public static const Int32 HCI_CONNECTIVITY_EVENT       = 0x13;
    }

    public class BrowserTerminationCauses {
        public static const Int32 USER_TERMINATION             = 0x00;
        public static const Int32 ERROR_TERMINATION            = 0x01;
    }

    CatCmdMessage(CommandParams cmdParams) {
        mCmdDet = cmdParams.mCmdDet;
        mLoadIconFailed =  cmdParams.mLoadIconFailed;
        Switch(GetCmdType()) {
        case SET_UP_MENU:
        case SELECT_ITEM:
            mMenu = ((SelectItemParams) cmdParams).mMenu;
            break;
        case DISPLAY_TEXT:
        case SET_UP_IDLE_MODE_TEXT:
        case SEND_DTMF:
        case SEND_SMS:
        case REFRESH:
        case SEND_SS:
        case SEND_USSD:
            mTextMsg = ((DisplayTextParams) cmdParams).mTextMsg;
            break;
        case GET_INPUT:
        case GET_INKEY:
            mInput = ((GetInputParams) cmdParams).mInput;
            break;
        case LAUNCH_BROWSER:
            mTextMsg = ((LaunchBrowserParams) cmdParams).mConfirmMsg;
            mBrowserSettings = new BrowserSettings();
            mBrowserSettings.url = ((LaunchBrowserParams) cmdParams).mUrl;
            mBrowserSettings.mode = ((LaunchBrowserParams) cmdParams).mMode;
            break;
        case PLAY_TONE:
            PlayToneParams params = (PlayToneParams) cmdParams;
            mToneSettings = params.mSettings;
            mTextMsg = params.mTextMsg;
            break;
        case GET_CHANNEL_STATUS:
            mTextMsg = ((CallSetupParams) cmdParams).mConfirmMsg;
            break;
        case SET_UP_CALL:
            mCallSettings = new CallSettings();
            mCallSettings.confirmMsg = ((CallSetupParams) cmdParams).mConfirmMsg;
            mCallSettings.callMsg = ((CallSetupParams) cmdParams).mCallMsg;
            break;
        case OPEN_CHANNEL:
        case CLOSE_CHANNEL:
        case RECEIVE_DATA:
        case SEND_DATA:
            BIPClientParams param = (BIPClientParams) cmdParams;
            mTextMsg = param.mTextMsg;
            break;
        case SET_UP_EVENT_LIST:
            mSetupEventListSettings = new SetupEventListSettings();
            mSetupEventListSettings.eventList = ((SetEventListParams) cmdParams).mEventInfo;
            break;
        case ACTIVATE:
        case PROVIDE_LOCAL_INFORMATION:
        default:
            break;
        }
    }

    public CatCmdMessage(Parcel in) {
        mCmdDet = in->ReadParcelable(NULL);
        mTextMsg = in->ReadParcelable(NULL);
        mMenu = in->ReadParcelable(NULL);
        mInput = in->ReadParcelable(NULL);
        mLoadIconFailed = (in->ReadByte() == 1);
        Switch (GetCmdType()) {
        case LAUNCH_BROWSER:
            mBrowserSettings = new BrowserSettings();
            mBrowserSettings.url = in->ReadString();
            mBrowserSettings.mode = LaunchBrowserMode->Values()[in->ReadInt()];
            break;
        case PLAY_TONE:
            mToneSettings = in->ReadParcelable(NULL);
            break;
        case SET_UP_CALL:
            mCallSettings = new CallSettings();
            mCallSettings.confirmMsg = in->ReadParcelable(NULL);
            mCallSettings.callMsg = in->ReadParcelable(NULL);
            break;
        case SET_UP_EVENT_LIST:
            mSetupEventListSettings = new SetupEventListSettings();
            Int32 length = in->ReadInt();
            mSetupEventListSettings.eventList = new Int32[length];
            For (Int32 i = 0; i < length; i++) {
                mSetupEventListSettings.eventList[i] = in->ReadInt();
            }
            break;
        default:
            break;
        }
    }

    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteParcelable(mCmdDet, 0);
        dest->WriteParcelable(mTextMsg, 0);
        dest->WriteParcelable(mMenu, 0);
        dest->WriteParcelable(mInput, 0);
        dest->WriteByte((Byte) (mLoadIconFailed ? 1 : 0));
        Switch(GetCmdType()) {
        case LAUNCH_BROWSER:
            dest->WriteString(mBrowserSettings.url);
            dest->WriteInt(mBrowserSettings.mode->Ordinal());
            break;
        case PLAY_TONE:
            dest->WriteParcelable(mToneSettings, 0);
            break;
        case SET_UP_CALL:
            dest->WriteParcelable(mCallSettings.confirmMsg, 0);
            dest->WriteParcelable(mCallSettings.callMsg, 0);
            break;
        case SET_UP_EVENT_LIST:
            dest->WriteIntArray(mSetupEventListSettings.eventList);
            break;
        default:
            break;
        }
    }

    public static const Parcelable.Creator<CatCmdMessage> CREATOR = new Parcelable.Creator<CatCmdMessage>() {
        //@Override
        public CatCmdMessage CreateFromParcel(Parcel in) {
            return new CatCmdMessage(in);
        }

        //@Override
        public CatCmdMessage[] NewArray(Int32 size) {
            return new CatCmdMessage[size];
        }
    };

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    /* external API to be used by application */
    public AppInterface.CommandType GetCmdType() {
        return AppInterface.CommandType->FromInt(mCmdDet.typeOfCommand);
    }

    public Menu GetMenu() {
        return mMenu;
    }

    public Input GeInput() {
        return mInput;
    }

    public TextMessage GeTextMessage() {
        return mTextMsg;
    }

    public BrowserSettings GetBrowserSettings() {
        return mBrowserSettings;
    }

    public ToneSettings GetToneSettings() {
        return mToneSettings;
    }

    public CallSettings GetCallSettings() {
        return mCallSettings;
    }

    /**
     * API to be used by application to check if loading optional icon
     * has failed
     */
    public Boolean HasIconLoadFailed() {
        return mLoadIconFailed;
    }

    public Boolean IsRefreshResetOrInit() {
        If ((mCmdDet.commandQualifier == REFRESH_NAA_INIT_AND_FULL_FILE_CHANGE)
            || (mCmdDet.commandQualifier == REFRESH_NAA_INIT_AND_FILE_CHANGE )
            || (mCmdDet.commandQualifier == REFRESH_NAA_INIT)
            || (mCmdDet.commandQualifier == REFRESH_UICC_RESET)) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    public SetupEventListSettings GetSetEventList() {
        return mSetupEventListSettings;
    }
}
