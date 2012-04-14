#ifndef JAR_CORE_LUA_HPP
#define JAR_CORE_LUA_HPP

#include <string>
#include <jar/core/API.hpp>
#include <cassert>

//forward declarations
struct lua_State;

namespace jar
{
//error handling function that adds file and line number to errors
JARCOREAPI int add_file_and_line_to_error(lua_State* L);
//error handling function that adds a stack trace to errors
JARCOREAPI int add_stacktrace_to_error(lua_State* L);

/** \brief Wrapper class for commonly used Lua functions **/
class JARCOREAPI Lua
{
    public:
        Lua();
        /** \brief Destructor. Starts deinitialization, if necessary. **/
        virtual ~Lua();

        /** \brief returns the lua_State so you can do specific stuff.
            \note In Debug mode asserts that initialization has been done.
        **/
        lua_State* GetState() const
        {
#ifdef _DEBUG
            assert(mState);
#endif
            return mState;
        }

        /** \brief Returns the last error. **/
        const std::string& GetLastError() const {return mLastError;}

        /** \brief Initializes Lua and luabind
            \note Only initializes luabind if this is the first Lua thread. **/
        const bool Init();

        /** \brief Opens all the default Lua libraries. **/
        const bool OpenLibaries();

        /** \brief Opens the basic library. **/
        const bool OpenBaseLibrary();

        /** \brief Opens the package library. **/
        const bool OpenPackageLibrary();

        /** \brief Opens the string library. **/
        const bool OpenStringLibrary();

        /** \brief Opens the table library. **/
        const bool OpenTableLibrary();

        /** \brief Opens the math library. **/
        const bool OpenMathLibrary();

        /** \brief Opens the I/O library. **/
        const bool OpenIOLibrary();

        /** \brief Opens the Operating System library. **/
        const bool OpenOSLibrary();

        /** \brief Opens the debug library. **/
        const bool OpenDebugLibrary();

        /** \brief Deinitializes Lua.
            \note Automatically done in destructor, if necessary. **/
        const bool Deinit();

        /** \brief Executes a given string
            \param str the string
            \param chunkname what name to give this string (i.e. chunk), e.g. used in tracebacks. (note: @ at the beginning indicates a filename) **/
        const bool ExecuteString(const std::string& str, const std::string& chunkname);

        /** \brief Executes a given file
            \param filename name of the file to execute **/
        const bool ExecuteFile(const std::string& filename);

        /** \brief Whether this is the main Lua thread.

            Only the main Lua thread may use luabind. So you might want to check this before doing luabind stuff. **/
        const bool IsMainThread() {return mIsMainThread; }

    private:
        /** \brief The Lua State. Access via GetState(), if necessary. **/
        lua_State* mState;
        /** \brief The last error encountered. Acces via GetLastError().**/
        std::string mLastError;
        /** \brief Whether this is the main Lua thread. Only the main thread may use luabind. **/
        bool mIsMainThread;

        /// Don't want to include lua.hpp, thus I copied this here.
        typedef int (*lua_CFunction) (lua_State *L);
        /// luaopen_*** calls must be done through lua API. This function handles that.
        const bool ProtectedCall(lua_CFunction fn);
};

} // namespace jar

#endif // JAR_CORE_LUA_HPP
