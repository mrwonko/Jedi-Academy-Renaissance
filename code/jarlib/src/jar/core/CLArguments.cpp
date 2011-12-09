#include "jar/core/CLArguments.hpp"
#include "jar/core/Helpers.hpp"

namespace jar
{
    //Singleton
    template<> CLArguments* Singleton<CLArguments>::mSingleton = NULL;

    CLArguments::CLArguments(int argc, char** argv)
    {
        bool first = true;
        //Loop through the arguments
        for(int i = 0; i < argc; ++i)
        {
            //First argument contains the executable path: C:\Path\program.exe
            if(i==0)
            {
                //Strip everything until the last \ or /
                mWorkingDir=argv[0];
                mExecutable=argv[0];

                //Windows or Unix style? Windows has \, Unix /
                mWindows = mWorkingDir.find('/', 0)==std::string::npos;
                std::string::size_type pos;
                if(mWindows)
                {
                    pos = mWorkingDir.find_last_of("\\", mWorkingDir.length());
                    if(pos != std::string::npos)
                    {
                        mWorkingDir.erase(pos+1, mWorkingDir.length());
                    }
                    else
                    {
                        //if it couldn't be found it's because the program's being run from command line from *within* the right folder. In that case, we're already at the right working directory.
                        mWorkingDir = "./";
                    }
                }
                else //didn't find /, search for \.
                {
                    pos = mWorkingDir.find_last_of("/", mWorkingDir.length());
                }
                if(pos != std::string::npos)
                {
                    mWorkingDir.erase(pos+1, mWorkingDir.length());
                }
                else
                {
                    mWorkingDir = "./";
                }
            }
            else
            {
                mArguments.push_back(argv[i]);
                if(!first)
                {
                    mArgumentString += " ";
                }
                mArgumentString += argv[i];
                first = false;
            }
        }
    }

    std::string CLArguments::MatchPath(const std::string& dir) const
    {
        //if the given path is already absolute, do nothing to it
        if(dir.find_first_of("/")==0
        || dir.find(":")!=std::string::npos)
        {
            return (dir);
        }
        //else make it an absolute path
        std::string result=GetWorkingDirectory()+dir;

        //if we are on a windows machine make sure to use backslashes
        if(mWindows)
        {
            //as long as slashes can be found
            std::string::size_type pos = std::string::npos;
            while( (pos = result.find("/")) != std::string::npos)
            {
                //replace the slash with a backslash
                result[pos] = '\\';
            }
        }
        return(result);
    }

    const int CLArguments::FindArgument(const std::string& requestedArgument, bool caseSensitive) const
    {
        // the position of the argument, stays -1 if nothing is found
        int position = -1;
        std::string pattern = requestedArgument;
        if(!caseSensitive) Helpers::ToLower(pattern);
        //loop through the arguments, looking for the specified argument
        for(unsigned int i = 0; i < mArguments.size(); ++i)
        {
            std::string copy(mArguments.at(i));
            //if this argument is the one which was requested
            if( (caseSensitive && copy == pattern ) ||
                (!caseSensitive && Helpers::ToLower(copy) == pattern)
               )
            {
                //save its position and exit the loop
                position = i;
                break;
            }
        }
        //return the position of the requested argument
        return position;
    }

}
