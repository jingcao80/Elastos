
#include "elastos/droid/internal/telephony/RetryManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CPair.h"

#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CPair;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger32;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CRandom;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  RetryManager::RetryRec::
//==============================================================

RetryManager::RetryRec::RetryRec(
    /* [in] */ Int32 delayTime,
    /* [in] */ Int32 randomizationTime)
{
    mDelayTime = delayTime;
    mRandomizationTime = randomizationTime;
}

//==============================================================
//  RetryManager::
//==============================================================
String RetryManager::LOGTAG("RetryManager");
Boolean RetryManager::DBG = FALSE;
Boolean RetryManager::VDBG = FALSE;

CAR_INTERFACE_IMPL(RetryManager, Object, IRetryManager)

RetryManager::RetryManager()
    : mRetryForever(FALSE)
    , mMaxRetryCount(0)
    , mCurMaxRetryCount(0)
    , mRetryCount(0)
    , mConfig("")
{}

ECode RetryManager::constructor()
{
    CArrayList::New((IArrayList**)&mRetryArray);

    CRandom::New((IRandom**)&mRng);

    if (VDBG) Logger::D(LOGTAG, "constructor");
    return NOERROR;
}

ECode RetryManager::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    String ret = String("RetryManager: { forever=") + StringUtils::ToString(mRetryForever)
            + String(" maxRetry=") + StringUtils::ToString(mMaxRetryCount)
            + String(" curMaxRetry=") + StringUtils::ToString(mCurMaxRetryCount)
            + String(" retry=") + StringUtils::ToString(mRetryCount)
            + String(" config={") + mConfig
            + String("} retryArray={");
    AutoPtr<IIterator> it;
    mRetryArray->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<RetryRec> r = (RetryRec*)(IObject*)p.Get();
        ret += StringUtils::ToString(r->mDelayTime) + String(":")
                + StringUtils::ToString(r->mRandomizationTime) + String(" ");
    }
    ret += "}}";
    *str = ret;
    return NOERROR;
}

ECode RetryManager::Configure(
    /* [in] */ Int32 maxRetryCount,
    /* [in] */ Int32 retryTime,
    /* [in] */ Int32 randomizationTime,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPair> value;

    if (VDBG) {
        Logger::D(LOGTAG, String("configure: ") + StringUtils::ToString(maxRetryCount)
            + String(", ") + StringUtils::ToString(retryTime)
            + String(",") + StringUtils::ToString(randomizationTime));
    }

    if (!ValidateNonNegativeInt(String("maxRetryCount"), maxRetryCount)) {
        *result = FALSE;
        return NOERROR;
    }

    if (!ValidateNonNegativeInt(String("retryTime"), retryTime)) {
        *result = FALSE;
        return NOERROR;
    }

    if (!ValidateNonNegativeInt(String("randomizationTime"), randomizationTime)) {
        *result = FALSE;
        return NOERROR;
    }

    mMaxRetryCount = maxRetryCount;
    mCurMaxRetryCount = mMaxRetryCount;

    ResetRetryCount();
    mRetryArray->Clear();
    AutoPtr<RetryRec> p = new RetryRec(retryTime, randomizationTime);
    mRetryArray->Add((IInterface*)(IObject*)(p.Get()));

    *result = TRUE;
    return NOERROR;
}

