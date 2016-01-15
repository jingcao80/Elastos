
#ifndef _ELASTOS_DROID_WEBKIT_NET_NETERROR_H_
#define _ELASTOS_DROID_WEBKIT_NET_NETERROR_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.net;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Net {

class NetError
    : public Object
{
public:
    static const Int32 ERR_IO_PENDING = -1;
    static const Int32 ERR_FAILED = -2;
    static const Int32 ERR_ABORTED = -3;
    static const Int32 ERR_INVALID_ARGUMENT = -4;
    static const Int32 ERR_INVALID_HANDLE = -5;
    static const Int32 ERR_FILE_NOT_FOUND = -6;
    static const Int32 ERR_TIMED_OUT = -7;
    static const Int32 ERR_FILE_TOO_BIG = -8;
    static const Int32 ERR_UNEXPECTED = -9;
    static const Int32 ERR_ACCESS_DENIED = -10;
    static const Int32 ERR_NOT_IMPLEMENTED = -11;
    static const Int32 ERR_INSUFFICIENT_RESOURCES = -12;
    static const Int32 ERR_OUT_OF_MEMORY = -13;
    static const Int32 ERR_UPLOAD_FILE_CHANGED = -14;
    static const Int32 ERR_SOCKET_NOT_CONNECTED = -15;
    static const Int32 ERR_FILE_EXISTS = -16;
    static const Int32 ERR_FILE_PATH_TOO_LONG = -17;
    static const Int32 ERR_FILE_NO_SPACE = -18;
    static const Int32 ERR_FILE_VIRUS_INFECTED = -19;
    static const Int32 ERR_BLOCKED_BY_CLIENT = -20;
    static const Int32 ERR_NETWORK_CHANGED = -21;
    static const Int32 ERR_BLOCKED_BY_ADMINISTRATOR = -22;
    static const Int32 ERR_SOCKET_IS_CONNECTED = -23;
    static const Int32 ERR_BLOCKED_ENROLLMENT_CHECK_PENDING = -24;
    static const Int32 ERR_CONNECTION_CLOSED = -100;
    static const Int32 ERR_CONNECTION_RESET = -101;
    static const Int32 ERR_CONNECTION_REFUSED = -102;
    static const Int32 ERR_CONNECTION_ABORTED = -103;
    static const Int32 ERR_CONNECTION_FAILED = -104;
    static const Int32 ERR_NAME_NOT_RESOLVED = -105;
    static const Int32 ERR_INTERNET_DISCONNECTED = -106;
    static const Int32 ERR_SSL_PROTOCOL_ERROR = -107;
    static const Int32 ERR_ADDRESS_INVALID = -108;
    static const Int32 ERR_ADDRESS_UNREACHABLE = -109;
    static const Int32 ERR_SSL_CLIENT_AUTH_CERT_NEEDED = -110;
    static const Int32 ERR_TUNNEL_CONNECTION_FAILED = -111;
    static const Int32 ERR_NO_SSL_VERSIONS_ENABLED = -112;
    static const Int32 ERR_SSL_VERSION_OR_CIPHER_MISMATCH = -113;
    static const Int32 ERR_SSL_RENEGOTIATION_REQUESTED = -114;
    static const Int32 ERR_PROXY_AUTH_UNSUPPORTED = -115;
    static const Int32 ERR_CERT_ERROR_IN_SSL_RENEGOTIATION = -116;
    static const Int32 ERR_BAD_SSL_CLIENT_AUTH_CERT = -117;
    static const Int32 ERR_CONNECTION_TIMED_OUT = -118;
    static const Int32 ERR_HOST_RESOLVER_QUEUE_TOO_LARGE = -119;
    static const Int32 ERR_SOCKS_CONNECTION_FAILED = -120;
    static const Int32 ERR_SOCKS_CONNECTION_HOST_UNREACHABLE = -121;
    static const Int32 ERR_NPN_NEGOTIATION_FAILED = -122;
    static const Int32 ERR_SSL_NO_RENEGOTIATION = -123;
    static const Int32 ERR_WINSOCK_UNEXPECTED_WRITTEN_BYTES = -124;
    static const Int32 ERR_SSL_DECOMPRESSION_FAILURE_ALERT = -125;
    static const Int32 ERR_SSL_BAD_RECORD_MAC_ALERT = -126;
    static const Int32 ERR_PROXY_AUTH_REQUESTED = -127;
    static const Int32 ERR_SSL_UNSAFE_NEGOTIATION = -128;
    static const Int32 ERR_SSL_WEAK_SERVER_EPHEMERAL_DH_KEY = -129;
    static const Int32 ERR_PROXY_CONNECTION_FAILED = -130;
    static const Int32 ERR_MANDATORY_PROXY_CONFIGURATION_FAILED = -131;
    static const Int32 ERR_PRECONNECT_MAX_SOCKET_LIMIT = -133;
    static const Int32 ERR_SSL_CLIENT_AUTH_PRIVATE_KEY_ACCESS_DENIED = -134;
    static const Int32 ERR_SSL_CLIENT_AUTH_CERT_NO_PRIVATE_KEY = -135;
    static const Int32 ERR_PROXY_CERTIFICATE_INVALID = -136;
    static const Int32 ERR_NAME_RESOLUTION_FAILED = -137;
    static const Int32 ERR_NETWORK_ACCESS_DENIED = -138;
    static const Int32 ERR_TEMPORARILY_THROTTLED = -139;
    static const Int32 ERR_HTTPS_PROXY_TUNNEL_RESPONSE = -140;
    static const Int32 ERR_SSL_CLIENT_AUTH_SIGNATURE_FAILED = -141;
    static const Int32 ERR_MSG_TOO_BIG = -142;
    static const Int32 ERR_SPDY_SESSION_ALREADY_EXISTS = -143;
    static const Int32 ERR_WS_PROTOCOL_ERROR = -145;
    static const Int32 ERR_PROTOCOL_SWITCHED = -146;
    static const Int32 ERR_ADDRESS_IN_USE = -147;
    static const Int32 ERR_SSL_HANDSHAKE_NOT_COMPLETED = -148;
    static const Int32 ERR_SSL_BAD_PEER_PUBLIC_KEY = -149;
    static const Int32 ERR_SSL_PINNED_KEY_NOT_IN_CERT_CHAIN = -150;
    static const Int32 ERR_CLIENT_AUTH_CERT_TYPE_UNSUPPORTED = -151;
    static const Int32 ERR_ORIGIN_BOUND_CERT_GENERATION_TYPE_MISMATCH = -152;
    static const Int32 ERR_SSL_DECRYPT_ERROR_ALERT = -153;
    static const Int32 ERR_WS_THROTTLE_QUEUE_TOO_LARGE = -154;
    static const Int32 ERR_TOO_MANY_SOCKET_STREAMS = -155;
    static const Int32 ERR_SSL_SERVER_CERT_CHANGED = -156;
    static const Int32 ERR_SSL_INAPPROPRIATE_FALLBACK = -157;
    static const Int32 ERR_CT_NO_SCTS_VERIFIED_OK = -158;
    static const Int32 ERR_SSL_UNRECOGNIZED_NAME_ALERT = -159;
    static const Int32 ERR_SOCKET_SET_RECEIVE_BUFFER_SIZE_ERROR = -160;
    static const Int32 ERR_SOCKET_SET_SEND_BUFFER_SIZE_ERROR = -161;
    static const Int32 ERR_SOCKET_RECEIVE_BUFFER_SIZE_UNCHANGEABLE = -162;
    static const Int32 ERR_SOCKET_SEND_BUFFER_SIZE_UNCHANGEABLE = -163;
    static const Int32 ERR_CERT_COMMON_NAME_INVALID = -200;
    static const Int32 ERR_CERT_DATE_INVALID = -201;
    static const Int32 ERR_CERT_AUTHORITY_INVALID = -202;
    static const Int32 ERR_CERT_CONTAINS_ERRORS = -203;
    static const Int32 ERR_CERT_NO_REVOCATION_MECHANISM = -204;
    static const Int32 ERR_CERT_UNABLE_TO_CHECK_REVOCATION = -205;
    static const Int32 ERR_CERT_REVOKED = -206;
    static const Int32 ERR_CERT_INVALID = -207;
    static const Int32 ERR_CERT_WEAK_SIGNATURE_ALGORITHM = -208;
    static const Int32 ERR_CERT_NON_UNIQUE_NAME = -210;
    static const Int32 ERR_CERT_WEAK_KEY = -211;
    static const Int32 ERR_CERT_NAME_CONSTRAINT_VIOLATION = -212;
    static const Int32 ERR_CERT_END = -213;
    static const Int32 ERR_INVALID_URL = -300;
    static const Int32 ERR_DISALLOWED_URL_SCHEME = -301;
    static const Int32 ERR_UNKNOWN_URL_SCHEME = -302;
    static const Int32 ERR_TOO_MANY_REDIRECTS = -310;
    static const Int32 ERR_UNSAFE_REDIRECT = -311;
    static const Int32 ERR_UNSAFE_PORT = -312;
    static const Int32 ERR_INVALID_RESPONSE = -320;
    static const Int32 ERR_INVALID_CHUNKED_ENCODING = -321;
    static const Int32 ERR_METHOD_NOT_SUPPORTED = -322;
    static const Int32 ERR_UNEXPECTED_PROXY_AUTH = -323;
    static const Int32 ERR_EMPTY_RESPONSE = -324;
    static const Int32 ERR_RESPONSE_HEADERS_TOO_BIG = -325;
    static const Int32 ERR_PAC_STATUS_NOT_OK = -326;
    static const Int32 ERR_PAC_SCRIPT_FAILED = -327;
    static const Int32 ERR_REQUEST_RANGE_NOT_SATISFIABLE = -328;
    static const Int32 ERR_MALFORMED_IDENTITY = -329;
    static const Int32 ERR_CONTENT_DECODING_FAILED = -330;
    static const Int32 ERR_NETWORK_IO_SUSPENDED = -331;
    static const Int32 ERR_SYN_REPLY_NOT_RECEIVED = -332;
    static const Int32 ERR_ENCODING_CONVERSION_FAILED = -333;
    static const Int32 ERR_UNRECOGNIZED_FTP_DIRECTORY_LISTING_FORMAT = -334;
    static const Int32 ERR_INVALID_SPDY_STREAM = -335;
    static const Int32 ERR_NO_SUPPORTED_PROXIES = -336;
    static const Int32 ERR_SPDY_PROTOCOL_ERROR = -337;
    static const Int32 ERR_INVALID_AUTH_CREDENTIALS = -338;
    static const Int32 ERR_UNSUPPORTED_AUTH_SCHEME = -339;
    static const Int32 ERR_ENCODING_DETECTION_FAILED = -340;
    static const Int32 ERR_MISSING_AUTH_CREDENTIALS = -341;
    static const Int32 ERR_UNEXPECTED_SECURITY_LIBRARY_STATUS = -342;
    static const Int32 ERR_MISCONFIGURED_AUTH_ENVIRONMENT = -343;
    static const Int32 ERR_UNDOCUMENTED_SECURITY_LIBRARY_STATUS = -344;
    static const Int32 ERR_RESPONSE_BODY_TOO_BIG_TO_DRAIN = -345;
    static const Int32 ERR_RESPONSE_HEADERS_MULTIPLE_CONTENT_LENGTH = -346;
    static const Int32 ERR_INCOMPLETE_SPDY_HEADERS = -347;
    static const Int32 ERR_PAC_NOT_IN_DHCP = -348;
    static const Int32 ERR_RESPONSE_HEADERS_MULTIPLE_CONTENT_DISPOSITION = -349;
    static const Int32 ERR_RESPONSE_HEADERS_MULTIPLE_LOCATION = -350;
    static const Int32 ERR_SPDY_SERVER_REFUSED_STREAM = -351;
    static const Int32 ERR_SPDY_PING_FAILED = -352;
    static const Int32 ERR_CONTENT_LENGTH_MISMATCH = -354;
    static const Int32 ERR_INCOMPLETE_CHUNKED_ENCODING = -355;
    static const Int32 ERR_QUIC_PROTOCOL_ERROR = -356;
    static const Int32 ERR_RESPONSE_HEADERS_TRUNCATED = -357;
    static const Int32 ERR_QUIC_HANDSHAKE_FAILED = -358;
    static const Int32 ERR_REQUEST_FOR_SECURE_RESOURCE_OVER_INSECURE_QUIC = -359;
    static const Int32 ERR_SPDY_INADEQUATE_TRANSPORT_SECURITY = -360;
    static const Int32 ERR_SPDY_FLOW_CONTROL_ERROR = -361;
    static const Int32 ERR_SPDY_FRAME_SIZE_ERROR = -362;
    static const Int32 ERR_SPDY_COMPRESSION_ERROR = -363;
    static const Int32 ERR_PROXY_AUTH_REQUESTED_WITH_NO_CONNECTION = -364;
    static const Int32 ERR_CACHE_MISS = -400;
    static const Int32 ERR_CACHE_READ_FAILURE = -401;
    static const Int32 ERR_CACHE_WRITE_FAILURE = -402;
    static const Int32 ERR_CACHE_OPERATION_NOT_SUPPORTED = -403;
    static const Int32 ERR_CACHE_OPEN_FAILURE = -404;
    static const Int32 ERR_CACHE_CREATE_FAILURE = -405;
    static const Int32 ERR_CACHE_RACE = -406;
    static const Int32 ERR_CACHE_CHECKSUM_READ_FAILURE = -407;
    static const Int32 ERR_CACHE_CHECKSUM_MISMATCH = -408;
    static const Int32 ERR_INSECURE_RESPONSE = -501;
    static const Int32 ERR_NO_PRIVATE_KEY_FOR_CERT = -502;
    static const Int32 ERR_ADD_USER_CERT_FAILED = -503;
    static const Int32 ERR_FTP_FAILED = -601;
    static const Int32 ERR_FTP_SERVICE_UNAVAILABLE = -602;
    static const Int32 ERR_FTP_TRANSFER_ABORTED = -603;
    static const Int32 ERR_FTP_FILE_BUSY = -604;
    static const Int32 ERR_FTP_SYNTAX_ERROR = -605;
    static const Int32 ERR_FTP_COMMAND_NOT_SUPPORTED = -606;
    static const Int32 ERR_FTP_BAD_COMMAND_SEQUENCE = -607;
    static const Int32 ERR_PKCS12_IMPORT_BAD_PASSWORD = -701;
    static const Int32 ERR_PKCS12_IMPORT_FAILED = -702;
    static const Int32 ERR_IMPORT_CA_CERT_NOT_CA = -703;
    static const Int32 ERR_IMPORT_CERT_ALREADY_EXISTS = -704;
    static const Int32 ERR_IMPORT_CA_CERT_FAILED = -705;
    static const Int32 ERR_IMPORT_SERVER_CERT_FAILED = -706;
    static const Int32 ERR_PKCS12_IMPORT_INVALID_MAC = -707;
    static const Int32 ERR_PKCS12_IMPORT_INVALID_FILE = -708;
    static const Int32 ERR_PKCS12_IMPORT_UNSUPPORTED = -709;
    static const Int32 ERR_KEY_GENERATION_FAILED = -710;
    static const Int32 ERR_ORIGIN_BOUND_CERT_GENERATION_FAILED = -711;
    static const Int32 ERR_PRIVATE_KEY_EXPORT_FAILED = -712;
    static const Int32 ERR_SELF_SIGNED_CERT_GENERATION_FAILED = -713;
    static const Int32 ERR_CERT_DATABASE_CHANGED = -714;
    static const Int32 ERR_DNS_MALFORMED_RESPONSE = -800;
    static const Int32 ERR_DNS_SERVER_REQUIRES_TCP = -801;
    static const Int32 ERR_DNS_SERVER_FAILED = -802;
    static const Int32 ERR_DNS_TIMED_OUT = -803;
    static const Int32 ERR_DNS_CACHE_MISS = -804;
    static const Int32 ERR_DNS_SEARCH_EMPTY = -805;
    static const Int32 ERR_DNS_SORT_ERROR = -806;
};

} // namespace Net
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_NET_NETERROR_H_

