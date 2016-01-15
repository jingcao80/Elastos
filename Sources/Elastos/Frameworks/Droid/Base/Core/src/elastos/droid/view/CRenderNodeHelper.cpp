
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CRenderNodeHelper.h"
#include "elastos/droid/view/RenderNode.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CRenderNodeHelper, Singleton, IRenderNodeHelper)

CAR_SINGLETON_IMPL(CRenderNodeHelper)

ECode CRenderNodeHelper::constructor()
{
    return NOERROR;
}

ECode CRenderNodeHelper::Create(
    /* [in] */ const String& name,
    /* [in] */ IView* owningView,
    /* [out] */ IRenderNode** node)
{
    AutoPtr<IRenderNode> tmp = RenderNode::Create(name, owningView);
    *node = tmp;
    REFCOUNT_ADD(tmp)
    return NOERROR;
}

ECode CRenderNodeHelper::Adopt(
    /* [in] */ Handle64 nativePtr,
    /* [out] */ IRenderNode** node)
{
    AutoPtr<IRenderNode> tmp = RenderNode::Adopt(nativePtr);
    *node = tmp;
    REFCOUNT_ADD(tmp)
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
