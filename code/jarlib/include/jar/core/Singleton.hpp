/*
===========================================================================
Copyright (C) 2010 Willi Schinmeyer and Marcus Riemer

This file is part of the Jedi Academy: Renaissance source code.

Jedi Academy: Renaissance source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Jedi Academy: Renaissance source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Jedi Academy: Renaissance source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

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
