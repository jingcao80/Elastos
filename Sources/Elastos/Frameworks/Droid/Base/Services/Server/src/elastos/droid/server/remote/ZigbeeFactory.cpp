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

#include "remote/ZigbeeFactory.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IClassLoader;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

const String ZigbeeFactory::ParsingManager::mSpliter = String("?"); //check split function if this char changes
const String ZigbeeFactory::ParsingManager::mEnd = String("$");

/*
 *  Split a string with specific spliter
 *  Parameters: string need to be split, spliter
 *  Returns: string in a list - success, NULL- fail
*/
AutoPtr<ArrayOf<String> > ZigbeeFactory::ParsingManager::Split(
    /* [in] */ const String& str)
{
    AutoPtr<ArrayOf<String> > stringList;

    if (str == NULL) {
        Slogger::E(TAG, "ParsingManager:split(NULL, %s): NULL string", mSpliter.string());
        return NULL;
    }

    if (str.Equals("")) {
        Slogger::E(TAG, "ParsingManager:split(\"\", %s): empty string", mSpliter.string());
        stringList = ArrayOf<String>::Alloc(1);
        (*stringList)[0] = String("");
        return stringList;
    }

    if (!str.Contains(mSpliter)) {
        if (DBG) Slogger::D(TAG, "ParsingManager:split(%s): no spliter char, no split needed", str.string());
        stringList = ArrayOf<String>::Alloc(1);
        (*stringList)[0] = str;
        return stringList;
    }

    StringUtils::Split(str, String("\\") + mSpliter, (ArrayOf<String>**)&stringList); //add escape due to special char "?"

    if (DBG) {
        for (Int32 i = 0; i < stringList->GetLength(); i++) {
            Slogger::D(TAG, "ParsingManager:Split(%s): %s", str.string(), (*stringList)[i].string());
        }
    }

    return stringList;
}

/*
 *  Combine a two-diamension array to a string: use "?" to split fields, use "$" to end a line
 *  Ex. zigbee1,zigbee2 ,group1(contains device "zigbee1,zigbee2"): combined string as following:
 *      2?1$zigbee1?1?258?65535?1$zigbee2?1?258?65535?1$zigbee2?1?258?65535?1$group1?258?65535?zigbee1?zigbee2$
 *      1'st segemnt: 2 means 2 devices,   1 means 1 group
 *      2'nd segement: zigbee1(device name), 1(device online status), 258(device type), 65535(device zone type),
 *                                          1(device state: on/off state - bulb, temperature - temperature sensor, door state -  door sensor)
 *      3'rd segement: group1(group name), 258(group type), 65535(group zone type), zigbee1?zigbee2 (device in the group)
 *  Parameters: string need to be combined, spliter
 *  Returns: combined string - success, NULL- fail
*/
String ZigbeeFactory::ParsingManager::Combine(
    /* [in] */ ArrayOf<IParcelStringList*>* list)
{
    AutoPtr<ArrayOf<String> > stringList;
    String result("");

    if (list == NULL) {
        Slogger::E(TAG, "ParsingManager:Combine(): NULL string list");
        return String(NULL);
    }

    if (list->GetLength() == 0) {
        Slogger::E(TAG, "ParsingManager:Combine(): empty string list");
        return String("");
    }

    for (Int32 i = 0; i < list->GetLength(); i++) {
        stringList = NULL;
        (*list)[i]->GetStringList((ArrayOf<String>**)&stringList);
        for (Int32 j = 0; j < stringList->GetLength(); j++) {
            if (j == 0) {
                result += (*stringList)[j];
            }
            else {
                result += mSpliter + (*stringList)[j];
            }
        }
        result += mEnd;
    }
    if (DBG) Slogger::D(TAG, "ParsingManager:Combine(): pack string result: %s", result.string());

    return result;
}

ZigbeeFactory::SocketCmd_Task::SocketCmd_Task(
    /* [in] */ const String& cmd,
    /* [in] */ ZigbeeFactory* host)
    : mCmd(cmd)
    , mIsVoid(TRUE)
    , mHost(host)
{
}

