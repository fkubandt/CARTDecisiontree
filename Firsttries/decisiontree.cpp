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

const char label = "+";

Decisiontree::max_information_gain(vector<dataset> data, vector<vector<char>> possible_categories, vector<vector<double>> possible_values){
  /*
   * We have just binary classes, +/- The gini impurity therefore reduces to
   * I_G = p_+(1-p_+) + p_-(1-p_-) = 2*p_+(1-p_+) 
   * This gives the probability for every item of a group being incorrectly labeled when
   * randomly matched with a label of that group
   *
   * Splitting Criterion: max(I_G, parent - p_left * I_G,left - p_right * I_G,right)
   * with p_left = numdata_left/numdata
   *                                          */
  float information_gain = 0;
  float best_information_gain = 0;
  float impurity_parent = gini_impurity(data) ;
  float impurity_left;
  float impurity_right;
  int total_data
  float ratio_left;
  int num_features;
  int best_feature;
  int best_threshold;
  char num_or_cat = "num";                                //best split for numerical or categorical feature?

  vector<dataset> left_data;
  vector<dataset> right_data;
  // test all numerical thresholds
  num_features = data[0].num_features.size();
  for (int ifeatures=0; ifeatures<num_features; ifeatures++){
    int num_thresholds = possible_values[ifeatures].size();
    for (int ithresholds=0; ithresholds<num_thresholds, ithresholds++){
      double threshold = possible_values[ifeatures][ithresholds];
      left_data.clear();
      right_data.clear();
      for (int idata=0; idata<num_data; idata++){
        dataset thisdata=data[idata]
        if (thisdata.num_features(ifeatures) <= threshold)
          left_data.pushback(thisdata);
        else
          right_data.pushback(thisdata);
      } // data iteration
      information_gain = impurity_parent - gini_impurity(left_data) - gini_impurity(right_data);
      if(information_gain > best_information_gain){
        best_information_gain = information_gain;
        best_feature = ifeatures;
        best_threshold = ithresholds;
        char num_or_cat = "num";
      } // test for best gain
    } // threshold iteration
  } // feature iteration

  // test all category flags
  num_features = data[0].cat_features.size();
  for (int ifeatures=0; ifeatures<num_features; ifeatures++){
    int num_thresholds = possible_categories[ifeatures].size();
    for (int ithresholds=0; ithresholds<num_thresholds, ithresholds++){
      char flag = possible_categories[ifeatures][ithresholds];
      left_data.clear();
      right_data.clear();
      for (int idata=0; idata<num_data; idata++){
        dataset thisdata=data[idata]
        if (thisdata.cat_features(ifeatures) == flag)
          left_data.pushback(thisdata);
        else
          right_data.pushback(thisdata);
      } // data iteration
      information_gain = impurity_parent - gini_impurity(left_data) - gini_impurity(right_data);
      if(information_gain > best_information_gain){
        best_information_gain = information_gain;
        best_feature = ifeatures;
        best_threshold = ithresholds;
        char num_or_cat = "cat";
      } // test for best gain
    } // threshold iteration
  } // feature iteration

}


Decisiontree::information_gain(float impurity_left, float impurity_right, float impurity_parent){
  return impurity_parent - impurity_left - impurity_right
}


Decisiontree::gini_impurity(vector<dataset> data){
  int positive_data = 0;                         //count the + labels on the left
  int total_data = data.size();
  double p_positives = 0;
  for (int d=0; d<total_count; d++){
    if (data[d].label == flag)
      positive_data +=1;
  p_positives = (double)positive_data/total_data;
  return 2*p_positives*(1-p_positives)
  }
}
