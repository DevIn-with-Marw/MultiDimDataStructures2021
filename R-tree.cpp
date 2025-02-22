// R-tree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "simint.h"

//group of nodes is initialized with 4 nodes which have all NULL values

struct node;
struct group_of_nodes;

int diff4words(std::string s1, std::string s2)          
{
    int distance = 0;
    distance = abs((int)s2[0] - (int)s1[0]);    

    return distance;
}

struct node                     
{
    std::string mbr_coords[6]; //= { NULL, NULL, NULL, NULL, NULL, NULL };   //mbr for internal groups (size =6) and coordinates for leaf groups (size = 3)
                                                                          //internal groups: [x_low, x_high, y_low, y_high, z_low, z_high] 
    group_of_nodes* ptr = NULL;               //NULL for leaf nodes
    group_of_nodes* in_group = NULL;          //non_NULL fro both leaves and internal nodes //needs initialization in insert
    std::string filepath;       //NULL for non-leaf nodes
    

    void get_coords()
    {
        std::ifstream inputfile(this->filepath);
        std::string temp;

        getline(inputfile, temp);
        std::stringstream title(temp);

        if (inputfile.is_open())
        {
            for (int i = 0; i < k; i++)
            {
                title >> this->mbr_coords[i];
            }
        }
        else std::cerr << "getcoords faced an error opening the file" << std::endl;

    }

    bool belongs_to(std::vector<std::string> crd, std::vector<std::string> mbr)
    {
        if (crd[0] >= mbr[0] && crd[0] <= mbr[1] && crd[1] >= mbr[2] && crd[1] <= mbr[3] && crd[2] >= mbr[4] && crd[2] <= mbr[5])
            return true;
        else return false;
    }

    void makeMBR();

    bool operator <(node& n)
    {
        if (!this->filepath.empty())
        {
            if (this->mbr_coords[0] < n.mbr_coords[0]) return true;
            else if (this->mbr_coords[0] == n.mbr_coords[0] && this->mbr_coords[1] < n.mbr_coords[1]) return true;
            else if (this->mbr_coords[0] == n.mbr_coords[0] && this->mbr_coords[1] == n.mbr_coords[1] && this->mbr_coords[2] < n.mbr_coords[2]) return true;
            else return false;
        }
        else
        {
            if (this->mbr_coords[0] < n.mbr_coords[0]) return true;
            else if (this->mbr_coords[0] == n.mbr_coords[0] && this->mbr_coords[1] < n.mbr_coords[1]) return true;
            else if (this->mbr_coords[0] == n.mbr_coords[0] && this->mbr_coords[1] == n.mbr_coords[1] && this->mbr_coords[2] < n.mbr_coords[2]) return true;
            else if (this->mbr_coords[0] == n.mbr_coords[0] && this->mbr_coords[1] == n.mbr_coords[1] && this->mbr_coords[2] == n.mbr_coords[2] && this->mbr_coords[3] < n.mbr_coords[3]) return true;
            else if (this->mbr_coords[0] == n.mbr_coords[0] && this->mbr_coords[1] == n.mbr_coords[1] && this->mbr_coords[2] == n.mbr_coords[2] && this->mbr_coords[3] == n.mbr_coords[3] && this->mbr_coords[4] < n.mbr_coords[4]) return true;
            else if (this->mbr_coords[0] == n.mbr_coords[0] && this->mbr_coords[1] == n.mbr_coords[1] && this->mbr_coords[2] == n.mbr_coords[2] && this->mbr_coords[3] == n.mbr_coords[3] && this->mbr_coords[4] == n.mbr_coords[4] && this->mbr_coords[5] < n.mbr_coords[5]) return true;
            else return false;

        }
    }
};

struct group_of_nodes //this is what the R-tree consists of
{
    node arr[4];
    node *parent;

