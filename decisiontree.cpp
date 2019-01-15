/*
 * ************************************************
 *  Filename:     decisiontree.cpp
 *
 *  Description:  Node Class for Decision Tree
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

#include "decisiontree.h"
//#include "node.h"
//

float Decisiontree::gini_impurity(std::vector<int> data_indices) const{
  /* 
   * calculate the gini impurity I_G for binary classes +/- as 
   * I_G = p_+(1-p_+) + p_-(1-p_-) = 2*p_+(1-p_+) 
   * This gives the probability for every item of a group being incorrectly labeled when
   * randomly matched with a label of that group
   *                                                              */
  int positive_data = 0;                         //count the + labels on the left
  int ntotal_data = data_indices.size();
  std::vector<int>::iterator 
  double p_positives = 0;
  for (int d=0; d<ntotal_data; d++){
    if (data[d].label == label)
      positive_data +=1;
  p_positives = (double)positive_data/ntotal_data;
  return 2*p_positives*(1-p_positives);
  }
}

void Decisiontree::max_information_gain(std::vector<int> data_indices){
  /*
   * Iterates over all possible features and thresholds and maximizes 
   * to find max(I_G,parent - p_left * I_G,left - p_right * I_G,right)
   * with p_i = numdata_i/numdata_parent
   *                                          */
  float information_gain = 0;
  float best_information_gain = 0;
  float impurity_parent = gini_imp;
  float impurity_left;
  float impurity_right;
  int ntotal_data;
  float p_left;
  float p_right;
  int num_features;
  char best_feature;
  float best_threshold = 0;
  std::string best_flag = "none";                       //TODO: decide between char and string for cat and flag
  char num_or_cat = 'n';                                //best split for numerical or categorical feature?
  float threshold;
  int num_thresholds;
  std::set<float> this_features_values;
  std::set<std::string> this_features_categories;


  //TODO: get the following from dataset
  std::vector<std::set<float>> possible_values;           //all possible values for each numerical feature
  std::vector<std::set<std::string>> possible_categories; //all possible values for each categorical feature 


  std::vector<Dataset> left_data;
  std::vector<Dataset> right_data;
  
  // test all numerical thresholds
  num_features = data[0].num_features.size();
  for (int ifeatures=0; ifeatures<num_features; ifeatures++){
    this_features_values = possible_values[ifeatures];
    num_thresholds = this_features_values.size();
    for (std::set<float>::iterator ithresholds=this_features_values.begin(); ithresholds!=this_features_values.end(); ithresholds++){
      threshold = *ithresholds;
      left_data.clear();
      right_data.clear();
      for (int idata=0; idata<ntotal_data; idata++){
        Dataset thisdata=data[idata];
        if (thisdata.num_features[ifeatures] <= threshold)
          left_data.push_back(thisdata);
        else
          right_data.push_back(thisdata);
      } // data iteration
      p_left = left_data.size()/ntotal_data;
      p_right = right_data.size()/ntotal_data;
      information_gain = impurity_parent - p_left*gini_impurity(left_data) - p_right*gini_impurity(right_data);
      if(information_gain > best_information_gain){
        best_information_gain = information_gain;
        best_feature = ifeatures;
        best_threshold = *ithresholds;
        num_or_cat = 'n';
      } // test for best gain
    } // threshold iteration
  } // feature iteration

  // test all category flags
  num_features = data[0].cat_features.size();
  for (int ifeatures=0; ifeatures<num_features; ifeatures++){
    this_features_categories = possible_categories[ifeatures];
    num_thresholds = this_features_categories.size();
    for (std::set<std::string>::iterator ithresholds=this_features_categories.begin(); ithresholds!=this_features_categories.end(); ithresholds++){
      std::string flag = *ithresholds;
      left_data.clear();
      right_data.clear();
      for (int idata=0; idata<ntotal_data; idata++){
        Dataset thisdata=data[idata];
        if (thisdata.cat_features[ifeatures] == flag[0])      //TODO: decide between string and char!
          left_data.push_back(thisdata);
        else
          right_data.push_back(thisdata);
      } // data iteration
      information_gain = impurity_parent - gini_impurity(left_data) - gini_impurity(right_data);
      if(information_gain > best_information_gain){
        best_information_gain = information_gain;
        best_feature = ifeatures;
        best_flag = *ithresholds;
        num_or_cat = 'c';
      } // test for best gain
    } // threshold iteration
  } // feature iteration
  //store the best split in this node
  sep_feature_type = num_or_cat;
  sep_feature_index = best_feature;
  if(sep_feature_type == 'n'){
    sep_threshold = best_threshold;
  } // numerical threshold
  else if(sep_feature_type == 'c'){
    sep_category_flag = best_flag[0];
  } // category flag
}

void Decisiontree::train(std::vector<Dataset> data_, float min_gini){
  gini_imp = gini_impurity(data_);
  int ndata = data.size();
  std::vector<int> left_data;
  std::vector<int> right_data;
  while(gini_imp > min_gini){
    max_information_gain(data);
    //split data (for readability in own function?)
    if(sep_feature_type == 'n'){
      for (int idata=0; idata<ndata; idata++){
        Dataset thisdata=data[idata];
        if (thisdata.num_features[sep_feature_index] <= sep_threshold)      //TODO: decide between string and char!
          left_data.push_back(idata);
        else
          right_data.push_back(idata);
      } // iterate over data
    } // seperate by numeric
    else if(sep_feature_type == 'c'){
      for (int idata=0; idata<ndata; idata++){
        Dataset thisdata=data[idata];
        if (thisdata.cat_features[sep_feature_index] ==sep_category_flag)      
          left_data.push_back(idata);
        else
          right_data.push_back(idata);
      } // iterate over data
    } // seperate by category
    leftchild = &Decisiontree(depth+1, left_data);
    //leftchild->train(data_, min_gini);
    rightchild = &Decisiontree(depth+1, right_data);
    //rightchild->train(data_,min_gini);
  } // while gini_imp>min_gini

};