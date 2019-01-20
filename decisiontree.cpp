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
extern bool testing;
//


Decisiontree::Decisiontree(int depth, std::vector<int> dataslice, std::vector<Dataset> data):depth(depth), dataslice(dataslice), data(data){};


Decisiontree::Decisiontree(std::vector<Dataset> data_){
  depth = 0;
  // std::vector<Dataset> this.data = data_;
  this->data = data_;
  // dataslice should include all indices in data
  dataslice.reserve(data.size());
  for(int i=0; i<data.size();i++){
    dataslice.push_back(i);
  }
}

  float Decisiontree::gini_impurity(std::vector<int> data_indices) const{
  /* 
   * calculate the gini impurity I_G for binary classes +/- as 
   * I_G = p_+(1-p_+) + p_-(1-p_-) = 2*p_+(1-p_+) 
   * This gives the probability for every item of a group being incorrectly labeled when
   * randomly matched with a label of that group
   *                                                              */
  if (testing)
    std::cout << "calculating gini impurity ....." ;
  int positive_data = 0;                         //count the + labels on the left
  int ntotal_data = data_indices.size();
  double p_positives = 0.;
  for (auto idata : data_indices){
    if (data[idata].label == label)
      positive_data +=1;
  }
  p_positives = (double)positive_data/ntotal_data;
  if (testing)
    std::cout << 2*p_positives*(1-p_positives) << " for " << data_indices.size() << std::endl;
  return 2*p_positives*(1-p_positives);
}


  void Decisiontree::max_information_gain(std::vector<int> data_indices){
  /*
   * Iterates over all possible features and thresholds and maximizes 
   * to find max(I_G,parent - p_left * I_G,left - p_right * I_G,right)
   * with p_i = numdata_i/numdata_parent
   *                                          */
  if (testing)
    std::cout << "maximize the information gain ....." << std::endl;
  float information_gain = 0;
  float best_information_gain = 0;
  float impurity_parent = gini_imp;
  float impurity_left;
  float impurity_right;
  int ntotal_data = data_indices.size();
  float p_left;
  float p_right;
  int nfeatures;
  int best_feature;
  float best_threshold = 0;
  std::string best_flag = "none";                       //TODO: decide between char and string for cat and flag
  char num_or_cat = 'n';                                //best split for numerical or categorical feature?
  float threshold;
  int num_thresholds;
  std::set<float> this_features_values;
  std::set<std::string> this_features_categories;


  //TODO: get the following from dataset
  std::vector<std::set<float>> possible_values = Dataset::numerical_sets;           //all possible values for each numerical feature
  std::vector<std::set<std::string>> possible_categories = Dataset::feature_sets; //all possible values for each categorical feature 


  std::vector<int> left_data;
  std::vector<int> right_data;
  
  // test all numerical thresholds
  nfeatures = data[0].num_features.size();
  if (testing)
    std::cout << "looking for numerical separation" << data[0].num_features.size() << std::endl;
  for (int ifeatures=0; ifeatures<nfeatures; ifeatures++){
    this_features_values = possible_values[ifeatures];
    for(auto ithresholds : this_features_values){
      threshold = ithresholds;
      left_data.clear();
      right_data.clear();
      for (auto idata : data_indices){
        Dataset thisdata=data[idata];
        if (thisdata.num_features[ifeatures] <= threshold){
          left_data.push_back(idata);
      }
        else{
          right_data.push_back(idata);
        }
      } // data iteration
      p_left = (double)left_data.size()/ntotal_data;
      p_right = (double)right_data.size()/ntotal_data;
      if(left_data.size() != 0 and right_data.size() != 0){
        information_gain = impurity_parent - p_left*gini_impurity(left_data) - p_right*gini_impurity(right_data);
        if (testing){
        std::cout << "information gain " << information_gain << " for gain = " << impurity_parent
        << "-" << p_left << "*" << gini_impurity(left_data) << " - " << p_right << "*" << gini_impurity(right_data) << std::endl
        << "nleft = " << left_data.size() << std::endl;}
        if(information_gain > best_information_gain){
         best_information_gain = information_gain;
         best_feature = ifeatures;
         best_threshold = ithresholds;
         num_or_cat = 'n';
       } // test for best gain
      }
    } // threshold iteration
  } // feature iteration

  // test all category flags
  nfeatures = data[0].cat_features.size();
  if (testing)
    std::cout << "looking for categorical separation" << std::endl;
  for (int ifeatures=0; ifeatures<nfeatures; ifeatures++){
    this_features_categories = possible_categories[ifeatures];
    num_thresholds = this_features_categories.size();
    for(auto ithresholds : this_features_categories){
      std::string flag = ithresholds;
      left_data.clear();
      right_data.clear();
      for (auto idata : data_indices){
        Dataset thisdata=data[idata];
        if (thisdata.cat_features[ifeatures] == flag[0])      //TODO: decide between string and char!
          left_data.push_back(idata);
        else
          right_data.push_back(idata);
      } // data iteration
      if(left_data.size() != 0 and right_data.size() != 0){
        information_gain = impurity_parent - gini_impurity(left_data) - gini_impurity(right_data);
        if (testing)
          std::cout << "information gain " << information_gain << std::endl;
        if(information_gain > best_information_gain){
          best_information_gain = information_gain;
         best_feature = ifeatures;
          best_flag = ithresholds;
          num_or_cat = 'c';
       } // test for best gain
      }
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
  if (testing)
    std::cout << "information gain maximized to ....." << best_information_gain << std::endl; 
}//max information gain

char Decisiontree::predict(const Dataset &data){
  if (is_leaf){
    return prediction;
  }
  else{
    if (is_in_left_child(data))
      leftchild->predict(data);
    else
      rightchild->predict(data);
  }
}

char Decisiontree::predict(const Dataset &data, float &certainty_){
  if (is_leaf){
    certainty_ = certainty;
    return prediction;
  }
  else{
    if (is_in_left_child(data))
      leftchild->predict(data);
    else
      rightchild->predict(data);
  }
}

//template<typename T>
//bool Decisiontree<T>::is_in_left_child(const Dataset &data){
  bool Decisiontree::is_in_left_child(const Dataset &data){
  if (sep_feature_type == 'c'){
    if (data.cat_features[sep_feature_index] == sep_category_flag)
      return true;
    else
      return false; 
  }
  else{
    if (data.num_features[sep_feature_index] <= sep_threshold)
      return true;
    else
      return false;
  }
}

void Decisiontree::save(std::string filename){
  std::ofstream file;
  file.open(filename);
  std::cout << "saving tree...\n";
  file << "depth, is_leaf, sep_feature_type, sep_feature_index, sep_threshold, sep_category_flag, prediction, certainty\n";
  save_to_file(file);
  file.close();
  std::cout << "completed\n";
}

void Decisiontree::save_to_file(std::ofstream &file){
  file << depth <<", " << is_leaf <<", " << sep_feature_type <<", " << sep_feature_index;
  file  <<", " << sep_threshold <<", " << sep_category_flag <<", " << prediction;
  file  <<", " << certainty << std::endl;
  if (!is_leaf){
    leftchild->save_to_file(file);
    rightchild->save_to_file(file);
  }
}
/* 
links ist kleiner gleich
links ist wahr
*/

//for testing purposes
float Decisiontree::gini_impurity_of_all_leaves(){
  float full_gini = 0;
  if (is_leaf == true){
    int n = dataslice.size();
    int n_tot = data.size();
    float p = (double) n/n_tot;
    return p*gini_impurity(dataslice);
  }
  else{
    full_gini += leftchild->gini_impurity_of_all_leaves();
    full_gini += rightchild->gini_impurity_of_all_leaves();
  }
  return full_gini;
}

float Decisiontree::test(std::vector<int> &testdata, const int leafsize){
  char prediction{'x'};
  int counter{0};
  for (auto i:testdata){
      prediction = this->predict(data[i]);
      if (testing){
        std::cout << "Label für Person " << i <<": "<<data[i].label <<std::endl;
        std::cout << "Vorhersage für Person " << i <<": "<< prediction <<std::endl;}
      if (prediction == data[i].label)
        counter++;
    }
    double total_prediction = (double)counter/testdata.size();
    std::cout << "right labled: " << total_prediction *100<<"\%"<< " with leafsize: " << leafsize << std::endl;
    return total_prediction;
}