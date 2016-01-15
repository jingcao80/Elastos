
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
    AutoPtr<IFade> f;
    CFade::New(CFade::OUT, (IFade**)&f);
    AddTransition(ITransition::Probe(f));
    AutoPtr<IChangeBounds> c;
    CChangeBounds::New((IChangeBounds**)&c);
    AddTransition(ITransition::Probe(c));
    AutoPtr<IFade> fIn;
    CFade::New(CFade::IN, (IFade**)&fIn);
    AddTransition(ITransition::Probe(fIn));
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
