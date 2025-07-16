
---

#project #network #server #socket_programming 

_OS term project_

---

# 개요

2인 이상의 사용자가 서로 채팅을 주고 받을 수 있도록 하는 채팅 서버와 클라이언트의 구현을 목적으로 한다. 세부적인 사항은 다음과 같다.

1. 채팅 서버를 관리하는 관리자와, 채팅 클라이언트를 이용하는 2인 이상의 사용자가 있다고 가정한다.
2. 관리자는 관리자 프로그램을 이용해 서버를 관리한다. 관리자 프로그램은 최대 5개의 서버까지 관리할 수 있으며, 서버들을 자식 프로세스로 두어 관리한다. 각각의 서버는 고유의 이름을 부여받는다. 관리자 프로그램은 관리자로부터 관리자 명령어를 입력받는다.
3. 사용자는 채팅 클라이언트를 통해 채팅 서버에 접속할 수 있다. 서버는 각각의 클라이언트에게 고유의 사용자 이름을 부여하여 관리한다. 사용자는 사용자 명령어를 입력하여 서버로부터 서비스를 제공받는다.

# 명령어

다음의 규칙을 준수하여 명령어를 사용하여야 한다.

1. 명령어 인자의 개수를 준수하여야 한다.
2. 인자(사용자 이름, 서버 이름, 채널 이름 등등)는 영문, 숫자로만 구성되어야 한다. 단, 예외적으로 msg, channelMsg, broadcast의 메세지 인자는 공백을 포함할 수 있다.
3. 명령어 인자는 공백이어서는 안된다.

따라서 명령어들은 다음의 예외들을 반환할 수 있다. 차후 모든 명령어의 예외 정의에서 다음의 예외들은 생략하도록 한다.

- WrongArgsNum
- InvalidChar
- BlankArgs

이 외의 발생할 수 있는 예외는 _예외_ 문단에 후술한다.

## 관리자 명령어

### create

서버를 1개 증설한다. 단, 현재 서버가 5개인 경우, 예외를 반환한다.
서버의 이름은 9글자를 초과할 수 없다.
아래와 같이 사용한다.

```text

/create <만들고자 하는 서버의 이름> <서버가 사용할 포트 번호>

```

==예외==

- TooManyServer
- DuplicateName
- TooLongName

### delete

서버를 1개 제거한다. 단, 현재 서버가 0개인 경우, 예외를 반환한다.
아래와 같이 사용한다.

```text

/delete <제거하고자 하는 서버의 이름>

```

==예외==

- NoServerToDelete
- InvalidName

### rename

기존의 서버의 이름을 변경한다.
아래와 같이 사용한다.

```text

/rename <이름을 변경하고자 하는 서버의 이름> <새로운 이름>

```

==예외==

- InvalideName
- DuplicateName

### list

동작 중인 서버들의 정보 - 서버 이름, 포트 번호  - 를 목록화하여 출력한다.

```text

/list

```

### quit

서버를 모두 닫고 프로그램을 종료한다.

```text

/quit

```

### help

관리자 명령 매뉴얼을 출력한다.

```text

/help

```

## 사용자 명령어

사용자는 quit, help 명령어를 제외한 다른 명령어를 사용하기에 앞서 무조건 connect 명령어를 사용하여 서버에 사용자 이름을 등록하여야 한다. 그렇지 않으면 NotConnected 예외를 반환한다.

- NotConnected

### connect

고유한 사용자 이름으로 서버에 접속한다. 클라이언트 프로그램을 사용하는 사용자는 무조건 다른 명령어 사용에 앞서 이 명령어를 1회 사용하여야 한다.

```text

/connect <접속하고자 하는 사용자 이름>

```

==예외==

- DuplicateName
- AlreadyConnected

### msg

특정 사용자에게 1대 1 메세지를 보낸다.
두번째 인자 앞에 ;가 포함되어야 한다.

```text

/msg <메세지를 보내고자 하는 사용자 이름> ;<메세지 내용>

```

수신자는 다음의 형식을 한 메세지를 받는다.

