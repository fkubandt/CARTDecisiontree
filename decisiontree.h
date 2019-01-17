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
#include <fstream>
#include <iostream>

#include "data_class.h"

class Decisiontree
{
    public:
    std::vector<int> dataslice;
     std::vector<Dataset> data;  
    //separation information:
    Decisiontree* leftchild;
    Decisiontree* rightchild;
    char sep_feature_type='x';
    int sep_feature_index=-1;
    char sep_category_flag = 'x';
    float sep_threshold = 0.;
    //prediction information, for leaves only
    float certainty = 0.;
    char prediction = 'x';
    float gini_imp = -1;
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
    template<typename T> void train(std::vector<int> data_indices, T exit_condition);
    //
    char predict(const Dataset &data);
    char predict(const Dataset &data, float &certainty_);
    bool is_in_left_child(const Dataset &data);

    void save(std::string filename);
    void save_to_file(std::ofstream &file);
    //for testing
    float gini_impurity_of_all_leaves();

    //void split_dataset(float* dataset);       //make more readable?                                               
    //TODO: void save(std::string filename) const;
    //TODO: int load(std::string filename, float* dataset);
};

/*
 * definition */

template<typename T> //TODO: this contains a not-ending loop --> exit statements aren't updated yet
void Decisiontree::train(std::vector<int> data_indices, T exit_condition){
  std::cout <<std::endl << "start training on depth " << depth << " ....." << std::endl;
  gini_imp = gini_impurity(data_indices);
  bool exit_statement;
  //
  if(typeid(exit_condition) == typeid(1)){
    exit_statement = (data_indices.size() <= exit_condition or gini_imp == 0);
  }
  else if(typeid(T) == typeid(1.)){
    exit_statement = (gini_imp <= exit_condition or gini_imp == 0);
  }
  else  {
    std::cout << "no valid exit condition given. Assume minimum 5 datasets at leaf node." << std::endl;
    exit_statement = (data_indices.size() > 5 or gini_imp == 0);
  }

  //int ndata = data.size();
  std::vector<int> left_data;
  std::vector<int> right_data;
  if(exit_statement){
    std::cout << "training done" << std::endl;
    is_leaf = true;
    int ntrues = 0;
    //count '+' labels
    for (auto idata : data_indices){
      if(data[idata].label == label)
        ntrues += 1;
    } //data iteration
    certainty = (float)ntrues/(float)data_indices.size();
    if (certainty <= 0.5){
      prediction = '-';
      certainty = 1- certainty;
    } //if neg prediction
    else{
      prediction = '+';
    }// if pos prediction
  }// if exit condition finish training
  else{
    max_information_gain(data_indices);
    //split data (for readability in own function?)
    if(sep_feature_type == 'n'){
      for (auto idata : data_indices){
        Dataset thisdata=data[idata];
        if (thisdata.num_features[sep_feature_index] <= sep_threshold)      //TODO: decide between string and char!
          left_data.push_back(idata);
        else
          right_data.push_back(idata);
      } // iterate over data
    } // seperate by numeric
    else if(sep_feature_type == 'c'){
      for (auto idata : data_indices){
        Dataset thisdata=data[idata];
        if (thisdata.cat_features[sep_feature_index] ==sep_category_flag)      
          left_data.push_back(idata);
        else
          right_data.push_back(idata);
      } // iterate over data
    } // seperate by category
    leftchild = new Decisiontree(depth+1, left_data, data);
    leftchild->train(left_data, exit_condition);
    rightchild = new Decisiontree(depth+1, right_data, data);
    rightchild->train(right_data, exit_condition);
  } // else training
}; //train