#ifndef JAR_CORE_LOGGER_HPP
#define JAR_CORE_LOGGER_HPP

#include <string>
#include <jar/core/API.hpp>

namespace jar
{
    //another way of doing this would be to inherit from std::ostream and then put one of "my" loggers into std::cout - though that way I'd only have Log(), not the others.
    /** \brief Interface for a Logger that logs stuff.
        \headerfile <jar/core/Logger.hpp>
    **/
    class JARCOREAPI Logger
    {
        public:
            /**
                \brief Constructor.

                First Logger to be created is the default Logger. Use SetDefaultLogger() to change it.
            **/
            Logger();
            virtual ~Logger();

            //functions that are overwritten by ancestors

            /**
                \brief Logs a text.
            **/
            virtual void Log(const std::string& message) {}
            /**
                \brief Logs a warning.

                That may be different from normal logging due to special prefixes, highlighting etc.
            **/
            virtual void Warning(const std::string& message) {}
            /**
                \brief Logs an Error.

                That may be different from normal logging due to special prefixes, highlighting etc.
            **/
            virtual void Error(const std::string& message) {}

            //functions that are the same for all loggers

            /**
                \brief Logs a text using Log() if the logging level is high enough.
                \param minlevel The minimum logging level required for the message to be logged. Change the current level with IncreaseLoggingLevel(), DecreaseLoggingLevel() and SetLoggingLevel().
            **/
            void Info(const std::string& message, int minlevel=0);
            /**
                \brief Sets the logging level.

                Info() only logs messages "important" enough.
                \note This library's maximum used level is 3, i.e. SetLoggingLevel(3) makes everything logged to the default logger visible.
            **/
            void SetLoggingLevel(int newlevel);
            /**
                \brief returns the current logging level
            **/
            const int GetLoggingLevel();
            /**
                \brief Increases the current logging level by 1 (by default)
                \param amount The amount by which the logging level is to be increased. Default: 1
            **/
            void IncreaseLoggingLevel(int amount = 1);
            /**
                \brief Decreases the current logging level by 1 (by default)
                \param amount The amount by which the logging level is to be decreased. Default: 1
            **/
            void DecreaseLoggingLevel(int amount = 1);
            /**
                \brief returns the default logger (or a VoidLogger if none exists to prevent runtime errors)

                The default logger is used by this library for warnings. Make sure to initialize it.
            **/
            static Logger& GetDefaultLogger();
            /**
                \brief sets the default logger
            **/
            static void SetDefaultLogger(Logger& l);
        private:
            /**
                \brief The logging level. Default: 0

                Info(messages, minlevel) only logs messages "important" enough, i.e. mLoggingLevel is at least minlevel.
            **/
            int mLoggingLevel;
            static Logger* mDefaultLogger;
            /// When GetDefaultLogger is called and no default logger exists, a VoidLogger is created and a pointer to it is saved here. It is deleted when a "real" Logger is created.
            static Logger* mFallbackVoidLogger;

    };

} // namespace jar

#endif // JAR_LOGGER_HPP
