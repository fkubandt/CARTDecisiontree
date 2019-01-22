#include"data_class.h"
#include "decisiontree.h"
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<iterator>
#include<set>
#include<map>
#include<numeric>
// #include<utility>



bool save_file = false;
bool testing = false;
double train_percent = 0.7;
double test_percent = 0.2;
double analyse_percent = 0.1;
int Dataset::nrDatasets{700};  //Number of Datasets just to boost the code
bool find_optimum_leaf_size = false;
int size_of_leaf = 20;
int begin_leaf_size = 1;
int end_leaf_size = 50;
std::string data_file_name = "Data/shuffled_credit_approval_australia.dat";
std::string tree_file_name = "tree.dat";
int change_index{3};  //=3 for ethnicity 

int main(){
  char prediction{'x'};
  int counter{0};
  if (train_percent+test_percent+analyse_percent > 1){
    std::cout << "Error with percent of data split\n";
    return 1; }
  if (size_of_leaf == 0){
    std::cout << "set min number of datasets in leaf: ";
    std::cin >> size_of_leaf;
    std::cout << std::endl;
  }

  auto a = create_Data(data_file_name, true);

  int size_traindata = (int)(train_percent*a.size());
  int size_testdata = (int)(test_percent*a.size());
  int size_analysedata = (int)(analyse_percent*a.size()) + 1;
  std::vector<int> traindata(size_traindata);
  std::vector<int>testdata(size_testdata);
  std::vector<int>analysedata(size_analysedata);
  std::iota(traindata.begin(), traindata.end(), 0);
  std::iota(testdata.begin(), testdata.end(), size_traindata);
  std::iota(analysedata.begin(), analysedata.end(), size_testdata+size_traindata);

  std::cout << a.size() << " Test " << size_traindata << " "<<size_testdata << " "<<size_analysedata<< std::endl;
  // Test if data is correct:
  //Decisiontree::data = a;
  if (testing){
    for (auto i:Dataset::feature_sets){
      for (auto j:i)
        std::cout << j<< ", ";
      std::cout << std::endl;}  }

  int best_leaf_size = 0;
  double best_prediction = 0.;

  Decisiontree* mytree = new Decisiontree(a, traindata);
  
  if (find_optimum_leaf_size){
    for (int i = begin_leaf_size; i<end_leaf_size; i++){
      mytree->train(traindata, i);

      if (testing){
        float gini = mytree->gini_impurity_of_all_leaves();
        std::cout << "gini at node " << mytree->gini_imp << std::endl;
        std::cout << "gini of all leaves " << gini << std::endl;
        std::cout << "trained" << std::endl;  }

      float total_prediction = mytree->test(testdata, i);
      if (total_prediction > best_prediction){
        best_prediction = total_prediction;
        best_leaf_size = i;
        mytree->save(tree_file_name);
      }
    }
    std::cout << "best tree with leafsize: " << best_leaf_size << " and prediction: "<<best_prediction << std::endl;
    mytree->train(traindata, best_leaf_size);
    mytree->test(testdata, best_leaf_size);
    mytree->save_for_visualisation("visualisierung.dot");
  }
  else{
    mytree->train(traindata, size_of_leaf);
    mytree->test(testdata, size_of_leaf);
    mytree->save(tree_file_name);
    mytree->save_for_visualisation("visualisierung.dot");
  }



  //make a copy of all analyse data without ethnicity
  // int change_index{3};  //=3 for ethnicity 
  char prediction_befor_change{'x'};
  char prediction_after_change{'x'};
  int analyse_counter{0};
  std::vector<Dataset> data_to_analyse(a.begin()+ traindata.size()+testdata.size() ,a.end() );
  for ( auto idata: data_to_analyse){
    prediction_befor_change = mytree->predict(idata);
    idata.cat_features[change_index] = '?'; //stands for no information
    prediction_after_change = mytree->predict(idata);
    if (prediction_befor_change == prediction_after_change)
      analyse_counter++;
  }
  std::cout << "same classification although no ethnicity: "<<(double)analyse_counter/data_to_analyse.size()*100<<"\%"<<std::endl;


  //make a copy of all analyse data with most frequent ethnicity
  // int change_index{3};  //=3 for ethnicity 
  prediction_befor_change = 'x';
  prediction_after_change = 'x';
  analyse_counter = 0;
  char biggest_ethnicity{};
  std::vector<Dataset> data_to_analyse_2(a.begin()+ traindata.size()+testdata.size() ,a.end() );
  std::map<char, int> ethnicity_counter;
  for (auto i:Dataset::feature_sets[change_index])    //all possible ethnicity in map with value 0
    ethnicity_counter[i.c_str()[0]]=0;

  for (auto i:data_to_analyse_2){                     //count how often each ethnicity occurs
    // std::cout << i.cat_features[change_index]<<std::endl;
    ethnicity_counter[i.cat_features[change_index]]++;
  }
  int x = 0;
  for (auto i:ethnicity_counter)                      //finds the most ethnicity
    if (i.second > x){
      x = i.second;
      biggest_ethnicity=i.first;
    }
  for ( auto idata: data_to_analyse_2){               //compares all data with standart and new ethnicity
    prediction_befor_change = mytree->predict(idata);
    idata.cat_features[change_index] = biggest_ethnicity; 
    prediction_after_change = mytree->predict(idata);
    if (prediction_befor_change == prediction_after_change)
      analyse_counter++;
  }
  std::cout << "same classification although set to biggest ethnicity: "<<(double)analyse_counter/data_to_analyse.size()*100<<"\%"<<std::endl;
  std::cout << "biggest ethnicity: " << biggest_ethnicity <<" mit Häufigkeit: " << x<<std::endl;
  delete mytree;

  if (save_file)    //save dataset    until now not needed because we do not change any data
    save_Dataset_to_file("abc.dat", a);
  return 0;
}

/* 
important features in cat ethnicity:
b, e, m
*/
