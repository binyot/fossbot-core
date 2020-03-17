#include <chrono>
#include <fstream>

#include <ext/stdio_filebuf.h>

#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <spdlog/spdlog.h>
#include "rfcomm.h"

namespace core {

auto handle_client(int client, const stream_handle_t &handle) -> void
{
  auto filebuf_in = __gnu_cxx::stdio_filebuf<char>(client, std::ios::in);
  auto filebuf_out = __gnu_cxx::stdio_filebuf<char>(client, std::ios::out);
  auto input_stream = std::istream(&filebuf_in);
  auto output_stream = std::ostream(&filebuf_out);
  handle(input_stream, output_stream);
}

auto listen_to_rfcomm(const stream_handle_t &handle, uint8_t channel) -> void
{
  int sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  if (sock < 0) {
    const std::string error = std::strerror(errno);
    spdlog::error(error);
    throw std::runtime_error(error);
  }
  const bdaddr_t bdaddr_any{ { 0, 0, 0, 0, 0, 0 } };
  sockaddr_rc addr_local{ AF_BLUETOOTH, bdaddr_any, channel };
  if (bind(sock, reinterpret_cast<sockaddr *>(&addr_local), sizeof(addr_local)) < 0) {
    const std::string error = std::strerror(errno);
    spdlog::error(error);
    throw std::runtime_error(error);
  }
  const int queue_size{ 0 };
  listen(sock, queue_size);
  int client;
  socklen_t addr_len;
  while ((client = accept4(sock, reinterpret_cast<sockaddr *>(&addr_local), &addr_len, SOCK_CLOEXEC)) != 0) {
    spdlog::info("Accepted client");
    handle_client(client, handle);
    close(client);
    spdlog::info("Client disconnected");
  }
}

auto listen_to_stdin(const stream_handle_t &handle) -> void
{
  handle(std::cin, std::cout);
}

}// namespace core