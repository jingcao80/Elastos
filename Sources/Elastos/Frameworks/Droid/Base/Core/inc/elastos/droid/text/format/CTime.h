#ifndef __ELASTOS_DROID_TEXT_FORMAT_CTIME_H__
#define __ELASTOS_DROID_TEXT_FORMAT_CTIME_H__

#include "_Elastos_Droid_Text_Format_CTime.h"
#include "elastos/droid/text/format/Time.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Format {

CarClass(CTime)
    , public Time
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Format
} // namespace Text
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_FORMAT_CTIME_H__
