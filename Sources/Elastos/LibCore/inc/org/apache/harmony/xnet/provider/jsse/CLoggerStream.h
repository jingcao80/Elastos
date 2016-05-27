
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CLOGGERSTREAM_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CLOGGERSTREAM_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CLoggerStream.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CLoggerStream)
    , public Object
    , public ILoggerStream
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Close();

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI WriteBytes(
        /* [in] */ const ArrayOf<Byte> & buffer);

    CARAPI WriteBytesEx(
        /* [in] */ const ArrayOf<Byte> & buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI CheckError(
        /* [out] */ Boolean * pHasError);

    CARAPI GetLock(
        /* [out] */ IInterface ** ppLockObj);

    CARAPI Format(
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface *> * pArgs,
        /* [out] */ Elastos::IO::IPrintStream ** ppPs);

    CARAPI FormatEx(
        /* [in] */ Libcore::ICU::ILocale * pL,
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface *> * pArgs,
        /* [out] */ Elastos::IO::IPrintStream ** ppPs);

    CARAPI Printf(
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface *> * pArgs,
        /* [out] */ Elastos::IO::IPrintStream ** ppPs);

    CARAPI PrintfEx(
        /* [in] */ Libcore::ICU::ILocale * pL,
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<IInterface *> * pArgs,
        /* [out] */ Elastos::IO::IPrintStream ** ppPs);

    CARAPI PrintChars(
        /* [in] */ const ArrayOf<Char32> & chars);

    CARAPI PrintChar(
        /* [in] */ Char32 c);

    CARAPI PrintDouble(
        /* [in] */ Double d);

    CARAPI PrintFloat(
        /* [in] */ Float f);

    CARAPI PrintInt32(
        /* [in] */ Int32 i);

    CARAPI PrintInt64(
        /* [in] */ Int64 l);

    CARAPI PrintObject(
        /* [in] */ IInterface * pO);

    CARAPI PrintString(
        /* [in] */ const String& str);

    CARAPI PrintBoolean(
        /* [in] */ Boolean b);

    CARAPI Println();

    CARAPI PrintCharsln(
        /* [in] */ const ArrayOf<Char32> & chars);

    CARAPI PrintCharln(
        /* [in] */ Char32 c);

    CARAPI PrintDoubleln(
        /* [in] */ Double d);

    CARAPI PrintFloatln(
        /* [in] */ Float f);

    CARAPI PrintInt32ln(
        /* [in] */ Int32 i);

    CARAPI PrintInt64ln(
        /* [in] */ Int64 l);

    CARAPI PrintObjectln(
        /* [in] */ IInterface * pO);

    CARAPI PrintStringln(
        /* [in] */ const String& str);

    CARAPI PrintBooleanln(
        /* [in] */ Boolean b);

    CARAPI NewIndent();

    CARAPI EndIndent();

    CARAPI PrintBytes(
        /* [in] */ ArrayOf<Byte> * pData);

    CARAPI PrintBytesEx(
        /* [in] */ ArrayOf<Byte> * pData,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI PrintAsHex(
        /* [in] */ Int32 perLine,
        /* [in] */ const String& prefix,
        /* [in] */ const String& delimiter,
        /* [in] */ ArrayOf<Byte> * pData);

    CARAPI PrintAsHexEx(
        /* [in] */ Int32 perLine,
        /* [in] */ const String& prefix,
        /* [in] */ const String& delimiter,
        /* [in] */ ArrayOf<Byte> * pData,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI constructor(
        /* [in] */ const String& name);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CLOGGERSTREAM_H__
