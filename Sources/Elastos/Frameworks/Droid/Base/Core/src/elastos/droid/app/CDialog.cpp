//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/app/CDialog.h"

namespace Elastos {
namespace Droid {
namespace App {


IDIALOG_METHODS_IMPL(CDialog, Dialog);
IWINDOWCALLBACK_METHODS_IMPL(CDialog, Dialog);
IKEYEVENTCALLBACK_METHODS_IMPL(CDialog, Dialog);

ECode CDialog::constructor(
    /* [in] */ IContext* context)
{
    return Dialog::Init(context);
}

ECode CDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme)
{
    return Dialog::Init(context, theme);
}

ECode CDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ Boolean createContextThemeWrapper)
{
    return Dialog::Init(context, theme, createContextThemeWrapper);
}

ECode CDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean cancelable,
    /* [in] */ IDialogInterfaceOnCancelListener* cancelListener)
{
    return Dialog::Init(context, cancelable, cancelListener);
}

PInterface CDialog::Probe(
    /* [in] */ REIID riid)
{
    return _CDialog::Probe(riid);
}

ECode CDialog::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return Dialog::OnCreateContextMenu(menu, v, menuInfo);
}


} // namespace App
} // namespace Droid
} // namespace Elastos
