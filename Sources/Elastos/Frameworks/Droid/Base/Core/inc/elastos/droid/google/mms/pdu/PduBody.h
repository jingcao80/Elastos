#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUBODY_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUBODY_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IVector;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class PduBody
    : public Object
    , public IPduBody
{
public:
    CAR_INTERFACE_DECL();

    /**
      * Constructor.
      */
    PduBody();

    /**
      * Appends the specified part to the end of this body.
      *
      * @param part part to be appended
      * @return true when success, false when fail
      * @throws NullPointerException when part is null
      */
    virtual CARAPI AddPart(
        /* [in] */ IPduPart* part,
        /* [out] */ Boolean* result);

    /**
      * Inserts the specified part at the specified position.
      *
      * @param index index at which the specified part is to be inserted
      * @param part part to be inserted
      * @throws NullPointerException when part is null
      */
    virtual CARAPI AddPart(
        /* [in] */ Int32 index,
        /* [in] */ IPduPart* part);

    /**
      * Removes the part at the specified position.
      *
      * @param index index of the part to return
      * @return part at the specified index
      */
    virtual CARAPI RemovePart(
        /* [in] */ Int32 index,
        /* [out] */ IPduPart** result);

    /**
      * Remove all of the parts.
      */
    virtual CARAPI RemoveAll();

    /**
      * Get the part at the specified position.
      *
      * @param index index of the part to return
      * @return part at the specified index
      */
    virtual CARAPI GetPart(
        /* [in] */ Int32 index,
        /* [out] */ IPduPart** result);

    /**
      * Get the index of the specified part.
      *
      * @param part the part object
      * @return index the index of the first occurrence of the part in this body
      */
    virtual CARAPI GetPartIndex(
        /* [in] */ IPduPart* part,
        /* [out] */ Int32* result);

    /**
      * Get the number of parts.
      *
      * @return the number of parts
      */
    virtual CARAPI GetPartsNum(
        /* [out] */ Int32* result);

    /**
      * Get pdu part by content id.
      *
      * @param cid the value of content id.
      * @return the pdu part.
      */
    virtual CARAPI GetPartByContentId(
        /* [in] */ const String& cid,
        /* [out] */ IPduPart** result);

    /**
      * Get pdu part by Content-Location. Content-Location of part is
      * the same as filename and name(param of content-type).
      *
      * @param fileName the value of filename.
      * @return the pdu part.
      */
    virtual CARAPI GetPartByContentLocation(
        /* [in] */ const String& contentLocation,
        /* [out] */ IPduPart** result);

    /**
      * Get pdu part by name.
      *
      * @param fileName the value of filename.
      * @return the pdu part.
      */
    virtual CARAPI GetPartByName(
        /* [in] */ const String& name,
        /* [out] */ IPduPart** result);

    /**
      * Get pdu part by filename.
      *
      * @param fileName the value of filename.
      * @return the pdu part.
      */
    virtual CARAPI GetPartByFileName(
        /* [in] */ const String& filename,
        /* [out] */ IPduPart** result);

private:
    CARAPI_(void) PutPartToMaps(
        /* [in] */ IPduPart* part);

private:
    AutoPtr<IVector> mParts; //IPduPart
    AutoPtr<IMap> mPartMapByContentId; // String, PduPart
    AutoPtr<IMap> mPartMapByContentLocation; // String, PduPart
    AutoPtr<IMap> mPartMapByName; // String, PduPart
    AutoPtr<IMap> mPartMapByFileName; // String, PduPart
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUBODY_H__

