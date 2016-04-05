#ifndef  __ELASTOS_DROID_LAUNCHER2_CFOLDEREDITTEXT_H__
#define  __ELASTOS_DROID_LAUNCHER2_CFOLDEREDITTEXT_H__

#include "_Launcher2.h"
#include "_Elastos_Droid_Launcher2_CFolderEditText.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Launcher2_CFolderEditText.h"
#include "elastos/droid/widget/EditText.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Widget::EditText;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CFolderEditText)
    , public EditText
    , public IFolderEditText
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetFolder(
        /* [in] */ IFolder* folder);

    //@Override
    CARAPI OnKeyPreIme(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

private:
    AutoPtr<IFolder> mFolder;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CFOLDEREDITTEXT_H__