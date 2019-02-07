#include"data_class.h"
#include "decisiontree.h"
#include<iostream>
#include<cstdlib>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<iterator>
#include<set>
#include<map>
#include<numeric>



bool load_tree = false;
bool save_file = false;
bool full_analysis = false;
bool find_optimum_leaf_size = false; //if set to true, validation dataset can't be empty!
int size_of_leaf = 9;
int begin_leaf_size = 1;
int end_leaf_size = 50;
bool testing = false;
double train_percent = 0.7;
double validation_percent = 0.;
double test_percent = 0.3;
int Datapoint::nr_datapoints{700};  //Number of Datapoints just to boost the code
std::string data_file_name = "Data/shuffled_credit_approval_australia.dat";
//std::string data_file_name = "Data/shuffled_5000_adult_data.dat";  //set with 5000 data
std::string tree_file_name = "tree.dat";
std::string tree_for_visualisation_file_name = "visualisierung.dot";
std::string visualized_tree_file_name = "tree_60000_leaf_180.pdf";
std::string train_result_file_name = "results.txt";
int change_index{4};  //=4 for ethnicity 
const char Decisiontree::label = '+';



// methods to use in main function for analysis
void analyse_all_cat_features(std::vector<Datapoint> &a, const std::vector<Datapoint> &data_to_analyse_2, Decisiontree* mytree);
template<typename T> T find_optimum_tree( const T start_value, const T end_value, const T step_size ,Decisiontree* mytree, std::vector<int> &traindata, 
                                          std::vector<int> &validationdata, std::ofstream &trainresults, const std::string tree_file_name, 
                                          const std::string tree_for_visualisation_file_name);

