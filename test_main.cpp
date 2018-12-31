#include"data_class.h"
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<iterator>
#include<set>

extern std::vector<std::string> feature_names;
extern std::vector<std::string> feature_types;
extern std::vector<std::set<std::string>> feature_sets;
extern std::vector<std::string> num_feature_names;     //for saving in correct order
extern std::vector<std::string> cat_feature_names;
bool save_file = false;

int main(){
  std::vector<Dataset> a;
  a = load(true, "devtesting.dat");
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
  if (save_file)
    save_Dataset_to_file("abc.dat", a);
  return 0;
}