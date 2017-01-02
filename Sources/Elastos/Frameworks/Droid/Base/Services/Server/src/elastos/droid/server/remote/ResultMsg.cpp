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

#include "remote/ResultMsg.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

const AutoPtr<ResultMsg> ResultMsg::RESULTMSG_NOERROR = new ResultMsg(1);

/** relay login errors */
const AutoPtr<ResultMsg> ResultMsg::ER_HTTPSTATUS_FAIL = new ResultMsg(11);
const AutoPtr<ResultMsg> ResultMsg::ER_LOGIN_EXCEPTION = new ResultMsg(12);
const AutoPtr<ResultMsg> ResultMsg::ER_DEVICE_OFFLINE = new ResultMsg(13);
const AutoPtr<ResultMsg> ResultMsg::ER_DEVICE_NOT_FOUND = new ResultMsg(14);
const AutoPtr<ResultMsg> ResultMsg::EC_ACCOUNTINFO_FAIL = new ResultMsg(31);// cant get account info from navigator
const AutoPtr<ResultMsg> ResultMsg::ER_LOGIN_PWD_FAIL = new ResultMsg(461);
const AutoPtr<ResultMsg> ResultMsg::ER_LOGINID_NOT_EXIST = new ResultMsg(464);

/** lan connect errors */
const AutoPtr<ResultMsg> ResultMsg::EC_CONNECT_EXCEPTION = new ResultMsg(32);
const AutoPtr<ResultMsg> ResultMsg::EC_NOT_CONNECTED = new ResultMsg(33);

/** cmd errors */
const AutoPtr<ResultMsg> ResultMsg::EC_SEND_CMD_EXCEPTION = new ResultMsg(34);
const AutoPtr<ResultMsg> ResultMsg::EC_CMD_RESULT_FAIL = new ResultMsg(35);

/** service errors */
const AutoPtr<ResultMsg> ResultMsg::ES_VERIFIEDFAIL = new ResultMsg(51);
const AutoPtr<ResultMsg> ResultMsg::ES_EXCEPTION = new ResultMsg(52);
const AutoPtr<ResultMsg> ResultMsg::ES_NORESPONSE = new ResultMsg(53);
const AutoPtr<ResultMsg> ResultMsg::ES_FACTORY_NOT_FOUND_EXCEPTION = new ResultMsg(54);
const AutoPtr<ResultMsg> ResultMsg::ES_METHOD_NOT_FOUND_EXCEPTION = new ResultMsg(55);
const AutoPtr<ResultMsg> ResultMsg::ES_ILLEGAL_ACCESS_EXCEPTION = new ResultMsg(56);
const AutoPtr<ResultMsg> ResultMsg::ES_INVOCATION_TARGET_EXCEPTION = new ResultMsg(57);
const AutoPtr<ResultMsg> ResultMsg::ES_INSTANTIATION_EXCEPTION = new ResultMsg(58);
const AutoPtr<ResultMsg> ResultMsg::ES_CMD_TIMEOUT = new ResultMsg(59);

/** unknow status */
const AutoPtr<ResultMsg> ResultMsg::UNKNOW = new ResultMsg(99);

ResultMsg::ResultMsg(
    /* [in] */ Int32 val)
    : mCode(val)
{
}

Int32 ResultMsg::ToInt32()
{
    return mCode;
}

AutoPtr<ResultMsg> ResultMsg::SetResult(
    /* [in] */ const String& str)
{
    mResult = str;
    return this;
}

String ResultMsg::GetResult()
{
    if(mCode == 1 && mResult != NULL)
        return mResult;
    return ToString();
}

String ResultMsg::ToString()
{
    return StringUtils::Int32ToString(mCode);
}

} // Remote
} // Server
} // Droid
} // Elastos
