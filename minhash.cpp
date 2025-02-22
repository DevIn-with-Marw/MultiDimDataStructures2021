#include "simint.h"

Eigen::Matrix<uint32_t,sigr,Eigen::Dynamic>  makeSignatures(std::vector<std::set<uint32_t>> S) // type , rows , columns
{
   
    Eigen::Matrix<uint32_t,sigr,Eigen::Dynamic> signatures; 

    signatures.resize(100,S.size());
     //initialize signature array with infinity aka UINT32_MAX
    for(int columns=0; columns<S.size(); columns++)
    {
        for(int rows=0; rows<sigr; rows++)
            signatures(rows,columns) = UINT32_MAX;
    }
   
    //calculates 100 hash values for each hashed-shingle by choosing randomly from a uniform distribution in (0, 2^32-1)
    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);  
    // passes the Sets = columns of matrix signatures
    for(int i =0; i<S.size(); i++) 
    {
        // passes the shingles of given Set
        for(auto it = S[i].begin(); it != S[i].end(); it++) 
        {
            std::mt19937 generator(*it);        //takes a hashed-shingle and produces pseudo-random numbers using the mersenne twister algorithm (unbiased and just)
            
            for (int j = 0; j<sigr; ++j) // passes the number of signatures
                signatures(j,i) = std::min(signatures(j,i), dist(generator)); // dist(generator) each time that is called it produces a different result
        }
    }
    
    return signatures;
}


