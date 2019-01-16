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

int main(){
  int nTest=40; 
  std::vector<int> traindata(nTest);
  std::vector<int>testdata;

  std::vector<Dataset> a;
  a = load_Dataset_from_file(true, "devtesting.dat");
  set_feature_set(a);
  set_numerical_set(a);

  std::cout << a[5].num_features.size() << std::endl;
  // Test if data is correct:
  //Decisiontree::data = a;
  Decisiontree* mytree = new Decisiontree(a);
  std::iota(traindata.begin(), traindata.end(), 0);
  for(auto i : traindata){
    std::cout << i << " " ;
    std::cout << a[i].label << std::endl;
  }
  std::cout << std::endl;

  mytree->train(traindata, 10);

  float gini = mytree->gini_impurity_of_all_leaves();

  std::cout << "gini at node " << mytree->gini_imp << std::endl;
  std::cout << "gini of all leaves " << gini << std::endl;
  std::cout << "trained" << std::endl;
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