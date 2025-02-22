#include "simint.h"

void SkipBOM(std::ifstream &in)
{
    char test[3] = {0};
    in.read(test, 3);
    if ((unsigned char)test[0] == 0xEF &&
        (unsigned char)test[1] == 0xBB &&
        (unsigned char)test[2] == 0xBF)
    {
        return;
    }
    in.seekg(0);
}

std::string conv_to_lower(std::string shingle)
{
    std::transform(shingle.begin(), shingle.end(),shingle.begin(),[](unsigned char ch){ return std::tolower(ch); });
    return shingle;
}

uint32_t MurmurOAAT_32(const char *str, uint32_t h)
{
    // One-byte-at-a-time hash based on Murmur's mix
    // Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
    for (; *str; ++str)
    {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}


std::set<std::uint32_t> Murmurshingling(const std::string filepath)    
{

    //lvariables
    std::ifstream openstream(filepath);
    std::string temp = "";
    std::set<std::uint32_t> k_shingles_hashed;

    
    
    if (openstream.is_open())
    {
        if (openstream.good()) //check to see if stream is good enough to work
        {
            SkipBOM(openstream); // Due to the file Formatting  ( UTF-8 ) we need to skip the 3 first characters of the txt
            while(openstream >> temp) {
                temp = conv_to_lower(temp);
                temp.erase(std::remove_if(temp.begin(), temp.end(), ispunct), temp.end());      //remove punctuation
                k_shingles_hashed.insert(MurmurOAAT_32(temp.c_str(), 0x12345678));
            }
            
            openstream.close();
            return k_shingles_hashed;
        }
        else
        {
            std::cerr << "Stream is bad... " << std::endl;
        }
    }
    else
    {
        //error opening the file abort
        std::cerr << "Error opening the file" << std::endl;
        return k_shingles_hashed;
    }
    assert(k_shingles_hashed.size()!=0); //we should never reach here;
    return k_shingles_hashed;
}