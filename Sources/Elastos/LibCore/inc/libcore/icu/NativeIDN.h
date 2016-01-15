
#ifndef __LIBCORE_ICU_NATIVEIDN_H__
#define __LIBCORE_ICU_NATIVEIDN_H__

#include <elastos.h>
#include <elatypes.h>

using namespace Elastos;

namespace Libcore {
namespace ICU {

class NativeIDN
{
public:
    static CARAPI_(String) ToASCII(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags);

    static CARAPI_(String) ToUnicode(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags);

private:
    NativeIDN();

    static CARAPI Convert(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean toAscii,
        /* [out] */ String* result);

    static CARAPI ConvertImpl(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean toAscii,
        /* [out] */ String* result);
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_NATIVEIDN_H__
