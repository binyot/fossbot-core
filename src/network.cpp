#include <chrono>
#include <fstream>

#include <ext/stdio_filebuf.h>

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <spdlog/spdlog.h>
#include "network.h"

namespace nonstd {
auto perror() -> void
{
  const std::string error = std::strerror(errno);
  spdlog::error(error);
  throw std::runtime_error(error);
}
}// namespace nonstd

namespace core {

auto handle_client(int client, const stream_handle_t &handle) -> void
{
  auto filebuf_in = __gnu_cxx::stdio_filebuf<char>(client, std::ios::in);
  auto filebuf_out = __gnu_cxx::stdio_filebuf<char>(client, std::ios::out);
  auto input_stream = std::istream(&filebuf_in);
  auto output_stream = std::ostream(&filebuf_out);
  handle(input_stream, output_stream);
}

auto listen_socket(const stream_handle_t &handle, int sock, int queue_size = 1)
{
  listen(sock, queue_size);
  int client;
  while ((client = accept(sock, nullptr, nullptr)) != 0) {
    spdlog::info("Accepted client");
    handle_client(client, handle);
    close(client);
    spdlog::info("Client disconnected");
  }
}

auto listen_to_rfcomm(const stream_handle_t &handle, int channel) -> void
{
  auto sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  if (sock < 0) {
    nonstd::perror();
  }
  const bdaddr_t bdaddr_any{ { 0, 0, 0, 0, 0, 0 } };
  sockaddr_rc addr_local{ AF_BLUETOOTH, bdaddr_any, static_cast<uint8_t>(channel) };
  if (bind(sock, reinterpret_cast<sockaddr *>(&addr_local), sizeof(addr_local)) < 0) {
    nonstd::perror();
  }
  listen_socket(handle, sock);
}

auto listen_to_tcp(const stream_handle_t &handle, int port) -> void
{
  auto sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    nonstd::perror();
  }
  auto addr_loc = sockaddr_in{};
  addr_loc.sin_family = AF_INET;
  addr_loc.sin_addr.s_addr = htonl(INADDR_ANY);
  addr_loc.sin_port = htons(static_cast<uint16_t>(port));
  if ((bind(sock, reinterpret_cast<sockaddr *>(&addr_loc), sizeof(addr_loc))) != 0) {
    nonstd::perror();
  }
  listen_socket(handle, sock);
}

auto listen_to_stdin(const stream_handle_t &handle) -> void
{
  handle(std::cin, std::cout);
}

}// namespace core
