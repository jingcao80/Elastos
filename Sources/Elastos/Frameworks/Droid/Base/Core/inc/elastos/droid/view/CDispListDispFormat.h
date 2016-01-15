#include "_Elastos_Droid_View_CDispListDispFormat.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CDispListDispFormat)
{
public:
    CDispListDispFormat();

    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ Int32 format);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* rst);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetOutputType(
        /* [in] */ Int32 type);

    CARAPI GetOutputType(
        /* [out] */ Int32* type);

    CARAPI SetFormat(
        /* [in] */ Int32 item);

    CARAPI GetFormat(
        /* [out] */ Int32* item);
private:

    Int32 mOutputType;
    Int32 mFormat;
};

}// namespace View
}// namespace Droid
}// namespace View
