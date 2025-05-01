#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

bool LoadVolume(const std::string& filePath,
                   int width, int height, int depth,
                   std::vector<unsigned char>& outVolume);