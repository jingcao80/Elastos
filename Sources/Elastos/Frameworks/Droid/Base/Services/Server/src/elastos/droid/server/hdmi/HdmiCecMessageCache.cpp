
#include "elastos/droid/server/hdmi/HdmiCecMessageCache.h"
#include <Elastos.Droid.Utility.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

HdmiCecMessageCache::HdmiCecMessageCache()
{
#if 0 // TODO: Translate codes below
    AutoPtr<ISparseArray> mCache = new SparseArray<>();
#endif
}

ECode HdmiCecMessageCache::constructor()
{
    return NOERROR;
}

ECode HdmiCecMessageCache::GetMessage(
    /* [in] */ Int32 address,
    /* [in] */ Int32 opcode,
    /* [out] */ IHdmiCecMessage** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SparseArray<HdmiCecMessage> messages = mCache.get(address);
        if (messages == NULL) {
            return NULL;
        }
        return messages.get(opcode);

#endif
}

ECode HdmiCecMessageCache::FlushMessagesFrom(
    /* [in] */ Int32 address)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mCache.remove(address);

#endif
}

ECode HdmiCecMessageCache::FlushAll()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mCache.clear();

#endif
}

ECode HdmiCecMessageCache::CacheMessage(
    /* [in] */ IHdmiCecMessage* message)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int opcode = message.getOpcode();
        if (!isCacheable(opcode)) {
            return;
        }
        int source = message.getSource();
        SparseArray<HdmiCecMessage> messages = mCache.get(source);
        if (messages == NULL) {
            messages = new SparseArray<>();
            mCache.put(source, messages);
        }
        messages.put(opcode, message);

#endif
}

ECode HdmiCecMessageCache::IsCacheable(
    /* [in] */ Int32 opcode,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return CACHEABLE_OPCODES.contains(opcode);

#endif
}

AutoPtr<IFastImmutableArraySet> HdmiCecMessageCache::InitCACHEABLE_OPCODES()
{
    AutoPtr<IFastImmutableArraySet> rev;
#if 0 // TODO: Translate codes below
     = new FastImmutableArraySet<>(new Integer[] {
                        Constants.MESSAGE_SET_OSD_NAME,
                        Constants.MESSAGE_REPORT_PHYSICAL_ADDRESS,
                        Constants.MESSAGE_DEVICE_VENDOR_ID,
                        Constants.MESSAGE_CEC_VERSION,
                }
#endif
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
