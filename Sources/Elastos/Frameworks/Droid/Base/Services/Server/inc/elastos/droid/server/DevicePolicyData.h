#ifndef __ELASTOS_DROID_SERVER_DEVICEPOLICYDATA_H__
#define __ELASTOS_DROID_SERVER_DEVICEPOLICYDATA_H__

namespace Elastos {
namespace Droid {
namespace Server {

class CDevicePolicyManagerService;
class DevicePolicyData
{
public:
    DevicePolicyData(
        /* [in] */ Int32 userHandle);

    ~DevicePolicyData();

    CARAPI_(void) SetActivePasswordQuality(
        /* [in] */ Int32 quality);

    CARAPI_(Int32) GetActivePasswordQuality();

    CARAPI_(void) SetActivePasswordLength(
        /* [in] */ Int32 length);

    CARAPI_(Int32) GetActivePasswordLength();

    CARAPI_(void) SetActivePasswordUpperCase(
        /* [in] */ Int32 upperCase);

    CARAPI_(Int32) GetActivePasswordUpperCase();

    CARAPI_(void) SetActivePasswordLowerCase(
        /* [in] */ Int32 lowerCase);

    CARAPI_(Int32) GetActivePasswordLowerCase();

    CARAPI_(void) SetActivePasswordLetters(
        /* [in] */ Int32 letters);

    CARAPI_(Int32) GetActivePasswordLetters();

    CARAPI_(void) SetActivePasswordNumeric(
        /* [in] */ Int32 numeric);

    CARAPI_(Int32) GetActivePasswordNumeric();

    CARAPI_(void) SetActivePasswordSymbols(
        /* [in] */ Int32 symbols);

    CARAPI_(Int32) GetActivePasswordSymbols();

    CARAPI_(void) SetActivePasswordNonLetter(
        /* [in] */ Int32 nonLetter);

    CARAPI_(Int32) GetActivePasswordNonLetter();

    CARAPI_(void) SetFailedPasswordAttempts(
        /* [in] */ Int32 attempts);

    CARAPI_(Int32) GetFailedPasswordAttempts();

    CARAPI_(void) SetUserHandle(
        /* [in] */ Int32 userHandle);

    CARAPI_(Int32) GetUserHandle();

    CARAPI_(void) SetPasswordOwner(
        /* [in] */ Int32 owner);

    CARAPI_(Int32) GetPasswordOwner();

    CARAPI_(void) SetLastMaximumTimeToLock(
        /* [in] */ Int64 timeToLock);

    CARAPI_(Int64) GetLastMaximumTimeToLock();

public:
    HashMap< AutoPtr<IComponentName>, AutoPtr<ActiveAdmin> > mAdminMap;
    Vector< AutoPtr<ActiveAdmin> > mAdminList;

private:
    Int32 mActivePasswordQuality;
    Int32 mActivePasswordLength;
    Int32 mActivePasswordUpperCase;
    Int32 mActivePasswordLowerCase;
    Int32 mActivePasswordLetters;
    Int32 mActivePasswordNumeric;
    Int32 mActivePasswordSymbols;
    Int32 mActivePasswordNonLetter;
    Int32 mFailedPasswordAttempts;

    Int32 mUserHandle;
    Int32 mPasswordOwner;
    Int64 mLastMaximumTimeToLock;

    friend class CDevicePolicyManagerService;
};

}// Server
}// Droid
}// Elastos

#endif //__ELASTOS_DROID_SERVER_DEVICEPOLICYDATA_H__
