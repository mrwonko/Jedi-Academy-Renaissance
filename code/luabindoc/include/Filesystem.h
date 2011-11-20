#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <list>
#include <fstream>

class Filesystem
{
    public:
        /** \param includeDirs List of directories to search for files, in addition to the working directory. Keep alive, not copied! **/
        Filesystem(const std::list<std::string>& includeDirs);
        virtual ~Filesystem();

        /** \brief Searches a given file
            \param filename relative (to working directory or one of the includeDirs) or absolute file name
            \return Valid filename for std::fstream or empty string if not found **/
        std::string FindFile(const std::string& filename);

    private:
        const std::list<std::string>& mIncludeDirs;
};

#endif // FILESYSTEM_H
