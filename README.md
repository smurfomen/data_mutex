## QDataMutex

Implementation of the solution to the problem of race for a resource in multithreaded control.
QDataMutex allows you to manage data in blocking mode with a mutex bound to that data.
While the QDataMutex<T>::locker object exists, access to the data will be blocked.

```C++
QDataMutex<int> storage(25);

// like this
{
  for(int s = 0; s < 100; ++s) {
      auto lck = storage.lock();
      qDebug() << lck.value()++;
  }
}

// or
for(int s = 0; s < 100; ++s) {
  storage.locked([](int & i) {
    qDebug() << i++;
  });
}
```
The same can be done in different threads, execution in this case will be sequential:
```C++
QDataMutex<int> storage(25);

// run thread
// thread t will block this thread while lck object is exists
std::thread * t = new std::thread([&](){
  auto lck = storage.lock();
  for(int i = 0; i < 100; i++) {
    lck.value() += 1;
    qDebug()<<lck.value();
  }
});

{
  auto lck = storage.lock();
  for(int i = 0; i < 100; i++) {
    lck.value() += 1;
    qDebug()<<lck.value();
  }
}

t->join();
```