int main(){
  
  //load data
  auto a = create_dataset(data_file_name, true);
  int y = a.size();

    // make sure all values are set correctly
  if (train_percent+validation_percent+test_percent > 1){
    std::cout << "Error with percent of data split\n";
    return 1; }
  if (size_of_leaf == 0){
    std::cout << "set min number of Datapoints in leaf: ";
    std::cin >> size_of_leaf;
    std::cout << std::endl;
  }
  if (find_optimum_leaf_size and validation_percent*a.size()<1){
    std::cout << "to find optimal hyperparameters a validation set is needed. "<<
       "Please set a percentage of data that should be used as validation set (maximum allowed " << 1-train_percent <<"): ";
    std::cin >> validation_percent;
    std::cout << std::endl;
    test_percent = train_percent - validation_percent;
  }

  //partition data into trainingset, validationset and testset
  int size_traindata = (int)(train_percent*a.size());
  int size_validationdata = (int)(validation_percent*a.size());
  int size_testdata = (int)(test_percent*a.size()) + 1;
  std::vector<int> traindata(size_traindata);
  std::vector<int>validationdata(size_validationdata);
  std::vector<int>testdata(size_testdata);
  std::iota(traindata.begin(), traindata.end(), 0);
  std::iota(validationdata.begin(), validationdata.end(), size_traindata);
  std::iota(testdata.begin(), testdata.end(), size_validationdata+size_traindata);

  
  //std::cout << a.size() << " Test " << size_traindata << " "<<size_validationdata << " "<<size_testdata<< std::endl;

  if (testing){
    for (auto i:Datapoint::cat_sets){
      for (auto j:i)
        std::cout << j<< ", ";
      std::cout << std::endl;}  }

  int best_leaf_size = 0;
  double best_prediction = 0.;
  float train_prediction = 0.;
  float total_prediction = 0.;

  Decisiontree* mytree = new Decisiontree(a, traindata);
  
  std::ofstream trainresults;
  trainresults.open(train_result_file_name);

  //optimize 
  if (find_optimum_leaf_size and !load_tree){
    // find_optimum_tree(0., 0.05, 0.001, mytree, traindata, validationdata, trainresults, tree_file_name, tree_for_visualisation_file_name);
    find_optimum_tree(1, 50, 1, mytree, traindata, validationdata, trainresults, tree_file_name, tree_for_visualisation_file_name);
    if (testing){
    std::cout << "best tree with leafsize: " << best_leaf_size << " and prediction: "<<best_prediction << std::endl;
    mytree->train(traindata, best_leaf_size);
    std::cout << "Test with validationdata:\n";
    mytree->test(validationdata, best_leaf_size);
    std::cout << "Test with Traindata:\n";
    mytree->test(traindata, best_leaf_size);
    mytree->save_for_visualisation(tree_for_visualisation_file_name);
    }
  }
  //train and test 
  else if(!find_optimum_leaf_size and !load_tree){
    mytree->train(traindata, size_of_leaf);  // train tree with manually set exit condition
    mytree->test(testdata, size_of_leaf);
    mytree->save(tree_file_name);
    mytree->save_for_visualisation(tree_for_visualisation_file_name);
  }
//load tree from file
  else if (load_tree){
    delete mytree;
    Decisiontree* mytree = new Decisiontree();
    mytree->load_from_file("result600/tree_op_gini.dat", data_file_name);
    // std::string abc = "secondtree.dot";
    // mytree->save_for_visualisation(abc);
    mytree->test(testdata, size_of_leaf);
  }
  else std::cout << "conflicting request, tree cannot be optimized and loaded simultaneously." << std::endl;

//percent of all "+" labels in data, needed to rate the performance of the tree
  int counter = 0;
  for(auto i:a)
    if (i.label == '+')
      counter++;
  std::cout << (double)counter/a.size() *100<< "% of total data are labeld with '+', in a set of " << a.size() << " datapoints \n";

  std::vector<Datapoint> b_test(a.begin()+traindata.size(),a.begin()+traindata.size()+validationdata.size());
  std::vector<Datapoint> b_analyse(a.begin()+traindata.size()+testdata.size(),a.end());

  for (auto i:validationdata){
    mytree->predict(a[i]);
  }
  for (auto i:testdata){
    mytree->predict(a[i]);
  }
  // std::cout << "blub" << mytree->predict(a[0]) << std::endl;
  // std::cout << "Testvorhersage: " << a[0].prediction << std::endl;
  std::ofstream prediction_file;
  prediction_file.open("test_data_prediction.dat");
  a[0].save_feature_names_to_file(prediction_file);
  for (auto i:validationdata){
    a[i].save_features_values_to_file(prediction_file);
  }
  for (auto i:testdata)  
    a[i].save_features_values_to_file(prediction_file);  

  //save_dataset_to_file("test_data_prediction.dat", b_test);
  // save_dataset_to_file("analyse_data_prediction.dat", b_analyse);

  //make a copy of all analyse data without ethnicity
  // int change_index{3};  //=3 for ethnicity 
  if (full_analysis){
  char prediction_befor_change{'x'};
  char prediction_after_change{'x'};
  int analyse_counter{0};
  std::vector<Datapoint> data_to_analyse(a.begin()+ traindata.size()+validationdata.size() ,a.end() );
  for ( auto idata: data_to_analyse){
    prediction_befor_change = mytree->predict(idata);
    idata.cat_features[change_index] = "?"; //stands for no information
    prediction_after_change = mytree->predict(idata);
    if (prediction_befor_change == prediction_after_change)
      analyse_counter++;
  }
  std::cout << "same classification although no ethnicity: "<<(double)analyse_counter/data_to_analyse.size()*100<<"\%"<<std::endl;

  //make a copy of all analyse data with most frequent ethnicity

  std::vector<Datapoint> data_to_analyse_2(a.begin()+ traindata.size()+validationdata.size() ,a.end() );
  analyse_all_cat_features(a,data_to_analyse_2, mytree);
  delete mytree;
  }

  if (save_file)    //save dataset    //until now not needed because we do not change any data
    save_dataset_to_file("abc.dat", a);
  std::string command = "dot ";
  command.append(tree_for_visualisation_file_name);
  command.append(" -Tpdf -o ");
  command.append(visualized_tree_file_name);
  std::system(command.c_str());
  trainresults.close();
  return 0;
}



