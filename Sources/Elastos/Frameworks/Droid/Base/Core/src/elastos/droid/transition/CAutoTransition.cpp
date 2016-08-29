
#include "elastos/droid/transition/CAutoTransition.h"
#include "elastos/droid/transition/CFade.h"
#include "elastos/droid/transition/CChangeBounds.h"

using Elastos::Droid::Transition::CFade;
using Elastos::Droid::Transition::CChangeBounds;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CAutoTransition::
//===============================================================
CAR_OBJECT_IMPL(CAutoTransition)

ECode CAutoTransition::constructor()
{
    TransitionSet::constructor();
    Init();
    return NOERROR;
}

ECode CAutoTransition::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    TransitionSet::constructor(context, attrs);
    Init();
    return NOERROR;
}

void CAutoTransition::Init()
{
    SetOrdering(ORDERING_SEQUENTIAL);
    AutoPtr<ITransition> f;
    CFade::New(CFade::OUT, (ITransition**)&f);
    AddTransition(f);
    AutoPtr<ITransition> c;
    CChangeBounds::New((ITransition**)&c);
    AddTransition(c);
    AutoPtr<ITransition> fIn;
    CFade::New(CFade::IN, (ITransition**)&fIn);
    AddTransition(fIn);
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
