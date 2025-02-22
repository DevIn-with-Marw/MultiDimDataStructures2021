//rule: file's title (=the first line in the txt file) is LESSON LASTNAME_FIRSTNAME AM  (BE in the test files stands for Basic Eletronics)
//rule: points MUST be UNIQUE --> use set to store them 

#include "simint.h"
#define k 3	//number of keywords


struct kd_node
{
	std::filesystem::path location;
	std::string keywords[k];
	int d=0; //depth of kd-node

	kd_node* parent = NULL;			
	kd_node* left_child = NULL; 
	kd_node* right_child = NULL;

	//get coordinates = the keywords for each file according to the rule in line 1
	void getcoords(const std::string filepath)	
	{
		std::ifstream inputfile(filepath);
		std::string temp;

		getline(inputfile, temp);		//delimiter ' ' didn't work :( it took the \n character as if it was part of the string, and stopped at the next blank character
		std::stringstream title(temp);

		if (inputfile.is_open())
		{
			for (int i = 0; i < k; i++)
			{
				title>> this->keywords[i];
			}
		}
		else std::cerr << "getcoords faced an error opening the file" << std::endl;
	}

	//checks if a given point is on or between the two planes start and end
	bool is_between(std::string start, std::string end, int coord)
	{
		if (keywords[coord] >= start && keywords[coord] <= end) return true;
		else return false;
	}

