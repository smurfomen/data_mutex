/*
 MIT License

 Copyright (c) 2020 Agadzhanov Vladimir

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
                                                                                 */
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

    QDataMutex & operator=(const T & o) {
        {
            auto l = lock();
            value = o;
        }
        return *this;
    }

    QDataMutex & operator=(T && o) {
        {
            auto l = lock();
            value = std::forward<T>(o);
        }
        return *this;
    }

private:
    std::mutex mtx;

    /*! \brief  Stored value. */
    T value;
};
#endif // DATAMUTEX_H