AutoPtr<IInterface> ZigbeeFactory::SocketCmd_Task::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params)
{
    if (DBG) Slogger::D(ZigbeeFactory::TAG, "DoInBackground cmd %s", mCmd.string());
    AutoPtr<ArrayOf<String> > parts;
    StringUtils::Split(mCmd, String("_"), (ArrayOf<String>**)&parts);
    String resultType = (*parts)[CMDRESERVEDLENGTH-2];
    String methodName = (*parts)[CMDRESERVEDLENGTH-1].ToUpperCase(0, 1);
    String resultFromService;

    if (!resultType.Equals("void")) {
        mIsVoid = FALSE;
    }

    if (DBG) Slogger::D(ZigbeeFactory::TAG, "DoInBackground resultType = %s, methodName = %s", resultType.string(), methodName.string());

    AutoPtr<IClassLoader> cl;
    mHost->mContext->GetClassLoader((IClassLoader**)&cl);
    assert(cl != NULL);
    String path("/system/lib/Elastos.Droid.Server.eco");
    AutoPtr<IModuleInfo> moduleInfo;
    CReflector::AcquireModuleInfo(path.string(), (IModuleInfo**)&moduleInfo);
    assert(moduleInfo != NULL);
    String className("CZigbeeService");
    AutoPtr<IClassInfo> clazz;
    // cl->LoadClass(className, (IClassInfo**)&clazz);
    moduleInfo->GetClassInfo(className, (IClassInfo**)&clazz);
    assert(clazz != NULL);
    AutoPtr<IMethodInfo> method;
    clazz->GetMethodInfo(methodName, (IMethodInfo**)&method);
    if (method == NULL) {
        Slogger::E(ZigbeeFactory::TAG, "DoInBackground method %s does not exist!", methodName.string());
        return NULL;
    }

    AutoPtr<IArgumentList> args;
    method->CreateArgumentList((IArgumentList**)&args);
    assert(args != NULL);
    Int32 length = parts->GetLength();
    if (length > CMDRESERVEDLENGTH) {
        for (Int32 i = CMDRESERVEDLENGTH; i < length; i += CMDPAIR) {
            if ((i + (CMDPAIR - 1)) >= length) {    //make sure no index out of array length
                break;
            }
            String type = (*parts)[i];
            String value = (*parts)[i + (CMDPAIR - 1)];
            Int32 index = (i - CMDRESERVEDLENGTH) / CMDPAIR;
            if (type.Equals("int")) {
                args->SetInputArgumentOfInt32(index, StringUtils::ParseInt32(value));
            }
            else if (type.Equals("String")) {
                args->SetInputArgumentOfString(index, value);
            }
            else if (type.Equals("boolean")) {
                args->SetInputArgumentOfBoolean(index, value.EqualsIgnoreCase("TRUE"));
            }
            else if (type.Equals("List<String>")) {
                AutoPtr<ArrayOf<String> > cmdStringList = ParsingManager::Split(value);
                args->SetInputArgumentOfCarArray(index, cmdStringList);
            }
            else {
                Slogger::E(ZigbeeFactory::TAG, "DoInBackground(): unknown type: \"%s\"!", type.string());
            }
        }
    }

    AutoPtr<IInterface> zigbeeService = ServiceManager::GetService(String("zigbee"));
    assert(zigbeeService != NULL);
    ECode ec;
    if (mIsVoid) {
        ec = method->Invoke(zigbeeService, args);
    }
    else {
        Int32 outIndex = (length - CMDRESERVEDLENGTH) / CMDPAIR;
        if (resultType.Equals("int")) {
            Int32 res;
            args->SetOutputArgumentOfInt32Ptr(outIndex, &res);
            ec = method->Invoke(zigbeeService, args);
            resultFromService = StringUtils::ToString(res);
        }
        else if (resultType.Equals("String")) {
            String str;
            args->SetOutputArgumentOfStringPtr(outIndex, &str);
            ec = method->Invoke(zigbeeService, args);
            resultFromService = str;
        }
        else if (resultType.Equals("List<ParcelStringList>")) {
            AutoPtr<ArrayOf<IParcelStringList*> > list;
            args->SetOutputArgumentOfCarArrayPtrPtr(outIndex, (PCarQuintet*)&list);
            ec = method->Invoke(zigbeeService, args);
            String result = ParsingManager::Combine(list);
            if (result != NULL) {
                resultFromService = result;
            }
            else {
                resultFromService = String("");
            }
        }
    }

    if (FAILED(ec))
        Slogger::D(ZigbeeFactory::TAG, "Invoke method %s FAILED ec = 0x%08x", methodName.string(), ec);

    if (DBG) Slogger::D(ZigbeeFactory::TAG, "DoInBackground resultFromService %s", resultFromService.string());
    AutoPtr<ICharSequence> cs;
    CString::New(resultFromService, (ICharSequence**)&cs);
    return cs.Get();
}

Boolean ZigbeeFactory::SocketCmd_Task::IsFuncReturnVoid()
{
    return mIsVoid;
}

const String ZigbeeFactory::TAG("ZigbeeFactory");
const Boolean ZigbeeFactory::DBG = TRUE;
const Int32 ZigbeeFactory::CMDRESERVEDLENGTH;
const Int32 ZigbeeFactory::CMDPAIR;

ZigbeeFactory::ZigbeeFactory(
    /* [in] */ IContext* ctx)
    : AbstractDeviceFactory(ctx)
    , mCmdCheckSN(0)
{
    if (DBG) Slogger::D(TAG, "ZigbeeFactory");
}

ECode ZigbeeFactory::RunForResult(
    /* [in] */ const String& cmd,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Slogger::D(TAG, "RunForResult");
    *result = String("fail");
    // try {
        AutoPtr<SocketCmd_Task> sct = new SocketCmd_Task(cmd, this);
        ECode ec = sct->Execute((ArrayOf<IInterface*>*)NULL);
        AutoPtr<IInterface> ret;
        sct->Get((IInterface**)&ret);
        if (ICharSequence::Probe(ret) != NULL)
            ICharSequence::Probe(ret)->ToString(result);
        else {
            Slogger::D(TAG, "RunForResult result = NULL");
        }
    // }
    if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
        Slogger::E(TAG, "RunForResult(): InterruptedException!");
        ec = NOERROR;
    }
    else if (ec == (ECode)E_EXECUTION_EXCEPTION) {
        Slogger::E(TAG, "RunForResult(): ExecutionException!");
        ec = NOERROR;
    }
    return ec;
}

} // Remote
} // Server
} // Droid
} // Elastos