```text

msg from <메세지를 보낸 사용자의 이름> : <메세지 내용>

```

==예외==

- InvalidName

### broadcast

서버에 있는 다른 모든 사용자들에게 메세지를 보낸다.
첫번째 인자 앞에 ;가 포함되어야 한다.

```text

/broadcast ;<메세지 내용>

```

수신자들은 다음의 형식을 한 메세지를 받는다.

```text

broadcast from <메세지를 보낸 사용자의 이름> : <메세지 내용>

```

### channelMsg

채널의 모든 사용자에게 메세지를 보낸다.
두번째 인자 앞에 ;가 포함되어야 한다.

```text

/channelMsg <채널의 이름> ;<메세지 내용>

```

수신자들은 다음의 형식을 한 메세지를 받는다.

```text

<채널의 이름> channel msg from <메세지를 보낸 사용자의 이름> : <메세지 내용>

```

==예외==

- InvalidName

### isc

inter server communication. 관리자 프로그램 휘하의 다른 서버에 접속해있는 사용자에게 메세지를 보낸다. 세번째 인자 앞에 ;가 포함되어야 한다.

```text

/isc <서버 이름> <수신자 이름> ;<메세지 내용>

```

수신자는 다음과 같은 형식의 메세지를 받는다.

```text

inter server msg from <송신자 이름>, user of <송신 서버 이름> : <메세지 내용>

```

==예외==

- InvalidName

### createChannel

채널을 만든다. 이 명령어를 통해 채널을 만든 사용자는 해당 채널의 관리자가 된다. 채널 관리자만이 채널 관리자 전용 명령어를 사용할 수 있다.

```text

/createChannel <만들고자 하는 채널의 이름>

```

==예외==

- DuplicateName

### removeChannel

채널 관리자 전용 명령어. 채널을 없앤다.

```text

/removeChannel <없애고자 하는 채널의 이름>

```

채널에 참여하고 있던 사용자들에게 다음의 문구를 송신한다.

```text

Operator <관리자 이름> removed channel <채널 이름>!

```

==예외==

- InvalidName
- NotChannelOperator

### join

채널에 참여한다.

```text

/join <참여하고자 하는 채널의 이름>

```

==예외==

- InvalidName

### leave

채널에서 나간다. 채널 관리자가 나갔을 경우, 가장 처음에 들어온 사용자을 채널 관리자로 삼는다.

```text

/leave <나가고자 하는 채널의 이름>

```

==예외==

- InvalidName
- NotInChannel

### kick

채널 관리자 전용 명령어. 특정 사용자를 채널에서 추방한다.

```text

/kick <채널 이름> <추방하고자 하는 사용자 이름>

```

추방되는 대상은 다음의 형식의 메세지를 받는다.

```text

You are kicked off from <채널 이름> channel!

```

==예외==

- NotChannelOperator
- InvalidName
- NotInChannel

### refer

채널의 관리자, 멤버 정보를 받는다.

```text

/refer <채널 이름>

```

==예외==

- InvalidName

### rename

옵션에 따라 두가지 기능을 하는 명령어이다.

- user 옵션 : 자신의 사용자 이름을 변경한다.
- channel 옵션 : 채널 관리자 전용 명령어. 채널의 이름을 변경한다.

```text

/rename user <새로운 사용자 이름> <인자 개수를 통일하기 위한 더미 인자>

```

```text

/rename channel <기존의 채널 이름> <새로운 채널 이름>

```

channel 옵션의 경우, channel의 모든 사용자에게 다음의 메세지를 송신한다.

```text

Channel operator of <기존의 채널 이름> renamed channel! Channel name is now <새로운 채널 이름>.

```

==예외==

- 공통
	- InvalidOption

- user 옵션
	- DuplicateName

- channel 옵션
	- NotChannelOperator
	- InvalidName
	- DuplicateName

### list

옵션에 따라 세가지 기능을 하는 명령어이다.

