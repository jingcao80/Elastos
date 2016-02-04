#include "elastos/droid/emoji/CEmojiFactory.h"
#include "elastos/droid/emoji/CEmojiFactoryHelper.h"
#include "elastos/droid/emoji/EmojiFactory.h"

namespace Elastos {
namespace Droid {
namespace Emoji {

CAR_SINGLETON_IMPL(CEmojiFactoryHelper);
CAR_INTERFACE_IMPL(CEmojiFactoryHelper, Singleton, IEmojiFactoryHelper);

CEmojiFactoryHelper::CEmojiFactoryHelper()
{}

CEmojiFactoryHelper::~CEmojiFactoryHelper()
{}

ECode CEmojiFactoryHelper::NewInstance(
    /* [in] */ const String& className,
    /* [out] */ IEmojiFactory** emojiFactory)
{
    return EmojiFactory::NewInstance(className, emojiFactory);
}

ECode CEmojiFactoryHelper::NewAvailableInstance(
    /* [out] */ IEmojiFactory** emojiFactory)
{
    return EmojiFactory::NewAvailableInstance(emojiFactory);
}

} // namespace Emoji
} // namepsace Droid
} // namespace Elastos
