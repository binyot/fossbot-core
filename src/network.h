#ifndef FOSSBOT_CORE_NETWORK_H
#define FOSSBOT_CORE_NETWORK_H

#include <functional>
#include <iostream>
#include <string>

#include "defs.h"

namespace core {

/**
 * Opens Bluetooth/RFCOMM streams.
 * @param handle
 * @param channel
 */
auto listen_to_rfcomm(const stream_handle_t &handle, int channel) -> void;

/**
 * Uses standard io streams.
 * @param handle
 */
auto listen_to_stdin(const stream_handle_t &handle) -> void;

/**
 * Opens TCP socket streams.
 * @param handle
 * @param port
 */
auto listen_to_tcp(const stream_handle_t &handle, int port) -> void;

}// namespace core

#endif//FOSSBOT_CORE_NETWORK_H