	//finds the region of a node
	std::vector<std::string> make_region(int level, std::vector<std::string> qcoords)
	{
		std::vector<std::string> region;	//[x1,x2,y1,y2,z1,z2] region of current node (in the comments we refer to it as r, in the code it is refered as this)
		//x2 > x1, y2>y1, z2>z1
		//-inf and ~inf are the values that symbolize -infinite, +infinite 
		
		//terminal situations
		if (level == 0) return {"-inf", "~inf", "-inf", "~inf", "-inf", "~inf",};	//region of root is the whole space --> region.size() = 0; 
																					//The character ~ is chosen to represent + because + (and -) is ordered before all leters and numbers, while ~ is ordered after
		
		if (level == 1) 	//r = y value, r->parent = root = x value ( [Y], X ) 
		{
			//region is a block in 3d space that stops at/starts from plane x = x1
			
			//we can decide left or right block in search according to the kind of child r is to root (left or right respectively)
			//we can also be answer it now: 

			if (this == this->parent->left_child)
			{
				region.insert(region.end(), { "-inf", this->parent->keywords[0], "-inf", "~inf", "-inf", "~inf" });
				//region is (-inf, x1, -inf, +inf, -inf, +inf)
			}
			else if ((this == this->parent->right_child))
			{
				region.insert(region.end(), {this->parent->keywords[0], "~inf", "-inf", "~inf", "-inf", "~inf" });
				//region is (x1, +inf, -inf, +inf, -inf, +inf)
			}

			return region;	
		}
		else if (level == 2)	//r = z value ( [Z], Y, X )
		{
			//region is one of 4 possible blocks in 3d space that are formed by planes x = x1 = keywords[0], y = y1 = keywords[1]
			if (this == this->parent->left_child && this->parent == this->parent->parent->left_child)
			{
				//down left block in 3d space: x < X, y < Y where X, Y the values of the ancestors
				region.insert(region.end(), { "-inf" , this->parent->parent->keywords[0], "-inf", this->parent->keywords[1], "-inf", "~inf" });
				//region is (-inf, x1, -inf, y1, -inf, +inf)
			}
			else if (this == this->parent->right_child && this->parent == this->parent->parent->left_child)
			{
				//upper left block in 3d space x < X, y > Y
				region.insert(region.end(), { "-inf" , this->parent->parent->keywords[0], this->parent->keywords[1], "~inf", "-inf", "~inf" });
				//region is (-inf, x1, y1, +inf, -inf, +inf)
			}
			else if (this == this->parent->right_child && this->parent == this->parent->parent->right_child)
			{
				//upper right block in 3d space: x > X and y > Y 
				region.insert(region.end(), { this->parent->parent->keywords[0] , "~inf", this->parent->keywords[1] , "~inf", "-inf", "~inf" });
				//region is (x1, +inf, y1, +inf, -inf, +inf)
			}
			else if (this == this->parent->left_child && this->parent == this->parent->parent->right_child)
			{
				//down right block in 3d space x > X and y < Y 
				region.insert(region.end(), { this->parent->parent->keywords[0] , "~inf", "-inf", this->parent->keywords[1], "-inf", "~inf" });
				//region is (x1, +inf, -inf, y1, -inf, +inf)
			}
			return region;
		}
		//From now on, we agree that the first coordinate we encounter is x1, y1 or z2 and the second of its kind is x2, y2 or z2
		//At first, this naming has nothing to do with the values' order in the space! It is simply their name. 
		//To form the region, we first need to select the right x2, y2, z2. 
		//Example of invalid coordinate x2 is: x2 > x1 and r lies on x1's left or y2 < y1 and r is above y1. 
		//Valid situations: IF x2>x1, r needs to lie on x1's right 
		//					EXPLANATION: x1 < x2 means that x1 is on x2's left. r is a left successor of x2, because it is a child of x1.
		//								 r left successor of x2 <--> r.x < x2 AND r right child of x1 <--> r.x > x1 => x1 < r.x < x2
		//					IF x2<x1, r needs to lie on x1's left
		//Similarly for the rest of the values
		else 	
		{
			//starts at level 3
			//initialize known coordinates
			std::string x1, y1, z1, x2, y2, z2;
			kd_node* ancestor = this->parent; 	

			if (level % 3 == 0)	//r = x value, r->parent = z value, r->grandparent = y value ( [X], Z, Y, X, Z, Y , X...)
			{
				//ancestor has Z value
				z1 = ancestor->keywords[2];
				ancestor = ancestor->parent;	  //I have Y value	
				y1 = ancestor->keywords[1];
				ancestor = ancestor->parent;	  //I have X value  
				x1 = ancestor->keywords[0];
				ancestor = ancestor->parent;	  //I have Z value 
			}
			else if (level % 3 == 1) //r = y value  ( [Y], X, Z, Y, X, Z ...)
			{
				//ancestor has X value
				x1 = ancestor->keywords[2];
				ancestor = ancestor->parent;	  //I have Z value	
				z1 = ancestor->keywords[1];
				ancestor = ancestor->parent;	  //I have Y value  
				y1 = ancestor->keywords[0];
				ancestor = ancestor->parent;	  //I have X value 
			}
			else   //r = z value  ( [Z], Y, X, Z, Y, X ...)
			{
				//ancestor has Y value
				y1 = ancestor->keywords[2];
				ancestor = ancestor->parent;	  //I have X value	
				x1 = ancestor->keywords[1];
				ancestor = ancestor->parent;	  //I have Z value  
				z1 = ancestor->keywords[0];
				ancestor = ancestor->parent;	  //I have Y value 
			}

			//initialize flags for finding a second valid coordinate
			//(for limiting the area the first 3 create--> according to the pictures shown in class, region of node is the smallest region that contains it)
			bool valid_x, valid_y, valid_z, valid_all;
			valid_x = false;
			valid_y = false;
			valid_z = false;
			valid_all = false;

			//find second valid coordinates
			while (valid_all == false && ancestor != NULL)
			{
				if (valid_x == false and ancestor->d % 3 == 0)	//if we haven't found a valid x2 and we are currently at a node of x value
				{
					x2 = ancestor->keywords[0]; 
					if (x2 < x1 && this == this->parent->left_child) valid_x = true;	//me being the left child means my x value was smaller than my parent's x value
					else if (x2 > x1 && this == this->parent->right_child) valid_x = true; //just for understanding
				}
				else if (valid_y == false && ancestor->d % 3 == 1) //if we haven't found a valid y2 and we are currently at a node of y value
				{
					y2 = ancestor->keywords[1];
					if (y2 < y1 && this == this->parent->left_child) valid_y = true;
					else if (y2 > y1 && this == this->parent->right_child) valid_y = true;
				}
				else if (valid_z == false && level % 3 == 2)  //if we haven't found a valid z2 and we are currently at a node of z value
				{
					z2 = ancestor->keywords[2];
					if (z2 <z1 && this == this->parent->left_child) valid_z = true;
					else if (z2 > z1 && this == this->parent->right_child) valid_z = true;	
				}
				
				valid_all = valid_x && valid_y && valid_z;
				ancestor = ancestor->parent;
			}

			//form the region  
			
			//we found every 2nd valid coordinate, region is [x1, x2, y1, y2, z1, z2]
			if (valid_all == true)
			{
				region.insert(region.end(), { min(x1,x2), max(x1,x2), min(y1,y1), max(y1,y2), min(z1,z2), max(z1,z2) });
			}
			else if (valid_x == true && valid_y == true)
			{
				region.insert(region.end(), { min(x1,x2), max(x1,x2), min(y1,y1), max(y1,y2) });
				if (level % 3 == 0) //([X] Z)
				{
					//z1 is 1 level back
					if (this == this->parent->left_child)	// r.z <Z
						region.insert(region.end(), { "-inf", z1 });
					else region.insert(region.end(), { z1, "~inf" });
				}
				else if (level % 3 == 1) //[Y] X Z
				{
					//z1 is 2 lavels back
					if (this->parent == this->parent->parent->left_child) //r left successor of Z <--> r.z <Z
						region.insert(region.end(), { "-inf", z1 });
					else region.insert(region.end(), { z1, "~inf" });
					
				}
				else // [Z] Y X Z
				{
					//z1 is 3 levels back
					if (this->parent->parent == this->parent->parent->parent->left_child)	//r left successor of Z <--> r.z <Z
						region.insert(region.end(), { "-inf", z1 });
					else region.insert(region.end(), { z1, "~inf" });
				}
			}
			else if (valid_x == true && valid_z == true)
			{
				region.insert(region.end(), { min(x1,x2), max(x1,x2) });
				if (level % 3 == 0) //([X] Z Y)
				{
					//y1 is 2 levels back 
					if (this->parent == this->parent->parent->left_child) //r left successor of Y <--> r.y <Y
						region.insert(region.end(), { "-inf", y1 , min(z1,z2), max(z1,z2)});
					else region.insert(region.end(), { y1, "~inf", min(z1,z2), max(z1,z2) });
				}
				else if (level % 3 == 1) //[Y] X Z Y
				{
					//y1 is 3 levels back 
					if (this->parent->parent == this->parent->parent->parent->left_child)	//r left successor of Y <--> r.y <Y
						region.insert(region.end(), { "-inf", y1, min(z1,z2), max(z1,z2)});
					else region.insert(region.end(), { y1, "~inf", min(z1,z2), max(z1,z2) });

				}
				else // [Z] Y X 
				{
					//y1 is 1 level back
					if (this == this->parent->left_child)	// r.y <Y
						region.insert(region.end(), { "-inf", y1, min(z1,z2), max(z1,z2) });
					else region.insert(region.end(), { y1, "~inf", min(z1,z2), max(z1,z2) });
				}

			}
			else if (valid_y == true && valid_z == true)
			{
				if (level % 3 == 0) //([X] Z Y X)
				{
					//x1 is 3 levels back 
					if (this->parent->parent == this->parent->parent->parent->left_child)	//r left successor of X <--> r.x <X
						region.insert(region.end(), { "-inf", x1, min(y1,y2), max(y1,y2), min(z1,z2), max(z1,z2) });
					else region.insert(region.end(), { x1, "~inf", min(y1,y2), max(y1,y2), min(z1,z2), max(z1,z2) });
				}
				else if (level % 3 == 1) //[Y] X Z Y
				{
					//x1 is 1 levels back 
					if (this == this->parent->left_child)	// r.x <X
						region.insert(region.end(), { "-inf", x1, min(y1,y2), max(y1,y2), min(z1,z2), max(z1,z2) });
					else region.insert(region.end(), { x1, "~inf", min(y1,y2), max(y1,y2), min(z1,z2), max(z1,z2) });
				}
				else // [Z] Y X 
				{
					//x1 is 2 levels back
					if (this->parent == this->parent->parent->left_child) //r left successor of X<--> r.x <X
						region.insert(region.end(), { "-inf", x1, min(y1,y2), max(y1,y2),min(z1,z2), max(z1,z2) });
					else region.insert(region.end(), { x1, "~inf", min(y1,y2), max(y1,y2), min(z1,z2), max(z1,z2) });
				}
			}
			else if (valid_x == true)
			{
				region.insert(region.end(), { min(x1,x2), max(x1,x2) });
				if (level % 3 == 0)	//[X] Y Z
				{
					//y1 is 1 level back 
					if (this == this->parent->left_child)	//r left child of Y <--> r.y <Y
						region.insert(region.end(), {"-inf", y1});
					else region.insert(region.end(), {y1, "~inf" });
					//z1 is 2 levels back
					if (this->parent == this->parent->parent->left_child)	//r left successor of Z r.z <Z
						region.insert(region.end(), { "-inf", z1 });
					else region.insert(region.end(), { z1, "~inf" });
				}
				else if (level % 3 == 1)	//[Y] X Z Y
				{
					//y1 is 3 levels back 
					if (this->parent->parent == this->parent->parent->parent->left_child) //r left successor of Y <--> r.y <Y
						region.insert(region.end(), { "-inf", y1 });
					else region.insert(region.end(), { y1, "~inf" });
					//z1 is 2 levels back
					if (this->parent == this->parent->parent->left_child)	//r left successor of Z r.z <Z
						region.insert(region.end(), { "-inf", z1 });
					else region.insert(region.end(), { z1, "~inf" });
				}
				else //[Z] Y X Z
				{
					//y1  is 1 level back
					if (this == this->parent->left_child)	//r left child of Y <--> r.y <Y
						region.insert(region.end(), { "-inf", y1 });
					else region.insert(region.end(), { y1, "~inf" });
					//z1 is 3 levels back
					if (this->parent->parent == this->parent->parent->parent->left_child) //r left successor of Z <--> r.z <Z
						region.insert(region.end(), { "-inf", z1 });
					else region.insert(region.end(), { z1, "~inf" });
				}

			}
			else if (valid_y == true)
			{
				if (level % 3 == 0)	//[X] Z Y X
				{
					//x1 is 3 levels back 
					if (this->parent->parent == this->parent->parent->parent->left_child) //r left successor of X <--> r.x <X
						region.insert(region.end(), { "-inf", x1 });
					else region.insert(region.end(), { x1, "~inf" });
					//z1 is 1 level back
					if (this == this->parent->left_child)	//r left child of Y <--> r.y <Y
						region.insert(region.end(), { min(y1,y2), max(y1,y2), "-inf", z1 });
					else region.insert(region.end(), { min(y1,y2), max(y1,y2), z1, "~inf" });
				}
				else if (level % 3 == 1)	//[Y] X Z Y
				{
					//x1 is 1 level back 
					if (this == this->parent->left_child)	//r left child of X <--> r.x <X
						region.insert(region.end(), {"-inf", x1 });
					else region.insert(region.end(), { x1, "~inf" });
					//z1 is 2 levels back
					if (this->parent == this->parent->parent->left_child)	//r left successor of Z r.z <Z
						region.insert(region.end(), { min(y1,y2), max(y1,y2), "-inf", z1 });
					else region.insert(region.end(), { min(y1,y2), max(y1,y2), z1, "~inf" });
				}
				else //[Z] Y X Z
				{
					//x1  is 2 levels back
					if (this->parent == this->parent->parent->left_child)	//r left successor of X <--> r.x <X
						region.insert(region.end(), { "-inf", x1 });
					else region.insert(region.end(), { x1, "~inf" });
					//z1 is 3 levels back
					if (this->parent->parent == this->parent->parent->parent->left_child) //r left successor of Z <--> r.z <Z
						region.insert(region.end(), { min(y1,y2), max(y1,y2), "-inf", z1 });
					else region.insert(region.end(), { min(y1,y2), max(y1,y2), z1, "~inf" });
				}
			}
			else if (valid_z == true)
			{
				if (level % 3 == 0)	//[X] Z Y X
				{
					//x1 is 3 levels back 
					if (this->parent->parent == this->parent->parent->parent->left_child) //r left successor of X <--> r.x <X
						region.insert(region.end(), { "-inf", x1 });
					else region.insert(region.end(), { x1, "~inf" });
					//y1 is 2 levels back
					if (this->parent == this->parent->parent->left_child)	//r left successor of Y r.y <Y
						region.insert(region.end(), { "-inf", y1, min(z1,z2), max(z1,z2) });
					else region.insert(region.end(), { y1, "~inf", min(z1,z2), max(z1,z2) });
				}
				else if (level % 3 == 1)	//[Y] X Z Y
				{
					//x1 is 1 level back 
					if (this == this->parent->left_child)	//r left child of X <--> r.x <X
						region.insert(region.end(), { "-inf", x1 });
					else region.insert(region.end(), { x1, "~inf" });
					//y1 is 3 levels back
					if (this->parent->parent == this->parent->parent->parent->left_child) //r left successor of Y <--> r.y <Y
						region.insert(region.end(), { "-inf", y1, min(z1,z2), max(z1,z2) });
					else region.insert(region.end(), { y1, "~inf", min(z1,z2), max(z1,z2) });
				}
				else //[Z] Y X Z
				{	
					//x1  is 2 levels back
					if (this->parent == this->parent->parent->left_child)	//r left successor of X <--> r.x <X
						region.insert(region.end(), { "-inf", x1 });
					else region.insert(region.end(), { x1, "~inf" });
					//y1 is 1 level back
					if (this == this->parent->left_child)	//r left child of Y <--> r.y <Y
						region.insert(region.end(), { "-inf", y1, min(z1,z2), max(z1,z2) });
					else region.insert(region.end(), { y1, "~inf", min(z1,z2), max(z1,z2) });
				}

			}
			else //all invalid! We only have x1, y1, z1
			{
				if (level % 3 == 0)	//[X] Z Y X
				{
					//x1 is 3 levels back 
					if (this->parent->parent == this->parent->parent->parent->left_child) //r left successor of X <--> r.x <X
						region.insert(region.end(), { "-inf", x1 });
					else region.insert(region.end(), { x1, "~inf" });
					//y1 is 2 levels back
					if (this->parent == this->parent->parent->left_child)	//r left successor of Y <--> r.y <Y
						region.insert(region.end(), { "-inf", y1});
					else region.insert(region.end(), { y1, "~inf"});
					//z1 is  1 level back
					if (this == this->parent->left_child)		//r left child of Z <--> r.z < Z
						region.insert(region.end(), { "-inf", z1 });
					else region.insert(region.end(), {z1, "~inf"});
				}
				else if (level % 3 == 1)	//[Y] X Z Y
				{
					//x1 is 1 level back 
					if (this == this->parent->left_child)	//r left child of X <--> r.x <X
						region.insert(region.end(), { "-inf", x1 });
					else region.insert(region.end(), { x1, "~inf" });
					//y1 is 3 levels back
					if (this->parent->parent == this->parent->parent->parent->left_child) //r left successor of Y <--> r.y <Y
						region.insert(region.end(), { "-inf", y1 });
					else region.insert(region.end(), { y1, "~inf"});
					//z1 is 2 levels back
					if (this->parent == this->parent->parent->left_child)	//r left successor of Z <--> r.z <Z
						region.insert(region.end(), { "-inf", z1 });
					else region.insert(region.end(), { z1, "~inf" });
				}
				else //[Z] Y X Z
				{
					//x1  is 2 levels back
					if (this->parent == this->parent->parent->left_child)	//r left successor of X <--> r.x <X
						region.insert(region.end(), { "-inf", x1 });
					else region.insert(region.end(), { x1, "~inf" });
					//y1 is 1 level back
					if (this == this->parent->left_child)	//r left child of Y <--> r.y <Y
						region.insert(region.end(), { "-inf", y1 });
					else region.insert(region.end(), { y1, "~inf" });
					//z1 is 3 levels back
					if (this->parent->parent == this->parent->parent->parent->left_child) //r left successor of Z <--> r.z <Z
						region.insert(region.end(), { "-inf", z1 });
					else region.insert(region.end(), { z1, "~inf" });
				}
			}

			return region;
		}

	}
};

