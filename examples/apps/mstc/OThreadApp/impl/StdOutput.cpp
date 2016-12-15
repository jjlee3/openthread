#include "detail/pch.h"
#include "detail/StdOutput.h"
#include <iostream>

mstc::base::Monitor<std::ostream&> g_out{std::cout};