    int groupnodes(std::vector<node> temp)
    {
        //find the distance between two consecutive nodes
        int distance[4] = {0, 0, 0, 0};
        //for each consecutive pair of words 
        for (int i = 0; i < 4; i++)
        {
            //for each coordinate
            for (int c = 0; c < 2; c++)
            {
                distance[i] += pow(diff4words(temp[i].mbr_coords[c],temp[i+1].mbr_coords[c]), 2);      //([1].x - [0].x)^2 + ([1].y - [0].y)^2
            }
            //for z coordinate strings are numbers (A.M) 
            int z1 = stoi(temp[i].mbr_coords[2]);
            int z2 = stoi(temp[i+1].mbr_coords[2]);

            distance[i] += pow(z1 - z2, 2);
            distance[i] = sqrt(distance[i]);
        }
        //there are 3 groupings 3-2 we can make. check which gives the least overall covered area.
        //we can only make 3-2 groups bacause the parent group must show at at least 2 nodes to form an mbr.
        int overall_dist[3];

        //calculate overall distance of every group
        for (int i = 0; i < 3; i++)
        {
            overall_dist[i] = distance[i] + distance[i + 1];
        }

        overall_dist[0] += distance[3];
        overall_dist[1] += pow(diff4words(temp[0].mbr_coords[0], temp[4].mbr_coords[0]), 2) + pow(diff4words(temp[0].mbr_coords[1], temp[4].mbr_coords[1]), 2);
        int z1 = stoi(temp[0].mbr_coords[2]);
        int z2 = stoi(temp[4].mbr_coords[2]);
        overall_dist[1] += pow(z1 - z2, 2);
        overall_dist[2] += distance[0];

        //find miimum overall distance
        int min = 0;
        for (int i = 1; i < 3; i++)
        {
            if (overall_dist[i] < overall_dist[min]) min = i;
        }
        
        if (min == 0)//0,1,2
        {
            return 12;
        }
        else if (min == 1)
        {
            return 123;
        }
        else
        {
            return 234;
        }

    }

    int groupMBRs(std::vector<node> temp)
    {
        int lvol = 0;
        int rvol = 0;

        //MBRS are sorted! We will split the space in an x of the third MBR because that is in the middle. 
        //The only decision to be made is for that third MBR. It must on the side of the yz plane that will be created, where the overall volume will me less.
        
        //1. assume the third MBR is on the left
        for (int i = 0; i < 3; i++)
        {
            int z1 = stoi(temp[i].mbr_coords[4]);
            int z2 = stoi(temp[i].mbr_coords[5]);
            int zdist = z2 - z1;
            lvol += diff4words(temp[i].mbr_coords[0], temp[i].mbr_coords[1]) * diff4words(temp[i].mbr_coords[2], temp[i].mbr_coords[3])* zdist; 
        }

       //2. assume the third MBR is on the right
        for (int i = 2; i < 5; i++)
        {
            int z1 = stoi(temp[i].mbr_coords[4]);
            int z2 = stoi(temp[i].mbr_coords[5]);
            int zdist = z2 - z1;
            rvol += diff4words(temp[i].mbr_coords[0], temp[i].mbr_coords[1]) * diff4words(temp[i].mbr_coords[2], temp[i].mbr_coords[3]) * zdist;
        }

        if (lvol <= rvol) return 12;
        else return 234;
    }

