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

using Elastos::Droid::Graphics::IBitmap;

/**
 * Container class for proactive command parameters.
 *
 */
class CommandParams {
    CommandDetails mCmdDet;
 // Variable to track if an optional icon load has failed.
    Boolean mLoadIconFailed = FALSE;

    CommandParams(CommandDetails cmdDet) {
        mCmdDet = cmdDet;
    }

    AppInterface.CommandType GetCommandType() {
        return AppInterface.CommandType->FromInt(mCmdDet.typeOfCommand);
    }

    Boolean SetIcon(Bitmap icon) { return TRUE; }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return mCmdDet->ToString();
    }
}

class DisplayTextParams extends CommandParams {
    TextMessage mTextMsg;

    DisplayTextParams(CommandDetails cmdDet, TextMessage textMsg) {
        Super(cmdDet);
        mTextMsg = textMsg;
    }

    //@Override
    Boolean SetIcon(Bitmap icon) {
        If (icon != NULL && mTextMsg != NULL) {
            mTextMsg.icon = icon;
            return TRUE;
        }
        return FALSE;
    }
}

class LaunchBrowserParams extends CommandParams {
    TextMessage mConfirmMsg;
    LaunchBrowserMode mMode;
    String mUrl;

    LaunchBrowserParams(CommandDetails cmdDet, TextMessage confirmMsg,
            String url, LaunchBrowserMode mode) {
        Super(cmdDet);
        mConfirmMsg = confirmMsg;
        mMode = mode;
        mUrl = url;
    }

    //@Override
    Boolean SetIcon(Bitmap icon) {
        If (icon != NULL && mConfirmMsg != NULL) {
            mConfirmMsg.icon = icon;
            return TRUE;
        }
        return FALSE;
    }
}

class SetEventListParams extends CommandParams {
    Int32[] mEventInfo;
    SetEventListParams(CommandDetails cmdDet, Int32[] eventInfo) {
        Super(cmdDet);
        this.mEventInfo = eventInfo;
    }
}

class PlayToneParams extends CommandParams {
    TextMessage mTextMsg;
    ToneSettings mSettings;

    PlayToneParams(CommandDetails cmdDet, TextMessage textMsg,
            Tone tone, Duration duration, Boolean vibrate) {
        Super(cmdDet);
        mTextMsg = textMsg;
        mSettings = new ToneSettings(duration, tone, vibrate);
    }

    //@Override
    Boolean SetIcon(Bitmap icon) {
        If (icon != NULL && mTextMsg != NULL) {
            mTextMsg.icon = icon;
            return TRUE;
        }
        return FALSE;
    }
}

class CallSetupParams extends CommandParams {
    TextMessage mConfirmMsg;
    TextMessage mCallMsg;

    CallSetupParams(CommandDetails cmdDet, TextMessage confirmMsg,
            TextMessage callMsg) {
        Super(cmdDet);
        mConfirmMsg = confirmMsg;
        mCallMsg = callMsg;
    }

    //@Override
    Boolean SetIcon(Bitmap icon) {
        If (icon == NULL) {
            return FALSE;
        }
        If (mConfirmMsg != NULL && mConfirmMsg.icon == NULL) {
            mConfirmMsg.icon = icon;
            return TRUE;
        } else If (mCallMsg != NULL && mCallMsg.icon == NULL) {
            mCallMsg.icon = icon;
            return TRUE;
        }
        return FALSE;
    }
}

class SelectItemParams extends CommandParams {
    Menu mMenu = NULL;
    Boolean mLoadTitleIcon = FALSE;

    SelectItemParams(CommandDetails cmdDet, Menu menu, Boolean loadTitleIcon) {
        Super(cmdDet);
        mMenu = menu;
        mLoadTitleIcon = loadTitleIcon;
    }

    //@Override
    Boolean SetIcon(Bitmap icon) {
        If (icon != NULL && mMenu != NULL) {
            If (mLoadTitleIcon && mMenu.titleIcon == NULL) {
                mMenu.titleIcon = icon;
            } else {
                For (Item item : mMenu.items) {
                    If (item.icon != NULL) {
                        continue;
                    }
                    item.icon = icon;
                    break;
                }
            }
            return TRUE;
        }
        return FALSE;
    }
}

class GetInputParams extends CommandParams {
    Input mInput = NULL;

    GetInputParams(CommandDetails cmdDet, Input input) {
        Super(cmdDet);
        mInput = input;
    }

    //@Override
    Boolean SetIcon(Bitmap icon) {
        If (icon != NULL && mInput != NULL) {
            mInput.icon = icon;
        }
        return TRUE;
    }
}

/*
 * BIP (Bearer Independent Protocol) is the mechanism for SIM card applications
 * to access data connection through the mobile device.
 *
 * SIM utilizes proactive Commands (OPEN CHANNEL, CLOSE CHANNEL, SEND DATA and
 * RECEIVE DATA to control/read/write data for BIP. Refer to ETSI TS 102 223 for
 * the details of proactive commands procedures and their structures.
 */
class BIPClientParams extends CommandParams {
    TextMessage mTextMsg;
    Boolean mHasAlphaId;

    BIPClientParams(CommandDetails cmdDet, TextMessage textMsg, Boolean has_alpha_id) {
        Super(cmdDet);
        mTextMsg = textMsg;
        mHasAlphaId = has_alpha_id;
    }

    //@Override
    Boolean SetIcon(Bitmap icon) {
        If (icon != NULL && mTextMsg != NULL) {
            mTextMsg.icon = icon;
            return TRUE;
        }
        return FALSE;
    }
}
class ActivateParams extends CommandParams {
    Int32 mActivateTarget;


    ActivateParams(CommandDetails cmdDet, Int32 target) {
        Super(cmdDet);
        mActivateTarget = target;
    }
}

// Samsung STK
class SendSMSParams extends DisplayTextParams {
    String pdu;
    String smscAddress;

    SendSMSParams(CommandDetails cmdDet, TextMessage textmessage, String smscaddress, String smsPdu) {
        Super(cmdDet, textmessage);
        smscAddress = smscaddress;
        pdu = smsPdu;
    }
}

class SendUSSDParams extends DisplayTextParams {
    String ussdString;

    SendUSSDParams(CommandDetails cmdDet, TextMessage textmessage, String ussdstring) {
        Super(cmdDet, textmessage);
        ussdString = ussdstring;
    }
}
