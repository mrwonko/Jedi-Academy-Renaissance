#include "jar/graphics/g2CompBone.hpp"
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>

namespace jar
{
namespace g2
{
    void CompBone::Decompress(btVector3& out_position, btQuaternion& out_rotation) const
    {
        out_rotation.setValue(float(mCompressedData[1]) / 16383.f - 2.f,
            float(mCompressedData[2]) / 16383.f - 2.f,
            float(mCompressedData[3]) / 16383.f - 2.f,
            float(mCompressedData[0]) / 16383.f - 2.f);
        out_position.setValue(float(mCompressedData[4]) / 64.f - 512.f,
            float(mCompressedData[5]) / 64.f - 512.f,
            float(mCompressedData[6]) / 64.f - 512.f);
    }

    void CompBone::Compress(const btVector3& position, const btQuaternion& rotation)
    {
        mCompressedData[0] = unsigned short((rotation.getW() + 2.f) * 16383.f);
        mCompressedData[1] = unsigned short((rotation.getX() + 2.f) * 16383.f);
        mCompressedData[2] = unsigned short((rotation.getY() + 2.f) * 16383.f);
        mCompressedData[3] = unsigned short((rotation.getZ() + 2.f) * 16383.f);
        mCompressedData[3] = unsigned short((position.getX() + 512.f) * 64.f);
        mCompressedData[3] = unsigned short((position.getY() + 512.f) * 64.f);
        mCompressedData[3] = unsigned short((position.getZ() + 512.f) * 64.f);
    }
}
}
