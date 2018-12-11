/*
 * ************************************************
 *  Filename:     node.h
 *
 *  Description:  Node Class for Decision Tree
 *
 *  Version:      1.0
 *  Created:      11.12.2019
 *  Revision:     None
 *  Compile:      Makefile, g++
 *
 *  Author:       Frederike Kubandt, Maximilian Märcz
 *  Organization: Goehte University Frankfurt
 * ************************************************* 
*/

#include <string>
#include <set>

class node{
  public:
  bool is_root = false;
  bool is_leaf = false;
  node* leftchild = nullptr;
  node* rightchild = nullptr;
  std::string feature = "";
  std::string feature_type;
  double threshold = 0;
  std::string category_flag = " ";
  std::set<int> dataslice;
  std::string prediction;
  double certainty;

  public: 
//  std::string evaluate(dataset* dataset);
/**  std::string evaluate(float* dataset){
    if is_leaf
      return prediction;
    if (feature_typ == "num")
      bool question = dataset->feature == category_flag;
    else if (feature_type == "cat")
      bool question = dataset->feature <= threshold;
    else
      std::cout << "error!" << std::endl;
   if (question)
      return leftchild->eval(dataset);
   else
     return rightchild->eval(dataset);
**/};