    int choose2extend(std::vector<std::string> crd)
    {
        int region = 0; //4 at most
        int volume[4] = { -1, -1, -1, -1 };
        //virtually extend the regions, to find the one with the least volume after extension
        while (region < 4 && this->arr[region].ptr != NULL)
        {
            int diff_x;
            int diff_y;
            int diff_z;

            if (crd[0] < this->arr[region].mbr_coords[0])   //x_p < x_low
                diff_x = diff4words(crd[0],this->arr[region].mbr_coords[0]);
            else if (crd[0] > this->arr[region].mbr_coords[1])   //x_p > x_high
                diff_x = diff4words(crd[0],this->arr[region].mbr_coords[1]);
            else diff_x = diff4words(this->arr[region].mbr_coords[0], this->arr[region].mbr_coords[1]);

            if (crd[1] < this->arr[region].mbr_coords[2]) // y_p<y_low 
                diff_y = diff4words(crd[1],this->arr[region].mbr_coords[2]);
            else if (crd[1] > this->arr[region].mbr_coords[3]) // y_p >y_high
                diff_y = diff4words(this->arr[region].mbr_coords[3],crd[1]);
            else diff_y = diff4words(this->arr[region].mbr_coords[2],this->arr[region].mbr_coords[3]);

            int z_p = stoi(crd[2]);
            int z_low = stoi(this->arr[region].mbr_coords[4]);
            int z_high = stoi(this->arr[region].mbr_coords[5]);

            if (z_p < z_low)  
                diff_z = z_low - z_p;
            else if (z_p > z_high) 
                diff_z = z_p- z_high;
            else diff_z = z_high - z_low;

            volume[region] = diff_x * diff_y * diff_z;
            region++;
        }

        int min = 0;
        region = 0;
        for (region=1; region<4; region++)
        {
            if (volume[region] != -1 && volume[region] < volume[min])
                min = region;
        }

        return min;
    }

    bool contains(node& n, int mode)
    {
        if (mode == 1)  //contains point
        {
            for (int i = 0; i < 4; i++)
            {
                if (arr[i].filepath == n.filepath)
                    return true;
            }
        }
        else //contains MBR
        {
            for (int i = 0; i < 4; i++)
            {
                if (arr[i].ptr == n.ptr)
                    return true;
            }
        }

        return false;
    }
};

void node::makeMBR()
{
    std::vector<std::string> all_coords;
    int l = 0;

    //case 1: the node's child is a leaf-group
    if (this->ptr->arr[l].ptr == NULL)
    {
        while (!(this->ptr->arr[l].filepath.empty()) && l<4)
        {
            for (int j = 0; j < k; j++)
                all_coords.push_back(this->ptr->arr[l].mbr_coords[j]);
            l++;
        }

        mbr_coords[0] = all_coords[0];
        mbr_coords[1] = all_coords[0];
        mbr_coords[2] = all_coords[1];
        mbr_coords[3] = all_coords[1];
        mbr_coords[4] = all_coords[2];
        mbr_coords[5] = all_coords[2];

        for (int i = 0; i < all_coords.size(); i++)
        {
            if (i % 3 == 0 && all_coords[i] < mbr_coords[0])
                this->mbr_coords[0] = all_coords[i];
            else if (i % 3 == 0 && all_coords[i] > mbr_coords[1])
                this->mbr_coords[1] = all_coords[i];
            else if (i % 3 == 1 && all_coords[i] < mbr_coords[2])
                this->mbr_coords[2] = all_coords[i];
            else if (i % 3 == 1 && all_coords[i] > mbr_coords[3])
                this->mbr_coords[3] = all_coords[i];
            else if (i % 3 == 2 && all_coords[i] < mbr_coords[4])
                this->mbr_coords[4] = all_coords[i];
            else if (i % 3 == 2 && all_coords[i] > mbr_coords[5])
                this->mbr_coords[5] = all_coords[i];
        }
    }
    else
    {
        while (!(this->ptr->arr[l].ptr == NULL) && l<4)
        {
            for (int j = 0; j < 6; j++)
                all_coords.push_back(this->ptr->arr[l].mbr_coords[j]);
            l++;
        }

        //assume xlow, xhigh, ylow, yhigh, zlow, zhigh of region is the same as the first mbr
        mbr_coords[0] = all_coords[0];
        mbr_coords[1] = all_coords[1];
        mbr_coords[2] = all_coords[2];
        mbr_coords[3] = all_coords[3];
        mbr_coords[4] = all_coords[4];
        mbr_coords[5] = all_coords[5];

        for (int i = 6; i < all_coords.size(); i++)
        {
            if (i % 6 == 0 && all_coords[i] < mbr_coords[0])
                mbr_coords[0] = all_coords[i];
            else if (i % 6 == 1 && all_coords[i] > mbr_coords[1])
                mbr_coords[1] = all_coords[i];
            else if (i % 6 == 2 && all_coords[i] < mbr_coords[2])
                mbr_coords[2] = all_coords[i];
            else if (i % 6 == 3 && all_coords[i] > mbr_coords[3])
                mbr_coords[3] = all_coords[i];
            else if (i % 6 == 4 && all_coords[i] < mbr_coords[4])
                mbr_coords[4] = all_coords[i];
            else if (i % 6 == 5 && all_coords[i] > mbr_coords[5])
                mbr_coords[5] = all_coords[i];

        }
    }
    

}

