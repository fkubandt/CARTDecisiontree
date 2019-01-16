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

int main(){
  if (train_percent+test_percent+analyse_percent > 1){
    std::cout << "Error with percent of data split\n";
    return 1;
  }
  char prediction{};
  int counter{0};

  std::vector<Dataset> a;
  a = load_Dataset_from_file(true, "devtesting.dat");
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
  std::vector<int>testdata(size_testdata);
  std::vector<int>analysedata(size_analysedata);
  std::iota(traindata.begin(), traindata.end(), 0);
  std::iota(testdata.begin(), testdata.end(), size_traindata);
  std::iota(analysedata.begin(), analysedata.end(), size_testdata+size_traindata);

  std::cout << a[5].num_features.size() << std::endl;
  // Test if data is correct:
  //Decisiontree::data = a;

<<<<<<< HEAD
  Decisiontree* mytree = new Decisiontree(a);
  mytree->train(traindata, 5);
=======
  mytree->train(traindata, 10);

  float gini = mytree->gini_impurity_of_all_leaves();

  std::cout << "gini at node " << mytree->gini_imp << std::endl;
  std::cout << "gini of all leaves " << gini << std::endl;
>>>>>>> refs/remotes/origin/testing
  std::cout << "trained" << std::endl;
  for (auto i:testdata){
    std::cout << "Label für Person" << i <<": "<<a[i].label <<std::endl;
    prediction = mytree->predict(a[i]);
    std::cout << "Vorhersage für Person" << i <<": "<< prediction <<std::endl;
    if (prediction == a[i].label)
      counter++;
  }
  std::cout << "right labled: " << (double)counter/testdata.size() <<"\%"<< std::endl;
  mytree->save("tree.dat");
/* 
  int x = 0;
  std::cin >> x; 
*/

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
  if (save_file)
    save_Dataset_to_file("abc.dat", a);
  return 0;
}