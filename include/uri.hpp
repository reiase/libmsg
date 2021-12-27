#ifndef _LIBMSG_URI_HPP_
#define _LIBMSG_URI_HPP_

#include <map>
#include <string>

namespace msg {
struct URI;
bool parse_uri(const std::string &str, URI &uri);

// URI represent a resource as a string, for example:
// - http://www.example.com/path
// - redis://user@host:port/path
// -
// user:passwd@host:port/path/path?query_string=query_string;param=val;param=val
struct URI {
  std::string schema;
  std::string user;
  std::string passwd;
  std::string host;
  std::string port;
  std::string path;
  std::map<std::string, std::string> params;
  std::string query_string;
  std::string endpoint;
  std::string str() {
    std::string retval = "";
    retval += "schema: " + schema + "\n";
    retval += "user: " + user + ":" + passwd + "\n";
    retval += "host: " + host + "\n";
    retval += "port: " + port + "\n";
    retval += "path: " + path + "\n";
    retval += "query_string: " + query_string + "\n";
    return retval;
  }

  static URI create(const std::string &str) {
    URI uri;
    parse_uri(str, uri);
    uri.endpoint = uri.path;
    return uri;
  }
};

}  // namespace msg

#endif /* _LIBMSG_URI_HPP_ */
