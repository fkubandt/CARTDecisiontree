
#include"data_class.h"

//      initalize static class variables
std::vector<std::string> Dataset::feature_names{};
std::vector<std::string> Dataset::feature_types{};
std::vector<std::set<std::string>> Dataset::feature_sets{}; // set with all possible feature values
std::vector<std::set<float>> Dataset::numerical_sets{};     // set with all possible numerical values
std::vector<std::string> Dataset::num_feature_names{};      //for saving in correct order
std::vector<std::string> Dataset::cat_feature_names{};
std::vector<std::vector<int>> Dataset::missing_values_index{};
std::vector<int> Dataset::missing_value_counter{};
std::vector<float> Dataset::average_num_values{};

extern bool testing;  //global variable for testing

Dataset::Dataset(const int nrFeatures_):nrFeatures(nrFeatures_){
    num_features.reserve(nrFeatures);
    cat_features.reserve(nrFeatures);
  }

//sets all features of one dataset
void Dataset::set_features(std::ifstream &inputFile, const std::string &num, const bool set_label, int data_counter){ 
  double tmp_num;
  char tmp_cat;
  std::string tmp, line;

  std::getline(inputFile, line);
  std::istringstream iline(line);
  for(int i = 0; i<nrFeatures;i++){
    getline(iline, tmp, ',');
    if (Dataset::feature_types[i] == num){
      if (tmp =="?"){        //"?"" stands for no value!!
        num_features.push_back(0.);
        missing_values_index[i].push_back(data_counter);  //all missing num values are later the average
        missing_value_counter[i]++;
      }
      else{
        num_features.push_back(atof(tmp.c_str()));
      }
    }
    else{
      cat_features.push_back(tmp);
    }
   }
   if(set_label){
     getline(iline, tmp, ',');
     label = tmp.c_str()[0];
   }
   num_features.shrink_to_fit();
   cat_features.shrink_to_fit();
 }


/* returns all data from file and sets the sets with all possible values */
std::vector<Dataset> create_Data(const std::string filename, bool load_label){
  std::vector<Dataset> data;
  data = load_Dataset_from_file(load_label, filename);
  set_feature_set(data);
  set_numerical_set(data);
  return data;
}

/* load all datasets from file */
std::vector<Dataset> load_Dataset_from_file(const bool load_label, const std::string file_name){
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
  Dataset::missing_values_index.resize(nrFeatures);
  Dataset::missing_value_counter.resize(nrFeatures, 0);
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
  data.reserve(Dataset::nrDatasets); //        input length of datafile to boost!
  int data_counter = 0;
  while(!inputFile.eof()){
    Dataset a(nrFeatures);
    a.set_features(inputFile, numerical_token, load_label, data_counter);
    data.push_back(a);
    data_counter++;
  } 
  inputFile.close();
  data.pop_back();     //we initalize one Dataset to much so this is one was empty
  std::vector<std::vector<int>> tmp_missing_values_index = Dataset::missing_values_index;
  Dataset::missing_values_index.clear();
  for (auto i:tmp_missing_values_index){
    if (!i.empty())
      Dataset::missing_values_index.push_back(i);
  }
  Dataset::average_num_values.resize(Dataset::missing_values_index.size());
  for (int i =0; i<Dataset::missing_value_counter.size(); i++){
    float sum{0.};
    for (auto idata:data)
      sum += idata.num_features[i];
    Dataset::average_num_values.push_back(sum/(data.size()-Dataset::missing_value_counter[i]));
  }
  for (int i=0; i<Dataset::missing_values_index.size(); i++){
    for (auto j:Dataset::missing_values_index[i]){
      data[j].num_features[i] = Dataset::average_num_values[i];
    }
  }
  return data;
}

  /*  save Dataset with label to file 
      only usefull after training and evaluating TESTDATA
      order of features is now different. first numerical then categorical    
  */
void save_Dataset_to_file(const std::string file_name, const std::vector<Dataset> &data){
  std::ofstream file;
  file.open(file_name);
  file << std::endl;
  file << Dataset::feature_names.size() << std::endl;
  file << Dataset::num_feature_names[0];
  for(int i = 1; i<Dataset::num_feature_names.size(); i++){
    file << "," << Dataset::num_feature_names[i];
  }
  file << Dataset::cat_feature_names[0];
  for(int i = 1; i<Dataset::cat_feature_names.size(); i++){
    file << "," << Dataset::cat_feature_names[i];
  }
  file << "," << "label" <<std::endl;
  for (int i = 0;i<Dataset::num_feature_names.size(); i++)
    file << "num,";
  file << "cat";
  for (int i = 1; i<Dataset::cat_feature_names.size(); i++)
    file << ",cat";
  file << ",class\n";
  for(auto i:data){
    for(auto j:i.num_features)
      file << j << ",";
    for (auto j : i.cat_features)
      file << j << ",";
    file << i.label << std::endl;
  }
  file.close();
}

/* collects als possible sets for each feature category */
void set_feature_set(const std::vector<Dataset> &data){
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

/* collects als possible sets for each numerical category */
void set_numerical_set(const std::vector<Dataset> &data){
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

int skipComments(std::ifstream &fileInputStream)  //helper function from lecture
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