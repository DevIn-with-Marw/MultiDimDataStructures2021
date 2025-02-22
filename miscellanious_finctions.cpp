#include "simint.h"

// ------ FOR ALL TREES ------ //

//checks if query region R given by the user is valid. If not, the function fixes the query region.
bool check_validreg(std::vector<std::string> coords)
{
	if (coords[0] > coords[1]) std::swap(coords[0], coords[1]);
	else if (coords[2] > coords[3]) std::swap(coords[2], coords[3]);
	else if (coords[4] > coords[5]) std::swap(coords[4], coords[5]);

	return true;
}

// ------ FOR R-TREES AND KD-TREES ------//

//compares node region and query region: returns true if node region is fully contained in query region
bool NR_in_QR(std::vector<std::string> node_r, std::vector<std::string> query_r)
{
	for (int i = 0; i < 6; i++)
	{
		if (i % 2 == 0 && node_r[i] < query_r[i]) //in even positions of node_r are x1, y1, z1. If x1 < Xr1, then the region of node r  is not fully contained in query region R.
			return false;
		else if (i % 2 == 1 && node_r[i] > query_r[i]) //odd positions: x2, y2, z2. If x2 > X2, then the region of node r is not fully contained in query region R.  
			return false;								//is not fully contained means the two regions intersect, or are foreign.
	}
	return true;
}

//compares node region and query region: returns true if node region and query region are foreign
bool NRQRforeign(std::vector<std::string> node_r, std::vector<std::string> query_r)
{
	if (node_r[0] > query_r[1] || node_r[1] < query_r[0] || node_r[2]> query_r[3] || node_r[3] < query_r[2] || node_r[4]> query_r[5] || node_r[5] < query_r[4])	//x1> X2 OR x2 < X1 for y and z also
		return true;
	else return false;
}
