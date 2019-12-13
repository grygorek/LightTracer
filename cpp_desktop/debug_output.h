#ifndef RENDER_DEBUG_LOG_H
#define RENDER_DEBUG_LOG_H

#include <Windows.h>
#include <chrono>
#include <string>

inline void OutputDebugString(const std::string &s)
{
  OutputDebugStringA(s.c_str());
}

template <class Time>
auto DeltaTimeMilisec(Time end, Time start)
{
  using namespace std::chrono;
  using namespace std::chrono_literals;
  return duration_cast<milliseconds>(end - start).count();
}

#endif // !RENDER_DEBUG_LOG_H
