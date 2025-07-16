#ifndef SAFE_SYSTEM_CALL_HPP
#define SAFE_SYSTEM_CALL_HPP

#include <sys/shm.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <cstdio>
#include <cerrno>

void    print_error(const char *sys) noexcept;
void    print_error_exit(const char *sys) noexcept;

/* 접미사가 strict인 함수들은 sys call이 -1을 반환하면 errno를 출력하고 exit한다*/

int     shmget_strict(key_t key, size_t size, int shmflg) noexcept;
void    *shmat_strict(int shmid, const void *shmaddr, int shmflg) noexcept;
int     socket_strict(int domain, int type, int protocol) noexcept;
int     bind_strict(int fd, const sockaddr *addr, socklen_t len) noexcept;
int     listen_strict(int sock, int backlog) noexcept;
int     fcntl_strict(int fd, int cmd) noexcept;
int     fcntl_strict(int fd, int cmd, int args) noexcept;
int     epoll_create_strict(int size) noexcept;
int     epoll_ctl_strict(int epfd, int op, int fd, epoll_event *event) noexcept;

/* 접미사가 naive인 함수들은 sys call이 -1을 반환하면 errno를 출력한다.*/

pid_t   fork_naive() noexcept;
int     pthread_rwlock_wrlock_naive(pthread_rwlock_t *rwlock) noexcept;
int     pthread_rwlock_rdlock_naive(pthread_rwlock_t *rwlock) noexcept;
int     pthread_rwlock_unlock_naive(pthread_rwlock_t *rwlock) noexcept;
int     kill_naive(pid_t pid, int sig) noexcept;
int     epoll_wait_naive(int epfd, epoll_event *events, int maxevents, int timeout) noexcept;
int     epoll_ctl_naive(int epfd, int op, int fd, epoll_event *event) noexcept;
int     accept_naive(int fd, sockaddr *addr, socklen_t *addr_len) noexcept;
int     fcntl_naive(int fd, int cmd) noexcept;
int     fcntl_naive(int fd, int cmd, int args) noexcept;
int     socket_naive(int domain, int type, int protocol) noexcept;
int		connect_naive(int fd, const sockaddr *addr, socklen_t len) noexcept;

#endif