#ifndef QDATAMUTEX_H
#define QDATAMUTEX_H

#include <mutex>
#include <functional>
#ifdef QOPTION_INCLUDED
#include <QOption>
#endif

/*! \brief  Template class who providing storage for multi-thread safe blocking interaction with stored object. */
template <class T>
class QDataMutex
{
    class locker
    {
    public:
        /*!
         * \brief   Constructor.
         * \arg     value - raw pointer to stored value
         *          mtx - already locked mutex!
         */
        locker(T * value, std::mutex * mtx)
            : pmtx(mtx), pvalue(value) { }

        locker(locker && b) noexcept {
            pmtx = b.pmtx;
            b.pmtx = nullptr;
            pvalue = b.pvalue;
        }

        /* deleted */
        locker(const locker & o) = delete;
        locker& operator=(const locker & o) = delete;

        ~locker() {
            if(pmtx)
                pmtx->unlock();
        }

        /*!
         * \brief   Returns value ref.
         * \note    WARNING: reference lifetime must be less than lifetime of the locker object.
         */
        T& value() const
        { return *pvalue; }

        /*!
         * \brief   Provides access to value pointer.
         * \note    WARNING: pointer lifetime must be less than lifetime of the locker object.
         */
        T* operator->() const noexcept
        { return pvalue; }

        /*! \brief  Returns const data raw pointer. */
        const T * constData() const noexcept
        { return pvalue; }

        /*!
         * \brief   Returns value pointer.
         * \note    WARNING: pointer lifetime must be less than lifetime of the locker object.
         */
        T * data() const noexcept
        { return pvalue; }

#ifdef QOPTION_INCLUDED
        /*! \brief  Returns QOptional container with data raw pointer. */
        QOption<T*> optional() const noexcept {
            if(pvalue)
                return pvalue;

            return None();
        }
#endif

    private:
        /*! \brief  Pointer to locked mutex. */
        std::mutex * pmtx {nullptr};

        /*! \brief  Pointer to stored value. */
        T * pvalue {nullptr};
    };

public:
    /* deleted */
    QDataMutex(QDataMutex && o) = delete;
    QDataMutex & operator=(QDataMutex && o) = delete;
    QDataMutex & operator=(const QDataMutex & o) = delete;

    QDataMutex(const T & v)
        : value(v) { }

    QDataMutex()
        : value(T()) { }

    QDataMutex(T && v) noexcept
        : value(std::forward<T>(v)) { }

    /*!
     * \brief   Returns locker object.
     * \note    This call will blocked if some locker already exists.
     */
    locker lock() {
        mtx.lock();
        return locker(&value, &mtx);
    }

    /*!
     * \brief   Execute lambda expression in lock mode and returns _Res type value.
     * \note    Lambda must take as an argument a reference to an object of type
     *          This call will blocked if some locker already exists.
     */
    template<typename _Res = void>
    _Res locked(std::function<_Res(T&)> && fn) {
        auto b = lock();
        return fn(b.value());
    }

    /*!
     * \brief   Execute lambda expression in lock mode.
     * \note    Lambda must take as an argument a reference to an object of type
     *          This call will blocked if some locker already exists.
     */
    template<typename _Callable>
    void locked(_Callable && fn) {
        locked<void>(fn);
    }

    QDataMutex & operator=(const T& o) {
        auto l = lock();
        l.value() = o;
        return *this;
    }

    QDataMutex & operator=(T && o) {
        auto l = lock();
        l.value() = std::forward<T>(o);
        return *this;
    }

private:    
    std::mutex mtx;

    /*! \brief  Stored value. */
    T value;
};
#endif // DATAMUTEX_H

