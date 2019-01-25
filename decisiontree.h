/*
 * *************************************************************
 *  file_name:     decisiontree.h
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
 * *************************************************************
*/
#ifndef _DECISIONTREE_H_  
#define _DECISIONTREE_H_  
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include "data_class.h"

extern bool testing;

class Decisiontree
{
    private:
    std::vector<int> dataslice{};
    //separation information:
    Decisiontree* leftchild{nullptr};
    Decisiontree* rightchild{nullptr};
    char sep_feature_type='x';
    int sep_feature_index=-1;
    std::string sep_category_flag = "x";
    float sep_threshold = 0.;
    //prediction information, for leaves only
    float certainty = 0.;
    char prediction = 'x';
    float gini_imp = -1;
    int depth = 0;
    bool is_leaf = false;
    const char label = '+'; 
    static int n_nodes;
    int node_index = -1;

    static std::vector<Datapoint> data;  
//
    public:
    //Member Functions 
      //constructors
    Decisiontree(int depth, std::vector<int> dataslice);
    Decisiontree(std::vector<Datapoint> data, std::vector<int> dataslice_);
    explicit Decisiontree() = default;
    ~Decisiontree(); 
      //training
    public:
    template<typename T> void train(std::vector<int> data_indices, T exit_condition=0.);
    template<typename T> void train(T exit_condition=0.);

    private:
    float gini_impurity(std::vector<int> data_indices) const;
    float max_information_gain(std::vector<int> data_indices);
    void create_leaf(std::vector<int> data_indices);
    void split_data_num(std::vector<int> data_indices, std::vector<int> &left_data, std::vector<int> &right_data, int sep_ft_index, float threshold);
    void split_data_cat(std::vector<int> data_indices, std::vector<int> &left_data, std::vector<int> &right_data, int sep_ft_index, std::string flag);

      //prediction
    private:
    bool is_in_left_child(const Datapoint &data);
    bool handle_missing_data(const Datapoint &data);
    public:
    char predict(const Datapoint &data);
    char predict(const Datapoint &data, float &certainty_);

      //data im/export
    public:
    void save(std::string file_name);
    void save_for_visualisation(std::string file_name);
    void load_from_file(std::string file_name_tree, std::string file_name_data);  //no real dataslice, just the size fits!!!
    private:
    void save_to_file(std::ofstream &file);
    void save_node_for_visualisation(std::ofstream &file, int parent_id);
    void load_node_from_file(std::ifstream &file, int &counter);
    void set_node(std::ifstream &file);

    //for testing
    public:
    float gini_impurity_of_all_leaves();
    float test(std::vector<int> &testdata, const int leafsize);

};


/* ************************************************************* *
 * ************************************************************* *
 *           DEFINITION OF TEMPLATE MEMBER FUNCTIONS             *
 * ************************************************************* *
 * ************************************************************* */


/* ************************************************************* *
 *                             train                             *
 * ************************************************************* */
template<typename T>
void Decisiontree::train(T exit_condition){
  train(dataslice, exit_condition);
}
template<typename T> 
void Decisiontree::train(std::vector<int> data_indices, T exit_condition){
  /* 
   * builds the tree, seperating criterion is the information gain for a split. 
   *                                                                           */
  if (testing)
    std::cout <<std::endl << "start training on depth " << depth << " ....." << std::endl;
  gini_imp = gini_impurity(data_indices);
  bool create_children;
  float max_inf_gain;
  //
  std::vector<int> left_data;
  std::vector<int> right_data;
  //
  max_inf_gain = max_information_gain(data_indices);
  if (max_inf_gain == 0){
    create_leaf(data_indices);
    return;
  }

  if(sep_feature_type == 'n')
    split_data_num(data_indices, left_data, right_data, sep_feature_index, sep_threshold);
  else if(sep_feature_type == 'c')
    split_data_cat(data_indices, left_data, right_data, sep_feature_index, sep_category_flag);
  if (testing)
    std::cout << "sep_threshold " << sep_threshold << std::endl;
  if(typeid(exit_condition) == typeid(1)){
    create_children = ((left_data.size() >= exit_condition) and (right_data.size() >= exit_condition));
    if (testing){
      std::cout << data_indices.size() << " " << gini_imp << std::endl;
      std::cout << "create children " << create_children << std::endl;}
  }
  else if(typeid(exit_condition) == typeid(1.)){ //TODO
    create_children = max_inf_gain >= exit_condition;
    if (testing)
      std::cout << "*******************************\n";
  }
  else  {
    std::cout << "no valid exit condition given. Assume minimum 5 Datapoints at leaf node." << std::endl;
    //exit_statement = (data_indices.size() > 5 or gini_imp == 0);
  }
  if(create_children){
      leftchild = new Decisiontree(depth+1, left_data);
      leftchild->train(exit_condition);
      rightchild = new Decisiontree(depth+1, right_data);
      rightchild->train(exit_condition);
  }
  else{
    create_leaf(data_indices);
  }
};//train

#endif