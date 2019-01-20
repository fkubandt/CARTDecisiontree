#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<iterator>
#include<set>
#include"data_class.h"

/* 
Data Structur:
commentsymbol:    #     //skiped!!!
first line: number of features !!!! without label !!!! 
second line: names of features
third line: types of features (double or char)   //char for category   //d for double, c for category
data
....
 */

/*
alle möglichkeiten einer kategorie in extra vektor speichern!
*/

//      initalize static class variables
std::vector<std::string> Dataset::feature_names{};
std::vector<std::string> Dataset::feature_types{};
std::vector<std::set<std::string>> Dataset::feature_sets{};    // set with all possible feature values
std::vector<std::set<float>> Dataset::numerical_sets{};  // set with all possible numerical values
std::vector<std::string> Dataset::num_feature_names{};     //for saving in correct order
std::vector<std::string> Dataset::cat_feature_names{};

extern bool testing;


Dataset::Dataset(const int nrFeatures_):nrFeatures(nrFeatures_){
    num_features.reserve(nrFeatures);
    cat_features.reserve(nrFeatures);
  }

void Dataset::set_features(std::ifstream &inputFile, const std::string &num, const bool set_label){ 
  //sets all features
  double tmp_num;
  char tmp_cat;
  std::string tmp, line;
  // std::string num = "num";


  std::getline(inputFile, line);
  std::istringstream iline(line);
  for(int i = 0; i<nrFeatures;i++){
    getline(iline, tmp, ',');
    if (Dataset::feature_types[i] == num){
      if (tmp =="?"){        //"?"" stands for no value!!
        num_features.push_back(-1.);
      }
      else{
        num_features.push_back(atof(tmp.c_str()));
      }
      //num_feature_names.push_back(feature_names[i]);
    }
    else{
      cat_features.push_back(tmp.c_str()[0]);
      //cat_feature_names.push_back(feature_names[i]);
    }
   }
   if(set_label){
     getline(iline, tmp, ',');
     label = tmp.c_str()[0];
   }

   num_features.shrink_to_fit();
   cat_features.shrink_to_fit();
 }

int skipComments(std::ifstream &fileInputStream)  // passing stream by reference
{
 static int nComments = 0;
 char inChar = fileInputStream.peek();     // peak first char of file
 while (inChar=='#')                       // skipping comments
   {
    fileInputStream.ignore(INT8_MAX, '\n'); // skip to next line
    inChar = fileInputStream.peek();       // peak first char of line
    nComments++;
   } 
 return nComments;
}


std::vector<Dataset> load_Dataset_from_file(const bool load_label, const std::string file_name){
  /* load all features from file */
  int nrFeatures{0};
  std::ifstream inputFile;
  inputFile.open(file_name);
  skipComments(inputFile);
  inputFile >> nrFeatures;
  printf("nrFeatures: %d\n",nrFeatures);
  std::string tmp;
  std::string line;
  std::string numerical_token = "num";
  
  inputFile.ignore(INT8_MAX, '\n');
  std::getline(inputFile, line);
  std::istringstream iline(line);
  for(int i = 0; i<nrFeatures;i++){
      getline(iline, tmp, ',');
      Dataset::feature_names.push_back(tmp);
    }

  line.clear();
  std::getline(inputFile, line);
  iline.clear();
  iline.str(line);

  for(int i = 0; i<nrFeatures;i++){
      getline(iline, tmp, ',');
      Dataset::feature_types.push_back(tmp);
    }
  for(int i = 0; i < Dataset::feature_types.size(); i++){
    if (Dataset::feature_types[i] == numerical_token)
      Dataset::num_feature_names.push_back(Dataset::feature_names[i]);
    else
      Dataset::cat_feature_names.push_back(Dataset::feature_names[i]);
  }

  std::vector<Dataset> data{};
  data.reserve(Dataset::nrDatasets); //        !input length of datafile!
  while(!inputFile.eof()){
    Dataset a(nrFeatures);
    a.set_features(inputFile ,numerical_token ,load_label);
    data.push_back(a);
  } 
  inputFile.close();
  data.pop_back();     //we initalize one Dataset to much so this is one was empty
  return data;
}

void save_Dataset_to_file(const std::string file_name, const std::vector<Dataset> &data){
  /*  save Dataset with label to file 
      only usefull after training and evaluating TESTDATA
      order of features is now different. first numerical then categorical    
  */
  std::ofstream file;
  file.open(file_name);
  file << Dataset::num_feature_names[0];
  for(int i = 1; i<Dataset::num_feature_names.size(); i++){
    file << "," << Dataset::num_feature_names[i];
  }
  file << Dataset::cat_feature_names[0];
  for(int i = 1; i<Dataset::cat_feature_names.size(); i++){
    file << "," << Dataset::cat_feature_names[i];
  }
  file << "," << "label" <<std::endl;
  for(auto i:data){
    for(auto j:i.num_features)
      file << j << ",";
    for (auto j : i.cat_features)
      file << j << ",";
    file << i.label << std::endl;
  }
  file.close();
}

void set_feature_set(const std::vector<Dataset> &data){
  /* collects als possible sets for each feature category */
  std::string tmp;
  int nrCat = data[0].cat_features.size();
  Dataset::feature_sets.resize(nrCat);
  for (int  i= 0; i < data.size(); i++){     //
    for (int j = 0; j< nrCat; j++){
      tmp = data[i].cat_features[j];
      Dataset::feature_sets[j].insert(tmp);
    } // j++
  } // i++
}

void set_numerical_set(const std::vector<Dataset> &data){
  /* collects als possible sets for each numerical category */
  float tmp=0.;
  int nrCat = data[0].num_features.size();
  Dataset::numerical_sets.resize(nrCat);
  for (int  i= 0; i < data.size(); i++){     //
    for (int j = 0; j< nrCat; j++){
      tmp = data[i].num_features[j];
      Dataset::numerical_sets[j].insert(tmp);
    } // j++
  } // i++
}

std::vector<Dataset> create_Data(const std::string filename, bool load_label){
  std::vector<Dataset> data;
  data = load_Dataset_from_file(load_label, filename);
  set_feature_set(data);
  set_numerical_set(data);
  return data;
}