- user 옵션 : 현재 서버의 모든 사용자들의 이름을 출력
- channel 옵션 : 현재 서버의 모든 채널의 이름을 출력
- server 옵션 : 관리자 프로그램이 관리하고 있는 모든 서버의 이름을 출력

```text

/list user

```

```text

/list channel

```

```text

/list server

```

==예외==

- InvalidOption

### quit

서버와의 접속을 끊고 클라이언트 프로그램을 종료한다.

```text

/quit

```

### help

사용자 명령 매뉴얼을 출력한다.

```text

/help

```

## 예외

### InvalidCommand

잘못된 명령어를 사용했을 때 발생

```text

Error : Invalid command. Please check command list by using /help command.

```

### WrongArgsNum

명령어에 잘못된 인자 개수를 넘겼을 때 발생

```text

Error : Wrong command format. Please check command format by using /help command.

```

### InvalidChar

영문, 숫자 이외의 문자를 사용했을 때 발생

```text

Error : You can use only alphabet and number to compose argument.

```

### BlankArgs

명령어 인자로 공백("")을 사용했을 때 발생

```text

Error : You can't use blank as argument.

```

### TooLongName

서버 이름이 9자를 초과할 경우 발생

```text

Error : Length of Server name cannot exceed 9.

```

### NotConnected

connect 명령어를 사용하지 않은 채로 다른 사용자 명령어를 사용했을 때 발생

```text

Error : You are not connected yet.

```

### AlreadyConnected

이미 connect 되어있음에도 connect를 사용하는 경우 발생.

```text

Error : You are already connected;

```

### TooManyServer

현재 서버가 5개 이상일 때 관리자 명령어 create를 사용할 때 발생.

```text

Error : You can't build server more than 5.

```

### NoServerToDelete

현재 서버가 0개일 때 관리자 명령어 delete를 사용할 때 발생.

```text

Error : There's no server to delete.

```

### DuplicateName

생성, 변경하고자 하는 이름이 기존의 이름과 중복될 때 발생.

```text

Error : There's already same name. Please check name list by using /list command.

```

### InvalidName

명령어 인자로 존재하지 않는 이름을 전달했을 때 발생.

```text

Error : There's no such name. Please check valid name by using /list command.

```

### InvalidOption

명령어 인자로 존재하지 않는 옵션을 전달했을 때 발생.

```text

Error : There's no such option. Please check valid option by using /help command.

```

### NotChannelOperator

채널 관리자가 아닌데 채널 관리자 전용 명령어를 사용하려 했을 때 발생.

```text

Error : You are not a channel operator.

```

### NotInChannel

채널에 참여하지 않은 사용자를 대상으로 leave, kick 명령어를 사용했을 때 발생

```text

Error : Target is not in the channel.

```

# 구현

## 서버

### chat_server.cpp

관리자 프로그램(부모 프로세스)는 다음의 역할을 한다.

1. while(1) 블록 내에서 지속적으로 관리자 명령어를 입력받고 처리
2. 서버(자식 프로세스)들이 접근할 수 있는 공유 메모리와 이를 보호할 pthread_rwlock을 초기화. 공유 메모리에는 서버들의 pid, port, 이름 정보가 저장된다.
3. SIGCHLD 발생시(자식 프로세스가 종료되었을 시) 자동으로 wait를 실행

```cpp


#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <unordered_map>

#include "command.hpp"
#include "safe_syscall.hpp"
#include "sig_handler.hpp"
#include "parse.hpp"

#define SHARED_MEMORY_KEY 1

using namespace std;

int main()
{
	// 서버들의 정보를 저장할 공유 메모리 초기화
	int shm_id  = shmget_strict(SHARED_MEMORY_KEY, sizeof(SharedMemory), IPC_CREAT | 0666);
	shm = reinterpret_cast<SharedMemory *>(shmat_strict(shm_id, NULL, 0));
	*shm = SharedMemory();

	// data race 대책
	pthread_rwlock_init(&rwlock, NULL);

	// 자식 프로세스(서버) 종료 시 자동으로 wait 실행
	signal(SIGCHLD, sig_chld);

	while (1)
	{
		string          input;
		vector<string>  args;
		Command         *cmd;
		
		getline(cin, input);
		args = parse(input);
		
		try
		{
			if (args[0] == "/create")
				cmd = new Create(args);
			else if (args[0] == "/delete")
				cmd = new Delete(args);
			else if (args[0] == "/rename")
				cmd = new RenameServer(args);
			else if (args[0] == "/list")
				cmd = new ListServer(args);
			else if (args[0] == "/quit")
				cmd = new QuitServer(args);
			else if (args[0] == "/help")
				cmd = new HelpServer(args);
			else
				throw InvalidCommand();

			(*cmd)();
			delete cmd;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what();
		}
	}
	return 0;
}

```

