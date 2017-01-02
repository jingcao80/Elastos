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

#include "remote/SessionCmdTask.h"
#include "remote/RemoteUtils.h"
#include "remote/ResultMsg.h"
#include "remote/ZigbeeFactory.h"
#include "remote/RouterSettingFactory.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Utility::Concurrent::EIID_ICallable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

const Int32 SessionCmdTask::TargetID::Unknown;
const Int32 SessionCmdTask::TargetID::NoTarget;
const Int32 SessionCmdTask::TargetID::RouterSetting;
const Int32 SessionCmdTask::TargetID::Zigbee;
const Int32 SessionCmdTask::TargetID::TVDisp;
const String SessionCmdTask::TAG("SessionCmdTask");
const Boolean SessionCmdTask::DBG = RemoteUtils::IsDebug();

CAR_INTERFACE_IMPL(SessionCmdTask, ICallable);

SessionCmdTask::SessionCmdTask(
    /* [in] */ IContext* context,
    /* [in] */ const String& clientCmd)
    : mTargetID(0)
    , mCmdSN(0)
    , mResultNum(ResultMsg::UNKNOW->ToInt32())
{
    mContext = context;
    mCmdFromRemote = ParseRemoteCmd(clientCmd);

    AutoPtr<ArrayOf<String> > parts;
    StringUtils::Split(mCmdFromRemote, RemoteUtils::SPLITER, (ArrayOf<String>**)&parts);
    mFactoryName = (*parts)[0];
    StringBuilder sb(mFactoryName);
    sb.Append(RemoteUtils::SPLITER);
    String result;
    StringUtils::ReplaceFirst(mCmdFromRemote, sb.ToString(), String(""), &result);
    mCmdFromRemote = result;
    mResult = new StringBuilder("");
}

String SessionCmdTask::ReplaceLast(
    /* [in] */ const String& str,
    /* [in] */ const String& oldStr,
    /* [in] */ const String& newStr)
{
     Int32 lastIndex = str.LastIndexOf(oldStr);
     if (lastIndex < 0)
        return str;
     String tail;
     StringUtils::ReplaceFirst(str.Substring(lastIndex), oldStr, newStr, &tail);
     return str.Substring(0, lastIndex) + tail;
}

String SessionCmdTask::ParseRemoteCmd(
    /* [in] */ const String& clientCmd)
{
    String parseCmd = clientCmd;
    if (clientCmd.StartWith("GET")) {
        String result;
        StringUtils::ReplaceFirst(parseCmd, "GET /", "", &result);
        parseCmd = ReplaceLast(result, String(" HTTP/1.1"), String(""));
        return parseCmd;
    }
    else {
        return clientCmd;
    }
}

