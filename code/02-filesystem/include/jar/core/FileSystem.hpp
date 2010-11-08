#ifndef JAR_CORE_FILESYSTEM_HPP
#define JAR_CORE_FILESYSTEM_HPP

#include <string>
#include <set>
#include "jar/core/Singleton.hpp"

namespace jar
{

/** \brief The (virtual) file system, consisting of folders and zip files.
    \headerfile <jar/core/FileSystem.hpp>
**/
class FileSystem : public Singleton<FileSystem>
{
    public:
        class File
        {
            public:
                //TODO
        };

        FileSystem();
        virtual ~FileSystem();

        /** \brief Initializes the FileSystem

            Errors are logged with the default Logger.
            \note needs CLArguments.
            \return success, don't use FileSystem unless Init() returned true!
        **/
        const bool Init();

        /** \brief Deinitializes the FileSystem

            Errors are logged with the default Logger.
            \return success, don't \link Init() reinitialize \endlink FileSystem unless Deinit() returned true!
        **/
        const bool Deinit();

        /** \brief Mounts a folder or archive in the internal filesystem.

            Files/directories mounted last are checked first, i.e. "overwrite" data from previous ones.
            \param filename the name of the folder/archive
            \return Success. Errors are sent to the default Logger.
        **/
        const bool Mount(const std::string& filename);

        /** \brief Unmounts a folder or archieve in the internal filesystem.

            The folder needs to be \link #Mount mounted \endlink first.
            \param filename the name of the folder/archive
            \return Success. Errors are sent to the default Logger, e.g. that the file has not been mounted or there are still open file handles.
        **/
        const bool Unmount(const std::string& filename);

        /** \brief Opens a given file in the filesystem for reading.
            \param filename name of the file to be opened
            \param out_file output: the File will be put here, if opening succeeds
            \param sendWarnings Whether a warning should be sent to the default Logger on failure, defaults to true.
            \return Success. **/
        const bool OpenRead(const std::string& filename, File& out_file, const bool sendWarnings = true);

    private:
        std::set<std::string> mFiles;
};

} // namespace jar

#endif // JAR_CORE_FILESYSTEM_HPP
