/*
 * *************************************************************
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
 * ************************************************************* 
*/

#include "decisiontree.h"
#include "data_class.h"
extern bool testing;
//

int Decisiontree::n_nodes = 0;
std::vector<Datapoint> Decisiontree::data{};

/**
 * Constructor, only use inside the class
**/
Decisiontree::Decisiontree(int depth, std::vector<int> dataslice):depth(depth), dataslice(dataslice){
  node_index = n_nodes;
  n_nodes += 1;
  if (testing)
    std::cout << "node " << node_index  << "created" << std::endl;
}

/**
 * Constructor to use outside of class
**/
Decisiontree::Decisiontree(std::vector<Datapoint> data_, std::vector<int> training_data_indices){
  Decisiontree::data = data_; 
  depth = 0;
  node_index = n_nodes;
  n_nodes += 1;
  this->data = data_;
  dataslice = training_data_indices;
}
/**
 * Destructor
**/
Decisiontree::~Decisiontree(){
  if (testing)
    std::cout << "Destructor called" << std::endl;
  if (!is_leaf)
    delete leftchild;
    delete rightchild;
}


/* ************************************************************* *
 *                          training                             *
 * ************************************************************* */

//training also uses train template functions defined
//in decisiontree.h

/** 
 * calculate the gini impurity I_G for binary classes +/- as 
 * I_G = p_+(1-p_+) + p_-(1-p_-) = 2*p_+(1-p_+) 
 * This gives the probability for every item of a group being 
 * incorrectly labeled when randomly matched with a label of that group
**/
float Decisiontree::gini_impurity(std::vector<int> data_indices) const{
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



/**
 * Iterate over all possible features and thresholds and maximizes
 * to find max(I_G,parent - p_left * I_G,left - p_right * I_G,right)
 * with p_i = numdata_i/numdata_parent
**/
float Decisiontree::max_information_gain(std::vector<int> data_indices){
  //sep_feature_type ='t';  
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
  int best_feature = -1;
  float best_threshold = 0;
  std::string best_flag = "none";                       //TODO: decide between char and string for cat and flag
  char num_or_cat = 'x';                                //best split for numerical or categorical feature?
  float threshold;
  int num_thresholds;
  std::set<float> this_features_values;
  std::set<std::string> this_features_categories;
  std::string flag{};
//
  std::vector<std::set<float>> possible_values = Datapoint::num_sets;           //all possible values for each numerical feature
  std::vector<std::set<std::string>> possible_categories = Datapoint::cat_sets; //all possible values for each categorical feature 
//
  std::vector<int> left_data;
  std::vector<int> right_data;
//
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
      // split_data(data_indices, left_data, right_data, 'n', ifeatures, threshold);
      split_data_num(data_indices, left_data, right_data, ifeatures, threshold);
      if(!left_data.empty() and !right_data.empty() ){
        p_left = (double)left_data.size()/ntotal_data;
        p_right = (double)right_data.size()/ntotal_data;
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
       }//test for best gain
      }
    }//threshold iteration
  }//feature iteration
//
  // test all category flags
  nfeatures = data[0].cat_features.size();
  if (testing)
    std::cout << "looking for categorical separation" << std::endl;
  for (int ifeatures=0; ifeatures<nfeatures; ifeatures++){
    this_features_categories = possible_categories[ifeatures];
    num_thresholds = this_features_categories.size();
    for(auto ithresholds : this_features_categories){
      flag = ithresholds;
      left_data.clear();
      right_data.clear();
      split_data_cat(data_indices, left_data, right_data, ifeatures, flag);

      if(!left_data.empty() and !right_data.empty() ){
        p_left = (double)left_data.size()/ntotal_data;
        p_right = (double)right_data.size()/ntotal_data;
        information_gain = impurity_parent - p_left * gini_impurity(left_data) - p_right * gini_impurity(right_data);
        if (testing)
          std::cout << "information gain " << information_gain << std::endl;
        if(information_gain > best_information_gain){
          best_information_gain = information_gain;
          best_feature = ifeatures;
          // best_flag = ithresholds;
          best_flag = flag;
          num_or_cat = 'c';
       }//test for best gain
      }
    }//threshold iteration
  }//feature iteration
