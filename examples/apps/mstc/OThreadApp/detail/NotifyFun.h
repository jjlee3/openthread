#pragma once

#include <winsock2.h>
#include <functional>

using notify_left_function = std::function<void (SOCKET)>;