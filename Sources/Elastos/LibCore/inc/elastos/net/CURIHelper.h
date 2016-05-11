
#ifndef __ELASTOS_NET_CURIHELPER_H__
#define __ELASTOS_NET_CURIHELPER_H__

#include "_Elastos_Net_CURIHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

/**
 * A Uniform Resource Identifier that identifies an abstract or physical
 * resource, as specified by <a href="http://www.ietf.org/rfc/rfc2396.txt">RFC
 * 2396</a>.
 *
 * <h3>Parts of a URI</h3>
 * A URI is composed of many parts. This class can both parse URI strings into
 * parts and compose URI strings from parts. For example, consider the parts of
 * this URI:
 * {@code http://username:password@host:8080/directory/file?query#fragment}
 * <table>
 * <tr><th>Component                                            </th><th>Example value                                                      </th><th>Also known as</th></tr>
 * <tr><td>{@link #getScheme() Scheme}                          </td><td>{@code http}                                                       </td><td>protocol</td></tr>
 * <tr><td>{@link #getSchemeSpecificPart() Scheme-specific part}</td><td>{@code //username:password@host:8080/directory/file?query#fragment}</td><td></td></tr>
 * <tr><td>{@link #getAuthority() Authority}                    </td><td>{@code username:password@host:8080}                                </td><td></td></tr>
 * <tr><td>{@link #getUserInfo() User Info}                     </td><td>{@code username:password}                                          </td><td></td></tr>
 * <tr><td>{@link #getHost() Host}                              </td><td>{@code host}                                                       </td><td></td></tr>
 * <tr><td>{@link #getPort() Port}                              </td><td>{@code 8080}                                                       </td><td></td></tr>
 * <tr><td>{@link #getPath() Path}                              </td><td>{@code /directory/file}                                            </td><td></td></tr>
 * <tr><td>{@link #getQuery() Query}                            </td><td>{@code query}                                                      </td><td></td></tr>
 * <tr><td>{@link #getFragment() Fragment}                      </td><td>{@code fragment}                                                   </td><td>ref</td></tr>
 * </table>
 *
 * <h3>Absolute vs. Relative URIs</h3>
 * URIs are either {@link #isAbsolute() absolute or relative}.
 * <ul>
 *     <li><strong>Absolute:</strong> {@code http://android.com/robots.txt}
 *     <li><strong>Relative:</strong> {@code robots.txt}
 * </ul>
 *
 * <p>Absolute URIs always have a scheme. If its scheme is supported by {@link
 * URL}, you can use {@link #toURL} to convert an absolute URI to a URL.
 *
 * <p>Relative URIs do not have a scheme and cannot be converted to URLs. If you
 * have the absolute URI that a relative URI is relative to, you can use {@link
 * #resolve} to compute the referenced absolute URI. Symmetrically, you can use
 * {@link #relativize} to compute the relative URI from one URI to another.
 * <pre>   {@code
 *   URI absolute = new URI("http://android.com/");
 *   URI relative = new URI("robots.txt");
 *   URI resolved = new URI("http://android.com/robots.txt");
 *
 *   // print "http://android.com/robots.txt"
 *   System.out.println(absolute.resolve(relative));
 *
 *   // print "robots.txt"
 *   System.out.println(absolute.relativize(resolved));
 * }</pre>
 *
 * <h3>Opaque vs. Hierarchical URIs</h3>
 * Absolute URIs are either {@link #isOpaque() opaque or hierarchical}. Relative
 * URIs are always hierarchical.
 * <ul>
 *     <li><strong>Hierarchical:</strong> {@code http://android.com/robots.txt}
 *     <li><strong>Opaque:</strong> {@code mailto:robots@example.com}
 * </ul>
 *
 * <p>Opaque URIs have both a scheme and a scheme-specific part that does not
 * begin with the slash character: {@code /}. The contents of the
 * scheme-specific part of an opaque URI is not parsed so an opaque URI never
 * has an authority, user info, host, port, path or query. An opaque URIs may
 * have a fragment, however. A typical opaque URI is
 * {@code mailto:robots@example.com}.
 * <table>
 * <tr><th>Component           </th><th>Example value             </th></tr>
 * <tr><td>Scheme              </td><td>{@code mailto}            </td></tr>
 * <tr><td>Scheme-specific part</td><td>{@code robots@example.com}</td></tr>
 * <tr><td>Fragment            </td><td>                          </td></tr>
 * </table>
 * <p>Hierarchical URIs may have values for any URL component. They always
 * have a non-null path, though that path may be the empty string.
 *
 * <h3>Encoding and Decoding URI Components</h3>
 * Each component of a URI permits a limited set of legal characters. Other
 * characters must first be <i>encoded</i> before they can be embedded in a URI.
 * To recover the original characters from a URI, they may be <i>decoded</i>.
 * <strong>Contrary to what you might expect,</strong> this class uses the
 * term <i>raw</i> to refer to encoded strings. The non-<i>raw</i> accessors
 * return decoded strings. For example, consider how this URI is decoded:
 * {@code http://user:pa55w%3Frd@host:80/doc%7Csearch?q=green%20robots#over%206%22}
 * <table>
 * <tr><th>Component           </th><th>Legal Characters                                                    </th><th>Other Constraints                                  </th><th>Raw Value                                                      </th><th>Value</th></tr>
 * <tr><td>Scheme              </td><td>{@code 0-9}, {@code a-z}, {@code A-Z}, {@code +-.}                  </td><td>First character must be in {@code a-z}, {@code A-Z}</td><td>                                                               </td><td>{@code http}</td></tr>
 * <tr><td>Scheme-specific part</td><td>{@code 0-9}, {@code a-z}, {@code A-Z}, {@code _-!.~'()*,;:$&+=?/[]@}</td><td>Non-ASCII characters okay                          </td><td>{@code //user:pa55w%3Frd@host:80/doc%7Csearch?q=green%20robots}</td><td>{@code //user:pa55w?rd@host:80/doc|search?q=green robots}</td></tr>
 * <tr><td>Authority           </td><td>{@code 0-9}, {@code a-z}, {@code A-Z}, {@code _-!.~'()*,;:$&+=@[]}  </td><td>Non-ASCII characters okay                          </td><td>{@code user:pa55w%3Frd@host:80}                                </td><td>{@code user:pa55w?rd@host:80}</td></tr>
 * <tr><td>User Info           </td><td>{@code 0-9}, {@code a-z}, {@code A-Z}, {@code _-!.~'()*,;:$&+=}     </td><td>Non-ASCII characters okay                          </td><td>{@code user:pa55w%3Frd}                                        </td><td>{@code user:pa55w?rd}</td></tr>
 * <tr><td>Host                </td><td>{@code 0-9}, {@code a-z}, {@code A-Z}, {@code -.[]}                 </td><td>Domain name, IPv4 address or [IPv6 address]        </td><td>                                                               </td><td>host</td></tr>
 * <tr><td>Port                </td><td>{@code 0-9}                                                         </td><td>                                                   </td><td>                                                               </td><td>{@code 80}</td></tr>
 * <tr><td>Path                </td><td>{@code 0-9}, {@code a-z}, {@code A-Z}, {@code _-!.~'()*,;:$&+=/@}   </td><td>Non-ASCII characters okay                          </td><td>{@code /doc%7Csearch}                                          </td><td>{@code /doc|search}</td></tr>
 * <tr><td>Query               </td><td>{@code 0-9}, {@code a-z}, {@code A-Z}, {@code _-!.~'()*,;:$&+=?/[]@}</td><td>Non-ASCII characters okay                          </td><td>{@code q=green%20robots}                                       </td><td>{@code q=green robots}</td></tr>
 * <tr><td>Fragment            </td><td>{@code 0-9}, {@code a-z}, {@code A-Z}, {@code _-!.~'()*,;:$&+=?/[]@}</td><td>Non-ASCII characters okay                          </td><td>{@code over%206%22}                                            </td><td>{@code over 6"}</td></tr>
 * </table>
 * A URI's host, port and scheme are not eligible for encoding and must not
 * contain illegal characters.
 *
 * <p>To encode a URI, invoke any of the multiple-parameter constructors of this
 * class. These constructors accept your original strings and encode them into
 * their raw form.
 *
 * <p>To decode a URI, invoke the single-string constructor, and then use the
 * appropriate accessor methods to get the decoded components.
 *
 * <p>The {@link URL} class can be used to retrieve resources by their URI.
 */
CarClass(CURIHelper)
    , public Singleton
    , public IURIHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Create(
        /* [in] */ const String& uri,
        /* [out] */ IURI** obj);

    CARAPI GetEffectivePort(
        /* [in] */ const String& scheme,
        /* [in] */ Int32 specifiedPort,
        /* [out] */ Int32* result);
};

} // namespace Net
} // namespace Elastos

#endif //  __ELASTOS_NET_CURIHELPER_H__
