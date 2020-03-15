#ifndef FOSSBOT_CORE_RFCOMM_H
#define FOSSBOT_CORE_RFCOMM_H

#include <functional>
#include <iostream>
#include <string>

namespace core {

using stream_handle_t = std::function<void(std::istream &is, std::ostream &os)>;

auto listen_to_rfcomm(const stream_handle_t &handle, uint8_t channel) -> void;

}// namespace core

#endif//FOSSBOT_CORE_RFCOMM_H
