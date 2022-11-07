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
#ifndef DATAMUTEX_H
#define DATAMUTEX_H

#include <mutex>
#include <functional>


#ifdef OPTIONAL_INCLUDED
#include <optional>
#endif

/*! \brief  Template class who providing storage for multi-thread safe blocking interaction with stored object. */
template <class T>
class data_mutex
{
    // For generic types that are functors, delegate to its 'operator()'
    template <typename _FnT>
    struct __data_mutex_function_traits
        : public __data_mutex_function_traits<decltype(&_FnT::operator())>
    {};

    // for pointers to member function
    template <typename ClassType, typename ReturnType, typename... Args>
    struct __data_mutex_function_traits<ReturnType(ClassType::*)(Args...) const> {
        typedef std::function<ReturnType (Args...)> f_type;
    };

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

#ifdef OPTIONAL_INCLUDED
        /*! \brief  Returns QOptional container with data raw pointer. */
		tl::optional<T*> optional() const noexcept {
			if(pvalue) {
                return pvalue;
			}
			return nullopt;
        }
#endif

    private:
        /*! \brief  Pointer to locked mutex. */
        std::mutex * pmtx {nullptr};

        /*! \brief  Pointer to stored value. */
        T * pvalue {nullptr};
    };

public:

	data_mutex(const data_mutex & o)
		: value(o.value)
	{ }

	data_mutex(data_mutex && o)
		: value(std::move(o.value))
	{ }

	data_mutex(const T & v)
		: value(v)
	{ }

	data_mutex()
		: value(T())
	{ }

	data_mutex(T && v) noexcept
		: value(std::forward<T>(v))
	{ }

    /*!
     * \brief   Returns locker object.
     * \note    This call will blocked if some locker already exists.
     */
    locker lock() {
        mtx.lock();
        return locker(&value, &mtx);
    }

	data_mutex & operator=(data_mutex && o) {
		if(this != &o)
		{
			auto l = lock();
			value = o.value;
		}
		return *this;
	}

	data_mutex & operator=(const data_mutex & o) {
		if(this != &o)
		{
			auto l = lock();
			value = o.value;
		}
		return *this;
	};


    template<typename _Fn, class _Res = typename __data_mutex_function_traits<_Fn>::f_type::result_type>
    _Res locked(_Fn fn) {
        auto b = lock();
        return fn(b.value());
    }

	data_mutex & operator=(const T & o) {
        {
            auto l = lock();
            value = o;
        }
        return *this;
    }

	data_mutex & operator=(T && o) {
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



