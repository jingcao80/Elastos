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

public class CatResponseMessage {
        CommandDetails mCmdDet = NULL;
        ResultCode mResCode  = ResultCode.OK;
        Int32 mUsersMenuSelection = 0;
        String mUsersInput  = NULL;
        Boolean mUsersYesNoSelection = FALSE;
        Boolean mUsersConfirm = FALSE;
        Boolean mIncludeAdditionalInfo = FALSE;
        Int32 mAdditionalInfo = 0;
        Int32 mEventValue = -1;
        Byte[] mAddedInfo = NULL;

        public CatResponseMessage(CatCmdMessage cmdMsg) {
            mCmdDet = cmdMsg.mCmdDet;
        }

        CARAPI SetResultCode(ResultCode resCode) {
            mResCode = resCode;
        }

        CARAPI SetMenuSelection(Int32 selection) {
            mUsersMenuSelection = selection;
        }

        CARAPI SetInput(String input) {
            mUsersInput = input;
        }

        CARAPI SetEventDownload(Int32 event, Byte[] addedInfo) {
            this.mEventValue = event;
            this.mAddedInfo = addedInfo;
        }

        CARAPI SetYesNo(Boolean yesNo) {
            mUsersYesNoSelection = yesNo;
        }

        CARAPI SetConfirmation(Boolean confirm) {
            mUsersConfirm = confirm;
        }

        CARAPI SetAdditionalInfo(Int32 info) {
            mIncludeAdditionalInfo = TRUE;
            mAdditionalInfo = info;
        }

        CommandDetails GetCmdDetails() {
            return mCmdDet;
        }
    }