ECode RetryManager::Configure(
    /* [in] */ const String& _configStr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // Strip quotes if present.
    String configStr = _configStr;
    if ((configStr.StartWith("\"") && configStr.EndWith("\""))) {
        configStr = configStr.Substring(1, configStr.GetLength()-1);
    }
    if (VDBG) Logger::D(LOGTAG, String("configure: '") + configStr + String("'"));
    mConfig = configStr;

    if (!TextUtils::IsEmpty(configStr)) {
        Int32 defaultRandomization = 0;

        if (VDBG) Logger::D(LOGTAG, String("configure: not empty"));

        mMaxRetryCount = 0;
        ResetRetryCount();
        mRetryArray->Clear();

        AutoPtr<ArrayOf<String> > strArray;
        StringUtils::Split(configStr, String(","), (ArrayOf<String>**)&strArray);
        for (Int32 i = 0; i < strArray->GetLength(); i++) {
            if (VDBG) {
                Logger::D(LOGTAG, String("configure: strArray[") + StringUtils::ToString(i)
                    + String("]='") + (*strArray)[i] + String("'"));
            }
            AutoPtr<IPair> value;
            AutoPtr<ArrayOf<String> > splitStr;
            StringUtils::Split((*strArray)[i], String("="), 2, (ArrayOf<String>**)&splitStr);
            (*splitStr)[0] = (*splitStr)[0].Trim();
            if (VDBG) Logger::D(LOGTAG, String("configure: splitStr[0]='") + (*splitStr)[0] + String("'"));
            if (splitStr->GetLength() > 1) {
                (*splitStr)[1] = (*splitStr)[1].Trim();
                if (VDBG) Logger::D(LOGTAG, String("configure: splitStr[1]='") + (*splitStr)[1] + String("'"));
                if (TextUtils::Equals((*splitStr)[0], String("default_randomization"))) {
                    value = ParseNonNegativeInt((*splitStr)[0], (*splitStr)[1]);
                    AutoPtr<IInterface> first;
                    value->GetFirst((IInterface**)&first);
                    if (!first) {
                        *result = FALSE;
                        return NOERROR;
                    }
                    AutoPtr<IInterface> second;
                    value->GetSecond((IInterface**)&second);
                    IInteger32::Probe(second)->GetValue(&defaultRandomization);
                }
                else if (TextUtils::Equals((*splitStr)[0], String("max_retries"))) {
                    if (TextUtils::Equals(String("infinite"), (*splitStr)[1])) {
                        mRetryForever = TRUE;
                    }
                    else {
                        value = ParseNonNegativeInt((*splitStr)[0], (*splitStr)[1]);
                        AutoPtr<IInterface> first;
                        value->GetFirst((IInterface**)&first);
                        if (!first) {
                            *result = FALSE;
                            return NOERROR;
                        }
                        AutoPtr<IInterface> second;
                        value->GetSecond((IInterface**)&second);
                        IInteger32::Probe(second)->GetValue(&mMaxRetryCount);
                    }
                }
                else {
                    Logger::E(LOGTAG, "Unrecognized configuration name value pair: %s",
                            (*strArray)[i].string());
                    *result = FALSE;
                    return NOERROR;
                }
            }
            else {
                /**
                 * Assume a retry time with an optional randomization value
                 * following a ":"
                 */
                StringUtils::Split((*strArray)[i], String(":"), 2, (ArrayOf<String>**)&splitStr);
                (*splitStr)[0] = (*splitStr)[0].Trim();
                AutoPtr<RetryRec> rr = new RetryRec(0, 0);
                value = ParseNonNegativeInt(String("delayTime"), (*splitStr)[0]);
                AutoPtr<IInterface> first;
                value->GetFirst((IInterface**)&first);
                if (!first) {
                    *result = FALSE;
                    return NOERROR;
                }
                AutoPtr<IInterface> second;
                value->GetSecond((IInterface**)&second);
                IInteger32::Probe(second)->GetValue(&(rr->mDelayTime));

                // Check if optional randomization value present
                if (splitStr->GetLength() > 1) {
                    (*splitStr)[1] = (*splitStr)[1].Trim();
                    if (VDBG) Logger::D(LOGTAG, String("configure: splitStr[1]='") + (*splitStr)[1] + String("'"));
                    value = ParseNonNegativeInt(String("randomizationTime"), (*splitStr)[1]);
                    AutoPtr<IInterface> first;
                    value->GetFirst((IInterface**)&first);
                    if (!first) {
                        *result = FALSE;
                        return NOERROR;
                    }
                    AutoPtr<IInterface> second;
                    value->GetSecond((IInterface**)&second);
                    IInteger32::Probe(second)->GetValue(&(rr->mRandomizationTime));
                }
                else {
                    rr->mRandomizationTime = defaultRandomization;
                }
                mRetryArray->Add((IInterface*)(IObject*)(rr.Get()));
            }
        }
        Int32 size = 0;
        mRetryArray->GetSize(&size);
        if (size > mMaxRetryCount) {
            mMaxRetryCount = size;
            if (VDBG) {
                Logger::D(LOGTAG, String("configure: setting mMaxRetryCount=")
                    + StringUtils::ToString(mMaxRetryCount));
            }
        }
        mCurMaxRetryCount = mMaxRetryCount;
        if (VDBG) Logger::D(LOGTAG, String("configure: TRUE"));
        *result = TRUE;
        return NOERROR;
    }
    else {
        if (VDBG) Logger::D(LOGTAG, String("configure: FALSE it's empty"));
        *result = FALSE;
        return NOERROR;
    }
}

ECode RetryManager::IsRetryNeeded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean retVal = mRetryForever || (mRetryCount < mCurMaxRetryCount);
    if (DBG) Logger::D(LOGTAG, String("isRetryNeeded: ") + StringUtils::ToString(retVal));
    *result = retVal;
    return NOERROR;
}

ECode RetryManager::GetRetryTimer(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 index = 0;
    Int32 size = 0;
    mRetryArray->GetSize(&size);
    if (mRetryCount < size) {
        index = mRetryCount;
    }
    else {
        index = size - 1;
    }

    Int32 retVal = 0;
    if ((index >= 0) && (index < size)) {
        AutoPtr<IInterface> p;
        mRetryArray->Get(index, (IInterface**)&p);
        AutoPtr<RetryRec> cp = (RetryRec*)(IObject*)p.Get();
        retVal = cp->mDelayTime + NextRandomizationTime(index);
    }
    else {
        retVal = 0;
    }

    if (DBG) Logger::D(LOGTAG, String("getRetryTimer: ") + StringUtils::ToString(retVal));
    *result = retVal;
    return NOERROR;
}

ECode RetryManager::GetRetryCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(LOGTAG, String("getRetryCount: ") + StringUtils::ToString(mRetryCount));
    *result = mRetryCount;
    return NOERROR;
}

