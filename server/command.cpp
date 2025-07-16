#include "command.hpp"

/* 명령어 인터페이스 */
Command::Command(int args_num, const vector<string> &args) : args_num(args_num), args(args)
{
	if ((size_t)args_num != args.size() - 1)    // 커맨드 자체(args[0])은 제외해야 하므로 1을 뺀다
		throw WrongArgsNum();
	if (isArgsBlank(args))
		throw BlankArgs();
	if (isCharInvalid(args))
		throw InvalidChar();
}

/* 관리자 명령어 */

Create::Create(const vector<string> &args) : Command(2, args)
{
	if (args[1].size() > 9)
		throw TooLongName();
	if (shm->serv_num >= MAX_SERVER_NUM)
		throw TooManyServer();
	if (shm->isServerNameValid(args[1].c_str()))
		throw DuplicateName();
}

void Create::operator()() const
{
	pid_t   pid = fork_naive();
	if (pid > 0)
	{
		int serv_num = shm->serv_num;

		pthread_rwlock_wrlock_naive(&rwlock);
		strcpy(shm->serv_names[serv_num], args[1].c_str());
		shm->serv_pids[serv_num] = pid;
		shm->serv_ports[serv_num] = atoi(args[2].c_str());
		shm->serv_num++;
		pthread_rwlock_unlock_naive(&rwlock);
	}
	else if (pid == 0)
	{
		signal(SIGINT, notifyServerClose);

		int serv_fd = socket_strict(PF_INET, SOCK_STREAM, 0);
		int flags = fcntl_strict(serv_fd, F_GETFL);
		flags |= O_NONBLOCK;
		fcntl_strict(serv_fd, F_SETFL, flags);
		
		struct sockaddr_in  serv_addr;
		bzero(&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family        = AF_INET;
		serv_addr.sin_port          = htons(atoi(args[2].c_str()));
		serv_addr.sin_addr.s_addr   = htonl(INADDR_ANY);

		bind_strict(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		listen_strict(serv_fd, LISTEN_QUEUE_SIZE);

		int epoll_fd = epoll_create_strict(1024);
		
		struct epoll_event  events = getEpollEvent(serv_fd, EPOLLIN | EPOLLET);

		epoll_ctl_strict(epoll_fd, EPOLL_CTL_ADD, serv_fd, &events);

		struct epoll_event  epoll_events[MAX_CLIENT_NUM];
		int                 event_count;

		while (true)
		{
			event_count = epoll_wait_naive(epoll_fd, epoll_events, MAX_CLIENT_NUM, -1);
			for (int i = 0; i < event_count; i++)
			{
				if (epoll_events[i].data.fd == serv_fd)
				{
					if (clients_map_name.size() + not_connected_ones.size() >= MAX_CLIENT_NUM)
						continue;

					socklen_t           clnt_len    = sizeof(struct sockaddr_in);
					struct sockaddr_in  clnt_addr;
					int                 clnt_fd     = accept_naive(serv_fd, (struct sockaddr *)&clnt_addr, &clnt_len);
					if (clnt_fd == -1)
						continue;

					int flags = fcntl_naive(clnt_fd, F_GETFL);
					if (flags == -1)
					{
						close(clnt_fd);
						continue;
					}
					flags |= O_NONBLOCK;
					if (fcntl_naive(clnt_fd, F_SETFL, flags) == -1)
					{
						close(clnt_fd);
						continue;
					}

					struct epoll_event  events = getEpollEvent(clnt_fd, EPOLLIN | EPOLLET);

					if (epoll_ctl_naive(epoll_fd, EPOLL_CTL_ADD, clnt_fd, &events) == -1)
					{
						close(clnt_fd);
						continue;
					}

					not_connected_ones.push_back(clnt_fd);
				}
				else
				{
					/* epoll에 등록된 클라이언트들의 send data 처리 */
					int     msg_len;
					int     clnt_fd = epoll_events[i].data.fd;
					char    msg[MSG_BUF_LEN];
					bzero(msg, sizeof(char) * MSG_BUF_LEN);
					msg_len = read(clnt_fd, msg, sizeof(char) * MSG_BUF_LEN - 1);
					
					if (msg_len == -1)
					{
						print_error("read");
					}						
					else if (msg_len == 0)   // 클라이언트 접속 종료 요청
					{
						epoll_ctl_naive(epoll_fd, EPOLL_CTL_DEL, clnt_fd, NULL);
						close(clnt_fd);

						// 클라이언트 관리 stl에서 종료된 클라이언트를 제거한다.
						// 사용자 등록이 안된 클라이언트들을 먼저 확인한다.
						vector<int>::iterator   it = find(not_connected_ones.begin(), not_connected_ones.end(), clnt_fd);
						if (it != not_connected_ones.end())
						{
							not_connected_ones.erase(it);
						}
						// 없다면 사용자 등록이 된 클라이언트들을 확인한다.
						else
						{
							for (auto it = clients_map_name.begin(); it != clients_map_name.end(); it++)
							{
								if (it->second == clnt_fd)
									clients_map_name.erase(clients_map_fd[i]);
							}
							clients_map_fd.erase(clnt_fd);
						}
					}
					else    // 사용자 명령어 처리
					{
						string  buf(msg);
						while (1)
						{
							while (1)
							{
								size_t nl_pos = buf.find('\n');
								if (nl_pos == string::npos)
									break;
								else
								{
									string parsed = buf.substr(0, nl_pos);
									string cmd = buf = buf.substr(nl_pos + 1);
									vector<string>  args = parse(parsed);
									if (args.size() == 0)
										break;
									Command *c;
									
									try
									{
										if (args[0] == "/connect")
											c = new Connect(clnt_fd, args);
										else if (args[0] == "/msg")
											c = new Msg(clnt_fd, args);
										else if (args[0] == "/broadcast")
											c = new Broadcast(clnt_fd, args);
										else if (args[0] == "/channelMsg")
											c = new ChannelMsg(clnt_fd, args);
										else if (args[0] == "/isc")
											c = new IscSend(clnt_fd, args);
										else if (args[0] == "/iscRecv")
											c = new IscRecieve(clnt_fd, args);
										else if (args[0] == "/createChannel")
											c = new CreateChannel(clnt_fd, args);
										else if (args[0] == "/removeChannel")
											c = new RemoveChannel(clnt_fd, args);
										else if (args[0] == "/join")
											c = new Join(clnt_fd, args);
										else if (args[0] == "/leave")
											c = new Leave(clnt_fd, args);
										else if (args[0] == "/kick")
											c = new Kick(clnt_fd, args);
										else if (args[0] == "/refer")
											c = new Refer(clnt_fd, args);
										else if (args[0] == "/rename")
											c = new RenameClient(clnt_fd, args);
										else if (args[0] == "/list")
											c = new ListClient(clnt_fd, args);
										else if (args[0] == "/quit")
											c = new QuitClient(clnt_fd, args);
										else if (args[0] == "/help")
											c = new HelpClient(clnt_fd, args);
										else
											throw InvalidCommand();
										
										(*c)();
										delete c;
									}
									catch(const std::exception& e)
									{
										write(clnt_fd, e.what(), strlen(e.what()));
									}
								}
							}

							bzero(msg, sizeof(char) * MSG_BUF_LEN);
							msg_len = read(clnt_fd, msg, sizeof(char) * MSG_BUF_LEN - 1);
							if (msg_len >= 0)
								buf += msg;
							else
								break;
						}
					}
				}
			}
		}
	}
}

void    Create::notifyServerClose(int sig) noexcept
{
	(void)sig;
	for (const pair<const string, int> &p : clients_map_name)
	{
		write(p.second, "Server terminated by manager.\n", strlen("Server terminated by manager.\n"));
		close(p.second);
	}
	exit(0);
}

vector<int>                      Create::not_connected_ones;
unordered_map<string, int>       Create::clients_map_name;
unordered_map<int, string>       Create::clients_map_fd;

struct epoll_event  Create::getEpollEvent(int fd, int event) const noexcept
{
	struct epoll_event  epe;
	epe.events = event;
	epe.data.fd = fd;

	return epe;
}

Delete::Delete(const vector<string> &args) : Command(1, args)
{
	if (shm->serv_num <= 0)
		throw NoServerToDelete();
	if (!shm->isServerNameValid(args[1].c_str()))
		throw InvalidName();
}

void    Delete::operator()() const
{
	pthread_rwlock_rdlock_naive(&rwlock);
	pid_t pid = shm->getPidWithName(args[1].c_str());
	pthread_rwlock_unlock_naive(&rwlock);
	kill_naive(pid, SIGINT);
}

RenameServer::RenameServer(const vector<string> &args) : Command(2, args)
{
	if (!shm->isServerNameValid(args[1].c_str()))
		throw InvalidName();
	if (shm->isServerNameValid(args[2].c_str()))
		throw DuplicateName();
}

void    RenameServer::operator()() const
{
	shm->rename(args[1].c_str(), args[2].c_str());
}

ListServer::ListServer(const vector<string> &args) : Command(0, args) {}

void    ListServer::operator()() const
{
	shm->printServerInfo();
}

QuitServer::QuitServer(const vector<string> &args) : Command(0, args) {}

void    QuitServer::operator()() const
{
	while (shm->serv_num > 0)
	{
		Delete({"Delete", shm->serv_names[0]})();
		sleep(1000);
	}
	shmdt(shm);
	pthread_rwlock_destroy(&rwlock);
	exit(0);
}

HelpServer::HelpServer(const vector<string> &args) : Command(0, args) {}

void    HelpServer::operator()() const
{
	printf("==============================\n");
	printf("Command Guide\n");
	printf("/create : create new server\n");
	printf("Usage : '/create <server name> <port>'\n\n");
	printf("/delete : delete server with name\n");
	printf("Usage : '/delete <server name>'\n\n");
	printf("/rename : rename server\n");
	printf("Usage : '/rename <old name> <new name>'\n\n");
	printf("/list : list server info\n");
	printf("Usage : '/list'\n\n");
	printf("/quit : delete all server and exit\n");
	printf("Usage : '/quit'\n");
	printf("==============================\n");
}

ClientCommandNeedName::ClientCommandNeedName(int clnt_fd, int args_num, const vector<string> &args) : Command(args_num, args), clnt_fd(clnt_fd)
{
	vector<int> &nc = Create::not_connected_ones;
	vector<int>::iterator it = find(nc.begin(), nc.end(), clnt_fd);
	if (it != nc.end())
		throw NotConnected();
}

Connect::Connect(int clnt_fd, const vector<string> &args) : Command(1, args), clnt_fd(clnt_fd)
{
	unordered_map<string, int>  &c1 = Create::clients_map_name;
	unordered_map<int, string>  &c2 = Create::clients_map_fd;

	// 두 번 connect 하는 것을 방지한다.
	if (c2.find(clnt_fd) != c2.end())
		throw AlreadyConnected();

	// 중복 이름 체크
	if (c1.find(args[1]) != c1.end())
		throw DuplicateName();
}

void    Connect::operator()() const
{
	vector<int> &nc = Create::not_connected_ones;
	unordered_map<string, int>  &c1 = Create::clients_map_name;
	unordered_map<int, string>  &c2 = Create::clients_map_fd;

	nc.erase(find(nc.begin(), nc.end(), clnt_fd));
	c1[args[1]] = clnt_fd;
	c2[clnt_fd] = args[1];
}

Msg::Msg(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 2, args)
{
	unordered_map<string, int>  &c = Create::clients_map_name;
	// 존재하지 않는 사용자일 경우
	if (c.find(args[1]) == c.end())
		throw InvalidName();
}

void    Msg::operator()() const
{
	string msg = string("msg from ") + Create::clients_map_fd[clnt_fd] + string(" : ") + args[2] + "\n";
	write(Create::clients_map_name[args[1]], msg.c_str(), strlen(msg.c_str()));
}

Broadcast::Broadcast(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 1, args) {}

void    Broadcast::operator()() const
{
	unordered_map<string, int>  &c = Create::clients_map_name;
	string msg = string("broadcast from ") + Create::clients_map_fd[clnt_fd] + string(" : ") + args[1] + "\n";

	for (auto it = c.begin(); it != c.end(); it++)
	{
		if (it->second == clnt_fd)
			continue;
		write(it->second, msg.c_str(), strlen(msg.c_str()));
	}
}

IscSend::IscSend(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 3, args)
{
	pthread_rwlock_rdlock_naive(&rwlock);
	for (int i = 0; i < MAX_SERVER_NUM; i++)
	{
		if (strcmp(shm->serv_names[i], args[1].c_str()) == 0)
		{
			port = shm->serv_ports[i];
			return;
		}
	}
	pthread_rwlock_unlock_naive(&rwlock);
	throw InvalidName();
}

void	IscSend::operator()() const
{
	int sock = socket_naive(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		return;

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

	if (connect_naive(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		close(sock);
		return;
	}

	for (int i = 0; i < MAX_SERVER_NUM; i++)
	{
		if (shm->serv_pids[i] == (int)getpid())
		{
			string msg = string("/iscRecv ") + Create::clients_map_fd[clnt_fd] + string(" ") + args[2] + string(" ;") + args[3] + "\n";
			
			write(sock, msg.c_str(), strlen(msg.c_str()));
			close(sock);
			return ;
		}
	}
}

IscRecieve::IscRecieve(int clnt_fd, const vector<string> &args) : Command(3, args), clnt_fd(clnt_fd) {}

void	IscRecieve::operator()() const
{
	unordered_map<string, int>	&cm = Create::clients_map_name;
	if (cm.find(args[2]) != cm.end())
	{
		string msg = string("inter server msg from ") + args[1] + string(" : ") + args[3] + "\n";
		write(cm[args[2]], msg.c_str(), strlen(msg.c_str()));
	}
	vector<int>	&nc = Create::not_connected_ones;
	nc.erase(find(nc.begin(), nc.end(), clnt_fd));
	close(clnt_fd);
}

CreateChannel::CreateChannel(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 1, args)
{
	for (const Channel &c : channel_list)
	{
		if (c.getName() == args[1])
			throw DuplicateName();
	}
}

void    CreateChannel::operator()() const
{
	channel_list.push_back(Channel(args[1], {Create::clients_map_fd[clnt_fd], clnt_fd}));
}

vector<Channel> CreateChannel::channel_list;

vector<Channel>::iterator    CreateChannel::findChannelWithName(const string &name)
{
	for (auto iter = channel_list.begin(); iter != channel_list.end(); iter++)
	{
		if (iter->getName() == name)
			return iter;
	}
	return channel_list.end();
}

RemoveChannel::RemoveChannel(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 1, args)
{
	vector<Channel>::iterator	chan = CreateChannel::findChannelWithName(args[1]);
	if (chan == CreateChannel::channel_list.end())
		throw InvalidName();
	if (chan->getOperator().second != clnt_fd)
		throw NotChannelOperator();
}

void	RemoveChannel::operator()() const
{
	vector<Channel>::iterator	chan = CreateChannel::findChannelWithName(args[1]);
	string operatorName = Create::clients_map_fd[clnt_fd];
	string msg = "Operator " + operatorName + string(" removed ") + chan->getName() + string(" channel!\n");
	chan->broadcast(operatorName, msg);
	CreateChannel::channel_list.erase(chan);
}

ChannelMsg::ChannelMsg(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 2, args)
{
	if (CreateChannel::findChannelWithName(args[1]) == CreateChannel::channel_list.end())
		throw InvalidName();
}

void    ChannelMsg::operator()() const
{
	const Channel &ch = *CreateChannel::findChannelWithName(args[1]);

	string msg = ch.getName() + string(" channel msg from ") + Create::clients_map_fd[clnt_fd] + " : " + args[2] + "\n";
	ch.broadcast(Create::clients_map_fd[clnt_fd], msg);
}

Join::Join(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 1, args)
{
	if (CreateChannel::findChannelWithName(args[1]) == CreateChannel::channel_list.end())
		throw InvalidName();
}

void    Join::operator()() const
{
	Channel &ch = *CreateChannel::findChannelWithName(args[1]);
	ch.joinOne({Create::clients_map_fd[clnt_fd], clnt_fd});
}

Leave::Leave(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 1, args)
{
	vector<Channel>::iterator iter = CreateChannel::findChannelWithName(args[1]);
	if (iter == CreateChannel::channel_list.end())
		throw InvalidName();
	Channel &ch = *iter;
	if (ch.findMemberWithName(Create::clients_map_fd[clnt_fd]) == ch.getMember().end())
		throw NotInChannel();
	
}

void    Leave::operator()() const
{
	vector<Channel>::iterator chan = CreateChannel::findChannelWithName(args[1]);
	try
	{
		chan->kickOne({Create::clients_map_fd[clnt_fd], clnt_fd});
	}
	catch(const std::exception& e)
	{
		CreateChannel::channel_list.erase(chan);
	}
}

Kick::Kick(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 2, args)
{
	vector<Channel>::iterator iter = CreateChannel::findChannelWithName(args[1]);
	if (iter == CreateChannel::channel_list.end())
		throw InvalidName();
	if (iter->getOperator().second != clnt_fd)
		throw NotChannelOperator();
	Channel &ch = *iter;
	if (ch.findMemberWithName(args[2]) == ch.getMember().end())
		throw NotInChannel();
}

void    Kick::operator()() const
{
	vector<Channel>::iterator chan = CreateChannel::findChannelWithName(args[1]);
	try
	{
		string msg = string("You are kicked off from ") + args[1] + string(" channel!\n");
		write(Create::clients_map_name[args[2]], msg.c_str(), strlen(msg.c_str()));
		chan->kickOne({args[2], Create::clients_map_name[args[2]]});
	}
	catch(const std::exception& e)
	{
		CreateChannel::channel_list.erase(chan);
	}
}

Refer::Refer(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 1, args)
{
	if (CreateChannel::findChannelWithName(args[1]) == CreateChannel::channel_list.end())
		throw InvalidName();
}

void	Refer::operator()() const
{
	string msg = string("==============================\n");
	msg += args[1];
	msg += string(" channel info\n"); 

	Channel	&ch = *CreateChannel::findChannelWithName(args[1]);
	msg += string("operator : ");
	msg += ch.getOperator().first;

	msg += string("\n\nmember list\n");
	const unordered_map<string, int>	&member = ch.getMember();
	for (auto m : member)
	{
		msg += m.first;
		msg += "\n";
	}
	msg += string("==============================\n");

	write(clnt_fd, msg.c_str(), strlen(msg.c_str()));
}

RenameClient::RenameClient(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 3, args)
{
	if (args[1] != "user" && args[1] != "channel")
		throw InvalidOption();
	unordered_map<string, int>  &ch = Create::clients_map_name;
	if (args[1] == "user" && ch.find(args[2]) != ch.end())
		throw DuplicateName();
		
	if (args[1] == "channel")
	{
		vector<Channel>::iterator chan = CreateChannel::findChannelWithName(args[2]);
		if (chan->getOperator().second != clnt_fd)
			throw NotChannelOperator();
		if (chan == CreateChannel::channel_list.end())
			throw InvalidName();
		if (chan->findMemberWithName(args[3]) != chan->getMember().end())
			throw DuplicateName();
	}
}

void    RenameClient::operator()() const
{
	if (args[1] == "user")
	{
		string old_name = Create::clients_map_fd[clnt_fd];
		Create::clients_map_fd[clnt_fd] = args[2];
		Create::clients_map_name.erase(old_name);
		Create::clients_map_name[args[2]] = clnt_fd;

		vector<Channel>	&cl = CreateChannel::channel_list;
		for (auto ch = cl.begin(); ch != cl.end(); ch++)
		{
			if (ch->getOperator().first == old_name)
				ch->setOperator({args[2], clnt_fd});

			unordered_map<string, int> &channel_member = ch->getMember();
			auto it = channel_member.find(old_name);
			if (it != channel_member.end()) {
				channel_member.erase(it);
				channel_member[args[2]] = clnt_fd;
			}
		}
	}
	else if (args[1] == "channel")
	{
		vector<Channel>::iterator chan = CreateChannel::findChannelWithName(args[2]);
		
		string msg = string("Channel operator of ") + chan->getName() + string(" renamed channel! Channel name is now ") + args[3] + string(".\n");
		chan->broadcast(Create::clients_map_fd[clnt_fd], msg);
		
		chan->setName(args[3]);        
	}
}

ListClient::ListClient(int clnt_fd, const vector<string> &args) : ClientCommandNeedName(clnt_fd, 1, args)
{
	if (args[1] != "user" && args[1] != "channel" && args[1] != "server")
		throw InvalidOption();
}

void    ListClient::operator()() const
{
	string  msg;

	msg += "==============================\n";
	if (args[1] == "user")
	{
		msg += "Client List\n";
		msg += "------------------------------\n";
		for (const pair<const string, int> &client : Create::clients_map_name)
		{
			msg += client.first + "\n";
		}
	}
	else if (args[1] == "channel")
	{
		msg += "Channel List\n";
		msg += "------------------------------\n";
		for (const Channel &chan : CreateChannel::channel_list)
		{
			msg += chan.getName() + "\n";
		}
	}
	else if (args[1] == "server")
	{
		msg += "Server List\n";
		msg += "------------------------------\n";
		for (int i = 0; i < shm->serv_num; i++)
		{
			msg += string(shm->serv_names[i]) + "\n";
		}
	}
	msg += "==============================\n";

	write(clnt_fd, msg.c_str(), strlen(msg.c_str()));
}

QuitClient::QuitClient(int clnt_fd, const vector<string> &args) : Command(0, args), clnt_fd(clnt_fd) {}

void    QuitClient::operator()() const
{
	vector<Channel>	&cl = CreateChannel::channel_list;
	for (auto ch = cl.begin(); ch != cl.end();)
	{
		try
		{
			ch->kickOne({Create::clients_map_fd[clnt_fd], clnt_fd});
			ch++;
		}
		catch(const std::exception &e)
		{
			ch = cl.erase(ch);
		}
	}

	vector<int> &nc = Create::not_connected_ones;
	vector<int>::iterator   client = find(nc.begin(), nc.end(), clnt_fd);
	if (client != nc.end())
	{
		nc.erase(client);
	}
	else
	{
		string client_name = Create::clients_map_fd[clnt_fd];
		Create::clients_map_name.erase(client_name);
		Create::clients_map_fd.erase(clnt_fd);
	}
	close(clnt_fd);
}

HelpClient::HelpClient(int clnt_fd, const vector<string> &args) : Command(0, args), clnt_fd(clnt_fd) {}

void    HelpClient::operator()() const
{
	string msg = \
		string("==============================\n") \
		+ "Command Guide\n" \
		+ "/connect : connect to server with unique user name.\n"
		+ "Usage : /connect <user name>\n\n" \
		+ "/msg : send msg to specific user.\n"
		+ "Usage :/msg <reciever name> ;<msg>\n\n" \
		+ "/broadcast : send msg to all user : .\n"
		+ "Usage : /broadcast ;<msg>\n\n" \
		+ "/channelMsg : send msg to channel member.\n"
		+ "Usage : /channelMsg <channel name> ;<msg>\n\n" \
		+ "/isc : send msg to user of other server.\n"
		+ "Usage : /isc <server name> <reciever name> ;<msg>\n\n" \
		+ "/createChannel : create channel and be an operator of the channel.\n"
		+ "Usage : /createChannel <channel name>\n\n" \
		+ "/join : join to channel.\n"
		+ "Usage : /join <channel name>\n\n" \
		+ "/leave : leave channel.\n"
		+ "Usage : /leave <channel name>\n\n" \
		+ "/kick : channel operator command. kick off one from the channel.\n"
		+ "Usage : /kick <channel name> <name of the user to kick off>\n\n" \
		+ "/rename user : rename your user name.\n"
		+ "Usage : /rename user <new user name>\n\n" \
		+ "/rename channel : channel operator command. rename channel name.\n"
		+ "Usage : /rename channel <old channel name> <new channel name>\n\n" \
		+ "/list user : list all clients of the server.\n"
		+ "Usage : /list user\n\n" \
		+ "/list channel : list all channels of the server.\n"
		+ "Usage : /list channel\n\n" \
		+ "/list server : list all servers.\n"
		+ "Usage : /list server\n\n" \
		+ "/quit : disconnect from the server.\n"
		+ "Usage : /quit\n" \
		+ "==============================\n";
	write(clnt_fd, msg.c_str(), strlen(msg.c_str()));
}
