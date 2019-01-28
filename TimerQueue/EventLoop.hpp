#ifndef _NET_EVENTLOOP_H
#define _NET_EVENTLOOP_H

#include <memory>
#include <vector>
#include <functional>
#include <mutex>
#include <thread>

#include "Channel.hpp"

class Poller;
//class Channel;

class EventLoop
{
public:
	typedef std::function<void()> Functor;

	EventLoop();
	~EventLoop();
	void loop();
	void quit();

	void assertInLoopThread()
	{
		if(!isInloopThread())
		{
			abortNotInLoopThread();
		}
	}

	bool isInloopThread() const {return m_threadId == std::this_thread::get_id(); }

	void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);

	/*TimerId runAt(const TimeStamp& time, const NetCallBacks::TimerCallBack& cb);
	TimerId runAfter(double delay, const NetCallBacks::TimerCallBack& cb);
	TimerId runEvery(double interval, const NetCallBacks::TimerCallBack& cb);*/

	void runInLoop(const Functor& cb);
	void wakeup();
  void queueInLoop(const Functor& cb);

	static EventLoop* getEventLoopOfCurrentThread();

private:
	EventLoop& operator=(const EventLoop&);
	EventLoop(const EventLoop&);

	void abortNotInLoopThread();

  //used to waked up
  void handleRead();
	void doPendingFunctors();

	//used for loop to debug.
	void printActiveChannels() const;

	typedef std::vector<Channel*> ChannelList;

	bool m_looping;
	bool m_quit;
	std::thread::id m_threadId;
	std::unique_ptr<Poller> m_poller;
	ChannelList m_activeChannels;

	int m_wakeupFd;//... 放p_wakeupChannel 后面会出错,一定要按顺序来.
	std::unique_ptr<Channel> p_wakeupChannel;
	//int m_timerfd;
	//std::unique_ptr<Channel> p_timerfdChannel;
	mutable std::mutex m_mutex;
  bool m_callingPendingFunctors; /* atomic */
  std::vector<Functor> m_pendingFunctors; // @GuardedBy mutex_

};

#endif