#ifndef QDATAMUTEX_H
#define QDATAMUTEX_H

#include <mutex>
#include <QOption>
///@brief Class who providing storage for multi-thread safe blocking interaction with stored object of @e T type
template< class T>
class QDataMutex
{
public:
    class locker
    {
    public:
        /* deleted */
        locker(const locker & o) = delete;
        locker&operator = (const locker & o) = delete;

        // empty locker
        locker(std::mutex * m)
            : pmtx(m), pvalue(nullptr) { }

        ///@brief create new locker
        ///@arg pv - data raw pointer
        ///@arg m - already locked mutex!
        locker(T * value, std::mutex * mtx)
            : pmtx(mtx), pvalue(value) { }

        ///@brief Move constructor
        locker(locker && b) noexcept {
            pmtx = b.pmtx;
            b.pmtx = nullptr;
            pvalue = b.pvalue;
        }

        ~locker() {
            if(pmtx)
                pmtx->unlock();
        }

        ///@brief Returns data raw pointer
        T * data() const { return pvalue; }

        ///@brief Returns const data raw pointer
        const T * constData() const { return pvalue; }

        ///@brief Returns QOptional container with data raw pointer
        QOption<T*> optional() const {
            if(pvalue)
                return pvalue;
            return None();
        }

    private:
        std::mutex * pmtx {nullptr};
        T * pvalue {nullptr};
    };

    /* deleted */
    QDataMutex(const QDataMutex & o) = delete;
    QDataMutex(QDataMutex && o) = delete;
    QDataMutex & operator=(QDataMutex && o) = delete;

    ///@brief Copy constructor
    QDataMutex(const T & v)
        : value(v) { }

    ///@brief Move constructor
    QDataMutex(T && v) noexcept
        : value(std::forward<T>(v)) { }

    ///@brief Returns @e locker object. This call will blocked if some locker already exists
    locker lock() {
        mtx.lock();
        return locker(&value, &mtx);
    }

    ///@brief Execute lambda expression with T* argument. This call will blocked if some locker already exists
    template< typename _Callable>
    void locked(_Callable && fn) {
        auto b = lock();
        fn(b.data());
    }

    QDataMutex & operator=(const T& o) {
        auto l = lock();
        *l.data() = o;
        return *this;
    }

    QDataMutex & operator=(T && o) {
        auto l = lock();
        *l.data() = std::forward<T>(o);
        return *this;
    }

    QDataMutex & operator=(const QDataMutex & o) {
        auto l = lock();
        auto ol = o.lock();
        *l.data() = *ol.data();
        return *this;
    }

private:
    std::mutex mtx;
    T value;
};
#endif // DATAMUTEX_H
