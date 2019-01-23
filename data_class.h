#ifndef _DATA_CLASS_H_  
#define _DATA_CLASS_H_      //guard
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<iterator>
#include<set>

/* 
Data Structur:
commentsymbol:    #     //skiped!!!
first line: empty
second line: number of features !!!! without label !!!! 
third line: names of features
fourth line: types of features ("num" or "cat") 
data    (float or string for num or cat)
....
 */

class Dataset
{
  public:
  int nrFeatures{};
  std::vector<double> num_features{};
  std::vector<std::string> cat_features{};
  char label{};

  static std::vector<std::string> feature_names;          //all feature names
  static std::vector<std::string> feature_types;          //all feature types ("num" or "cat")
  static std::vector<std::set<std::string>> feature_sets; // set with all possible feature values
  static std::vector<std::set<float>> numerical_sets;     // set with all possible numerical values
  static std::vector<std::string> num_feature_names;      //for saving in correct order
  static std::vector<std::string> cat_feature_names;      //for saving in correct order
  static std::vector<std::vector<int>> missing_values_index;  //first index: feature; second: index of dataset
  static std::vector<int> missing_value_counter;          //how many datasets have a missing value in feature i
  static std::vector<float> average_num_values;           //average of all num feature
  static int nrDatasets;                                  //how many datasets do we have

  //Constructor sets size of vectors
  Dataset(const int nrFeatures_);

  //sets all features of one dataset
  void set_features(std::ifstream &inputFile, const std::string &num, const bool set_label, int data_counter);
};

std::vector<Dataset> create_Data(const std::string filename, bool load_label); //use this one in the main function!!
std::vector<Dataset> load_Dataset_from_file(const bool load_label, const std::string file_name);
void set_feature_set(const std::vector<Dataset> &data);
void set_numerical_set(const std::vector<Dataset> &data);
void save_Dataset_to_file(const std::string file_name,const  std::vector<Dataset> &data);
int skipComments(std::ifstream &fileInputStream);

#endif