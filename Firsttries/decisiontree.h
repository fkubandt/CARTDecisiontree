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

#include "node.h"

class Decisiontree
{
    float gini_impurity(float* dataset, const int num_features, const int num_data) const;
    void split_dataset(float* dataset);       
                                              
    void evaluate_all_splits(float* dataset, const int num_features, const int num_data);
                                              
    void build_tree(float* dataset, const int num_features, const int num_data, const int max_depth, 
                    const int min_features); 
                                             
    char predict(float* dataset, const int num_features, const int num_data);
                                              
    void save(std::string filename) const;
    int load(std::string filename, float* dataset);
};
