#include "elastos/droid/internal/widget/CLockPatternUtilsHelper.h"
#include "elastos/droid/internal/widget/LockPatternUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_INTERFACE_IMPL(CLockPatternUtilsHelper, Singleton, ILockPatternUtilsHelper)

CAR_SINGLETON_IMPL(CLockPatternUtilsHelper)

ECode CLockPatternUtilsHelper::MaxLengthSequence(
    /* [in] */ const String& string,
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len)
    *len = LockPatternUtils::MaxLengthSequence(string);
    return NOERROR;
}

ECode CLockPatternUtilsHelper::IsDeviceEncrypted(
    /* [out] */ Boolean* isDeviceEncrypted)
{
    VALIDATE_NOT_NULL(isDeviceEncrypted)
    *isDeviceEncrypted = LockPatternUtils::IsDeviceEncrypted();
    return NOERROR;
}

ECode CLockPatternUtilsHelper::IsDeviceEncryptionEnabled(
    /* [out] */ Boolean* isDeviceEncryptionEnabled)
{
    VALIDATE_NOT_NULL(isDeviceEncryptionEnabled)
    *isDeviceEncryptionEnabled = LockPatternUtils::IsDeviceEncryptionEnabled();
    return NOERROR;
}

ECode CLockPatternUtilsHelper::PatternToHash(
    /* [in] */ IList* pattern,
    /* [out, callee] */ ArrayOf<Byte>** arr)
{
    VALIDATE_NOT_NULL(arr)
    AutoPtr < ArrayOf<Byte> > bytes = LockPatternUtils::PatternToHash(pattern);
    *arr = bytes;
    REFCOUNT_ADD(*arr)
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
