/*
 * ************************************************
 *  Filename:     decisiontree.h
 *
 *  Description:  Class for Decision Tree
 *
 *  Version:      1.0
 *  Created:      11.12.2019
 *  Revision:     None
 *  Compile:      Makefile, g++
 *
 *  Author:       Frederike Kubandt, Maximilian Märcz
 *  Organization: Goehte University Frankfurt
 * ************************************************* 
*/

#include <string>
#include <set>
#include <vector>

#include "data_class.h"

class Decisiontree
{
    public:
    std::vector<int> dataslice;
     std::vector<Dataset> data;  
    //separation information:
    Decisiontree* leftchild;
    Decisiontree* rightchild;
    char sep_feature_type;
    int sep_feature_index;
    char sep_category_flag = '0';
    float sep_threshold = 0.;
    //prediction information, for leaves only
    float certainty = 0.;
    char prediction = '0';
    float gini_imp;
    //
    public:
    int depth = 0;
    bool is_leaf = false;
    const char label = '+'; 
    //
    // Member Functions 
    Decisiontree(int depth, std::vector<int> dataslice, std::vector<Dataset> data);
    Decisiontree(std::vector<Dataset> data);
    Decisiontree();
    ~Decisiontree();   
    float gini_impurity(std::vector<int> data_indices) const;
    void max_information_gain(std::vector<int> data_indices);
    void train(std::vector<int> data_indices, float min_gini);
    void train(std::vector<int> data_indices, int min_instances_at_leaf);
    //
    char predict(const Dataset &data);
    char predict(const Dataset &data, float &certainty_);
    bool is_in_left_child(const Dataset &data);

    //void split_dataset(float* dataset);       //make more readable?                                               
    //TODO: void save(std::string filename) const;
    //TODO: int load(std::string filename, float* dataset);
};
