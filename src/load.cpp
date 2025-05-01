#include "load.h"


bool LoadVolume(const std::string& filePath,
                int width, int height, int depth,
                std::vector<unsigned char>& outVolume)
{
    size_t voxelCount = static_cast<size_t>(width) * height * depth;
    outVolume.resize(voxelCount);

    std::ifstream file(filePath, std::ios::binary);
    if (!file) 
    {
        std::cout << "Failed to open raw file: " << filePath << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(outVolume.data()), voxelCount);
    if (!file) 
    {
        std::cout << "Failed to read full volume data from file." << std::endl;
        return false;
    }

    return true;
}