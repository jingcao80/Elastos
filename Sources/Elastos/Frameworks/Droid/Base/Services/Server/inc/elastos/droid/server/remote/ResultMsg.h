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

#ifndef __ELASTOS_DROID_SERVER_REMOTE_RESULTMSG_H__
#define __ELASTOS_DROID_SERVER_REMOTE_RESULTMSG_H__

#include <elastos.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {


/**
 * this class define the message for the cmd result
 *
 * Error on Relay: ER_Error
 * Error on Service: ES_Error
 * Error on Client: EC_Error
 */
class ResultMsg : public ElRefBase
{
public:
    ResultMsg(
        /* [in] */ Int32 val);

    CARAPI_(Int32) ToInt32();

    CARAPI_(AutoPtr<ResultMsg>) SetResult(
        /* [in] */ const String& str);

    CARAPI_(String) GetResult();

    CARAPI_(String) ToString();

public:
    static const AutoPtr<ResultMsg> RESULTMSG_NOERROR;

    /** relay login errors */
    static const AutoPtr<ResultMsg> ER_HTTPSTATUS_FAIL;
    static const AutoPtr<ResultMsg> ER_LOGIN_EXCEPTION;
    static const AutoPtr<ResultMsg> ER_DEVICE_OFFLINE;
    static const AutoPtr<ResultMsg> ER_DEVICE_NOT_FOUND;
    static const AutoPtr<ResultMsg> EC_ACCOUNTINFO_FAIL;// cant get account info from navigator
    static const AutoPtr<ResultMsg> ER_LOGIN_PWD_FAIL;
    static const AutoPtr<ResultMsg> ER_LOGINID_NOT_EXIST;

    /** lan connect errors */
    static const AutoPtr<ResultMsg> EC_CONNECT_EXCEPTION;
    static const AutoPtr<ResultMsg> EC_NOT_CONNECTED;

    /** cmd errors */
    static const AutoPtr<ResultMsg> EC_SEND_CMD_EXCEPTION;
    static const AutoPtr<ResultMsg> EC_CMD_RESULT_FAIL;

    /** service errors */
    static const AutoPtr<ResultMsg> ES_VERIFIEDFAIL;
    static const AutoPtr<ResultMsg> ES_EXCEPTION;
    static const AutoPtr<ResultMsg> ES_NORESPONSE;
    static const AutoPtr<ResultMsg> ES_FACTORY_NOT_FOUND_EXCEPTION;
    static const AutoPtr<ResultMsg> ES_METHOD_NOT_FOUND_EXCEPTION;
    static const AutoPtr<ResultMsg> ES_ILLEGAL_ACCESS_EXCEPTION;
    static const AutoPtr<ResultMsg> ES_INVOCATION_TARGET_EXCEPTION;
    static const AutoPtr<ResultMsg> ES_INSTANTIATION_EXCEPTION;
    static const AutoPtr<ResultMsg> ES_CMD_TIMEOUT;

    /** unknow status */
    static const AutoPtr<ResultMsg> UNKNOW;

private:
    const Int32 mCode;
    String mResult;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_RESULTMSG_H__
