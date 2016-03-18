
#include <Elastos.Droid.App.h>
#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/R.h"
#include "elastos/droid/webkit/Plugin.h"

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Webkit {

//===============================================================
//                    Plugin::DefaultClickHandler
//===============================================================

CAR_INTERFACE_IMPL_2(Plugin::DefaultClickHandler, Object, IPreferencesClickHandler, IDialogInterfaceOnClickListener);

ECode Plugin::DefaultClickHandler::HandleClickEvent(
    /* [in] */ IContext* context)
{
    // Show a simple popup dialog containing the description
    // string of the plugin.
    if (mDialog == NULL) {
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);

        AutoPtr<ICharSequence> charSequenceTmp;
        CString::New(mOwner->mName, (ICharSequence**)&charSequenceTmp);
        builder->SetTitle(charSequenceTmp);

        charSequenceTmp = NULL;
        CString::New(mOwner->mDescription, (ICharSequence**)&charSequenceTmp);
        builder->SetMessage(charSequenceTmp);
        builder->SetPositiveButton(R::string::ok, this);
        builder->SetCancelable(FALSE);
        builder->Show((IAlertDialog**)&mDialog);
    }
    return NOERROR;
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::DefaultClickHandler::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    IDialogInterface::Probe(mDialog)->Dismiss();
    mDialog = NULL;
    return NOERROR;
}

ECode Plugin::DefaultClickHandler::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "Plugin::DefaultClickHandler";
    return NOERROR;
}

//===============================================================
//                             Plugin
//===============================================================

CAR_INTERFACE_IMPL(Plugin, Object, IPlugin);

Plugin::Plugin()
{
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
Plugin::Plugin(
    /* [in] */ const String& name,
    /* [in] */ const String& path,
    /* [in] */ const String& fileName,
    /* [in] */ const String& description)
{
    Init(name, path, fileName, description);
}

ECode Plugin::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& path,
    /* [in] */ const String& fileName,
    /* [in] */ const String& description)
{
    Init(name, path, fileName, description);
    return NOERROR;
}


void Plugin::Init(
    /* [in] */ const String& name,
    /* [in] */ const String& path,
    /* [in] */ const String& fileName,
    /* [in] */ const String& description)
{
    mName = name;
    mPath = path;
    mFileName = fileName;
    mDescription = description;
    mHandler = new DefaultClickHandler(this);
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = mName;
    return NOERROR;
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::GetPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    *path = mPath;
    return NOERROR;
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::GetFileName(
    /* [out] */ String* fileName)
{
    VALIDATE_NOT_NULL(fileName);
    *fileName = mFileName;
    return NOERROR;
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::GetDescription(
    /* [out] */ String* description)
{
    VALIDATE_NOT_NULL(description);
    *description = mDescription;
    return NOERROR;
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::SetPath(
    /* [in] */ const String& path)
{
    mPath = path;
    return NOERROR;
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::SetFileName(
    /* [in] */ const String& fileName)
{
    mFileName = fileName;
    return NOERROR;
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::SetDescription(
    /* [in] */ const String& description)
{
    mDescription = description;
    return NOERROR;
}

/**
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
ECode Plugin::SetClickHandler(
    /* [in] */ IPreferencesClickHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

/**
* Invokes the click handler for this plugin.
*
* @hide
* @deprecated This interface was intended to be used by Gears. Since Gears was
* deprecated, so is this class.
*/
ECode Plugin::DispatchClickEvent(
    /* [in] */ IContext* context)
{
    if (mHandler != NULL) {
        mHandler->HandleClickEvent(context);
    }
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