class KD_Tree
{
private:
	kd_node root;
	int tree_depth = 0;
	int count = 0;
public:
	KD_Tree(){ }
	//returns the root of the tree
	kd_node* getroot()
	{
		return &root;
	}

	//*********NODE-ORIENTED TREE*********************//--> SPACE IS DEVIDED WITH LINES ON THE POINTS
	
	//finds where a node should be inserted for the construction of kd tree
	kd_node *Access(kd_node &curr, kd_node &to_ins, int &depth, bool &pos)	//depth passes with reference so that isert knows the depth of the node to_ins
	{
		int coordinate4compare;

			coordinate4compare = depth % k;
			
			if (to_ins.keywords[coordinate4compare] <= curr.keywords[coordinate4compare])
			{
				if (curr.left_child == NULL)
				{
					pos = true;				//node needs to be inserted on curr's left 
					return &curr;
				}
				return Access(*(curr.left_child), to_ins, ++depth, pos);
			}
			else
			{
				if (curr.right_child == NULL)
				{
					//pos remains false because this means the node needs to be inserted as a right child
					return &curr;
				}
				return Access(*(curr.right_child), to_ins, ++depth, pos);
			}
	}

	//converts a file to a kd_node according to the rule in line 1 and inserts the node in the kd tree
	void insert(std::string filepath)
	{
		bool left = false; 
		int depth = 0;
		kd_node* kd = new kd_node;
		kd->getcoords(filepath);
		kd->location = filepath;

		if (count == 0)
		{
			this->root = *kd;
			count++;
		}
		else
		{
			kd_node* insertion_node = Access(this->root, *kd, depth, left);


			if (left == true)
			{
				insertion_node->left_child = kd;
				insertion_node->left_child->parent = insertion_node;
				insertion_node->left_child->d = ++depth;
				tree_depth++;
			}
			else
			{
				insertion_node->right_child = kd;
				insertion_node->right_child->parent = insertion_node;
				insertion_node->right_child->d = ++depth;
				tree_depth++;

			}
			count++;
		}
		
	}

