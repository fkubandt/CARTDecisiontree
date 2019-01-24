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

class Datapoint
{
  public:
  std::vector<double> num_features{};
  std::vector<std::string> cat_features{};
  char label{};

  static std::vector<std::set<std::string>> cat_sets;   // set with all possible cat values
  static std::vector<std::set<float>> num_sets;         // set with all possible num values
  static std::vector<std::string> num_feature_id;       //for saving in correct order
  static std::vector<std::string> cat_feature_id;       //for saving in correct order
  static std::vector<std::vector<int>> missing_values_index;  //first index: feature; second: index of dataset
  static std::vector<int> missing_value_counter;          //how many datasets have a missing value in feature i
  static std::vector<float> average_num_values;           //average of all num feature
  static int nr_datapoints;                                  //how many datasets do we have
  static int nr_features;
  
  //Constructor sets size of vectors
  Datapoint(const int nr_features_);

  //sets all features of one Datapoint
  void set_features(std::ifstream &inputFile, const std::string &num, const bool set_label,
                     int data_counter, std::vector<std::string> &feature_types);
};

std::vector<Datapoint> create_Data(const std::string file_name, bool load_label); //use this one in the main function!!
std::vector<Datapoint> load_Dataset_from_file(const bool load_label, const std::string file_name);
void set_cat_set(const std::vector<Datapoint> &data);
void set_num_set(const std::vector<Datapoint> &data);
void save_dataset_to_file(const std::string file_name,const  std::vector<Datapoint> &data);
int skip_comments(std::ifstream &fileInputStream);

#endif