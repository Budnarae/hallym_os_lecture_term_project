#include "channel.hpp"

Channel::Channel(const string &name, const pair<string, int> &oper) : name(name), oper(oper)
{
	member.insert(oper);
}

const string            &Channel::getName() const
{
	return name;
}

void                    Channel::setName(const string &name)
{
	this->name = name;
}

const pair<string, int> &Channel::getOperator() const
{
	return oper;
}

void					Channel::setOperator(const pair<string, int> &newOperator)
{
	oper = newOperator;
}

const unordered_map<string, int>    &Channel::getMember() const
{
	return member;
}

unordered_map<string, int>    &Channel::getMember()
{
	return member;
}

unordered_map<string, int>::iterator	Channel::findMemberWithName(const string &name)
{
	return member.find(name);
}

void                    Channel::joinOne(const pair<string, int> &client)
{
	member.insert(client);
}

void                    Channel::kickOne(const pair<string, int> &client)
{
	member.erase(client.first);
	if (member.size() == 0)
		throw Channel::ChannelEmpty();
	if (client.first == oper.first)
		oper = *member.begin();
}

void                    Channel::broadcast(const string &sender, const string &msg) const
{
	for (const pair<const string, int> &m : member)
	{
		if (m.first == sender)
			continue;
		else
		{
			write(m.second, msg.c_str(), strlen(msg.c_str()));
		}
	}
}

const char	*Channel::ChannelEmpty::what() const noexcept
{
	return "Nobody in channel.\n";
}
