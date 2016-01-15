
#include "elastos/droid/os/CStrictModeVmPolicyBuilder.h"
#include "elastos/droid/os/CStrictMode.h"

namespace Elastos {
namespace Droid {
namespace Os {

ECode CStrictModeVmPolicyBuilder::constructor()
{
    mMask = 0;
    mClassInstanceLimitNeedCow = FALSE;
    return NOERROR;
}

ECode CStrictModeVmPolicyBuilder::constructor(
    /* [in] */ IStrictModeVmPolicy* policy)
{
    CStrictMode::VmPolicy* vp = (CStrictMode::VmPolicy*)policy;
    mMask = vp->mMask;
    mClassInstanceLimitNeedCow = TRUE;
    mClassInstanceLimit = vp->mClassInstanceLimit;
    return NOERROR;
}

ECode CStrictModeVmPolicyBuilder::SetClassInstanceLimit(
    /* [in] */ Handle32 klass,
    /* [in] */ Int32 instanceLimit)
{
    if (klass == 0) {
        //throw new NullPointerException("klass == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (mClassInstanceLimitNeedCow) {
        if (mClassInstanceLimit.Find(klass) != mClassInstanceLimit.End() &&
            mClassInstanceLimit[klass] == instanceLimit) {
            // no-op; don't break COW
            return NOERROR;
        }
        mClassInstanceLimitNeedCow = FALSE;
        //mClassInstanceLimit = (HashMap<Handle32, Int32>) mClassInstanceLimit.clone();
    }
    // else if (mClassInstanceLimit == null) {
    //     mClassInstanceLimit = new HashMap<Class, Integer>();
    // }
    mMask |= CStrictMode::DETECT_VM_INSTANCE_LEAKS;
    mClassInstanceLimit[klass] = instanceLimit;
    return NOERROR;
}

ECode CStrictModeVmPolicyBuilder::DetectActivityLeaks()
{
    return Enable(IStrictMode::DETECT_VM_ACTIVITY_LEAKS);
}

ECode CStrictModeVmPolicyBuilder::DetectAll()
{
    return Enable(IStrictMode::DETECT_VM_ACTIVITY_LEAKS | IStrictMode::DETECT_VM_CURSOR_LEAKS
                | IStrictMode::DETECT_VM_CLOSABLE_LEAKS | IStrictMode::DETECT_VM_REGISTRATION_LEAKS);
}

ECode CStrictModeVmPolicyBuilder::DetectLeakedSqlLiteObjects()
{
    return Enable(IStrictMode::DETECT_VM_CURSOR_LEAKS);
}

ECode CStrictModeVmPolicyBuilder::DetectLeakedClosableObjects()
{
    return Enable(IStrictMode::DETECT_VM_CLOSABLE_LEAKS);
}

ECode CStrictModeVmPolicyBuilder::DetectLeakedRegistrationObjects()
{
    return Enable(IStrictMode::DETECT_VM_REGISTRATION_LEAKS);
}

ECode CStrictModeVmPolicyBuilder::PenaltyDeath()
{
    return Enable(IStrictMode::PENALTY_DEATH);
}

ECode CStrictModeVmPolicyBuilder::PenaltyLog()
{
    return Enable(IStrictMode::PENALTY_LOG);
}

ECode CStrictModeVmPolicyBuilder::PenaltyDropBox()
{
    return Enable(IStrictMode::PENALTY_DROPBOX);
}

ECode CStrictModeVmPolicyBuilder::Build(
    /* [out] */ IStrictModeVmPolicy** policy)
{
    // If there are detection bits set but no violation bits
    // set, enable simple logging.
    VALIDATE_NOT_NULL(policy);
    if (mMask != 0 &&
        (mMask & (IStrictMode::PENALTY_DEATH | IStrictMode::PENALTY_LOG |
                  IStrictMode::PENALTY_DROPBOX | IStrictMode::PENALTY_DIALOG)) == 0) {
        PenaltyLog();
    }
    AutoPtr<IStrictModeVmPolicy> vp = new CStrictMode::VmPolicy(mMask,
            !mClassInstanceLimit.IsEmpty() ? mClassInstanceLimit : CStrictMode::EMPTY_CLASS_LIMIT_MAP);
    *policy = vp;
    REFCOUNT_ADD(*policy);
    return NOERROR;
}

ECode CStrictModeVmPolicyBuilder::Enable(
    /* [in] */ Int32 bit)
{
    mMask |= bit;
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos