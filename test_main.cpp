#include"data_class.h"
#include "decisiontree.h"
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<iterator>
#include<set>
#include<numeric>

bool save_file = false;
double train_percent = 0.7;
double test_percent = 0.2;
double analyse_percent = 0.1;
bool testing = false;
int Dataset::nrDatasets{700};  //Number of Datasets just to boost the code
int size_of_leaf = 0;

int main(){
  if (train_percent+test_percent+analyse_percent > 1){
    std::cout << "Error with percent of data split\n";
    return 1;
  }
  char prediction{'x'};
  int counter{0};
  if (size_of_leaf == 0){
    std::cout << "set min number of datasets in leaf: ";
    std::cin >> size_of_leaf;
    std::cout << std::endl;
  }

  std::vector<Dataset> a;
  a = load_Dataset_from_file(true, "Data/shuffled_credit_approval_australia.dat");
  // a = load_Dataset_from_file(true, "devtesting.dat");
  set_feature_set(a);
  set_numerical_set(a);
  int size_traindata = (int)(train_percent*a.size());
  int size_testdata = (int)(test_percent*a.size()) + 1;
  int size_analysedata = (int)(analyse_percent*a.size()) + 1;
/* 
  std::cout << "test gesamtgröße" << a.size() << std::endl;
  std::cout << "test 1: " << size_traindata << std::endl;
  std::cout << "test 2: " << size_testdata << std::endl;
  std::cout << "test 3: " << size_analysedata << std::endl;
 */
  std::vector<int> traindata(size_traindata);
  std::vector<int>testdata(size_testdata+size_analysedata);
  std::vector<int>analysedata(size_analysedata);
  std::iota(traindata.begin(), traindata.end(), 0);
  std::iota(testdata.begin(), testdata.end(), size_traindata);
  std::iota(analysedata.begin(), analysedata.end(), size_testdata+size_traindata);

  std::cout << a[5].num_features.size() << std::endl;
  // Test if data is correct:
  //Decisiontree::data = a;
  if (testing){
    for (auto i:Dataset::feature_sets){
      for (auto j:i)
        std::cout << j<< ", ";
      std::cout << std::endl;}
  }
  int best_leaf_size = 0;
  double best_prediction = 0.;
  Decisiontree* mytree = new Decisiontree(a);
  
  for (int i = 1; i<50;i++){

  mytree->train(traindata, i);
  //mytree->train(traindata, 10);

  if (testing){
    float gini = mytree->gini_impurity_of_all_leaves();
    std::cout << "gini at node " << mytree->gini_imp << std::endl;
    std::cout << "gini of all leaves " << gini << std::endl;
    std::cout << "trained" << std::endl;
  }
  for (auto i:testdata){
    prediction = mytree->predict(a[i]);
    if (testing){
      std::cout << "Label für Person " << i <<": "<<a[i].label <<std::endl;
      std::cout << "Vorhersage für Person " << i <<": "<< prediction <<std::endl;}
    if (prediction == a[i].label)
      counter++;
  }
  double total_prediction = (double)counter/testdata.size();
  std::cout << "right labled: " << total_prediction *100<<"\%"<< " with leafsize: " << i << std::endl;
  if (total_prediction > best_prediction){
    best_prediction = total_prediction;
    best_leaf_size = i;
    mytree->save("tree.dat");
  }
  counter = 0;
  }
  std::cout << "best tree with leafsize: " << best_leaf_size << "and prediction: "<<best_prediction << std::endl;
  if (save_file)
    save_Dataset_to_file("abc.dat", a);
  return 0;
}



/*
  std::cout << a[44].num_features[0] << std::endl;
  // int b;
  // std::cin >> b;
  std::cout << a[46].cat_features[0] << std::endl;
  std::cout << a[0].label << std::endl;
  std::cout << a.size() << std::endl;
  std::cout << a[46].cat_features.size() << std::endl;
  std::cout << a[46].num_features.size() << std::endl;
  for ( auto j : Dataset::feature_sets){
    for (auto i : j)
      std::cout << i;
    std::cout << std::endl;
  }
  */