class Rtree
{
private:
    group_of_nodes* root;
public:
    Rtree(group_of_nodes* first)
    {
        root = first;
    }
    void splitrecur(group_of_nodes& to_spl, node& to_ins, int count)
    {
        //phase 1: fill a temp vector with all the nodes u want to group
        int gi = 0;
        std::vector<node> temp;

        for (int i = 0; i < 4; i++)
        {
            temp.push_back(to_spl.arr[i]);
        }
        temp.push_back(to_ins);

        //sort the nodes based on x,y,z coordinate
        std::sort(temp.begin(), temp.end());        
        
        int team = 0;
        //phase 2: decide how the group is going to split
        if (count == 1)
            team = to_spl.groupnodes(temp);
        else
            team = to_spl.groupMBRs(temp);
        
        //phase 3: create two new groups of nodes
        group_of_nodes* group1 = new group_of_nodes;
        group_of_nodes* group2 = new group_of_nodes;

        //if we need to split at root 
        if (&to_spl == root)
        {
            if (team == 12)
            {
                for (int i = 0; i < 3; i++)
                {
                    group1->arr[i] = temp[i];
                }
                for (int i = 0; i < 2; i++)
                {
                    group2->arr[i] = temp[i+3];
                }
            }
            else if (team == 123)
            {
                for (int i = 0; i < 3; i++)
                {
                    group1->arr[i] = temp[i + 1];
                }
                group2->arr[0] = to_spl.arr[0];
                group2->arr[1] = to_spl.arr[3];
            }
            else if (team == 234)
            {
                for (int i = 0; i < 3; i++)
                {
                    group1->arr[i] = temp[i + 2];
                }
                for (int i = 0; i < 2; i++)
                {
                    group2->arr[i] = temp[i];
                }
            }
            
            //phase 4: update root and make it parent of the new groups
            group_of_nodes* new_root = new group_of_nodes;
            new_root->arr[0].ptr = group1;
            new_root->arr[1].ptr = group2;
            new_root->arr[0].makeMBR();
            new_root->arr[1].makeMBR();
            *root = *new_root;
            root->arr[0].in_group = root;
            root->arr[1].in_group = root;
            group1->parent = &(root->arr[0]);
            group2->parent = &(root->arr[1]);
             
            //state that group1 and group2 belong to groups 
            int g1 = 0;
            int g2 = 0;
            while (g1<4 && (!group1->arr[g1].filepath.empty()))
            {
                group1->arr[g1].in_group = group1->parent->ptr;
                g1++;
            }
            while (g2 < 4 && (!group2->arr[g2].filepath.empty()))
            {
                group2->arr[g2].in_group = group2->parent->ptr;
                g2++;
            }
            
            new_root = NULL;
            delete new_root;
        }
        else
        {
            node* new_reg = new node;
            
            if (team == 12)
            {
                for (int i = 0; i < 3; i++)
                {
                    group1->arr[i] = temp[i];
                }
                for (int i = 0; i < 2; i++)
                {
                    group2->arr[i] = temp[i + 3];
                }

                //group2 contains to_ins
                if (group2->contains(to_ins,count))
                {
                    //phase 4: preserve parent for the group that does not contain to_ins
                    group1->parent = to_spl.parent;
                    group1->parent->filepath = "";
                    group1->parent->ptr = group1;
                    group1->parent->makeMBR();

                    //phase 5: create parent for second group (the one that contains to ins) 
                    new_reg->ptr = group2;
                    group2->parent = new_reg;
                    new_reg->makeMBR();

                    //insert the new parent in insertion_group->parent (insertion_node = group1+group1-to_ins)
                    for (gi = 0; gi < 4; gi++)
                    {
                        if (to_spl.parent->in_group->arr[gi].ptr == NULL)
                        {
                            new_reg->in_group = to_spl.parent->in_group;
                            to_spl.parent->in_group->arr[gi] = *new_reg;
                            group2->parent = &(to_spl.parent->in_group->arr[gi]);
                            new_reg = NULL;
                            delete new_reg;
                            break;
                        }
                    }
                }
                else
                {
                    //phase 4: preserve parent for the group that does not contain to_ins
                    group2->parent = to_spl.parent;
                    group2->parent->filepath = "";
                    group2->parent->ptr = group2;
                    group2->parent->makeMBR();

                    //phase 5: create parent for second group (the one that contains to ins) 
                    new_reg->ptr = group1;
                    group1->parent = new_reg;
                    new_reg->makeMBR();

                    //insert the new parent in insertion_group->parent (insertion_node = group1+group1-to_ins)
                    for (gi = 0; gi < 4; gi++)
                    {
                        if (to_spl.parent->in_group->arr[gi].ptr == NULL)
                        {
                            new_reg->in_group = to_spl.parent->in_group;
                            to_spl.parent->in_group->arr[gi] = *new_reg;
                            group1->parent = &(to_spl.parent->in_group->arr[gi]);
                            new_reg = NULL;
                            delete new_reg;
                            break;
                        }
                }
                
                }

            }
            else if (team == 123)
            {
                for (int i = 0; i < 3; i++)
                {
                    group1->arr[i] = temp[i + 1];
                }
                group2->arr[0] = temp[0];
                group2->arr[1] = temp[4];

                //group1 contains to_ins
                if (group1->contains(to_ins, count))
                {
                    //phase 4: preserve parent for the group that does not contain to_ins
                    group2->parent = to_spl.parent;
                    group2->parent->filepath = "";
                    group2->parent->ptr = group2;
                    group2->parent->makeMBR();

                    //phase 5: create parent for the group that contains to ins
                    new_reg->ptr = group1;
                    group1->parent = new_reg;
                    new_reg->makeMBR();

                    //insert the new parent in insertion_group->parent (insertion_node = group1+group1-to_ins)
                    for (gi = 0; gi < 4; gi++)
                    {
                        if (to_spl.parent->in_group->arr[gi].ptr == NULL)
                        {
                            new_reg->in_group = to_spl.parent->in_group;
                            to_spl.parent->in_group->arr[gi] = *new_reg;
                            group1->parent = &(to_spl.parent->in_group->arr[gi]);
                            new_reg = NULL;
                            delete new_reg;
                            break;
                        }
                    }
                }
                else
                {
                    //phase 4: preserve parent for the group that does not contain to_ins
                    group1->parent = to_spl.parent;
                    group1->parent->filepath = "";
                    group1->parent->ptr = group1;
                    group1->parent->makeMBR();

                    //phase 5: create parent for the group that contains to ins
                    new_reg->ptr = group2;
                    group2->parent = new_reg;
                    new_reg->makeMBR();

                    //insert the new parent in insertion_group->parent (insertion_node = group1+group1-to_ins)
                    for (gi = 0; gi < 4; gi++)
                    {
                        if (to_spl.parent->in_group->arr[gi].ptr == NULL)
                        {
                            new_reg->in_group = to_spl.parent->in_group;
                            to_spl.parent->in_group->arr[gi] = *new_reg;
                            group2->parent = &(to_spl.parent->in_group->arr[gi]);
                            new_reg = NULL;
                            delete new_reg;
                            break;
                        }
                    }
                }
                
            }
            else if (team == 234)
            {
                for (int i = 0; i < 3; i++)
                {
                    group1->arr[i] = temp[i + 2];
                }
                for (int i = 0; i < 2; i++)
                {
                    group2->arr[i] = temp[i];
                }

                if (group2->contains(to_ins, count))
                {
                    //phase 4: preserve parent for the group that does not contain to_ins
                    group1->parent = to_spl.parent;
                    group1->parent->filepath = "";
                    group1->parent->ptr = group1;
                    group1->parent->makeMBR();

                    //phase 5: create parent for second group (the one that contains to ins) 
                    new_reg->ptr = group2;
                    group2->parent = new_reg;
                    new_reg->makeMBR();

                    //insert the new parent in insertion_group->parent (insertion_node = group1+group1-to_ins)
                    for (gi = 0; gi < 4; gi++)
                    {
                        if (to_spl.parent->in_group->arr[gi].ptr == NULL)
                        {
                            new_reg->in_group = to_spl.parent->in_group;
                            to_spl.parent->in_group->arr[gi] = *new_reg;
                            group2->parent = &(to_spl.parent->in_group->arr[gi]);
                            new_reg = NULL;
                            delete new_reg;
                            break;
                        }
                    }
                }
                else
                {
                    //phase 4: preserve parent for the group that does not contain to_ins
                    group2->parent = to_spl.parent;
                    group2->parent->filepath = "";
                    group2->parent->ptr = group2;
                    group2->parent->makeMBR();

                    //phase 5: create parent for second group (the one that contains to ins) 
                    new_reg->ptr = group1;
                    group1->parent = new_reg;
                    new_reg->makeMBR();

                    //insert the new parent in insertion_group->parent (insertion_node = group1+group1-to_ins)
                    for (gi = 0; gi < 4; gi++)
                    {
                        if (to_spl.parent->in_group->arr[gi].ptr == NULL)
                        {
                            new_reg->in_group = to_spl.parent->in_group;
                            to_spl.parent->in_group->arr[gi] = *new_reg;
                            group1->parent = &(to_spl.parent->in_group->arr[gi]);
                            new_reg = NULL;
                            delete new_reg;
                            break;
                        }
                    }
                }
                
                
            }
            
            //state the groups in which group1 and group2 belong to 
            int g1 = 0;
            int g2 = 0;
            while (g1 < 4 && !(group1->arr[g1].filepath.empty()))
            {
                group1->arr[g1].in_group = group1->parent->ptr;
                g1++;
            }
            while (g2 < 4 && !(group2->arr[g2].filepath.empty()))
            {
                group2->arr[g2].in_group = group2->parent->ptr;
                g2++;
            }
            
            if (gi == 4) splitrecur(*(to_spl.parent->in_group), *new_reg, count + 1);
            delete& to_spl;
        }



    }
    group_of_nodes* Access(group_of_nodes* r, node& to_ins, std::vector<std::string> crd, bool &flag)
    {
        int i;

        //checks if node is leaf 
        for (i = 0; i < 4; i++)
        {
            if (r->arr[i].ptr == NULL) continue;
            else break;
        }

        if (i == 4)  //terminal condition: leaf
        {
            //is there an empty spot for me?
            for (int pos = 0; pos < 4; pos++)
            {
                if (r->arr[pos].filepath.empty())
                {
                    return r;
                }
                else continue;
            }

            flag = true;

            //if the function exits this for, there is no empty spot for the file.
            //split the leaf and create new group that points at each of the new leaves.
            //criteria of splitiing: group near points together to minimize empty space within a region and the space a region covers

        }
        else     //non-leaf: compare the file-point's coordinates with the mbr of every node in this group
        {
            int pos;
            for ( pos = 0; pos < 4; pos++)
            {
                //rule: if a node belongs to more than 1 mbrs, it is given to the first mbr we encounter
                //checks whether a node belongs in a region or not, given that the region (node) is not null  
                std::vector<std::string> temp;
                for (int s = 0; s < 6; s++)
                    temp.push_back(r->arr[pos].mbr_coords[s]);
                if (!(r->arr[pos].ptr == NULL) && to_ins.belongs_to(crd, temp))
                {
                    r = r->arr[pos].ptr;
                    Access(r, to_ins, crd, flag);
                    break;
                }
            }

            //if node to_ins belongs to none of the above nodes, extend 1 of them according to some criteria
            //ctriterion used: minimum distance of node to insert from all the regions of the group
            //count the distance from all the coordinates of all the regions
            //sort the distances and pick the minimum to change
            //check if change of distance causes extend or shrink
            if (pos == 4)
            {
                int index = r->choose2extend(crd);

                if (crd[0] < r->arr[index].mbr_coords[0])   //x_p < x_low
                    r->arr[index].mbr_coords[0] = crd[0];
                else if (crd[0] > r->arr[index].mbr_coords[1])   //x_p > x_high
                    r->arr[index].mbr_coords[1] = crd[0];

                if (crd[1] < r->arr[index].mbr_coords[2])   //y_p < _low
                    r->arr[index].mbr_coords[2] = crd[1];
                else if (crd[1] > r->arr[index].mbr_coords[3])   //y_p > y_high
                    r->arr[index].mbr_coords[3] = crd[1];

                if (crd[2] < r->arr[index].mbr_coords[4])   //z_p < z_low
                    r->arr[index].mbr_coords[4] = crd[2];
                else if (crd[2] > r->arr[index].mbr_coords[5])   //z_p > z_high
                    r->arr[index].mbr_coords[5] = crd[2];

                r = r->arr[index].ptr;
                Access(r, to_ins, crd, flag);
            }
            
        }

        return r;
    }
    void Insert(std::string filepath)
    {
        bool full = false;      //declares if the leaf access finds is full or not
        node* to_ins = new node;
        to_ins->filepath = filepath;
        to_ins->get_coords();

        std::vector<std::string> coords;
        for (int i = 0; i < k; i++)
        {
             coords.push_back(to_ins->mbr_coords[i]);
        }
        
        group_of_nodes* insertion_group = Access(root, *to_ins, coords, full);

        if (full == false)
        {
            //there is an empty spot in insertion group
            int i;
            for (i = 0; i < 4; i++)
            {
                if (insertion_group->arr[i].filepath.empty())
                    break;
            }

            insertion_group->arr[i].filepath = to_ins->filepath;
            insertion_group->arr[i].in_group = insertion_group;
            for (int j = 0; j < k; j++)
            {
                insertion_group->arr[i].mbr_coords[j] = to_ins->mbr_coords[j];
            }
        }
        else
        {
            splitrecur(*insertion_group, *to_ins, 1);
        }
    }

