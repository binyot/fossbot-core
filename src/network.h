#ifndef FOSSBOT_CORE_NETWORK_H
#define FOSSBOT_CORE_NETWORK_H

#include <functional>
#include <iostream>
#include <string>

#include "defs.h"

namespace core {

auto listen_to_rfcomm(const stream_handle_t &handle, int channel) -> void;

auto listen_to_stdin(const stream_handle_t &handle) -> void;

auto listen_to_tcp(const stream_handle_t &handle, int port) -> void;

}// namespace core

#endif//FOSSBOT_CORE_NETWORK_H
