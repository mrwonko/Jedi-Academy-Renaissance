#ifndef JAR_JARIMPL_HPP
#define JAR_JARIMPL_HPP

#include "jar/core/Time.hpp"
#include <string>
#include <set>
#include <cassert>

namespace jar {

//forward declarations
class CLArguments;
class Lua;
class Component;
class Updatable;

/** \brief Implementation of the Jar class **/
class CoreImpl
{
    public:
        CoreImpl();
        ~CoreImpl();

        const bool Init(int argc, char** argv, const std::string& rootPath);
        const bool Deinit();

        const std::string& GetRootPath() { return mRootPath; }

        void Update(const TimeType deltaT);

        Lua& GetLua() { assert(mLua); return *mLua;}

        void RegisterComponent(Component* c);
        void UnregisterComponent(Component* c);

        void RegisterUpdatable(Updatable* u);
        void UnregisterUpdatable(Updatable* u);

    private:
        CLArguments* mCLArguments;
        /** \brief Relative path from binaries to root folder **/
        std::string mRootPath;
        Lua* mLua;
        bool mPhysFSInitialized;

        std::set<Component*> mComponents;
        std::set<Updatable*> mUpdatables;
};

} // namespace jar

#endif // JAR_JARIMPL_HPP