//
  //store the best split values in this node
  sep_feature_type = num_or_cat;
  sep_feature_index = best_feature;
  if(sep_feature_type == 'n'){
    sep_threshold = best_threshold;
  }//numerical threshold
  else if(sep_feature_type == 'c'){
    sep_category_flag = best_flag;
  }//category flag
  if (testing)
    std::cout << "information gain maximized to ....." << best_information_gain << std::endl;
  return best_information_gain; 
}//max information gain

void Decisiontree::split_data_num(std::vector<int> data_indices, std::vector<int> &left_data, std::vector<int> &right_data, int sep_ft_index, float threshold){
  for (auto idata : data_indices){
    Datapoint thisdata=data[idata];
    if (thisdata.num_features[sep_ft_index] <= threshold){      //TODO: decide between string and char!
      left_data.push_back(idata);
    }
    else{
      right_data.push_back(idata);
    }
  }//iterate over data
  if (testing)
    std::cout << "left data in split" << left_data.size() << std::endl;
}
void Decisiontree::split_data_cat(std::vector<int> data_indices, std::vector<int> &left_data, std::vector<int> &right_data, int sep_ft_index, std::string flag){
  for (auto idata : data_indices){
    Datapoint thisdata=data[idata];
    if (thisdata.cat_features[sep_ft_index] == flag)      
      left_data.push_back(idata);
    else
      right_data.push_back(idata);
  }//iterate over data
  if (testing)
    std::cout << "left data in split" << left_data.size() << std::endl;
}


/**
 * store prediction and certainty of prediction and set is_leaf true
**/
void Decisiontree::create_leaf(std::vector<int> data_indices){
    if (testing)
      std::cout << "training done" << std::endl;
    is_leaf = true;
    int ntrues = 0;
    //count '+' labels
    for (auto idata : data_indices){
      if(data[idata].label == label)
        ntrues += 1;
    }//data iteration
    certainty = (float)ntrues/(float)data_indices.size();
    if (certainty <= 0.5){
      prediction = '-';
      certainty = 1- certainty;
    }//if neg prediction
    else{
      prediction = '+';
    }//if pos prediction
}


/* ************************************************************* *
 *                       prediction                              *
 * ************************************************************* */
