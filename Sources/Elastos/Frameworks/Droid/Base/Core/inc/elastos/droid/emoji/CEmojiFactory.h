
#ifndef __ELASTOS_DROID_EMOJI_CEMOJIFACTORY_H__
#define __ELASTOS_DROID_EMOJI_CEMOJIFACTORY_H__

#include "elastos/droid/emoji/EmojiFactory.h"

namespace Elastos {
namespace Droid {
namespace Emoji {

CarClass(CEmojiFactory)
    , public EmojiFactory
{
public:
    CAR_OBJECT_DECL()

    CEmojiFactory();

    virtual ~CEmojiFactory();
};

} // namespace Emoji
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_EMOJI_CEMOJIFACTORY_H__
