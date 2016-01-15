#ifndef __ELASTOS_DROID_UTILITY_PROPERTYHELPER_H__
#define __ELASTOS_DROID_UTILITY_PROPERTYHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Core_server.h"

namespace Elastos {
namespace Droid {
namespace Utility {

class ReflectiveProperty
    : public IReflectiveProperty
    , public ElRefBase
{
public:
    CAR_INTERFACE_DECL()

    ReflectiveProperty(
        /* [in] */ ClassID clsType,
        /* [in] */ CarDataType dataType,
        /* [in] */ const String& name);

    CARAPI IsReadOnly(
        /* [out] */ Boolean* readOnly);

    CARAPI Set(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* value);

    CARAPI Get(
        /* [in] */ IInterface* obj,
        /* [out] */ IInterface** rst);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetType(
        /* [out] */ ClassID* info);

    CARAPI IsWriteOnly(
        /* [out] */ Boolean* writeOnly);

    CARAPI ForbiddenOperate(
        /* [out] */ Boolean* forbidden);

private:
    const static String PREFIX_GET;
    const static String PREFIX_IS;
    const static String PREFIX_SET;
    const static String TAG;
    AutoPtr<IMethodInfo> mSetter;
    AutoPtr<IMethodInfo> mGetter;
    String mName;
    ClassID mClsId;
    Boolean mReadOnly;
    Boolean mWriteOnly;
    Boolean mForbiddenOperate;
    CarDataType mDataType;
};
} // namespace Utility
} // namepsace Droid
} // namespace Elastos

#endif
