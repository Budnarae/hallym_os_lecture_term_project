#ifndef CHANNEL_HPP
#define CHANNEL_HPP

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

#endif