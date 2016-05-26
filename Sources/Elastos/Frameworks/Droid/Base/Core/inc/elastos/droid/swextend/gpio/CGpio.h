
#ifndef __ELASTOS_DROID_SWEXTEND_CGPIO_H__
#define __ELASTOS_DROID_SWEXTEND_CGPIO_H__

#include "_Elastos_Droid_Swextend_CGpio.h"
#include "elastos/droid/ext/frameworkext.h"
#include "IGpioService.h"

namespace Elastos {
namespace Droid {
namespace Swextend {

/**
 * Class that provides access to some of the gpio management functions.
 *
 * {@hide}
 */
CarClass(CGpio)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CGpio();

    CARAPI WriteGpio(
        /* [in] */ Char32 group,
        /* [in] */ Int32 num,
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

    CARAPI ReadGpio(
        /* [in] */ Char32 group,
        /* [in] */ Int32 num,
        /* [out] */ Int32* result);

    CARAPI SetPull(
        /* [in] */ Char32 group,
        /* [in] */ Int32 num,
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

    CARAPI GetPull(
        /* [in] */ Char32 group,
        /* [in] */ Int32 num,
        /* [out] */ Int32* result);

    CARAPI SetDrvLevel(
        /* [in] */ Char32 group,
        /* [in] */ Int32 num,
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

    CARAPI GetDrvLevel(
        /* [in] */ Char32 group,
        /* [in] */ Int32 num,
        /* [out] */ Int32* result);

    CARAPI SetMulSel(
        /* [in] */ Char32 group,
        /* [in] */ Int32 num,
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

    CARAPI GetMulSel(
        /* [in] */ Char32 group,
        /* [in] */ Int32 num,
        /* [out] */ Int32* result);

private:
    static String ComposePinPath(
        /* [in] */ Char32 group,
        /* [in] */ Int32 num);

    static CARAPI NativeReadGpio(
        /* [in] */ const String& path,
        /* [out] */ Int32* result);

    static CARAPI NativeWriteGpio(
        /* [in] */ const String& path,
        /* [in] */ const String& value,
        /* [out] */ Int32* result);

    static CARAPI_(android::sp<android::IGpioService>) NativeInit();

public:
    static const String TAG;

protected:
    static android::sp<android::IGpioService> mGpioService;

private:
    static const String mPathstr;
    static const String mDataName;
    static const String mPullName;
    static const String mDrvLevelName;
    static const String mMulSelName;
};

} // namespace Elastos
} // namespace Droid
} // namespace Swextend

#endif // __ELASTOS_DROID_SWEXTEND_CGPIO_H__