### command.hpp command.cpp

프로그램의 동작 - 관리자 명령어, 사용자 명령어의 구현 - 을을 담당하는 모든 클래스는 Command 클래스를 상속한다. 예외 처리는 생성자에서 담당하고, 동작의 구현은 operator()에서 담당한다.

```cpp

class Command
{
	public :
	int             args_num;   // 명령어의 인자 수
	vector<string>  args;       // 명령어 인자

	Command(int args_num, const vector<string> &args);
	virtual ~Command() = default;
	virtual void operator()() const = 0;
};

```

```cpp

Command::Command(int args_num, const vector<string> &args) : args_num(args_num), args(args)
{
	if ((size_t)args_num != args.size() - 1)    // 커맨드 자체(args[0])은 제외해야 하므로 1을 뺀다
		throw WrongArgsNum();
	if (isArgsBlank(args))
		throw BlankArgs();
	if (isCharInvalid(args))
		throw InvalidChar();
}

```

WrongArgsNum, isArgsBlank, isCharInvalid는 모든 명령어에서 발생할 수 있는 예외이기 때문에 부모 클래스에서 공통적으로 처리한다.

NotConnected 예외를 발생시킬 수 있는 명령어들은 Command를 상속한 ClientCommandNeedName을 상속한다.

```cpp

// connect 명령이 선행되어야 하는 명령어들(connect, help, info를 제외한 모든 사용자 명령어들)은 이 클래스를 상속한다.
class ClientCommandNeedName : public Command
{
	public :
	int clnt_fd;
	ClientCommandNeedName(int clnt_fd, int args_num, const vector<string> &args);
};

```

```cpp

ClientCommandNeedName::ClientCommandNeedName(int clnt_fd, int args_num, const vector<string> &args) : Command(args_num, args), clnt_fd(clnt_fd)
{
	vector<int> &nc = Create::not_connected_ones;
	vector<int>::iterator it = find(nc.begin(), nc.end(), clnt_fd);
	if (it != nc.end())
		throw NotConnected();
}

```

Create는 프로그램의 핵심이 되는 관리자 명령어이다. 다음의 기능을 실행한다.

1. 만들 서버의 정보를 공유 메모리에 저장한다.
2. 서버(자식 프로세스)를 fork한다.
3. 서버 프로세스는 epoll을 이용한 멀티플렉싱 방식으로 동작한다.
4. 서버는 연결된 클라이언트를 connect를 실행한 그룹과 그렇지 못한 그룹으로 나누어 Create 클래스의 정적 멤버 변수의 형태로 관리한다. msg, broadcast 등의 사용자 명령어들은 이 클라이언트 정보들을 사용하여 기능을 구현한다.
5. 서버는 클라이언트가 송신한 내용을 파싱하여 적절한 사용자 명령어 객체를 할당하여 처리한다.
6. 관리자 프로세스는 delete 명령어를 통해 실행 중인 서버를 종료할 수 있다. 서버는 부모 프로세스로부터 SIGINT를 받으면 연결되어 있는 모든 클라이언트에게 종료 메세지를 보내고 exit()한다.

```cpp

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

```

### exception.hpp

프로그램의 모든 예외는 `std::exception`을 상속하여 만들어진다. 에러 로그는 예외 객체의 what()을 호출함으로서 얻을 수 있다.

