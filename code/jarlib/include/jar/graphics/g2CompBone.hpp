#ifndef JAR_G2_COMP_BONE_HPP
#define JAR_G2_COMP_BONE_HPP

class btVector3;
class btQuaternion;

namespace jar
{
namespace g2
{
    struct CompBone
    {
    private:
        unsigned short mCompressedData[7];
    public:
        void Decompress(btVector3& out_position, btQuaternion& out_rotation) const;
        void Compress(const btVector3& position, const btQuaternion& rotation);
    };
}
}

#endif
