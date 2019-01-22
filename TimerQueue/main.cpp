#include <errno.h>
#include <thread>
#include <strings.h>
#include <poll.h>
#include <chrono>
#include <functional>
#include <iostream>
#include "EventLoop.hh"
#include "Channel.hh"
#include "Poller.hh"
#include "Logger.hh"
#include "TimeStamp.hh"
#include "TimerQueue.hh"

void print() { LOG_DEBUG << "test print()";  }

void test()
{

  LOG_DEBUG << "[test] : test timerQue happended ";

  std::cout << "[test] : test timerQue happended at " << std::chrono::system_clock::now().time_since_epoch() / std::chrono::microseconds(1) << std::endl;

}


int main()
{

  //Logger::setLogLevel(Logger::TRACE);

  TimerQueue* timer_queue = TimerQueue::GetInstance();
  timer_queue->Start();
  timer_queue->runAfter(1.0, test);
  timer_queue->runAfter(1.0, test);
  timer_queue->runAfter(3.0, test);

  timer_queue->runEvery(5.0, test);
  getchar();
  return 0;
}