관리자 명령어가 throw하는 예외의 로그는 표준 오류로 출력하고, 사용자 명령어가 throw한 예외의 로그는 해당 명령어를 사용한 사용자에게 송신한다.

```cpp

#include <exception>
#include <string>
#include <vector>

using namespace std;

/* 예외 조건 확인에 사용되는 함수들 */

bool isArgsBlank(const vector<string> &args) noexcept;
bool isCharInvalid(const vector<string> &args) noexcept;

/* 예외 객체들 */

class InvalidCommand : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class WrongArgsNum : public exception
{
	public :
	virtual const char *what() const noexcept;
};

// 후략

```

### channel.hpp

Channel 클래스는 채널의 관리자, 채널의 멤버 정보를 관리하는 객체이다. 그리고 Channel 클래스에는 Channel 객체가 관리하는 자원을 조작하기 위한 멤버함수들이 구현되어 있다.
channelMsg, refer, join, leave, kick 등 채널에 연관된 명령어들은 Channel 클래스에서 제공하는 자원과 멤버 함수를 통해 그 기능을 구현한다.

```cpp

/* channel.hpp */

#include <string>
#include <cstring>
#include <utility>
#include <unordered_map>
#include <unistd.h>
#include <exception>

using namespace std;

class Channel
{
	private :
	string                      name;
	pair<string, int>           oper;
	unordered_map<string, int>  member;
	public :
	Channel(const string &name, const pair<string, int> &oper);

	const string            &getName() const;
	void                    setName(const string &name);
	const pair<string, int> &getOperator() const;
	void					setOperator(const pair<string, int> &newOperator);
	const unordered_map<string, int>    &getMember() const;
	unordered_map<string, int>    &getMember();
	unordered_map<string, int>::iterator	findMemberWithName(const string &name);
	void                    joinOne(const pair<string, int> &client);
	void                    kickOne(const pair<string, int> &client);
	void                    broadcast(const string &sender, const string &msg) const;

	class ChannelEmpty : public exception
	{
		virtual const char	*what() const noexcept;
	};
};

```

## 클라이언트

### client.cpp



```cpp

/**    chat_client **/
 
/**    chat_client **/
 
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

using namespace std;

#define BUF_SIZE 4096
#define NORMAL_SIZE 20
 
void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(const char* msg);
 
int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void* thread_return;
 
    if (argc!=3)
    {
        printf(" Usage : %s <ip> <port>\n", argv[0]);
        exit(1);
    }
 
	char	*clnt_ip = argv[1];
	int serv_port = atoi(argv[2]);
    sock=socket(PF_INET, SOCK_STREAM, 0);
 
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(clnt_ip);
    serv_addr.sin_port = htons(serv_port);
 
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling(" connect() error");
 
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}
 
void* send_msg(void* arg)
{
    int sock=*((int*)arg);
 
    /** request command guide **/
    const char	*help = "/help\n";
    write(sock, help, strlen(help));
 
    while(1)
    {
        string input;
		if (!getline(cin, input, '\n')) {
			// Ctrl+D 또는 입력 스트림 오류 발생
			cout << "입력 스트림에 오류가 발생했습니다. 서버에 종료 요청을 보냅니다." << endl;
			string quit_cmd = "/quit\n";
			write(sock, quit_cmd.c_str(), quit_cmd.length());
			close(sock);
			return (void *)0;
		}
		input += "\n";	
		
        write(sock, input.c_str(), input.length());
		if (input == "/quit\n")
		{
			close(sock);
			return (void *)0;
		}
    }
	close(sock);
    return NULL;
}
 
void* recv_msg(void* arg)
{
    int sock=*((int*)arg);
    char msg[BUF_SIZE];
    int str_len;
 
    while(1)
    {
		bzero(msg, sizeof(char) * BUF_SIZE);
        str_len = read(sock, msg, BUF_SIZE);
        if (str_len <= 0)
		{
			close(sock);
			return (void *)0;
		}
        fputs(msg, stdout);
    }
	close(sock);
    return NULL;
}
 
void error_handling(const char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

```
