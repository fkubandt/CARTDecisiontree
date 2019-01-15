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
#include "data_class.h"

class Decisiontree
{
    private:
    const std::vector<int> dataslice;
    static std::vector<Dataset> data;  //TODO: for recursive functions, we do not pass data but the set of indices in dataslice -> transform functions?
    //separation information:
    const Decisiontree* leftchild;
    const Decisiontree* rightchild;
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
    Decisiontree(int depth, std::vector<int> dataslice);
    ~Decisiontree();   
    float gini_impurity(std::vector<int> data_indices) const;
    void max_information_gain(std::vector<int> data_indices);
    void train(std::vector<Dataset> data, float min_gini);
    void train(std::vector<Dataset> data, int min_data_at_leaf);
    //
    bool predict(Dataset data);
    bool predict(Dataset data, float &certainty_);


    //void split_dataset(float* dataset);       //make more readable?                                               
    //TODO: void save(std::string filename) const;
    //TODO: int load(std::string filename, float* dataset);
};
