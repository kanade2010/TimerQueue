# TimerQueue
A TimerQueue Modified Based On  Muduo Timerqueue.

*Async
*thread-safe
*based on poll


simple use

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
