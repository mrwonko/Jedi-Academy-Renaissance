#ifndef JAR_INPUT_INPUT_HPP
#define JAR_INPUT_INPUT_HPP

#include "jar/core/Singleton.hpp"
#include "jar/input/API.hpp"

namespace jar {

class InputImpl;
class JARINPUTAPI Input : public Singleton<Input>
{
    public:
        Input();
        virtual ~Input();

    private:
        InputImpl* mImpl;
};

} // namespace jar

#endif // JAR_INPUT_INPUT_HPP
