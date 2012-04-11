#ifndef JAR_CORE_HPP
#define JAR_CORE_HPP

#include <string>
#include <cassert>
#include <jar/core/API.hpp>
#include <jar/core/Time.hpp>

namespace jar {
//forward declarations
class CoreImpl;
class Lua;

/** \brief Main Framework class for core systems. You, the user, just need to create & initialize this (only once!), plus any other systems you need. **/
class JARCOREAPI Core
{
    public:
        Core();
        ~Core();

        /** \brief Initializes the whole framework.
         * \param argc first parameter of main()
         * \param argv second parameter of main()
         * \param rootPath path to application's root folder, relative to binary
         * \return Success */
        const bool Init(int argc, char** argv, const std::string& rootPath);
        const bool Deinit();

        void Update(const TimeType deltaT);

        const std::string& GetRootPath();

        Lua& GetLua();

        CoreImpl& GetImpl();

        static Core& GetSingleton() { assert(mSingleton); return *mSingleton; }
        static const bool HasSingleton() { return mSingleton != NULL; }

    private:
        /** \brief Pointer to implementation of this class, so user doesn't need to include all the files. **/
        CoreImpl* mImpl;
        static Core* mSingleton;
};

} // namespace jar

#endif // JAR_CORE_HPP
