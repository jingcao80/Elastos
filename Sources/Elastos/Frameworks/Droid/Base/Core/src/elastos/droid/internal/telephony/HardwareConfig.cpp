
#include "elastos/droid/internal/telephony/HardwareConfig.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CBitSet;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String HardwareConfig::TAG("HardwareConfig");
CAR_INTERFACE_IMPL(HardwareConfig, Object, IHardwareConfig)

/**
 * default constructor.
 */
HardwareConfig::HardwareConfig(
    /* [in] */ Int32 type)
    : mType(type)
    , mState(0)
    , mRilModel(0)
    , mMaxActiveVoiceCall(0)
    , mMaxActiveDataCall(0)
    , mMaxStandby(0)
{
}

/**
 * create from a resource string format.
 */
HardwareConfig::HardwareConfig(
    /* [in] */ const String& res)
    : mType(0)
    , mState(0)
    , mRilModel(0)
    , mMaxActiveVoiceCall(0)
    , mMaxActiveDataCall(0)
    , mMaxStandby(0)
{
    AutoPtr<ArrayOf<String> > split;
    StringUtils::Split(res, String(","), (ArrayOf<String>**)&split);

    mType = StringUtils::ParseInt32((*split)[0]);

    switch (mType) {
        case DEV_HARDWARE_TYPE_MODEM: {
            AssignModem(
                (*split)[1].Trim(),            /* uuid */
                StringUtils::ParseInt32((*split)[2]), /* state */
                StringUtils::ParseInt32((*split)[3]), /* ril-model */
                StringUtils::ParseInt32((*split)[4]), /* rat */
                StringUtils::ParseInt32((*split)[5]), /* max-voice */
                StringUtils::ParseInt32((*split)[6]), /* max-data */
                StringUtils::ParseInt32((*split)[7])  /* max-standby */
            );
            break;
        }
        case DEV_HARDWARE_TYPE_SIM: {
            AssignSim(
                (*split)[1].Trim(),            /* uuid */
                StringUtils::ParseInt32((*split)[2]), /* state */
                (*split)[3].Trim()             /* modem-uuid */
            );
            break;
        }
    }
}

ECode HardwareConfig::AssignModem(
    /* [in] */ const String& id,
    /* [in] */ Int32 state,
    /* [in] */ Int32 model,
    /* [in] */ Int32 ratBits,
    /* [in] */ Int32 maxV,
    /* [in] */ Int32 maxD,
    /* [in] */ Int32 maxS)
{
    if (mType == DEV_HARDWARE_TYPE_MODEM) {
        String value = StringUtils::ToBinaryString(ratBits);
        AutoPtr<ArrayOf<Char32> > bits = value.GetChars();
        mUuid = id;
        mState = state;
        mRilModel = model;
        Int32 length = bits->GetLength();
        CBitSet::New(length, (IBitSet**)&mRat);
        for (Int32 i = 0 ; i < length ; i++) {
            mRat->Set(i, ((*bits)[i] == '1' ? TRUE : FALSE));
        }
        mMaxActiveVoiceCall = maxV;
        mMaxActiveDataCall = maxD;
        mMaxStandby = maxS;
    }
    return NOERROR;
}

ECode HardwareConfig::AssignSim(
    /* [in] */ const String& id,
    /* [in] */ Int32 state,
    /* [in] */ const String& link)
{
    if (mType == DEV_HARDWARE_TYPE_SIM) {
        mUuid = id;
        mModemUuid = link;
        mState = state;
    }
    return NOERROR;
}

ECode HardwareConfig::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder builder;
    if (mType == DEV_HARDWARE_TYPE_MODEM) {
        builder.Append("Modem ");
        builder.Append(String("{ uuid=") + mUuid);
        builder.Append(String(", state=") + StringUtils::ToString(mState));
        builder.Append(String(", rilModel=") + StringUtils::ToString(mRilModel));
        builder.Append(String(", rat=") + TO_CSTR(mRat));
        builder.Append(String(", maxActiveVoiceCall=") + StringUtils::ToString(mMaxActiveVoiceCall));
        builder.Append(String(", maxActiveDataCall=") + StringUtils::ToString(mMaxActiveDataCall));
        builder.Append(String(", maxStandby=") + StringUtils::ToString(mMaxStandby));
        builder.Append(" }");
    }
    else if (mType == DEV_HARDWARE_TYPE_SIM) {
        builder.Append("Sim ");
        builder.Append(String("{ uuid=") + mUuid);
        builder.Append(String(", modemUuid=") + mModemUuid);
        builder.Append(String(", state=") + StringUtils::ToString(mState));
        builder.Append(" }");
    }
    else {
        builder.Append("Invalid Configration");
    }
    return builder.ToString(result);
}

ECode HardwareConfig::CompareTo(
    /* [in] */ IHardwareConfig* hw,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    String one;
    ToString(&one);
    String two;
    IObject::Probe(hw)->ToString(&two);

    *result = (one.Compare(two));
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