    //for query: if leaf, your nodes have 3 coordinates, otherwise, your nodes have mbr

    std::vector<std::string> Search(group_of_nodes *cur, std::vector<std::string> query, std::vector<std::string> &answer)
    {
        //cur is a group of leaf-nodes
        if (!cur->arr[0].filepath.empty())
        {
            for (int i = 0; i < 4; i++)
            {
                std::vector<std::string> tempcoords;
                if (!(cur->arr[i].filepath.empty()))
                {
                    //pass coordinates to a vector
                    for (int j = 0; j < 3; j++)
                        tempcoords.push_back(cur->arr[i].mbr_coords[j]);
                        //if node in query region, add it to the answer
                        if (cur->arr[i].belongs_to(tempcoords, query))
                        {
                            answer.push_back(cur->arr[i].filepath);
                        }
                }
                
            }
        }
        else //internal group
        {
            //for each MBR in the group
            for (int i = 0; i < 4; i++)
            {
                std::vector<std::string> tempcoords;
                if (cur->arr[i].ptr != NULL)    //just in case there are less than 4 nodes in a group
                {
                    //pass MBR to a vector
                    for (int j = 0; j < 6; j++)
                        tempcoords.push_back(cur->arr[i].mbr_coords[j]);
                    //if MBR is fully contained in query region, all the leaves below it are contained in the query region
                    if (NR_in_QR(tempcoords, query))
                    {
                        reportAll_leaves(&cur->arr[i], answer);
                    }
                    else if (!NRQRforeign(tempcoords, query))
                    {
                        //if MBR is not fully contained in query region and is not foreign to it, then MBR and query region intersect
                        //search in the child of this MBR
                        Search(cur->arr[i].ptr, query, answer);
                    }
                }
               
            }
        }

        return answer;
    }