	//search //rule: "~inf" and "-inf" mean +inf, -inf, in query region R too.
	std::vector<std::string> search(kd_node* r, int level, std::vector<std::string> coords, std::vector<std::string> &answer)
	{
		//make region of r 
		std::vector<std::string> reg = r->make_region(level, coords);	

		//if current node is leaf, check if it lies in region R to add it in the answer
		if (r->left_child == NULL && r->right_child == NULL)
		{ 
			if (r->is_between(coords[0], coords[1], 0) && r->is_between(coords[2], coords[3], 1) && r->is_between(coords[4], coords[5], 2))
			{
				answer.push_back(r->location.string());
			}
		}
		//if current node is not leaf, check if its region is fully contained in query region R
		else if (NR_in_QR(reg, coords) == true)	
		{
			reportInorder(r, answer);
		}
		else
		{
			//checks if a child's region intersects query region and searches there
			if (r->left_child != NULL && NRQRforeign(reg,coords) ==false) 
				search(r->left_child, level+1, coords, answer);
			if (r->right_child != NULL && NRQRforeign(reg, coords) == false) 
				search(r->right_child, level+1, coords, answer);
			if (r->is_between(coords[0], coords[1], 0) && r->is_between(coords[2], coords[3], 1) && r->is_between(coords[4], coords[5], 2))
			{
				answer.push_back(r->location.string());
			}
		}
		
		return answer;
	}

