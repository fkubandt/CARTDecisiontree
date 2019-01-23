
#include"data_class.h"

//      initalize static class variables
std::vector<std::set<std::string>> Datapoint::cat_sets{};
std::vector<std::set<float>> Datapoint::num_sets{};
std::vector<std::string> Datapoint::num_feature_id{};
std::vector<std::string> Datapoint::cat_feature_id{};
std::vector<std::vector<int>> Datapoint::missing_values_index{};
std::vector<int> Datapoint::missing_value_counter{};
std::vector<float> Datapoint::average_num_values{};
int Datapoint::nr_features{};

extern bool testing;  //global variable for testing

Datapoint::Datapoint(const int nr_features_){
    num_features.reserve(nr_features_);
    cat_features.reserve(nr_features_);
  }

//sets all features of one Datapoint
void Datapoint::set_features(std::ifstream &inputFile, const std::string &num, const bool set_label,
                             int data_counter, std::vector<std::string> &feature_types){ 
  double tmp_num;
  char tmp_cat;
  std::string tmp, line;

  std::getline(inputFile, line);
  std::istringstream iline(line);
  for(int i = 0; i<Datapoint::nr_features;i++){
    getline(iline, tmp, ',');
    if (feature_types[i] == num){
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
std::vector<Datapoint> create_Data(const std::string filename, bool load_label){
  std::vector<Datapoint> data;
  data = load_Dataset_from_file(load_label, filename);
  set_feature_set(data);
  set_numerical_set(data);
  return data;
}

/* load all Datapoints from file */
std::vector<Datapoint> load_Dataset_from_file(const bool load_label, const std::string file_name){
  std::vector<std::string> feature_names{};
  std::vector<std::string> feature_types{};

  std::ifstream inputFile;
  inputFile.open(file_name);
  skipComments(inputFile);
  inputFile >> Datapoint::nr_features;
  printf("nrFeatures: %d\n",Datapoint::nr_features);
  std::string tmp;
  std::string line;
  std::string numerical_token = "num";
  
  inputFile.ignore(INT8_MAX, '\n');
  std::getline(inputFile, line);
  std::istringstream iline(line);
  for(int i = 0; i<Datapoint::nr_features;i++){
      getline(iline, tmp, ',');
      feature_names.push_back(tmp);
    }

  line.clear();
  std::getline(inputFile, line);
  iline.clear();
  iline.str(line);
  Datapoint::missing_values_index.resize(Datapoint::nr_features);
  Datapoint::missing_value_counter.resize(Datapoint::nr_features, 0);
  for(int i = 0; i<Datapoint::nr_features;i++){
      getline(iline, tmp, ',');
      feature_types.push_back(tmp);
    }
  for(int i = 0; i < feature_types.size(); i++){
    if (feature_types[i] == numerical_token)
      Datapoint::num_feature_id.push_back(feature_names[i]);
    else
      Datapoint::Datapoint::cat_feature_id.push_back(feature_names[i]);
  }

  std::vector<Datapoint> data{};
  data.reserve(Datapoint::nr_datapoints); //        input length of datafile to boost!
  int data_counter = 0;
  while(!inputFile.eof()){
    Datapoint a(Datapoint::nr_features);
    a.set_features(inputFile, numerical_token, load_label, data_counter, feature_types);
    data.push_back(a);
    data_counter++;
  } 
  inputFile.close();
  data.pop_back();     //we initalize one Datapoint to much so this is one was empty
  std::vector<std::vector<int>> tmp_missing_values_index = Datapoint::missing_values_index;
  Datapoint::missing_values_index.clear();
  for (auto i:tmp_missing_values_index){
    if (!i.empty())
      Datapoint::missing_values_index.push_back(i);
  }
  Datapoint::average_num_values.resize(Datapoint::missing_values_index.size());
  for (int i =0; i<Datapoint::missing_value_counter.size(); i++){
    float sum{0.};
    for (auto idata:data)
      sum += idata.num_features[i];
    Datapoint::average_num_values.push_back(sum/(data.size()-Datapoint::missing_value_counter[i]));
  }
  for (int i=0; i<Datapoint::missing_values_index.size(); i++){
    for (auto j:Datapoint::missing_values_index[i]){
      data[j].num_features[i] = Datapoint::average_num_values[i];
    }
  }
  return data;
}

  /*  save Dataset with label to file 
      only usefull after training and evaluating TESTDATA
      order of features is now different. first numerical then categorical    
  */
void save_Dataset_to_file(const std::string file_name, const std::vector<Datapoint> &data){
  std::ofstream file;
  file.open(file_name);
  file << std::endl;
  file << Datapoint::nr_features << std::endl;
  file << Datapoint::num_feature_id[0];
  for(int i = 1; i<Datapoint::num_feature_id.size(); i++){
    file << "," << Datapoint::num_feature_id[i];
  }
  file << Datapoint::Datapoint::cat_feature_id[0];
  for(int i = 1; i<Datapoint::Datapoint::cat_feature_id.size(); i++){
    file << "," << Datapoint::Datapoint::cat_feature_id[i];
  }
  file << "," << "label" <<std::endl;
  for (int i = 0;i<Datapoint::num_feature_id.size(); i++)
    file << "num,";
  file << "cat";
  for (int i = 1; i<Datapoint::Datapoint::cat_feature_id.size(); i++)
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
void set_feature_set(const std::vector<Datapoint> &data){
  std::string tmp;
  int nrCat = data[0].cat_features.size();
  Datapoint::cat_sets.resize(nrCat);
  for (int  i= 0; i < data.size(); i++){     //
    for (int j = 0; j< nrCat; j++){
      tmp = data[i].cat_features[j];
      Datapoint::cat_sets[j].insert(tmp);
    } // j++
  } // i++
}

/* collects als possible sets for each numerical category */
void set_numerical_set(const std::vector<Datapoint> &data){
  float tmp=0.;
  int nrCat = data[0].num_features.size();
  Datapoint::num_sets.resize(nrCat);
  for (int  i= 0; i < data.size(); i++){     //
    for (int j = 0; j< nrCat; j++){
      tmp = data[i].num_features[j];
      Datapoint::num_sets[j].insert(tmp);
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