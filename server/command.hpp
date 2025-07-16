#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <algorithm>
#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>

#include "exception.hpp"
#include "safe_syscall.hpp"
#include "shared_memory.hpp"
#include "sig_handler.hpp"
#include "channel.hpp"
#include "parse.hpp"

#define LISTEN_QUEUE_SIZE 15
#define MAX_CLIENT_NUM 1024
#define MSG_BUF_LEN 4096

using namespace std;

/* 명령어 인터페이스 */
class Command
{
	public :
	int             args_num;   // 명령어의 인자 수
	vector<string>  args;       // 명령어 인자

	Command(int args_num, const vector<string> &args);
	virtual ~Command() = default;
	virtual void operator()() const = 0;
};

/* 관리자 명령어 */

class Create : public Command
{
	public :
	// 소켓 연결이 되었으나 connect 명령어를 통해 사용자 이름을 등록하지 않은 사용자들
	static vector<int>                      not_connected_ones;
	// 사용자 이름을 등록한 사용자들
	/*
	가장 좋은 성능으로 클라이언트를 관리할 수 있는 것은 O(1)의 시간복잡도인 unordered_map이다.
	fd로 이름을 조회해야 할 때도 있고, 이름으로 fd를 조회해야 할 때도 있으므로 둘 다 만들어서 관리한다.
	*/
	static unordered_map<string, int>       clients_map_name;
	static unordered_map<int, string>       clients_map_fd;

	Create(const vector<string> &args);
	virtual void operator()() const;
	private :
	static void notifyServerClose(int sig) noexcept;
	struct epoll_event  getEpollEvent(int fd, int event) const noexcept;
};

class Delete : public Command
{
	public :
	Delete(const vector<string> &args);
	virtual void operator()() const;
};

class RenameServer : public Command
{
	public :
	RenameServer(const vector<string> &args);
	virtual void operator()() const;
};

class ListServer : public Command
{
	public :
	ListServer(const vector<string> &args);
	virtual void operator()() const;
};

class QuitServer : public Command
{
	public :
	QuitServer(const vector<string> &args);
	virtual void operator()() const;
};

class HelpServer : public Command
{
	public :
	HelpServer(const vector<string> &args);
	virtual void operator()() const;
};

/* 사용자 명령어 */

// connect 명령이 선행되어야 하는 명령어들(connect, help, info를 제외한 모든 사용자 명령어들)은 이 클래스를 상속한다.
class ClientCommandNeedName : public Command
{
	public :
	int clnt_fd;
	ClientCommandNeedName(int clnt_fd, int args_num, const vector<string> &args);
};

class Connect : public Command
{
	private:
	int clnt_fd;
	public :
	Connect(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class Msg : public ClientCommandNeedName
{
	public :
	Msg(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class Broadcast : public ClientCommandNeedName
{
	public :
	Broadcast(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class IscSend : public ClientCommandNeedName
{
	private :
	int port;
	public :
	IscSend(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class IscRecieve : public Command
{
	private :
	int clnt_fd;
	public :
	IscRecieve(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class CreateChannel : public ClientCommandNeedName
{
	public :
	static vector<Channel>  channel_list;

	static vector<Channel>::iterator    findChannelWithName(const string &name);

	CreateChannel(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class ChannelMsg : public ClientCommandNeedName
{
	public :
	ChannelMsg(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class RemoveChannel : public ClientCommandNeedName
{
	public :
	RemoveChannel(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};


class Join : public ClientCommandNeedName
{
	public :
	Join(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class Leave : public ClientCommandNeedName
{
	public :
	Leave(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class Kick : public ClientCommandNeedName
{
	public :
	Kick(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class Refer : public ClientCommandNeedName
{
	public :
	Refer(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class RenameClient : public ClientCommandNeedName
{
	public :
	RenameClient(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class ListClient : public ClientCommandNeedName
{
	public :
	ListClient(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class QuitClient : public Command
{
	private:
	int clnt_fd;
	public :
	QuitClient(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

class HelpClient : public Command
{
	private:
	int clnt_fd;
	public :
	HelpClient(int clnt_fd, const vector<string> &args);
	virtual void operator()() const;
};

// class : public ClientCommandNeedName
// {
//     public :
//     (int clnt_fd, const vector<string> &args);
//     virtual void operator()() const;
// };

#endif