	//prints the nodes of the tree
	void printTree(kd_node *r)
	{
		std::cout << "Current: " << r->location.string() << " Depth: "<<r->d<< " Left Child: ";
		if (r->left_child != NULL) std::cout << r->left_child->location.string();
		else std::cout << "NULL";
		std::cout << " Right Child ";
		if (r->right_child != NULL) std::cout << r->right_child->location.string() << std::endl;
		else std::cout << "NULL" << std::endl;

		if (r->left_child != NULL)	printTree(r->left_child); 
		if (r->right_child != NULL) printTree(r->right_child);

	}

	void reportInorder(kd_node *r, std::vector<std::string> &report)
	{
		if (r == NULL) return;

		reportInorder(r->left_child, report);
		report.push_back(r->location.string());
		reportInorder(r->right_child, report);
	
	}
};

//don't forget to check if query region from input makes sense or make sure to alter it to make sense! x1<x2, y1<y2, z1<z2 
int main()
{
	std::vector<std::set<std::uint32_t>> sets_of_answer;
	std::vector<std::string> answer;
	KD_Tree kdtree = KD_Tree();
	const std::filesystem::path testfolder= "../testfiles" ;
	
	auto startconstruction = std::chrono::steady_clock::now();
	for (const auto& entry : std::filesystem::directory_iterator(testfolder))
	{
		const std::string filenameStr = testfolder.string() + "/"+ entry.path().filename().string();
		if (entry.is_regular_file())
		{
			kdtree.insert(filenameStr);
		}
	}
	auto endconstruction = std::chrono::steady_clock::now();
	std::cout << "Elapsed time in microseconds for building kd-Tree:" << std::chrono::duration_cast<std::chrono::microseconds>(endconstruction - startconstruction).count() <<
		"us" << std::endl;

	std::vector<std::string> queryR;
	std::string temp;
	queryR.reserve(6);
	std::cout << "Please insert coordinates for query region R: " << std::endl;
	std::cout << "Lessons Range: " << std::endl<<"From: ";
	std::cin >> temp;
	queryR.push_back(temp);
	std::cout << std::endl<< "To: ";
	std::cin >> temp;
	queryR.push_back(temp);
	std::cout << "Student's Full Name's Range " << std::endl<< "From: " ;
	std::cin >> temp;
	queryR.push_back(temp);
	std::cout << std::endl << "To: ";
	std::cin >> temp;
	queryR.push_back(temp);
	std::cout << "SID Range: " << std::endl << "From: ";
	std::cin >> temp;
	queryR.push_back(temp);
	std::cout << std::endl << "To: ";
	std::cin >> temp;
	queryR.push_back(temp);

	check_validreg(queryR);

	auto startquery = std::chrono::steady_clock::now();
	kdtree.search(kdtree.getroot(), 0, queryR, answer);
	auto endquery = std::chrono::steady_clock::now();
	
	if (answer.size() == 0)
		std::cout << "There are no files in query region." << std::endl;
	else
	{
		for (int i = 0; i < answer.size(); i++)
		{
			std::cout << answer[i] << std::endl;
		}
	}
	std::cout << "Elapsed time in microseconds for searching kd-Tree:" << std::chrono::duration_cast<std::chrono::microseconds>(endquery - startquery).count() << "us" << std::endl;

	//create sets for each file in answer
	for (int i = 0; i < answer.size(); i++)
	{
		sets_of_answer.push_back(Murmurshingling(answer[i]));
	}

	Eigen::Matrix<uint32_t, 100, Eigen::Dynamic> signatures = makeSignatures(sets_of_answer);

	//Candidatepairs.txt has every doc with similarity
	std::ofstream candidatepairs("candidatepairs.txt");

	std::vector<std::unique_ptr<htable>> vhf;
	vhf = hashbands(signatures);

	candidatepairs << "THESE ARE DOCUMENTS THAT NEED TO BE CHECKED FOR PLAGIARISM !! \n\n\t\t\t\t\tKDTree+LSH\n";
	//for each hashtable 
	int hnum = 0;
	for (auto&& htable : vhf)
	{
		std::cout << "[" << hnum++ << "]:";
		candidatepairs << "[" << hnum << "]:";
		//skanaroume ton vb poy einai oi kouvades toy hashmap oysiastika
		for (int i = 0; i < htable->vb.size(); i++)
		{
			//an se kapoion kouva exoyn hasharistei >1 stiles dhladh docs typose ta
			if (htable->vb[i].size() > 1)
			{

				std::cout << "{" << i << "}:";
				candidatepairs << "{" << i << "}:";
				for (int j = 0; j < htable->vb[i].size(); j++) {
					std::cout << answer[htable->vb[i][j]] << " ";
					//28 may need to be changed based on the last / of the path of the dataset.
					//For example here path is /home/dimet/Desktop/dataset/ so the last / corresponds to the 28th character
					candidatepairs << answer[htable->vb[i][j]].substr(28) << " ";
				}
				candidatepairs << "]";
				candidatepairs << '\t';
				std::cout << "]";
				std::cout << '\t';
			}
		}
		std::cout << "\n";
		candidatepairs << "\n";
	}
	
	return(0);
}
