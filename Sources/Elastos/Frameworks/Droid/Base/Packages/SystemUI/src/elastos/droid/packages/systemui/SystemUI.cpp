#include "elastos/droid/systemui/SystemUI.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {

SystemUI::SystemUI()
{}

SystemUI::~SystemUI()
{}

ECode SystemUI::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    return NOERROR;
}

ECode SystemUI::SetContext(
    /* [in] */ IContext* context)
{
    assert(context != NULL && "Context can not be NULL.");
    mContext = context;
    return NOERROR;
}

AutoPtr<IContext> SystemUI::GetContext()
{
    return mContext;
}

}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
