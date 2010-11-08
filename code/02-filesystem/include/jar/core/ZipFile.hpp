#ifndef JAR_CORE_ZIPFILE_H
#define JAR_CORE_ZIPFILE_H

#include <string>

namespace jar
{

class ZipFileImpl;
class ZipFile
{
    public:
        ZipFile(const std::string& filename);
        virtual ~ZipFile();

        /** \brief Tries to open the zip file

            Errors are sent to the default Logger.
            \return success **/
        const bool Open();

        /** \brief Closes the zip file, provided it has been \link #Open opened \endlink first.

            Errors are sent to the default Logger. The file is automatically closed in the destructor, if necessary, though you <i>should</i> close it yourself. (Thus automatic closing causes a warning.)
            \return success **/
        const bool Close();

    private:
        /// Whether the file is currently opened
        bool mOpened;
        ZipFileImpl* mImpl;
};

} // namespace jar

#endif // JAR_CORE_ZIPFILE_H
