#ifndef SIG_HANDLER_HPP
#define SIG_HANDLER_HPP

#include <signal.h>
#include <sys/wait.h>
#include <cstdio>
#include "shared_memory.hpp"

void    sig_chld(int sig) noexcept;

#endif