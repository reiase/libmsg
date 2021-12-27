#include <string>

#include "uri.hpp"

//#include "logging.h"

namespace msg {

%%{

  machine url;

  action mark_head {
    head = fpc;
    //TRACE("M {}", head);
  }

  action get_token {
    token.clear();
    token.append(head, fpc-head);
  }

  action set_protocol {
    //TRACE("schema {} {}", head, fpc);
    uri.schema = token;
  }

  action set_user {
    //TRACE("user {} {}", head, fpc);
    uri.user.clear();
    uri.user.append(head, fpc-head);
  }

  action set_passwd {
    //TRACE("user {} {}", head, fpc);
    uri.passwd.clear();
    uri.passwd.append(head, fpc-head);
  }

  action set_host {
    //TRACE("host {} {}", head, fpc);
    uri.host.clear();
    uri.host.append(head, fpc - head);
  }

  action set_port {
    //TRACE("port {} {}", head, fpc);
    uri.port.clear();
    uri.port.append(head, fpc-head);
  }

  action set_path {
    //TRACE("path {} {}", head, fpc);
    uri.path.clear();
    uri.path.append(head, fpc - head);
  }

  action set_query_string {
    //TRACE("query {} {}", head, fpc);
    uri.query_string.clear();
    uri.query_string.append(head, fpc - head);
  }

  action set_param {
    std::string key = token;
    std::string val;
    val.append(head, fpc-head);
    uri.params[key] = val;
  }

  schema_char = (lower | digit | "+" | "-" | ".");
  protocol = ((upper|lower) schema_char*) >mark_head %get_token;
  user_char = (upper|lower|digit);
  user = (user_char+) > mark_head %set_user;
  passwd_char = (upper|lower|digit);
  passwd = (passwd_char+) >mark_head %set_passwd;
  host_char = alpha | digit | '\.' | '-' | '_';
  host = host_char+ >mark_head %set_host;
  port = (digit+) > mark_head %set_port;
  path_char = alpha | digit | '.' | '-' | '_' | '/';
  path = ("/" path_char*) >mark_head %set_path;
  query_string_char = alpha | digit | '.' | "=" |'-' | '_' | '?' | '&';
  query_string = (query_string_char+) >mark_head %set_query_string ;

  param_key = ((upper|lower|digit|"-"|"_")+);
  param_val = ((upper|lower|digit|"-"|"_")+);
  param_pair = (";" param_key >mark_head %get_token "=" param_val >mark_head %set_param);

  full_host = host ":" port
    | host;
  auth_host = user ":" passwd "@" full_host
    | user "@" full_host
    | full_host;
  endpoint = ((protocol "://") %set_protocol auth_host)
    | auth_host;
  full_path = endpoint
    | (endpoint path)
    | (endpoint path param_pair*)
    | (endpoint path "?" query_string)
    | (endpoint path "?" query_string param_pair*)
    | path;
  main := full_path;
}%%

bool parse_uri(const std::string &str, URI &uri) {
  char *p = (char *) str.data();
  char *pe = p + str.size();
  char *eof = pe;
  int cs;

  char *head;
  std::string token;

  %% write data;

  %% write init;
  %% write exec;

  return true;
}

}  // namespace msg
