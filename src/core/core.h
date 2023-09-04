#pragma once

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
