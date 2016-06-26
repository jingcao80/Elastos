
#ifndef __CTESTCALLBACKARGUMENTLIST_H__
#define __CTESTCALLBACKARGUMENTLIST_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestCallbackArgumentList.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestCallbackArgumentList)
    , public Object
    , public ITestCallbackArgumentList
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetCallbackMethodInfo(
        /* [out] */ ITestCallbackMethodInfo ** ppCallbackMethodInfo);

    CARAPI GetServerPtrArgument(
        /* [out] */ PInterface * ppServer);

    CARAPI GetInt16Argument(
        /* [in] */ Int32 index,
        /* [out] */ Int16 * pValue);

    CARAPI GetInt32Argument(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetInt64Argument(
        /* [in] */ Int32 index,
        /* [out] */ Int64 * pValue);

    CARAPI GetByteArgument(
        /* [in] */ Int32 index,
        /* [out] */ Byte * pValue);

    CARAPI GetFloatArgument(
        /* [in] */ Int32 index,
        /* [out] */ Float * pValue);

    CARAPI GetDoubleArgument(
        /* [in] */ Int32 index,
        /* [out] */ Double * pValue);

    CARAPI GetCharArgument(
        /* [in] */ Int32 index,
        /* [out] */ Char32 * pValue);

    CARAPI GetStringArgument(
        /* [in] */ Int32 index,
        /* [out] */ String * pValue);

    CARAPI GetBooleanArgument(
        /* [in] */ Int32 index,
        /* [out] */ Boolean * pValue);

    CARAPI GetEMuidArgument(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetEGuidArgument(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetECodeArgument(
        /* [in] */ Int32 index,
        /* [out] */ ECode * pValue);

    CARAPI GetLocalPtrArgument(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetEnumArgument(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetCarArrayArgument(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetStructPtrArgument(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * pValue);

    CARAPI GetObjectPtrArgument(
        /* [in] */ Int32 index,
        /* [out] */ PInterface * ppValue);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICallbackArgumentList * pCallbackArgumentList);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<ICallbackArgumentList> mCallbackArgumentList;
};

}
}
}
}

#endif // __CTESTCALLBACKARGUMENTLIST_H__
