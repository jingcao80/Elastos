#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_ERIMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_ERIMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.content.res.Resources;
// import android.content.res.XmlResourceParser;
// import android.telephony.Rlog;
// import android.util.Xml;
// import com.android.internal.util.XmlUtils;
// import org.xmlpull.v1.XmlPullParser;
// import org.xmlpull.v1.XmlPullParserException;
// import java.io.FileInputStream;
// import java.io.FileNotFoundException;
// import java.io.IOException;
// import java.util.HashMap;

using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Content::IContext;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * EriManager loads the ERI file definitions and manages the CDMA roaming information.
  *
  */
class EriManager
    : public Object
    , public IEriManager
{
public:
    class EriFile
        : public Object
    {
    public:
        // Roaming Indicator Table
        EriFile();

    public:
        Int32 mVersionNumber;
        // File version number
        Int32 mNumberOfEriEntries;
        // Number of entries
        Int32 mEriFileType;
        // Eri Phase 0/1
        //int mNumberOfIconImages;               // reserved for future use
        //int mIconImageType;                    // reserved for future use
        AutoPtr<ArrayOf<String> > mCallPromptId;
        // reserved for future use
        AutoPtr<IHashMap> mRoamIndTable; // Integer, EriInfo
    };

    class EriDisplayInformation
        : public Object
    {
    public:
        EriDisplayInformation(
            /* [in] */ Int32 eriIconIndex,
            /* [in] */ Int32 eriIconMode,
            /* [in] */ const String& eriIconText);

        // public void setParameters(int eriIconIndex, int eriIconMode, String eriIconText){
        //    mEriIconIndex = eriIconIndex;
        //    mEriIconMode = eriIconMode;
        //    mEriIconText = eriIconText;
        // }

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Int32 mEriIconIndex;
        Int32 mEriIconMode;
        String mEriIconText;
    };

public:
    CAR_INTERFACE_DECL();

    EriManager();

    CARAPI constructor(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ IContext* context,
        /* [in] */ Int32 eriFileSource);

    virtual CARAPI Dispose();

    virtual CARAPI LoadEriFile();

    /**
      * Returns the version of the ERI file
      *
      */
    virtual CARAPI GetEriFileVersion(
        /* [out] */ Int32* result);

    /**
      * Returns the number of ERI entries parsed
      *
      */
    virtual CARAPI GetEriNumberOfEntries(
        /* [out] */ Int32* result);

    /**
      * Returns the ERI file type value ( 0 for Phase 0, 1 for Phase 1)
      *
      */
    virtual CARAPI GetEriFileType(
        /* [out] */ Int32* result);

    /**
      * Returns if the ERI file has been loaded
      *
      */
    virtual CARAPI IsEriFileLoaded(
        /* [out] */ Boolean* result);

    virtual CARAPI GetCdmaEriIconIndex(
        /* [in] */ Int32 roamInd,
        /* [in] */ Int32 defRoamInd,
        /* [out] */ Int32* result);

    virtual CARAPI GetCdmaEriIconMode(
        /* [in] */ Int32 roamInd,
        /* [in] */ Int32 defRoamInd,
        /* [out] */ Int32* result);

    virtual CARAPI GetCdmaEriText(
        /* [in] */ Int32 roamInd,
        /* [in] */ Int32 defRoamInd,
        /* [out] */ String* result);

private:
    /**
      * Load the ERI file from the MODEM through chipset specific RIL_REQUEST_OEM_HOOK
      *
      * In this case the ERI file can be updated from the Phone Support Tool available
      * from the Chipset vendor
      */
    CARAPI_(void) LoadEriFileFromModem();

    /**
      * Load the ERI file from a File System file
      *
      * In this case the a Phone Support Tool to update the ERI file must be provided
      * to the Operator
      */
    CARAPI_(void) LoadEriFileFromFileSystem();

    /**
      * Load the ERI file from the application framework resources encoded in XML
      *
      */
    CARAPI_(void) LoadEriFileFromXml();

    /**
      * Returns the EriInfo record associated with roamingIndicator
      * or null if the entry is not found
      */
    CARAPI_(AutoPtr<IEriInfo>) GetEriInfo(
        /* [in] */ Int32 roamingIndicator);

    CARAPI_(AutoPtr<EriDisplayInformation>) GetEriDisplayInformation(
        /* [in] */ Int32 roamInd,
        /* [in] */ Int32 defRoamInd);

public:
    static const Int32 ERI_FROM_XML = 0;
    static const Int32 ERI_FROM_FILE_SYSTEM = 1;
    static const Int32 ERI_FROM_MODEM = 2;

private:
    static const String LOGTAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    AutoPtr<IContext> mContext;
    Int32 mEriFileSource;
    Boolean mIsEriFileLoaded;
    AutoPtr<EriFile> mEriFile;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_ERIMANAGER_H__
