#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/audiopolicy/CAudioPolicyConfig.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

String CAudioPolicyConfig::TAG("AudioPolicyConfig");

CAR_INTERFACE_IMPL_2(CAudioPolicyConfig, Object, IAudioPolicyConfig, IParcelable)

CAR_OBJECT_IMPL(CAudioPolicyConfig)

CAudioPolicyConfig::CAudioPolicyConfig()
{
}

CAudioPolicyConfig::~CAudioPolicyConfig()
{
}

ECode CAudioPolicyConfig::constructor()
{
    return NOERROR;
}

ECode CAudioPolicyConfig::constructor(
    /* [in] */ IArrayList * mixes)
{
    mMixes = mixes;
    return NOERROR;
}

ECode CAudioPolicyConfig::AddMix(
    /* [in] */ IAudioMix * mix)
{
    if (mix == NULL) {
        // throw new IllegalArgumentException("Illegal null AudioMix argument");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMixes->Add(mix);
    return NOERROR;
}

ECode CAudioPolicyConfig::ToString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    String textDump("android.media.audiopolicy.AudioPolicyConfig:\n");
    Int32 size;
    mMixes->GetSize(&size);
    textDump += size + " AudioMix:\n";
    // for(AudioMix mix : mMixes) {
    //     // write mix route flags
    //     textDump += "* route flags=0x" + Integer.toHexString(mix.getRouteFlags()) + "\n";
    //     // write mix format
    //     textDump += "  rate=" + mix.getFormat().getSampleRate() + "Hz\n";
    //     textDump += "  encoding=" + mix.getFormat().getEncoding() + "\n";
    //     textDump += "  channels=0x";
    //     textDump += Integer.toHexString(mix.getFormat().getChannelMask()).toUpperCase() +"\n";
    //     // write mix rules
    //     final ArrayList<AttributeMatchCriterion> criteria = mix.getRule().getCriteria();
    //     for (AttributeMatchCriterion criterion : criteria) {
    //         switch(criterion.mRule) {
    //             case AudioMixingRule.RULE_EXCLUDE_ATTRIBUTE_USAGE:
    //                 textDump += "  exclude usage ";
    //                 textDump += criterion.mAttr.usageToString();
    //                 break;
    //             case AudioMixingRule.RULE_MATCH_ATTRIBUTE_USAGE:
    //                 textDump += "  match usage ";
    //                 textDump += criterion.mAttr.usageToString();
    //                 break;
    //             default:
    //                 textDump += "invalid rule!";
    //         }
    //         textDump += "\n";
    //     }
    // }
    *result = textDump;
    return NOERROR;
}

ECode CAudioPolicyConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&obj);
    mMixes = IArrayList::Probe(obj);
    return NOERROR;
}

ECode CAudioPolicyConfig::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mMixes);
    return NOERROR;
}


} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos
