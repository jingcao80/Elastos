
#include "DevicePolicyData.h"

namespace Elastos {
namespace Droid {
namespace Server {

DevicePolicyData::DevicePolicyData(
    /* [in] */ Int32 userHandle)
{
    mActivePasswordQuality = 0;
    mActivePasswordLength = 0;
    mActivePasswordUpperCase = 0;
    mActivePasswordLowerCase = 0;
    mActivePasswordLetters = 0;
    mActivePasswordNumeric = 0;
    mActivePasswordSymbols = 0;
    mActivePasswordNonLetter = 0;
    mFailedPasswordAttempts = 0;

    mUserHandle = userHandle;
    mPasswordOwner = -1;
    mLastMaximumTimeToLock = -1;
}

DevicePolicyData::~DevicePolicyData()
{
}

void DevicePolicyData::SetActivePasswordQuality(
    /* [in] */ Int32 quality)
{
    mActivePasswordQuality = quality;
}

Int32 DevicePolicyData::GetActivePasswordQuality()
{
    return mActivePasswordQuality;
}

void DevicePolicyData::SetActivePasswordLength(
    /* [in] */ Int32 length)
{
    mActivePasswordLength = length;
}

Int32 DevicePolicyData::GetActivePasswordLength()
{
    return mActivePasswordLength;
}

void DevicePolicyData::SetActivePasswordUpperCase(
    /* [in] */ Int32 upperCase)
{
    mActivePasswordUpperCase = upperCase;
}

Int32 DevicePolicyData::GetActivePasswordUpperCase()
{
    return mActivePasswordUpperCase;
}

void DevicePolicyData::SetActivePasswordLowerCase(
    /* [in] */ Int32 lowerCase)
{
    mActivePasswordLowerCase = lowerCase;
}

Int32 DevicePolicyData::GetActivePasswordLowerCase()
{
    return mActivePasswordLowerCase;
}

void DevicePolicyData::SetActivePasswordLetters(
    /* [in] */ Int32 letters)
{
    mActivePasswordLetters = letters;
}

Int32 DevicePolicyData::GetActivePasswordLetters()
{
    return mActivePasswordLetters;
}

void DevicePolicyData::SetActivePasswordNumeric(
    /* [in] */ Int32 numeric)
{
    mActivePasswordNumeric = numeric;
}

Int32 DevicePolicyData::GetActivePasswordNumeric()
{
    return mActivePasswordNumeric;
}

void DevicePolicyData::SetActivePasswordSymbols(
    /* [in] */ Int32 symbols)
{
    mActivePasswordSymbols = symbols;
}

Int32 DevicePolicyData::GetActivePasswordSymbols()
{
    return mActivePasswordSymbols;
}

void DevicePolicyData::SetActivePasswordNonLetter(
    /* [in] */ Int32 nonLetter)
{
    mActivePasswordNonLetter = nonLetter;
}

Int32 DevicePolicyData::GetActivePasswordNonLetter()
{
    return mActivePasswordNonLetter;
}

void DevicePolicyData::SetFailedPasswordAttempts(
    /* [in] */ Int32 attempts)
{
    mFailedPasswordAttempts = attempts;
}

Int32 DevicePolicyData::GetFailedPasswordAttempts()
{
    return mFailedPasswordAttempts;
}

void DevicePolicyData::SetUserHandle(
    /* [in] */ Int32 userHandle)
{
    mUserHandle = userHandle;
}

Int32 DevicePolicyData::GetUserHandle()
{
    return mUserHandle;
}

void DevicePolicyData::SetPasswordOwner(
    /* [in] */ Int32 owner)
{
    mPasswordOwner = owner;
}

Int32 DevicePolicyData::GetPasswordOwner()
{
    return mPasswordOwner;
}

void DevicePolicyData::SetLastMaximumTimeToLock(
    /* [in] */ Int64 timeToLock)
{
    mLastMaximumTimeToLock = timeToLock;
}

Int64 DevicePolicyData::GetLastMaximumTimeToLock()
{
    return mLastMaximumTimeToLock;
}

}// Server
}// Droid
}// Elastos
