#include "Elastos.Droid.Os.h"
#include "elastos/droid/media/session/CPlaybackStateCustomActionBuilder.h"
#include "elastos/droid/media/session/CPlaybackStateCustomAction.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CPlaybackStateCustomActionBuilder, Object, IISessionCallback)

CAR_OBJECT_IMPL(CPlaybackStateCustomActionBuilder)

CPlaybackStateCustomActionBuilder::CPlaybackStateCustomActionBuilder()
    : mAction(String(NULL))
    , mIcon(0)
{
}

CPlaybackStateCustomActionBuilder::~CPlaybackStateCustomActionBuilder()
{
}

ECode CPlaybackStateCustomActionBuilder::constructor(
    /* [in] */ const String& action,
    /* [in] */ ICharSequence * name,
    /* [in] */ Int32 icon)
{
    if (action.IsEmpty()) {
        // throw new IllegalArgumentException(
        //         "You must specify an action to build a CustomAction.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String str;
    name->ToString(&str);
    if (str.IsEmpty()) {
        // throw new IllegalArgumentException(
        //         "You must specify a name to build a CustomAction.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (icon == 0) {
        // throw new IllegalArgumentException(
        //         "You must specify an icon resource id to build a CustomAction.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAction = action;
    mName = name;
    mIcon = icon;
    return NOERROR;
}

ECode CPlaybackStateCustomActionBuilder::SetExtras(
    /* [in] */ IBundle * extras)
{
    VALIDATE_NOT_NULL(extras)
    mExtras = extras;
    return NOERROR;
}

ECode CPlaybackStateCustomActionBuilder::Build(
    /* [out] */ IPlaybackStateCustomAction ** result)
{
    VALIDATE_NOT_NULL(result)
    return CPlaybackStateCustomAction::New(mAction, mName, mIcon, mExtras, result);
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
