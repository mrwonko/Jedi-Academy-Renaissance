#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <list>

class Config
{
    public:
        Config();
        virtual ~Config();

        const bool Init(int argc, char** argv);

        const std::string& GetOutputDir() const { return mOutputDir; }
        const std::list<std::string>& GetIncludeDirs() const { return mIncludeDirs; }
        const std::list<std::string>& GetSourceFiles() const { return mSourceFiles; }

    private:
        /** \brief Attempts to parse a given config file **/
        const bool LoadFromFile(const std::string& filename);

        /** \brief Attempts to parse a block in the config file. **/
        const bool ReadBlock(std::ifstream& file);

        std::string mOutputDir;
        std::list<std::string> mIncludeDirs;
        std::list<std::string> mSourceFiles;
};

#endif // CONFIG_H
