
#ifndef __ELASTOS_DROID_APP_CRESULTINFO_H__
#define __ELASTOS_DROID_APP_CRESULTINFO_H__

#include "_Elastos_Droid_App_CResultInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CResultInfo)
    , public Object
    , public IResultInfo
    , public IParcelable
{
public:
    CResultInfo();

    virtual ~CResultInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    CARAPI GetResultWho(
        /* [out] */ String* who);

    CARAPI SetResultWho(
        /* [in] */ const String& who);

    CARAPI GetRequestCode(
        /* [out] */ Int32* code);

    CARAPI SetRequestCode(
        /* [in] */ Int32 code);

    CARAPI GetResultCode(
        /* [out] */ Int32* code);

    CARAPI SetResultCode(
        /* [in] */ Int32 code);

    CARAPI GetData(
        /* [out] */ IIntent** data);

    CARAPI SetData(
        /* [in] */ IIntent* data);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    String mResultWho;
    Int32 mRequestCode;
    Int32 mResultCode;
    AutoPtr<IIntent> mData;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CRESULTINFO_H__