    void reportAll_leaves(node* r, std::vector < std::string > & report)
    {
        int i = 0;
        if (r->in_group== NULL) return;

        //reached a leaf
        if (r->ptr == NULL)
        {
            report.push_back(r->filepath);
        }
        else
        {
            //as long as we haven't reached a leaf-group
            for (i=0; i < 4; i++)
            {
                reportAll_leaves(&r->ptr->arr[i], report);
            }
        }         

    }

    void printTree(group_of_nodes* r)
    {
        std::cout << "I am group: " << r << std::endl;
        std::cout << "I contain: ";
        int i = 0;
        if (r->arr[i].ptr == NULL)
        {
            while (!r->arr[i].filepath.empty() && i < 4)
            {
                std::cout << r->arr[i].filepath << " ";
                i++;
            }
            std::cout << std::endl;
        }
        else
        {
            while (!(r->arr[i].ptr == NULL) && i < 4)
            {
                std::cout << "MBR " << i << ": ";
                for (int j = 0; j < 6; j++)
                    std::cout << r->arr[i].mbr_coords[j] << '\t';
                std::cout << std::endl << "My children are :" << std::endl;
                printTree(r->arr[i].ptr);
                i++;
            }
        }

    }
};

int main()
{
    std::vector<std::set<std::uint32_t>> sets_of_answer;
    //create answer vector
    std::vector<std::string> answer;

    group_of_nodes* root = new group_of_nodes;
    Rtree rtree = Rtree(root);

    const std::filesystem::path testfolder = "../Rtestfiles";
    
    auto startconstruction = std::chrono::steady_clock::now();
    int count = 0;
    for (const auto& entry : std::filesystem::directory_iterator(testfolder))
    {
        const std::string filenameStr = testfolder.string() + "/" + entry.path().filename().string();
        if (entry.is_regular_file())
        {
            count++;
            rtree.Insert(filenameStr);
        }
    }
    
    auto endconstruction = std::chrono::steady_clock::now();
    std::cout << "Elapsed time in microseconds for building RTree:" << std::chrono::duration_cast<std::chrono::microseconds>(endconstruction - startconstruction).count() <<
        "us" << std::endl;

    std::vector<std::string> queryR;
    std::string temp;
    queryR.reserve(6);
    std::cout << "Please insert coordinates for query region R: " << std::endl;
    std::cout << "Lessons Range: " << std::endl << "From: ";
    std::cin >> temp;
    queryR.push_back(temp);
    std::cout << std::endl << "To: ";
    std::cin >> temp;
    queryR.push_back(temp);
    std::cout << "Student's Full Name's Range " << std::endl << "From: ";
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
    rtree.Search(root, queryR, answer);
    auto endquery = std::chrono::steady_clock::now();

    std::cout << "Files found in Query Region:" << std::endl;
    if (answer.size() == 0)
        std::cout << "There are no files in Query Region" << std::endl;
    else
    {
        for (int i = 0; i < answer.size(); i++)
        {
            std::cout << answer[i] << std::endl;
        }
    }

    std::cout << "Elapsed time in microseconds for searching RTree:" << std::chrono::duration_cast<std::chrono::microseconds>(endquery - startquery).count() << "us" << std::endl;

    for (int i = 0; i < answer.size(); i++)
    {
        sets_of_answer.push_back(Murmurshingling(answer[i]));
    }

    Eigen::Matrix<uint32_t, 100, Eigen::Dynamic> signatures = makeSignatures(sets_of_answer);

    //Candidatepairs.txt has every doc with similarity
    std::ofstream candidatepairs("candidatepairs.txt");

    std::vector<std::unique_ptr<htable>> vhf;
    vhf = hashbands(signatures);

    candidatepairs << "THESE ARE DOCUMENTS THAT NEED TO BE CHECKED FOR PLAGIARISM !! \n\n\t\t\t\t\tRTree+LSH\n";
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
    

   }
