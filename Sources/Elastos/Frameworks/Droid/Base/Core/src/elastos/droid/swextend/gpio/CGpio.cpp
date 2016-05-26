
#include "elastos/droid/swextend/gpio/CGpio.h"
#include "IGpioService.h"
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::CChar32;
using Elastos::Core::IChar32;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;


namespace Elastos {
namespace Droid {
namespace Swextend {

const String CGpio::TAG("CGpio");

const String CGpio::mPathstr = String("/sys/class/gpio_sw/P");
const String CGpio::mDataName = String("/data");
const String CGpio::mPullName = String("/pull");
const String CGpio::mDrvLevelName = String("/drv_level");
const String CGpio::mMulSelName = String("/mul_sel");

android::sp<android::IGpioService> CGpio::mGpioService;

CAR_INTERFACE_IMPL(CGpio, Object, IGpio)

CAR_OBJECT_IMPL(CGpio)

CGpio::CGpio()
{
    if (mGpioService == NULL) {
        mGpioService = NativeInit();
    }
}

ECode CGpio::WriteGpio(
    /* [in] */ Char32 group,
    /* [in] */ Int32 num,
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String dataPath = ComposePinPath(group, num) + mDataName;

    return NativeWriteGpio(dataPath, StringUtils::Int32ToString(value), result);
}

ECode CGpio::ReadGpio(
    /* [in] */ Char32 group,
    /* [in] */ Int32 num,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String dataPath = ComposePinPath(group, num) + mDataName;

    return NativeReadGpio(dataPath, result);
}

ECode CGpio::SetPull(
    /* [in] */ Char32 group,
    /* [in] */ Int32 num,
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String dataPath = ComposePinPath(group, num) + mPullName;

    return NativeWriteGpio(dataPath, StringUtils::Int32ToString(value), result);
}

ECode CGpio::GetPull(
    /* [in] */ Char32 group,
    /* [in] */ Int32 num,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String dataPath = ComposePinPath(group, num) + mPullName;

    return NativeReadGpio(dataPath, result);
}

ECode CGpio::SetDrvLevel(
    /* [in] */ Char32 group,
    /* [in] */ Int32 num,
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String dataPath = ComposePinPath(group, num) + mDrvLevelName;

    return NativeWriteGpio(dataPath, StringUtils::Int32ToString(value), result);
}

ECode CGpio::GetDrvLevel(
    /* [in] */ Char32 group,
    /* [in] */ Int32 num,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String dataPath = ComposePinPath(group, num) + mDrvLevelName;

    return NativeReadGpio(dataPath, result);
}

ECode CGpio::SetMulSel(
    /* [in] */ Char32 group,
    /* [in] */ Int32 num,
    /* [in] */ Int32 value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String dataPath = ComposePinPath(group, num) + mMulSelName;

    return NativeWriteGpio(dataPath, StringUtils::Int32ToString(value), result);
}

ECode CGpio::GetMulSel(
    /* [in] */ Char32 group,
    /* [in] */ Int32 num,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    String dataPath = ComposePinPath(group, num) + mMulSelName;

    return NativeReadGpio(dataPath, result);
}

String CGpio::ComposePinPath(
    /* [in] */ Char32 group,
    /* [in] */ Int32 num)
{
    String numstr;
    String groupstr;
    AutoPtr< ArrayOf<Char32> > strArray = ArrayOf<Char32>::Alloc(1);
    (*strArray)[0] = group;
    groupstr = String(*strArray).ToUpperCase();
    numstr = StringUtils::ToString(num);
    return mPathstr + groupstr + numstr;
}

ECode CGpio::NativeReadGpio(
    /* [in] */ const String& path,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if(mGpioService == NULL){
        Slogger::E(TAG, "gpio service has not start!");
        return E_NULL_POINTER_EXCEPTION;
    }
    if(path == NULL){
        *result = -1;
        return NOERROR;
    }
    const char *chars = path.string();
    *result = mGpioService->readData(chars);
    return NOERROR;
}

ECode CGpio::NativeWriteGpio(
    /* [in] */ const String& path,
    /* [in] */ const String& value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mGpioService == NULL) {
        Slogger::E(TAG, "gpio service has not start!");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (path == NULL) {
        *result = -1;
    } else {
        const char *chars = path.string();
        const char *valueStr = value.string();
        *result = mGpioService->writeData(valueStr, strlen(valueStr), chars);
    }
    return NOERROR;
}

android::sp<android::IGpioService> CGpio::NativeInit()
{
    Slogger::I(TAG, "Init gpio service.");

    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    android::sp<android::IBinder> binder;
    do {
        binder = sm->getService(android::String16("softwinner.gpio"));
        if (binder != 0) {
            break;
        }
        Slogger::I(TAG, "softwinner gpio service not published, waiting...");
        usleep(500000);
    } while (TRUE);

    return android::interface_cast<android::IGpioService>(binder);
}

} // namespace Elastos
} // namespace Droid
} // namespace Swextend
