#ifndef _NET_POLLER_HH
#define _NET_POLLER_HH

#include <vector>
#include <map>

#include "TimeStamp.hpp"
#include "EventLoop.hpp"
#include "Channel.hpp"

/*
 *#include <sys/poll.h>
 *struct pollfd
 *{
 *int fd; // 想查询的文件描述符.
 *short int events; // fd 上，我们感兴趣的事件
 *short int revents; /// 实际发生了的事件.
 *};
 */

struct pollfd;
//class Channel;

class Poller{
public:
  typedef std::vector<Channel*> ChannelList;

  Poller(EventLoop* loop);
  ~Poller();

  TimeStamp poll(int timeoutMs, ChannelList* activeChannels);

  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);

  void assertInLoopThread() { p_loop->assertInLoopThread(); }

private:
  Poller& operator=(const Poller&);
  Poller(const Poller&);

  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

  typedef std::vector<struct pollfd> PollFdList;
  typedef std::map<int, Channel*> ChannelMap;

  EventLoop* p_loop;
  PollFdList m_pollfds;
  ChannelMap m_channels;

};


#endif
