#include<iostream>
#include<map>
#include<vector>
#include<valarray>
#include<string>
#include<fstream>
#include<sstream>
#include<iterator>
#include<set>

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



std::vector<std::string> feature_names{};
std::vector<std::string> feature_types{};
// std::vector<std::set<std::string>> asdf;
std::vector<std::set<std::string>> feature_sets; 

class Dataset
{
  public:
  const int nrFeatures{};
  std::vector<double> num_features{};
  std::vector<char> cat_features{};
  char label{};
  //Constructor sets size of vectors
  Dataset(int nrFeatures_):nrFeatures(nrFeatures_){
    num_features.reserve(nrFeatures);
    cat_features.reserve(nrFeatures);
  }

  void set_features(std::vector<std::string> feature_types, std::ifstream &inputFile, bool set_label){ 
    //d for double, c for category
    //sets all features
    double tmp_num;
    char tmp_cat;
    std::string tmp, line;
    std::string num = "num";


    std::getline(inputFile, line);
    std::istringstream iline(line);
    for(int i = 0; i<nrFeatures;i++){
      getline(iline, tmp, ',');
      if (feature_types[i] == num)
        if (tmp =="?")        //"?"" stands for no value!!
          num_features.push_back(-1.);
        else
          num_features.push_back(atof(tmp.c_str()));
      else
        cat_features.push_back(tmp.c_str()[0]);
    }
    if(set_label){
      getline(iline, tmp, ',');
      label = tmp.c_str()[0];
    }

 
    num_features.shrink_to_fit();
    cat_features.shrink_to_fit();
  }

};

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


std::vector<Dataset> load(bool load_label, std::string file_name){
  /* load all features from file */
  int nrFeatures{0};
  std::ifstream inputFile;
  inputFile.open(file_name);
  skipComments(inputFile);
  inputFile >> nrFeatures;
  printf("nrFeatures: %d\n",nrFeatures);
  std::string tmp;
  std::string line;
  
  inputFile.ignore(INT8_MAX, '\n');
  std::getline(inputFile, line);
  std::istringstream iline(line);
  for(int i = 0; i<nrFeatures;i++){
      getline(iline, tmp, ',');
      feature_names.push_back(tmp);
    }

  line.clear();
  std::getline(inputFile, line);
  iline.clear();
  iline.str(line);

  for(int i = 0; i<nrFeatures;i++){
      getline(iline, tmp, ',');
      feature_types.push_back(tmp);
    }

  std::vector<Dataset> data{};
  data.reserve(100); //        !input length of datafile!
  while(!inputFile.eof()){
    Dataset a(nrFeatures);
    a.set_features(feature_types, inputFile, load_label);
    data.push_back(a);
  } 
  return data;
inputFile.close();
}

void save_Dataset_to_file(std::string file_name){
  /*  save Dataset with label to file 
      only usefull after training and evaluating TESTDATA*/
}

void set_feature_set(std::vector<Dataset> &data){
  /* collects als possible sets for each category */
  std::string tmp;
  int nrCat = data[0].cat_features.size();
  feature_sets.resize(nrCat);
  for (int  i= 0; i < data.size(); i++){     //
    for (int j = 0; j< nrCat; j++){
      tmp = data[i].cat_features[j];
      feature_sets[j].insert(tmp);
    } // j++
  } // i++
}


int main(){
  std::vector<Dataset> a;
  a = load(true, "devtesting.dat");
  a.pop_back();     //we initalize one Dataset to much so this is one was empty
  set_feature_set(a);
  // Test if data is correct:

  std::cout << a[44].num_features[0] << std::endl;
  // int b;
  // std::cin >> b;
  std::cout << a[46].cat_features[0] << std::endl;
  std::cout << a[0].label << std::endl;
  std::cout << a.size() << std::endl;
  std::cout << a[46].cat_features.size() << std::endl;
  std::cout << a[46].num_features.size() << std::endl;
  for ( auto j : feature_sets){
    for (auto i : j)
      std::cout << i;
    std::cout << std::endl;
  }
  return 0;
}