#include "simint.h"


	htable::htable()
	{
		//vb.reserve(n);
	}
	htable::~htable()
	{	
		//std::cout<<"Deleting...\n";
	}
	void htable::update_bucket(int hres, int index)
	{
		//the result of the hash function(hres) indexes the corresponding hash table.
		vb[hres].push_back(index);
	}

	int htable::hashfun(const Eigen::Matrix<uint32_t, 5, 1> tempmatrix)
	{
		int sum = 0;
		for(int i =0;i<tempmatrix.rows();i++)
			sum+= tempmatrix(i,0);//%prime_for_hash;
	
		return sum % prime_for_hash;	//the value 3 needs to be parameterized.	
	}

	std::vector<std::unique_ptr<htable>> hashbands(Eigen::Matrix<uint32_t, sigr, Eigen::Dynamic> tempMatrix)
	{ 
		std::vector<std::unique_ptr<htable>> vec_of_htables;
		int b = 20;
		int r = 5;		//t = 0.549 
		int bucket = 0;
		std::ofstream blocks("blocks.txt");
		//std::ofstream hashedbucks("hashblocks.txt");

		// every band has 1 hashtable
		for (int i = 0, jumper = 0; i < b; i++,jumper+=5)	//20 reps <--> #bands
		{
		
			vec_of_htables.push_back(std::make_unique<htable>());		//Each hash table in this vector corresponds to a band 
			for (int j = 0; j < tempMatrix.cols(); j++)
			{
				Eigen::Matrix<uint32_t, 5, 1> sm = tempMatrix.block<5,1>(jumper, j);
				blocks<<sm<<"\n";
				//we hash each column of the band
				bucket = vec_of_htables[i]->hashfun(sm);
				//we access the hash table and we run update 
				vec_of_htables[i]->update_bucket(bucket, j);			
			}
		 
		}

	return vec_of_htables;
	}


