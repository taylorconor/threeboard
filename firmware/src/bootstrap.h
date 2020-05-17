#pragma once

// bootstrap.h is responsible for exposing both a C and C++ API for running the
// threeboard. This is necessary because we link some additional C-only
// libraries into main.c when compiling the threeboard firmware for the
// simulator.

#ifdef __cplusplus
extern "C" {
#endif

void RunThreeboard();

#ifdef __cplusplus
}
#endif
