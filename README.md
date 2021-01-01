## AsyncStorage

Implementation of the solution to the problem of race for a resource in multithreaded control.
AsyncStorage allows you to manage data in blocking mode with a mutex bound to that data.
While the AsyncStorage <T>::locker object exists, access to the data will be blocked.

```C++
AsyncStorage<int> storage(25);

// like this
auto lck = storage.lock();
std::cout<< *lck.data()++;

// or
storage.locked([](int * i) {
  std::cout << *i++;
});
```
The same can be done in different threads, execution in this case will be sequential:
```C++
AsyncStorage<int> storage(25);

// run thread
// thread t will block this thread while lck object is exists
std::thread * t = new std::thread([&](){
    auto lck = storage.lock();
    for(int i = 0; i < 100; i++) {
        *lck.data()+=i;
        std::cout<<*lck.data();
    }
});

auto lck = storage.lock();
for(int i = 0; i < 100; i++) {
    *lck.data()+=i;
    std::cout<<*lck.data();
}

t->join();
```
