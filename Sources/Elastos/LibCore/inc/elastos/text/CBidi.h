
#ifndef __ELASTOS_TEXT_CBIDI_H__
#define __ELASTOS_TEXT_CBIDI_H__

#include "_Elastos_Text_CBidi.h"
#include "Bidi.h"

namespace Elastos {
namespace Text {

CarClass(CBidi) , public Bidi
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CBIDI_H__
