#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_GLOBALKEYMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_GLOBALKEYMANAGER_H__

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IKeyEvent;
using Elastos::IO::IPrintWriter;
using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
  * Stores a mapping of global keys.
  * <p>
  * A global key will NOT go to the foreground application and instead only ever be sent via targeted
  * broadcast to the specified component. The action of the intent will be
  * {@link Intent#ACTION_GLOBAL_BUTTON} and the KeyEvent will be included in the intent with
  * {@link Intent#EXTRA_KEY_EVENT}.
  */
class GlobalKeyManager
    : public Object
{
public:
    GlobalKeyManager(
        /* [in] */ IContext* context);

    /**
      * Broadcasts an intent if the keycode is part of the global key mapping.
      *
      * @param context context used to broadcast the event
      * @param keyCode keyCode which triggered this function
      * @param event keyEvent which trigged this function
      * @return {@code true} if this was handled
      */
    virtual CARAPI HandleGlobalKey(
        /* [in] */ IContext* context,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
      * Returns {@code true} if the key will be handled globally.
      */
    virtual CARAPI ShouldHandleGlobalKey(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* pw);

private:
    CARAPI_(void) LoadGlobalKeys(
        /* [in] */ IContext* context);

private:
    static const String TAG;
    static const String TAG_GLOBAL_KEYS;
    static const String ATTR_VERSION;
    static const String TAG_KEY;
    static const String ATTR_KEY_CODE;
    static const String ATTR_COMPONENT;
    static const Int32 GLOBAL_KEY_FILE_VERSION = 1;
    AutoPtr<ISparseArray> mKeyMapping;//IComponentName
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_GLOBALKEYMANAGER_H__

