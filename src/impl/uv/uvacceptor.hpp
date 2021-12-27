#include <iostream>
//
#include <uv.h>
//
#include "../acceptor_base.hpp"
#include "../reactor_base.hpp"
#include "../session_wrap.hpp"
#include "uri.hpp"

namespace msg {

namespace {
struct writeop {
  uv_write_t req;
  uv_buf_t buf;
  std::string raw;

  writeop(std::string &&data) : raw(std::move(data)) {
    buf.base = (char *)raw.data();
    buf.len = raw.size();
  }
};
}  // namespace

struct UVSessionData {
  uv_tcp_t client;
  std::string buf;
};

struct UVAcceptor : public AcceptorBase<UVAcceptor> {
  uv_tcp_t server_;

  UVAcceptor(const HandlerFactory &factory)
      : AcceptorBase<UVAcceptor>(factory) {}

  void set_port_reuse(uv_handle_t *tcp) {
    uv_os_fd_t fd;
    uv_fileno(tcp, &fd);
    int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));
  }

  void listen(const std::string &addr) {
    static std::mutex lock;
    std::lock_guard<std::mutex> lg(lock);

    URI uri = URI::create(addr);
    struct sockaddr_in sin;
    uv_ip4_addr(uri.host.c_str(),                         //
                std::strtol(uri.port.c_str(), NULL, 10),  //
                &sin);
    uv_loop_t *loop = current_thread_event_loop<uv_loop_t *>();
    server_.data = (void *)this;
    uv_tcp_init_ex(loop, &server_, AF_INET);
    set_port_reuse((uv_handle_t *)&server_);
    uv_tcp_bind(&server_, (const struct sockaddr *)&sin, 0);
    uv_listen((uv_stream_t *)(&server_), 128, UVAcceptor::on_connection);
  }

  static void on_connection(uv_stream_t *server, int status) {
    UVAcceptor *self = (UVAcceptor *)server->data;
    SessionWrap<UVSessionData> *session =
        new SessionWrap<UVSessionData>(self->factory_);
    uv_loop_t *loop = current_thread_event_loop<uv_loop_t *>();

    uv_tcp_init(loop, &(session->data.client));

    if (uv_accept(server, (uv_stream_t *)(&session->data.client)) == 0) {
      // set no delay
      uv_tcp_nodelay(&session->data.client, 1);

      /// set send/recv buffer size
      // int32_t I512 = 512;
      // uv_send_buffer_size((uv_handle_t *)&session->data.client, &I512);
      // uv_recv_buffer_size((uv_handle_t *)&session->data.client, &I512);

      session->data.client.data = (void *)session;
      session->send_ = [session](std::string &&data) {
        writeop *req = new writeop(std::move(data));
        uv_write((uv_write_t *)req,  //
                 (uv_stream_t *)(&session->data.client), &req->buf, 1,
                 on_write);
        return 0L;
      };
      uv_read_start((uv_stream_t *)(&session->data.client),
                    UVAcceptor::on_alloc, UVAcceptor::on_read);
    } else {
      uv_close((uv_handle_t *)(&session->data.client), NULL);
      delete session;
    }
  }

  static void on_alloc(uv_handle_t *handle, size_t suggested_size,
                       uv_buf_t *buf) {
    auto sess = (SessionWrap<UVSessionData> *)handle->data;
    if (sess->data.buf.size() < suggested_size)
      sess->data.buf.reserve(suggested_size);

    buf->base = (char *)sess->data.buf.data();
    buf->len = suggested_size;
  }

  static void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    auto sess = (SessionWrap<UVSessionData> *)client->data;

    if (nread > 0) {
      sess->recv_(buf->base, nread);
      return;
    }
    if (nread == UV_EOF) {
      uv_close((uv_handle_t *)client, NULL);
      delete sess;
      return;
    }
    if (nread < 0) {
      std::cout << "error:" << nread  //
                << ", msg: " << uv_err_name(nread) << std::endl;
      uv_close((uv_handle_t *)client, NULL);
      delete sess;
      return;
    }
    return;
  }

  static void on_write(uv_write_t *req, int status) {
    if (status) {
      fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    writeop *op = (writeop *)req;
    delete op;
  }
};

}  // namespace msg