/**
 * predict the label
**/
char Decisiontree::predict(const Datapoint &data){
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

char Decisiontree::predict(const Datapoint &data, float &certainty_){
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


  bool Decisiontree::is_in_left_child(const Datapoint &data){
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


/* ************************************************************* *
 *                       data export                             *
 * ************************************************************* */
/**
 * for saving and visualisation of the tree
**/
void Decisiontree::save(std::string file_name){
  std::ofstream file;
  file.open(file_name);
  std::cout << "saving tree.........  ";
  file << "depth, is_leaf, sep_feature_type, sep_feature_index, sep_threshold, sep_category_flag, prediction, certainty, nsample, gini_imp\n";
  save_to_file(file);
  file.close();
  std::cout << "completed\n";
}
/**
 * helper func for save()
 * rekursive save of all data of the tree
**/
void Decisiontree::save_to_file(std::ofstream &file){
  file << depth <<"," << is_leaf <<"," << sep_feature_type <<"," << sep_feature_index;
  file  <<"," << sep_threshold <<"," << sep_category_flag <<"," << prediction;
  file  <<"," << certainty << "," << dataslice.size() << gini_imp << std::endl;
  if (!is_leaf){
    leftchild->save_to_file(file);
    rightchild->save_to_file(file);
  }
}

/**
 * saves all important information for a visualisation with dot
**/
void Decisiontree::save_for_visualisation(std::string file_name){
  int node_nr = 0;
  std::ofstream file;
  file.open(file_name);
  std::cout << "saving tree for visualisation...    ";
  file << "digraph Tree {\n" << "node [shape=box] ;\n";
  std::string feature_name{};
  if (this->sep_feature_type == 'n'){
    feature_name = Datapoint::num_feature_id[sep_feature_index];
    file << node_index <<" [label=\"" << feature_name << " <= " << sep_threshold;
    file << "\\n nsample = " << dataslice.size();
    file << "\"] ;\n";
  }
  else{
    feature_name = Datapoint::cat_feature_id[sep_feature_index];
    file << node_index << " [label=\"" << feature_name << " == " << sep_category_flag;
    file << "\\n sample = " << dataslice.size();
    file << "\"] ;\n";
  }
  if (!is_leaf){
    leftchild->save_node_for_visualisation(file, node_index);
    rightchild->save_node_for_visualisation(file, node_index);
  }
  file << "}";
  file.close();
  std::cout << "completed\n";
}

/**
 * helperfunction for save_for_visualisation
**/
void Decisiontree::save_node_for_visualisation(std::ofstream &file, int parent_id){
  if (!is_leaf){    //is a node
    std::string feature_name{};
    if (sep_feature_type == 'n'){
      feature_name = Datapoint::num_feature_id[sep_feature_index];
      file << node_index <<" [label=\"" << feature_name << " <= " << sep_threshold;
      file << "\\n nsample = " << dataslice.size();
      file << "\"] ;\n";
    }
    else{
      feature_name = Datapoint::cat_feature_id[sep_feature_index];
      file << node_index << " [label=\"" << feature_name << " == " << sep_category_flag;
      file << "\\n sample = " << dataslice.size();
      file << "\"] ;\n";
    }
    file << parent_id << " -> " << node_index << " ;\n";
    leftchild->save_node_for_visualisation(file, node_index);
    rightchild->save_node_for_visualisation(file, node_index);
  }
  else{     //is a leaf
    file << node_index << " [label=\"" << "certainty "<<certainty;
    file << "\\n sample = " << dataslice.size();
    file << "\\n prediction "<<prediction;
    file << "\\n gini impurity = " << gini_imp << "\"";
    file << ", color=\"red\"] ;\n";
    file << parent_id << " -> " << node_index << " ;\n";
  }
}

/* 
links ist kleiner gleich
links ist wahr
*/

/* ************************************************************* *
 *                       data import                             *
 * ************************************************************* */

/**
 * loading the tree and the dataset from files
 * no training is needed but the dataslice is not saved and not loaded!
**/
void Decisiontree::load_from_file(std::string file_name_tree, std::string file_name_data){
  std::ifstream file;
  file.open(file_name_tree);
  std::string line;
  std::getline(file, line);
  int counter = 0;
  this->data = create_data(file_name_data, true);
  this->load_node_from_file(file, counter);
  file.close();
}

/**
 * sets all variables of one node from one line of ifstream
 * has to be formated like a data created from func save()
**/
void Decisiontree::set_node(std::ifstream &file){
  std::string tmp, line;
  std::getline(file, line);
  std::istringstream iline(line);
  getline(iline, tmp, ',');
  this->depth = atoi(tmp.c_str());
  getline(iline, tmp, ',');
  this->is_leaf = atoi(tmp.c_str());
  getline(iline, tmp, ',');
  this->sep_feature_type = tmp.c_str()[0];
  getline(iline, tmp, ',');
  this->sep_feature_index = atoi(tmp.c_str());
  getline(iline, tmp, ',');
  this->sep_threshold = atof(tmp.c_str());
  getline(iline, tmp, ',');
  this->sep_category_flag = tmp;
  getline(iline, tmp, ',');
  this->prediction = tmp.c_str()[0];
  getline(iline, tmp, ',');
  this->certainty = atof(tmp.c_str());
  getline(iline, tmp, ',');
  this->dataslice = std::vector<int>(atoi(tmp.c_str()),0);    //has only the size of dataslice!!!
  getline(iline, tmp, ',');
  this->gini_imp = atof(tmp.c_str());
}

/**
 * load all nodes rekursive from ifstream
**/
void Decisiontree::load_node_from_file(std::ifstream &file, int &counter){
  this->set_node(file);
  this->node_index = counter;
  counter++;
  if (!is_leaf){
    leftchild = new Decisiontree();
    leftchild->load_node_from_file(file, counter);
    rightchild = new Decisiontree();
    rightchild->load_node_from_file(file, counter);
  }
}


/**
 * returns the sum of the gini impurity of all leaves
**/
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

/**
 * ratio of right labled to all datapoints indices in testdata
 * leafsize just for output in terminal
**/
float Decisiontree::test(std::vector<int> &testdata, const int leafsize){
  char prediction{'x'};
  int counter{0};
  for (auto i:testdata){
      prediction = this->predict(data[i]);
      if (testing){
        std::cout << "label for person " << i <<": "<<data[i].label <<std::endl;
        std::cout << "predicion for person " << i <<": "<< prediction <<std::endl;}
      if (prediction == data[i].label)
        counter++;
    }
    double total_prediction = (double)counter/testdata.size();
    std::cout << "right labled: " << total_prediction *100<<"\%"<< " with leafsize: " << leafsize << std::endl;
    return total_prediction;
}