#ifndef __ELASTOS_DROID_TEXT_FORMAT_CFormatter_H__
#define __ELASTOS_DROID_TEXT_FORMAT_CFormatter_H__

#include "_Elastos_Droid_Text_Format_CFormatter.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

CarClass(CFormatter)
    , public Singleton
    , public IFormatter
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI FormatFileSize(
        /* [in] */ IContext* context,
        /* [in] */ Int64 number,
        /* [out] */ String* ret);

    CARAPI FormatIpAddress(
        /* [in] */ Int32 ipv4Address,
        /* [out] */ String* ret);

    CARAPI FormatShortFileSize(
        /* [in] */ IContext* context,
        /* [in] */ Int64 number,
        /* [out] */ String* ret);

    CARAPI FormatShortElapsedTime(
         /* [in] */ IContext* context,
         /* [in] */ Int64 millis,
         /* [out] */ String* ret);
};

} // namespace Format
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_FORMAT_CFormatter_H__
