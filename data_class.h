#ifndef _DATA_CLASS_H_  
#define _DATA_CLASS_H_      //guard
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<iterator>
#include<set>

class Dataset
{
  public:
  const int nrFeatures{};
  std::vector<double> num_features{};
  std::vector<char> cat_features{};
  char label{};
  //Constructor sets size of vectors
  Dataset(const int nrFeatures_);

  void set_features(const std::vector<std::string> &feature_types, std::ifstream &inputFile, const std::string &num, const bool set_label);
};

int skipComments(std::ifstream &fileInputStream);
std::vector<Dataset> load(const bool load_label, const std::string file_name);
void save_Dataset_to_file(const std::string file_name,const  std::vector<Dataset> &data);
void set_feature_set(const std::vector<Dataset> &data);

//      global variabels
// std::vector<std::string> feature_names;
// std::vector<std::string> feature_types;
// std::vector<std::set<std::string>> feature_sets;

#endif