void analyse_all_cat_features(std::vector<Datapoint> &a, const std::vector<Datapoint> &data_to_analyse_2, Decisiontree* mytree){
  char prediction{};
  int analyse_counter{};
  std::vector<int> index_vec{};
  index_vec.push_back(0);
  index_vec.push_back(4);
  // for (int change_index = 0; change_index<Datapoint::cat_feature_id.size(); change_index++){
  // for (int change_index = 5; change_index<7; change_index++){
    for (auto change_index:index_vec){
    analyse_counter = 0;
    std::string biggest_ethnicity{};
    // std::vector<Datapoint> data_to_analyse_2(a.begin()+ traindata.size()+validationdata.size() ,a.end() );
    std::map<std::string, int> ethnicity_counter;
    std::map<std::string, std::vector<int>> data_indices_for_each_ethnicity{};
    std::string selected_feature_name = Datapoint::cat_feature_id[change_index];
    int indices = data_to_analyse_2.size();
    std::cout << "\ncat_feature selected:" << selected_feature_name << std::endl;
    for (auto i:Datapoint::cat_sets[change_index]){    //all possible ethnicity in map with value 0
      ethnicity_counter[i]=0;
    }
    for (auto i:a){                     //count how often each ethnicity occurs
      ethnicity_counter[i.cat_features[change_index]]++;
    }
    for (auto i:data_to_analyse_2){     //sort indices of all data in dependence of one feature
      data_indices_for_each_ethnicity[i.cat_features[change_index]].push_back(indices);
      indices++;
    }
    for (auto i:data_indices_for_each_ethnicity){   //all posible values of the feature
      analyse_counter = 0;
      for (auto index:i.second){                    //every datapoint with this flag
        prediction = mytree->predict(a[index]);
        if(prediction == a[index].label)
          analyse_counter++;
      }
      // std::cout << "same classification with " << selected_feature_name << ": " << i.first << "  " << (double)analyse_counter/i.second.size() * 100 <<"\%";
      // std::cout << " with total frequency in analyse data : " << i.second.size() << std::endl; 
      // std::cout << " with total frequency in data : " << ethnicity_counter[i.first] << std::endl; 
      std::cout <<  i.first << "  " << (double)analyse_counter/i.second.size() <<" ";
      std::cout << i.second.size() << std::endl; 
      // std::cout << " with total frequency in data : " << ethnicity_counter[i.first] << std::endl; 
    }//all values of one feature
  }//all cat features
}

template<typename T>
T find_optimum_tree(const T start_value, const T end_value, T step_size, Decisiontree* mytree, std::vector<int> &traindata, 
                    std::vector<int> &validationdata, std::ofstream &trainresults, const std::string tree_file_name, 
                    const std::string tree_for_visualisation_file_name){
  T best_parameter{};
  float best_prediction{0.};
  float train_prediction, total_prediction;
  for (T i = start_value; i<end_value; i+=step_size){
    std::cout << "Schleifenwert für i: "<<i <<" "<<typeid(i).name() << std::endl;
    mytree->train(traindata, i);
    if (testing){
      float gini = mytree->gini_impurity_of_all_leaves();
      //std::cout << "gini at node " << mytree->gini_imp << std::endl;
      std::cout << "gini of all leaves " << gini << std::endl;
      std::cout << "trained" << std::endl;  }

    std::cout << "Test with Traindata:  ";
    train_prediction = mytree->test(traindata, i);
    std::cout << "Test with validationdata:   ";
    total_prediction = mytree->test(validationdata, i);
    trainresults << i <<","<< total_prediction << "," << train_prediction << std::endl;
    if (total_prediction >= best_prediction){
      best_prediction = total_prediction;
      best_parameter = i;
      mytree->save(tree_file_name);
      mytree->save_for_visualisation(tree_for_visualisation_file_name);
    }
  }
  std::cout << "best tree with parameter: " << best_parameter << " and prediction: "<<best_prediction << std::endl;
  // mytree->train(traindata, best_leaf_size);
  // std::cout << "Test with validationdata:\n";
  // mytree->test(validationdata, best_leaf_size);
  // std::cout << "Test with Traindata:\n";
  // mytree->test(traindata, best_leaf_size);
  // mytree->save_for_visualisation(tree_for_visualisation_file_name);

}

/* 
important features in cat ethnicity:
b, e, m
*/
