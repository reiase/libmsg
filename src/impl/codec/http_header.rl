#include "logging.h"

#include <map>
#include <sstream>

#include "impl/codec/http_codec.hpp"

namespace msg{

%%{

  machine http;

  action mark_head { head = fpc; }

  action create_http_context {
    ctx.reset();
    ctx.method_ = method;
    ctx.uri_ = uri;
    ctx.proto_ = version;
    ctx.code_ = code;
    // TRACE("add http ctx");
  }

  action set_http_method {
    method.clear();
    method.append(head, fpc - head);
    if (method == "GET") codec.content_length_ = 0;
    // TRACE("method {}", method.c_str());
  }

  action set_http_uri {
    uri.clear();
    uri.append(head, fpc - head);
    // TRACE("uri {}", uri.c_str());
  }

  action set_http_version {
    version.clear();
    version.append(head, fpc-head);
    if (version == "HTTP/1.1") {
      codec.flags_ |= HTTPCodec::KEEP_ALIVE;
    }
    // TRACE("version {}", version.c_str());
  }

  action set_http_code {
    std::string tmp;
    tmp.append(head, fpc - head);
    code = std::atoi(tmp.c_str());
    // TRACE("code %d", code);
  }

  action set_field_name {
    field_name.clear();
    field_name.append(head, fpc - head);
    // TRACE("field_name: {}", field_name.c_str());
  }

  action set_keepalive_param_value_timeout {
    param_value.clear();
    param_value.append(head, fpc - head);
    // (&wire)->handle_signal(SignalCode::WIRE_KEEPALIVE_TIMEOUT, atoi(param_value.c_str()));
    // TRACE("keepalive_timeout_value: {}", param_value.c_str());
  }

  action set_keepalive_param_value_max {
    param_value.clear();
    param_value.append(head, fpc - head);
    // (&wire)->handle_signal(SignalCode::WIRE_KEEPALIVE_MAX, atoi(param_value.c_str()));
    // TRACE("keepalive_max_value: {}", param_value.c_str());
  }

  action set_field_value {
    field_value.clear();
    field_value.append(head, fpc - head);
    // codec.ctx_.values[field_name] = field_value;
    // TRACE("field_value: {}", field_value.c_str());
  }

  action set_chunked_encoding { ; }

  action set_content_length {
    std::string tmp;
    tmp.append(head, fpc - head);
    codec.content_length_ = std::atoi(tmp.c_str());
    codec.flags_ |= HTTPCodec::CONTENT_LENGTH;
    // TRACE("bodysize {}", std::atoi(tmp.c_str()));
  }

  action set_keep_alive {
    codec.flags_ |= HTTPCodec::KEEP_ALIVE;
    // TRACE("keep alive");
  }

  action set_error_code {
    std::string tmp;
    tmp.append(head, fpc - head);
    codec.ctx_.code_ = -std::atoi(tmp.c_str());
  }

  action set_upgrade {
    std::string tmp;
    tmp.append(head, fpc - head);
    codec.ctx_.proto_ = tmp;
    // if (tmp == "websocket") {
    //    wire.flags_ |= WIRE_HTTP_KEEP_ALIVE;
    //    ctx.header_ += "Upgrade: websocket\r\n";
    //    ctx.header_ += "Connection: Upgrade\r\n";
    //    ctx.code_ = -101;
    // }
  }

  action set_timestamp_trace {
    // std::stringstream tmp;
    // tmp << "Client-Timestamp: ";
    // tmp.write(head, fpc-head);
    // tmp << "\r\n";
    // ctx.header_ = tmp.str();
    // ctx.tracing_ = true;
  }

  action set_ws_key {
    // std::string tmp;
    // tmp.append(head, fpc-head);

    // std::string sign =
    //     Sha1Signature({tmp, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"});

    // std::stringstream ss;
    // ss << "Sec-WebSocket-Accept: " << sign << "\r\n";
    // ctx.header_ += ss.str();
    // TRACE("set websocket accept");
  }

  action set_ws_protocol {
    // std::string tmp;
    // tmp.append(head, fpc-head);
    // std::stringstream ss;
    // ss << "Sec-WebSocket-Protocol: " << tmp << "\r\n";
    // ctx.header_ += ss.str();
  }

  action set_known { known = true; }

  cr = '\r';
  lf = '\n';
  sp = ' ' | '\t';
  lt = '\r' | '\n';
  comma = ',';

  kchar = alpha | digit | ':' | '+' | '-' | '_' | '.' | '=' | '\/' | '\\' | '*';

  http_method = ("GET" | "POST")
    >mark_head %set_http_method;
  http_uri = (any - sp)+
    >mark_head %set_http_uri;
  http_version = ('HTTP/' (digit '.' digit))
    >mark_head %set_http_version;
  http_code = (digit digit digit)
    >mark_head %set_http_code;

  field_name = ("Content-Md5" | "Content-MD5" | "Date" | "Content-Type" | "Content-type" | "Authorization" | "authorization")
    >mark_head %set_field_name;
  field_value = ((any - sp) any*)
    >mark_head %set_field_value lt*;

  keepalive_field = "Keep-Alive";

  keepalive_param_key_timeout = ("timeout");
  keepalive_param_key_max = ("max");
  keepalive_param_value_timeout = (digit+)
    >mark_head %set_keepalive_param_value_timeout lt*;
  keepalive_param_value_max = (digit+)
    >mark_head %set_keepalive_param_value_max lt*;
  keepalive_param_timeout = (keepalive_param_key_timeout "=" keepalive_param_value_timeout);
  keepalive_param_max = (keepalive_param_key_max "=" keepalive_param_value_max);
  keepalive_param = (keepalive_param_timeout | keepalive_param_max);
  keepalive_params = (keepalive_param (',' sp* keepalive_param)*);
  
  chunked = (('T'|'t')'ransfer-'('E'|'e')'ncoding' sp* ':' sp* ('C'|'c')'hunked')
    %set_chunked_encoding;
  bodysize = ('C'|'c')'ontent-'('L'|'l')'ength' sp* ':' sp* (digit+)
    >mark_head %set_content_length;
  keepalive = (('C'|'c')'onnection' sp* ':' sp* ('K'|'k')'eep-'('A'|'a')'live')
    %set_keep_alive;
  errorcode = ('E'|'e')'rror-'('C'|'c')'ode' sp* ':' sp* (digit+)
    >mark_head %set_error_code;
  upgrade = ('U'|'u')"pgrade" sp* ':' sp* (kchar+) >mark_head %set_upgrade;
  client_timestamp = ('C'|'c')'lient-'('T'|'t')'imestamp' sp* ':' sp* (digit+)
    >mark_head %set_timestamp_trace;
  sec_ws_key = ('S'|'s')'ec-'('W'|'w')'eb'('S'|'s')'ocket-'('K'|'k')'ey' sp* ':' sp* (kchar+)
    >mark_head %set_ws_key;
  sec_ws_protocol = ('S'|'s')'ec-'('W'|'w')'eb'('S'|'s')'ocket-'('P'|'p')'rotocol' sp* ':' sp* (kchar+)
    >mark_head %set_ws_protocol;

  response_status_line = (http_version sp http_code any*)
    %create_http_context;

  request_start_line = (http_method sp http_uri sp http_version lt*)
    %create_http_context;

  keepalive_header = (keepalive_field ":" sp* keepalive_params);
  
  known_cmd = (chunked
             | bodysize
             | keepalive
             | errorcode
             | upgrade
             | keepalive_header
             | client_timestamp
             | sec_ws_key
             | sec_ws_protocol) %set_known;
  header_line = (known_cmd | (field_name sp* ':' sp* field_value));

  main := request_start_line | response_status_line | header_line;
}%%
;

bool parse_header(const std::string &header, HTTPCodec &codec) {
  char *p = (char *)header.data();
  char *pe = p + header.size()-2;
  char *eof = pe;
  int cs;

  bool known = false;

  char * head = NULL;

  std::string field_name;
  std::string field_value;
  std::string param_key;
  std::string param_value;
  std::string method;
  std::string uri;
  std::string version;
  int code = 0;

  HTTPContext &ctx = codec.ctx_;

  %% write data;

  %% write init;
  %% write exec;

  //return true;
  return known;
}
} // msg
