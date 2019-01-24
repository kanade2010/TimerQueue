#ifndef _NET_TIMERQUEUE_HH
#define _NET_TIMERQUEUE_HH
#include "TimeStamp.hpp"

#include <stdint.h>
#include <set>
#include <vector>
#include <condition_variable>
#include <functional>
#include <thread>

template <typename T>
class AtomicIntegerT
{
public:
  AtomicIntegerT()
  :m_value(0)
  {

  }

  T get()
  {
    return __sync_val_compare_and_swap(&m_value, 0, 0);
  }

  T incrementAndGet()
  {
    return addAndGet(1);
  }

  T decrementAndGet()
  {
    return addAndGet(-1);
  }

private:
  AtomicIntegerT& operator=(const AtomicIntegerT&);
  AtomicIntegerT(const AtomicIntegerT&);

  T getAndAdd(T x)
  {
    return __sync_fetch_and_add(&m_value, x);
  }

  T addAndGet(T x)
  {
    return getAndAdd(x) + x;
  }

  volatile T m_value; 

};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;

class Timer{
public:
  typedef std::function<void()> TimerCallBack_t;

  Timer(const TimerCallBack_t& cb, TimeStamp when, double interval)
  :m_callBack(cb),
  m_expiration(when),
  m_interval(interval),
  m_repeat(interval > 0.0),
  m_sequence(s_numCreated.incrementAndGet())
{

}

  void run() const
  {
    m_callBack();
  }

  TimeStamp expiration() const { return m_expiration; }
  bool repeat() const { return m_repeat; }
  int64_t sequence() const { return m_sequence; }
  void restart(TimeStamp now);

  static int64_t numCreated(){ return s_numCreated.get(); }

private:
  Timer& operator=(const Timer&);
  Timer(const Timer&);

  const TimerCallBack_t m_callBack;
  TimeStamp m_expiration;
  const double m_interval;
  const bool m_repeat;
  const int64_t m_sequence;

  static AtomicInt64 s_numCreated;

};

///
/// An opaque identifier, for canceling Timer.
///
class TimerId
{
 public:
  TimerId()
    : m_timer(NULL),
      m_sequence(0)
  {
  }

  TimerId(Timer* timer, int64_t seq)
    : m_timer(timer),
      m_sequence(seq)
  {
  }

  // default copy-ctor, dtor and assignment are okay

  friend class TimerQueue;

 private:
  //TimerId& operator=(const TimerId&);
  //TimerId(const TimerId&);

  Timer* m_timer;
  int64_t m_sequence;
};

class Channel;
class EventLoop;

class TimerQueue
{
private:
  TimerQueue();
public:
  ~TimerQueue();

  static TimerQueue* GetInstance()
  {
    static TimerQueue instance;
    return &instance;
  }

  typedef std::function<void()> TimerCallBack_t;

  // Schedules the callback to be run at given time,
  void Start();

  TimerId runAt(const TimeStamp& time, const TimerCallBack_t& cb);
  TimerId runAfter(double delay, const TimerCallBack_t& cb);
  TimerId runEvery(double interval, const TimerCallBack_t& cb);

  void cancel(TimerId timerId);

private:
  typedef std::pair<TimeStamp, Timer*> Entry;
  typedef std::set<Entry> TimerList;
  typedef std::pair<Timer*, int64_t> ActiveTimer;
  typedef std::set<ActiveTimer> ActiveTimerSet;

  TimerId addTimer(const TimerCallBack_t& cb, TimeStamp when, double interval = 0.0);
  void addTimerInLoop(Timer* timer);
  void cancelInLoop(TimerId timerId);
  //called when timerfd alarms
  void handleRead();
  //move out all expired timers and return they.
  std::vector<Entry> getExpired(TimeStamp now);
  bool insert(Timer* timer);
  void reset(const std::vector<Entry>& expired, TimeStamp now);

  std::thread m_thread;
  const int m_timerfd;
  EventLoop* p_loop;
  Channel* p_timerfdChannel;

  //Timer List sorted by expiration
  TimerList m_timers;
  ActiveTimerSet m_activeTimers;

  bool m_callingExpiredTimers; /*atomic*/
  ActiveTimerSet m_cancelingTimers;

  std::condition_variable m_wait_loop_init;
};

#endif