ECode RetryManager::IncreaseRetryCount()
{
    mRetryCount++;
    if (mRetryCount > mCurMaxRetryCount) {
        mRetryCount = mCurMaxRetryCount;
    }
    if (DBG) Logger::D(LOGTAG, String("increaseRetryCount: ") + StringUtils::ToString(mRetryCount));
    return NOERROR;
}

ECode RetryManager::SetRetryCount(
    /* [in] */ Int32 count)
{
    mRetryCount = count;
    if (mRetryCount > mCurMaxRetryCount) {
        mRetryCount = mCurMaxRetryCount;
    }

    if (mRetryCount < 0) {
        mRetryCount = 0;
    }

    if (DBG) Logger::D(LOGTAG, String("setRetryCount: ") + StringUtils::ToString(mRetryCount));
    return NOERROR;
}

ECode RetryManager::SetCurMaxRetryCount(
    /* [in] */ Int32 count)
{
    mCurMaxRetryCount = count;

    // Make sure it's not negative
    if (mCurMaxRetryCount < 0) {
        mCurMaxRetryCount = 0;
    }

    // Make sure mRetryCount is within range
    SetRetryCount(mRetryCount);

    if (DBG) Logger::D(LOGTAG, String("setCurMaxRetryCount: ") + StringUtils::ToString(mCurMaxRetryCount));
    return NOERROR;
}

ECode RetryManager::RestoreCurMaxRetryCount()
{
    mCurMaxRetryCount = mMaxRetryCount;

    // Make sure mRetryCount is within range
    SetRetryCount(mRetryCount);
    return NOERROR;
}

ECode RetryManager::SetRetryForever(
    /* [in] */ Boolean retryForever)
{
    mRetryForever = retryForever;
    if (DBG) Logger::D(LOGTAG, String("setRetryForever: ") + StringUtils::ToString(mRetryForever));
    return NOERROR;
}

ECode RetryManager::ResetRetryCount()
{
    mRetryCount = 0;
    if (DBG) Logger::D(LOGTAG, String("resetRetryCount: ") + StringUtils::ToString(mRetryCount));
    return NOERROR;
}

ECode RetryManager::RetryForeverUsingLastTimeout()
{
    mRetryCount = mCurMaxRetryCount;
    mRetryForever = TRUE;
    if (DBG) {
        Logger::D(LOGTAG, String("retryForeverUsingLastTimeout: ") + StringUtils::ToString(mRetryForever)
            + String(", ") + StringUtils::ToString(mRetryCount));
    }
    return NOERROR;
}

ECode RetryManager::IsRetryForever(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(LOGTAG, String("isRetryForever: ") + StringUtils::ToString(mRetryForever));
    *result = mRetryForever;
    return NOERROR;
}

AutoPtr<IPair> RetryManager::ParseNonNegativeInt(
    /* [in] */ String name,
    /* [in] */ String stringValue)
{
    Int32 value = 0;
    AutoPtr<IPair> retVal;
    // try {
        StringUtils::Parse(stringValue, &value);
        Boolean res = ValidateNonNegativeInt(name, value);
        CPair::New(CoreUtils::Convert(res), CoreUtils::Convert(value), (IPair**)&retVal);
    // } Catch (NumberFormatException e) {
    //     Rlog->E(LOGTAG, name + " bad value: " + stringValue, e);
    //     retVal = new Pair<Boolean, Integer>(FALSE, 0);
    // }
    if (VDBG) {
        AutoPtr<IInterface> first, second;
        retVal->GetFirst((IInterface**)&first);
        retVal->GetSecond((IInterface**)&second);
        assert(0 && "TODO");
        // Logger::D(LOGTAG, String("parseNonNetativeInt: ") + name
        //     + String(", ") + stringValue + String(", ")
        //     + first + String(", ")
        //     + second);
    }
    return retVal;
}

Boolean RetryManager::ValidateNonNegativeInt(
    /* [in] */ String name,
    /* [in] */ Int32 value)
{
    Boolean retVal;
    if (value < 0) {
        Logger::E(LOGTAG, name + String(" bad value: is < 0"));
        retVal = FALSE;
    }
    else {
        retVal = TRUE;
    }
    if (VDBG) {
        Logger::D(LOGTAG, String("validateNonNegative: ") + name
            + String(", ") + StringUtils::ToString(value)
            + String(", ") + StringUtils::ToString(retVal));
    }
    return retVal;
}

Int32 RetryManager::NextRandomizationTime(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> p;
    mRetryArray->Get(index, (IInterface**)&p);
    AutoPtr<RetryRec> cp = (RetryRec*)(IObject*)p.Get();
    Int32 randomTime = cp->mRandomizationTime;
    if (randomTime == 0) {
        return 0;
    }
    else {
        Int32 res = 0;
        mRng->NextInt32(randomTime, &res);
        return res;
    }
}

void RetryManager::Log(
    /* [in] */ String s)
{
    Logger::D(LOGTAG, String("[RM] ") + s);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
