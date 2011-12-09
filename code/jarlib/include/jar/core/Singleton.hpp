// #############################################################
// Runs on: PLATFORM_WIN32, PLATFORM_LINUX, PLATFORM_APPLE
// A simple and generic way to implement a Singleton class.
// #############################################################

#ifndef JAR_CORE_SINGLETON_HPP
#define JAR_CORE_SINGLETON_HPP

// Includes and forwarddeclarations
// #############################################################
// -- C Standard Library
#include <assert.h>

namespace jar
{
    /** \brief A singleton that creates an instance of the class when first called.
        \headerfile <jar/core/Singleton.hpp>
    **/
    template <typename T>
	class CreatingSingleton
    {
    protected:
        CreatingSingleton(void)
        {
        }
    public:
        ~CreatingSingleton(void)
        {
			// assert (mSingleton);
			mSingleton = 0;
		}

		/**
            \brief Returns a reference of the one object of this class. If there is no object, one is created.
            \note Not Thread-Safe!
		 **/
        static T& GetSingleton(void)
		{
			if(!mSingleton) mSingleton = new T();
			return ( *mSingleton );
		}

		/**
            \brief Returns a pointer to the one object of this class. If there is no object, one is created.
            \note Not Thread-Safe!
		 **/
        static T* GetSingletonPtr(void)
		{
		    if(!mSingleton) mSingleton = new T();
			return mSingleton;
		}
    private:
        static T* mSingleton;
    };


    /** \brief A simple Singleton
        \headerfile <jar/core/Singleton.hpp>
    **/
    template <typename T>
	class Singleton
    {
    protected:
        Singleton(void)
        {
            assert(!mSingleton);
#if defined( _MSC_VER ) && _MSC_VER < 1200
            int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
            mSingleton = (T*)((int)this + offset);
#else
            mSingleton = static_cast< T* >( this );
#endif
        }
    public:
        ~Singleton(void)
        {
			assert (mSingleton);
			mSingleton = 0;
		}

		/**
            \brief Returns a reference of the one object of this class. If there is no object, an assert will fail.
            \note Not Thread-Safe!
		 **/
        static T& GetSingleton(void)
		{
		    assert(mSingleton);
			return ( *mSingleton );
		}

		/**
            \brief Returns a pointer to the one object of this class. If there is no object, an assert will fail.
            \note Not Thread-Safe!
		 **/
        static T* GetSingletonPtr(void)
		{
		    assert(mSingleton);
			return mSingleton;
		}

		/** \brief Returns whether an instance of this class exists. **/
		static const bool HasSingleton(void)
		{
		    return mSingleton;
		}
    private:
        static T* mSingleton;
    };
}
#endif
