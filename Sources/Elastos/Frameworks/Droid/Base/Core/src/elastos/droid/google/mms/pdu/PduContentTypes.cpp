
#include "elastos/droid/google/mms/pdu/PduContentTypes.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                           PduContentTypes
//=====================================================================
AutoPtr<ArrayOf<String> > PduContentTypes::sContentTypes = PduContentTypes::MiddleInitContenttypes();

AutoPtr<ArrayOf<String> > PduContentTypes::MiddleInitContenttypes()
{
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(83);
    (*result)[0] = String("*/*");                                        /* 0x00 */
    (*result)[1] = String("text/*");                                     /* 0x01 */
    (*result)[2] = String("text/html");                                  /* 0x02 */
    (*result)[3] = String("text/plain");                                 /* 0x03 */
    (*result)[4] = String("text/x-hdml");                                /* 0x04 */
    (*result)[5] = String("text/x-ttml");                                /* 0x05 */
    (*result)[6] = String("text/x-vCalendar");                           /* 0x06 */
    (*result)[7] = String("text/x-vCard");                               /* 0x07 */
    (*result)[8] = String("text/vnd.wap.wml");                           /* 0x08 */
    (*result)[9] = String("text/vnd.wap.wmlscript");                     /* 0x09 */
    (*result)[10] = String("text/vnd.wap.wta-event");                     /* 0x0A */
    (*result)[11] = String("multipart/*");                                /* 0x0B */
    (*result)[12] = String("multipart/mixed");                            /* 0x0C */
    (*result)[13] = String("multipart/form-data");                        /* 0x0D */
    (*result)[14] = String("multipart/byterantes");                       /* 0x0E */
    (*result)[15] = String("multipart/alternative");                      /* 0x0F */
    (*result)[16] = String("application/*");                              /* 0x10 */
    (*result)[17] = String("application/java-vm");                        /* 0x11 */
    (*result)[18] = String("application/x-www-form-urlencoded");          /* 0x12 */
    (*result)[19] = String("application/x-hdmlc");                        /* 0x13 */
    (*result)[20] = String("application/vnd.wap.wmlc");                   /* 0x14 */
    (*result)[21] = String("application/vnd.wap.wmlscriptc");             /* 0x15 */
    (*result)[22] = String("application/vnd.wap.wta-eventc");             /* 0x16 */
    (*result)[23] = String("application/vnd.wap.uaprof");                 /* 0x17 */
    (*result)[24] = String("application/vnd.wap.wtls-ca-certificate");    /* 0x18 */
    (*result)[25] = String("application/vnd.wap.wtls-user-certificate");  /* 0x19 */
    (*result)[26] = String("application/x-x509-ca-cert");                 /* 0x1A */
    (*result)[27] = String("application/x-x509-user-cert");               /* 0x1B */
    (*result)[28] = String("image/*");                                    /* 0x1C */
    (*result)[29] = String("image/gif");                                  /* 0x1D */
    (*result)[30] = String("image/jpeg");                                 /* 0x1E */
    (*result)[31] = String("image/tiff");                                 /* 0x1F */
    (*result)[32] = String("image/png");                                  /* 0x20 */
    (*result)[33] = String("image/vnd.wap.wbmp");                         /* 0x21 */
    (*result)[34] = String("application/vnd.wap.multipart.*");            /* 0x22 */
    (*result)[35] = String("application/vnd.wap.multipart.mixed");        /* 0x23 */
    (*result)[36] = String("application/vnd.wap.multipart.form-data");    /* 0x24 */
    (*result)[37] = String("application/vnd.wap.multipart.byteranges");   /* 0x25 */
    (*result)[38] = String("application/vnd.wap.multipart.alternative");  /* 0x26 */
    (*result)[39] = String("application/xml");                            /* 0x27 */
    (*result)[40] = String("text/xml");                                   /* 0x28 */
    (*result)[41] = String("application/vnd.wap.wbxml");                  /* 0x29 */
    (*result)[42] = String("application/x-x968-cross-cert");              /* 0x2A */
    (*result)[43] = String("application/x-x968-ca-cert");                 /* 0x2B */
    (*result)[44] = String("application/x-x968-user-cert");               /* 0x2C */
    (*result)[45] = String("text/vnd.wap.si");                            /* 0x2D */
    (*result)[46] = String("application/vnd.wap.sic");                    /* 0x2E */
    (*result)[47] = String("text/vnd.wap.sl");                            /* 0x2F */
    (*result)[48] = String("application/vnd.wap.slc");                    /* 0x30 */
    (*result)[49] = String("text/vnd.wap.co");                            /* 0x31 */
    (*result)[50] = String("application/vnd.wap.coc");                    /* 0x32 */
    (*result)[51] = String("application/vnd.wap.multipart.related");      /* 0x33 */
    (*result)[52] = String("application/vnd.wap.sia");                    /* 0x34 */
    (*result)[53] = String("text/vnd.wap.connectivity-xml");              /* 0x35 */
    (*result)[54] = String("application/vnd.wap.connectivity-wbxml");     /* 0x36 */
    (*result)[55] = String("application/pkcs7-mime");                     /* 0x37 */
    (*result)[56] = String("application/vnd.wap.hashed-certificate");     /* 0x38 */
    (*result)[57] = String("application/vnd.wap.signed-certificate");     /* 0x39 */
    (*result)[58] = String("application/vnd.wap.cert-response");          /* 0x3A */
    (*result)[59] = String("application/xhtml+xml");                      /* 0x3B */
    (*result)[60] = String("application/wml+xml");                        /* 0x3C */
    (*result)[61] = String("text/css");                                   /* 0x3D */
    (*result)[62] = String("application/vnd.wap.mms-message");            /* 0x3E */
    (*result)[63] = String("application/vnd.wap.rollover-certificate");   /* 0x3F */
    (*result)[64] = String("application/vnd.wap.locc+wbxml");             /* 0x40 */
    (*result)[65] = String("application/vnd.wap.loc+xml");                /* 0x41 */
    (*result)[66] = String("application/vnd.syncml.dm+wbxml");            /* 0x42 */
    (*result)[67] = String("application/vnd.syncml.dm+xml");              /* 0x43 */
    (*result)[68] = String("application/vnd.syncml.notification");        /* 0x44 */
    (*result)[69] = String("application/vnd.wap.xhtml+xml");              /* 0x45 */
    (*result)[70] = String("application/vnd.wv.csp.cir");                 /* 0x46 */
    (*result)[71] = String("application/vnd.oma.dd+xml");                 /* 0x47 */
    (*result)[72] = String("application/vnd.oma.drm.message");            /* 0x48 */
    (*result)[73] = String("application/vnd.oma.drm.content");            /* 0x49 */
    (*result)[74] = String("application/vnd.oma.drm.rights+xml");         /* 0x4A */
    (*result)[75] = String("application/vnd.oma.drm.rights+wbxml");       /* 0x4B */
    (*result)[76] = String("application/vnd.wv.csp+xml");                 /* 0x4C */
    (*result)[77] = String("application/vnd.wv.csp+wbxml");               /* 0x4D */
    (*result)[78] = String("application/vnd.syncml.ds.notification");     /* 0x4E */
    (*result)[79] = String("audio/*");                                    /* 0x4F */
    (*result)[80] = String("video/*");                                    /* 0x50 */
    (*result)[81] = String("application/vnd.oma.dd2+xml");                /* 0x51 */
    (*result)[82] = String("application/mikey");                          /* 0x52 */
    return result;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