ECode SessionCmdTask::FactoryRunForResult(
    /* [in] */ const String& factoryName,
    /* [in] */ const String& cmd,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Slogger::D(TAG, "FactoryRunForResult");

    ECode ec = NOERROR;
    if (factoryName.Equals("ZigbeeFactory")) {
        AutoPtr<ZigbeeFactory> factory = new ZigbeeFactory(mContext);
        ec = factory->RunForResult(cmd, result);
    }
    else if (factoryName.Equals("RouterSettingFactory")) {
        AutoPtr<RouterSettingFactory> factory = new RouterSettingFactory(mContext);
        ec = factory->RunForResult(cmd, result);
    }
    else {
        Slogger::E(TAG, "ClassNotFoundException: %s", factoryName.string());
        *result = ResultMsg::ES_FACTORY_NOT_FOUND_EXCEPTION->ToString();
        mResultNum = ResultMsg::ES_FACTORY_NOT_FOUND_EXCEPTION->ToInt32();
    }

    if (SUCCEEDED(ec))
        mResultNum = ResultMsg::RESULTMSG_NOERROR->ToInt32();
    else if (ec == (ECode)E_NO_SUCH_METHOD_EXCEPTION) {
        Slogger::E(TAG, "NoSuchMethodException!!");
        *result = ResultMsg::ES_METHOD_NOT_FOUND_EXCEPTION->ToString();
        mResultNum = ResultMsg::ES_METHOD_NOT_FOUND_EXCEPTION->ToInt32();
        ec = NOERROR;
    }
    // else if (ec == (ECode)E_ILLEGAL_ACCESS_EXCEPTION) {
    //     Slogger::E(TAG, "IllegalAccessException!!");
    //     *result = ResultMsg::ES_ILLEGAL_ACCESS_EXCEPTION->ToString();
    //     mResultNum = ResultMsg::ES_ILLEGAL_ACCESS_EXCEPTION->ToInt32();
    // }
    // else if (ec == (ECode)E_INVOCATION_TARGET_EXCEPTION) {
    //     Slogger::E(TAG, "InvocationTargetException!!");
    //     *result = ResultMsg::ES_INVOCATION_TARGET_EXCEPTION->ToString();
    //     mResultNum = ResultMsg::ES_INVOCATION_TARGET_EXCEPTION->ToInt32();
    // }
    // else if (ec == (ECode)E_INSTANTIATION_EXCEPTION) {
    //     Slogger::E(TAG, "InstantiationException!!");
    //     *result = ResultMsg::ES_INSTANTIATION_EXCEPTION->ToString();
    //     mResultNum = ResultMsg::ES_INSTANTIATION_EXCEPTION->ToInt32();
    // }

    // StringBuilder sb = new StringBuilder("com.android.server.remote.");
    // sb.append(factoryName);
    // String clazzName = sb.toString();
    // try {
    //     Class<?> clazzType = Class.forName(clazzName);
    //     Object deviceFactory = clazzType.getConstructor(Context.class).newInstance(mContext);
    //     Method runMethod = clazzType.getDeclaredMethod("runForResult", String.class);
    //     runMethod.setAccessible(true);
    //     Object resultObj = runMethod.invoke(deviceFactory, cmd);
    //     result = (String)resultObj;
    //     mResultNum = ResultMsg.NOERROR.toInt();
    // } catch (ClassNotFoundException ex) {
    //     Slog.e(TAG, "ClassNotFoundException!!");
    //     result = ResultMsg.ES_FACTORY_NOT_FOUND_EXCEPTION.toString();
    //     mResultNum = ResultMsg.ES_FACTORY_NOT_FOUND_EXCEPTION.toInt();
    // } catch (NoSuchMethodException ex) {
    //     Slog.e(TAG, "NoSuchMethodException!!");
    //     result = ResultMsg.ES_METHOD_NOT_FOUND_EXCEPTION.toString();
    //     mResultNum = ResultMsg.ES_METHOD_NOT_FOUND_EXCEPTION.toInt();
    // } catch (IllegalAccessException ex) {
    //     Slog.e(TAG, "IllegalAccessException!!");
    //     result = ResultMsg.ES_ILLEGAL_ACCESS_EXCEPTION.toString();
    //     mResultNum = ResultMsg.ES_ILLEGAL_ACCESS_EXCEPTION.toInt();
    // } catch (InvocationTargetException ex) {
    //     Slog.e(TAG, "InvocationTargetException!!");
    //     result = ResultMsg.ES_INVOCATION_TARGET_EXCEPTION.toString();
    //     mResultNum = ResultMsg.ES_INVOCATION_TARGET_EXCEPTION.toInt();
    // } catch (InstantiationException ex) {
    //     Slog.e(TAG, "InstantiationException!!");
    //     result = ResultMsg.ES_INSTANTIATION_EXCEPTION.toString();
    //     mResultNum = ResultMsg.ES_INSTANTIATION_EXCEPTION.toInt();
    // }
    if (DBG) Slogger::D(TAG, "FactoryRunForResult result %s", result->string());
    return ec;
}

/**
 * call() implements Callable<> and
 * will be timeout if future don't get result in period time.
 */
ECode SessionCmdTask::Call(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    String str;
    RunForResult(&str);
    AutoPtr<ICharSequence> csq;
    CString::New(str, (ICharSequence**)&csq);
    *result = csq.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SessionCmdTask::RunForResult(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Slogger::D(TAG, "RunForResult");
    *result = ResultMsg::ES_NORESPONSE->ToString();
    mResultNum = ResultMsg::ES_NORESPONSE->ToInt32();
    if (SUCCEEDED(FactoryRunForResult(mFactoryName, mCmdFromRemote, result)))
        mResultNum = ResultMsg::RESULTMSG_NOERROR->ToInt32();
    else
        mResultNum = ResultMsg::ES_EXCEPTION->ToInt32();

    mResult->AppendString(*result);
    *result = mResult->ToString();
    return NOERROR;
}

Int32 SessionCmdTask::GetResultNum()
{
    return mResultNum;
}

} // Remote
} // Server
} // Droid
} // Elastos
