
#include <algorithm>
#include <utility>

#include <boost/math/special_functions/factorials.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include "gurobi_c++.h"
#include "cpp/learning/SPLP/splp.hpp"
#include "cpp/learning/SPLP/utils.hpp"
#include "cpp/body_pose/common.hpp"

using namespace boost::assign;
using namespace std;

namespace learning
{

namespace splp
{


SPLP::SPLP(unsigned int num_labels_, float norm_size_, body_pose::BodyPoseTypes pose_type_):
                                num_labels(num_labels_),
                                norm_size(norm_size_),
                                pose_type(pose_type_){
  initialize_model();
}

SPLP::SPLP(unsigned int num_labels_, float norm_size_,
          body_pose::BodyPoseTypes pose_type_, bool use_single_solution_constraint_):
            num_labels(num_labels_),
            norm_size(norm_size_),
            pose_type(pose_type_),
            use_single_solution_constraint(use_single_solution_constraint_){
  initialize_model();
}

void SPLP::set_num_labels(unsigned int num_labels_){
  num_labels = num_labels_;
}

bool SPLP::set_pairwise_displacements(vector<vector<cv::EM> >& displacements){
  pairwise_displacements = displacements;
  return true;
}

cv::EM SPLP::get_pairwise_displacment(int pIdx, int ppIdx){
  CHECK(pairwise_displacements.size());
  return pairwise_displacements[pIdx][ppIdx];
}

bool SPLP::set_unaries(std::vector<Detection>& detections){
  x.clear();
  x.resize(detections.size());

  unaries = 0;
  int count = 0;

  float min_val = std::numeric_limits<float>::max();
  float max_val = std::numeric_limits<float>::max() * -1;

  // add variables
  std::vector<int> symmetric_part;
  get_symmetric_parts(symmetric_part, pose_type);


  for(unsigned int dIdx=0; dIdx<detections.size(); dIdx++){
    x[dIdx].resize(num_labels);

    for(unsigned int cIdx=0; cIdx<num_labels; cIdx++){
      string x_name = boost::str(boost::format("x_%1%_%2%") %dIdx %cIdx);
      double prob = static_cast<double>(detections[dIdx].conf_values.at<float>(0,cIdx+1));
      x[dIdx][cIdx] = model->addVar(0.0, 1.0, NULL , GRB_BINARY, x_name);

//      if(detections[dIdx].label == cIdx || symmetric_part[detections[dIdx].label] == cIdx){
//        if(prob <= 0){
//          model->update();
//          model->addConstr(x[dIdx][cIdx], GRB_EQUAL, 0);
//        }
//        else{
//          count++;
//        }
//      }
//      else{
//        model->update();
//        model->addConstr(x[dIdx][cIdx], GRB_EQUAL, 0);
//      }

      double coef_val = log((1-prob)/(prob));
      cout<<coef_val<<", ";
      unaries += coef_val*x[dIdx][cIdx];
      max_val = std::max(max_val, (float)coef_val);
      min_val = std::min(min_val, (float)coef_val);
    }
    cout<<endl;
  }
  LOG(INFO)<<"Unaries: min_val=<<"<<min_val<<"\t max_val="<<max_val;
  return true;
}

double SPLP::same_class_binary(int label, Detection d1,  Detection d2){
  cv::Mat feat = compute_spatial_relation_feature(d1.loc,d2.loc, norm_size);

  Mat mean_mat  =  same_class_binary_models[label].mean;
  Mat std_mat   =  same_class_binary_models[label].std;
  splp::normalize_data(feat, feat, mean_mat, std_mat);

  vector<double> probs;
  same_class_binary_models[label].model->predict_probability<float>(feat, probs);
  return probs[0];
}

double SPLP::diff_class_binary(int label1, Detection d1, int label2, Detection d2){

  cv::Point p1      = d1.loc;
  cv::Point p2      = d2.loc;
  cv::Point offset  = p2 - p1;

  cv::Mat feat;
  extract_pairwise_feat_from_offset(offset, feat);
  hconcat(feat, d1.conf_values, feat);
  hconcat(feat, d2.conf_values, feat);

  Mat mean_mat  =  diff_class_binary_models[label1][label2].mean;
  Mat std_mat   =  diff_class_binary_models[label1][label2].std;
  splp::normalize_data(feat, feat, mean_mat, std_mat);

  vector<double> probs;
  diff_class_binary_models[label1][label2].model->predict_probability<float>(feat, probs);
  return probs[0];
}

bool SPLP::set_binaries(std::vector<Detection>& detections){

  CHECK(detections.size());
  CHECK(model);
  CHECK(same_class_binary_models.size());

  y.clear();
  z.clear();

  binaries = 0;
  unsigned int num_det = detections.size();
  y.resize(num_det);
  z.resize(num_det);

  float min_val = std::numeric_limits<float>::max();
  float max_val = std::numeric_limits<float>::max() * -1;

  for(unsigned int dIdx = 0; dIdx<num_det-1; dIdx++){
    y[dIdx].resize(num_det);
    z[dIdx].resize(num_det);

    for(unsigned int ddIdx = dIdx+1; ddIdx<num_det; ddIdx++){
      z[dIdx][ddIdx].resize(num_labels);

      cout<<"\n==============================================================\n"<<dIdx<<"\t"<<ddIdx<<"\n";
//      std::string y_name = boost::str(boost::format("y_%1%_%2%") %dIdx %ddIdx);
      y[dIdx][ddIdx] = model->addVar(0.0, 1.0, NULL, GRB_BINARY);

      for(unsigned int cIdx = 0; cIdx < num_labels; cIdx++){

        z[dIdx][ddIdx][cIdx].resize(num_labels);

        for(unsigned int ccIdx=0; ccIdx < num_labels; ccIdx++){
          double prob = 0;

          if(cIdx == ccIdx){
           prob = same_class_binary(ccIdx, detections[dIdx], detections[ddIdx]);
          }
          else{
            prob = diff_class_binary(cIdx, detections[dIdx], ccIdx, detections[ddIdx]);
          }
          double coef_val  = log((1-prob)/(prob));

          if(detections[dIdx].label == 0 && detections[ddIdx].label == 0){
           coef_val = numeric_limits<double>::max();
          }

          max_val   = std::max(max_val, (float)coef_val);
          min_val   = std::min(min_val, (float)coef_val);
          if(cIdx == detections[dIdx].label && ccIdx == detections[ddIdx].label)
            cout<<coef_val<<",";
          z[dIdx][ddIdx][cIdx][ccIdx] = model->addVar(0.0, 1.0, NULL, GRB_BINARY);
          binaries += coef_val * z[dIdx][ddIdx][cIdx][ccIdx];
        }
//        cout<<endl;
      }
    }
    cout<<"\n////////////////////////////////////////////////////////////////////\n";
  }
  LOG(INFO)<<"Binaries: min_val="<<min_val<<"\t max_val="<<max_val;
  return true;
}

bool SPLP::add_contraints()
{
  // constraint-1
  CHECK(x.size());
  for(unsigned int dIdx=0; dIdx<x.size(); dIdx++){
    for(unsigned int cIdx=0; cIdx<x[dIdx].size()-1; cIdx++){
      for(unsigned int ccIdx=cIdx+1; ccIdx<x[dIdx].size(); ccIdx++){
        GRBLinExpr lhs = x[dIdx][cIdx] + x[dIdx][ccIdx];
        model->addConstr(lhs, GRB_LESS_EQUAL, 1);
      }
    }
  }

  // constraint-2
  CHECK(y.size());
  for(unsigned int dIdx=0; dIdx<x.size()-1; dIdx++){
    for(unsigned int ddIdx=dIdx+1; ddIdx < x.size(); ddIdx++){
      GRBLinExpr rhs1 = 0, rhs2=0;
      for(unsigned int cIdx=0; cIdx<num_labels; cIdx++){
        rhs1 += x[dIdx][cIdx];
        rhs2 += x[ddIdx][cIdx];
      }
      model->addConstr(y[dIdx][ddIdx], GRB_LESS_EQUAL, rhs1);
      model->addConstr(y[dIdx][ddIdx], GRB_LESS_EQUAL, rhs2);
    }
  }

  // constraint-3 (eqt-8 from the original paper)
  for(unsigned int dIdx=0; dIdx< x.size()-2; dIdx++){
    for(unsigned int ddIdx=dIdx+1; ddIdx< x.size()-1; ddIdx++){
      for(unsigned int dddIdx=ddIdx+1; dddIdx< x.size(); dddIdx++){
        GRBLinExpr lhs_1 = y[dIdx][ddIdx] + y[ddIdx][dddIdx] - y[dIdx][dddIdx];
        GRBLinExpr lhs_2 = y[dIdx][ddIdx] - y[ddIdx][dddIdx] + y[dIdx][dddIdx];
        GRBLinExpr lhs_3 = - y[dIdx][ddIdx] + y[ddIdx][dddIdx] + y[dIdx][dddIdx];
        model->addConstr(lhs_1, GRB_LESS_EQUAL, 1);
        model->addConstr(lhs_2, GRB_LESS_EQUAL, 1);
        model->addConstr(lhs_3, GRB_LESS_EQUAL, 1);
//        GRBLinExpr lhs_1 = y[dIdx][ddIdx]   +   y[ddIdx][dddIdx] - 1;
//        GRBLinExpr lhs_2 = y[dIdx][ddIdx]   +   y[dIdx][dddIdx]  - 1;
//        GRBLinExpr lhs_3 = y[ddIdx][dddIdx] +   y[dIdx][dddIdx]  - 1;
//        model->addConstr(lhs_1, GRB_LESS_EQUAL, y[dIdx][dddIdx]);
//        model->addConstr(lhs_2, GRB_LESS_EQUAL, y[ddIdx][dddIdx]);
//        model->addConstr(lhs_3, GRB_LESS_EQUAL, y[dIdx][ddIdx]);
      }
    }
  }

  // constraint-4
  for(unsigned int dIdx=0; dIdx<x.size()-1; dIdx++){
    for(unsigned int ddIdx=dIdx+1; ddIdx < x.size(); ddIdx++){
      for(unsigned int cIdx=0; cIdx<num_labels; cIdx++){
        for(unsigned int ccIdx=0; ccIdx<num_labels; ccIdx++){

          GRBLinExpr lhs2 = x[dIdx][cIdx] + x[ddIdx][ccIdx] + y[dIdx][ddIdx] - 2;
          GRBLinExpr rhs2 = z[dIdx][ddIdx][cIdx][ccIdx];
          model->addConstr(lhs2, GRB_LESS_EQUAL, rhs2);

          GRBLinExpr lhs = z[dIdx][ddIdx][cIdx][ccIdx];
          model->addConstr(lhs, GRB_LESS_EQUAL, x[dIdx][cIdx]);
          model->addConstr(lhs, GRB_LESS_EQUAL, x[ddIdx][ccIdx]);
          model->addConstr(lhs, GRB_LESS_EQUAL, y[dIdx][ddIdx]);

          // if only single solution is desired (eqt-5 of the original paper)
          if(use_single_solution_constraint){
            GRBLinExpr lhs3 = x[dIdx][cIdx] + x[ddIdx][ccIdx] - 1;
            GRBLinExpr rhs3 = y[dIdx][ddIdx];
            model->addConstr(lhs3, GRB_LESS_EQUAL, rhs3);
          }
        }
      }
    }
  }

  return true;
}

void SPLP::set_single_solution_contraint()
{
  use_single_solution_constraint = true;
}

bool SPLP::set_objective_function(std::vector<Detection>& detections)
{
  reset_model();

  LOG(INFO)<<"Setting Unaries";
  set_unaries(detections);

  LOG(INFO)<<"Setting Binaries";
  set_binaries(detections);

  model->update();
  LOG(INFO)<<"DONE";

  objective = unaries + binaries;
  LOG(INFO)<<"Setting Objective Function";
  try{
    model->setObjective(objective, GRB_MINIMIZE);
    LOG(INFO)<<"Setting Objective Function";
    model->update();
  }
  catch (const GRBException &exc)
  {
    std::cerr << exc.getErrorCode();
    exit(exc.getErrorCode());
  }

  LOG(INFO)<<"Adding Constraints";
  add_contraints();

//  model->getEnv().set(GRB_DoubleParam_MIPGap, 0.01);
  model->getEnv().set(GRB_IntParam_Threads, 12);
  model->update();

  return true;
}

bool SPLP::reset_model(){
  if(model){
    delete this->model;
    this->model = new GRBModel(*env);
    this->model->getEnv().set(GRB_IntParam_LogToConsole, 0);
    return true;
  }
  else{
    return false;
  }
}

bool SPLP::initialize_model(){
  this->env = new GRBEnv();
  this->model = new GRBModel(*env);
  return true;
}

int SPLP::connected_component(vector<int>& component)
{
  component.clear();
  using namespace boost;
  {
    typedef adjacency_list <vecS, vecS, undirectedS> Graph;
    Graph G;
    for(unsigned int dIdx=0; dIdx<y.size(); dIdx++){
      for(unsigned int ddIdx=dIdx+1; ddIdx<y[dIdx].size(); ddIdx++){
        bool status = y[dIdx][ddIdx].get(GRB_DoubleAttr_X);
        if(status){
          add_edge(dIdx, ddIdx, G);
        }
      }
    }
    component.resize(num_vertices(G));
    int num = connected_components(G, &component[0]);
//    std::vector<int>::size_type i;
//    cout << "Total number of components: " << num << endl;
//    for (i = 0; i != component.size(); ++i){
//      cout << "Vertex " << i <<" is in component " << component[i] << endl;
//      cout << endl;
//    }
    return num;
  }
}

int SPLP::optimize(std::vector<Detection>& detections, vector<vector<Point> >& minimas)
{
  set_objective_function(detections);
  model->optimize();
  double obj_val = model->get(GRB_DoubleAttr_ObjVal);
  LOG(INFO)<<"Final objective value = "<<obj_val;
    for(unsigned int dIdx=0; dIdx<x.size(); dIdx++){
      for(unsigned int cIdx=0; cIdx<num_labels; cIdx++){
        cout<<x[dIdx][cIdx].get(GRB_DoubleAttr_X)<<"("<<x[dIdx][cIdx].get(GRB_DoubleAttr_Obj)<<")\t";
      }
      cout<<endl;
    }
    for(unsigned int dIdx=0; dIdx<y.size(); dIdx++){
      for(unsigned int ddIdx=dIdx+1; ddIdx<y[dIdx].size(); ddIdx++){
        cout<<"("<<dIdx<<","<<ddIdx<<") = "<<y[dIdx][ddIdx].get(GRB_DoubleAttr_X)<<"\n";
      }
      cout<<endl;
    }
  vector<int> person_ids;
  int num_persons = connected_component(person_ids);
  LOG(INFO)<<"Detected Persons = "<<num_persons;

  vector<vector<double> > obj_values(num_persons);
  minimas.resize(num_persons);
  for(unsigned int pIdx=0; pIdx<num_persons; pIdx++){
    minimas[pIdx].resize(num_labels, Point(-1,-1));
    obj_values[pIdx].resize(num_labels, std::numeric_limits<double>::max());
  }

  CHECK_EQ(detections.size(), person_ids.size());
  for(unsigned int dIdx=0; dIdx<detections.size(); dIdx++){
    int pid = person_ids[dIdx];
    int cid = 0;
    double val = 0;
    for(unsigned int cIdx=0; cIdx<num_labels; cIdx++){
      if(x[dIdx][cIdx].get(GRB_DoubleAttr_X)){
        cid = cIdx;
        val = x[dIdx][cIdx].get(GRB_DoubleAttr_Obj);
        break;
      }
    }

    if(val < obj_values[pid][cid]){
      obj_values[pid][cid] =  val;
      minimas[pid][cid]    =  detections[dIdx].loc;
    }
  }
  return num_persons;
}

bool  SPLP::set_same_class_binary_models(std::vector<splp::BinaryModel>& models){
  same_class_binary_models = models;
  return true;
}

bool  SPLP::set_diff_class_binary_models(std::vector<std::vector<splp::BinaryModel> >& models){
  diff_class_binary_models = models;
  return true;
}

bool SPLP::save_same_class_binary_models(std::string save_path)
{
  try{
    std::ofstream ofs(save_path.c_str());
    if(ofs==0){
      LOG(INFO)<<"Error: Cannot open the given path to save same class binary models.";
      return false;
    }
    boost::archive::text_oarchive oa(ofs);
    oa<<same_class_binary_models;
    ofs.flush();
    ofs.close();
    LOG(INFO)<<"Models saved at :"<<save_path;
    return true;
  }
  catch(boost::archive::archive_exception& ex){
    LOG(INFO)<<"Archive exception during deserialization:" <<std::endl;
    LOG(INFO)<< ex.what() << std::endl;
    LOG(INFO)<< "it was file: "<<save_path;
  }
  return true;
}


bool SPLP::load_same_class_binary_models(std::string path)
{
    std::ifstream ifs(path.c_str());

    if(!ifs){
      LOG(INFO)<<"file not found.";
    }
    else{
      try{
        boost::archive::text_iarchive ia(ifs);
        ia>>same_class_binary_models;
        CHECK(same_class_binary_models.size());
        LOG(INFO)<<"Same class models loaded";

        for(unsigned int i=0; i<same_class_binary_models.size(); i++){
          string path = same_class_binary_models[i].path;
          same_class_binary_models[i].model->read_from_text(path.c_str());
        }

        return true;
      }
      catch(boost::archive::archive_exception& ex){
        LOG(INFO)<<"Reload Tree: Archive exception during deserializiation: "
              <<ex.what();
          LOG(INFO)<<"not able to load poses from: "<<path;
      }
    }
    return false;
}


bool SPLP::save_diff_class_binary_models(std::string save_path)
{
  try{
    std::ofstream ofs(save_path.c_str());
    if(ofs==0){
      LOG(INFO)<<"Error: Cannot open the given path to save diff class binary models.";
      return false;
    }
    boost::archive::text_oarchive oa(ofs);
    oa<<diff_class_binary_models;
    ofs.flush();
    ofs.close();
    LOG(INFO)<<"Models saved at :"<<save_path;
    return true;
  }
  catch(boost::archive::archive_exception& ex){
    LOG(INFO)<<"Archive exception during deserialization:" <<std::endl;
    LOG(INFO)<< ex.what() << std::endl;
    LOG(INFO)<< "it was file: "<<save_path;
  }
  return true;
}

bool SPLP::load_diff_class_binary_models(std::string path)
{
    std::ifstream ifs(path.c_str());

    if(!ifs){
      LOG(INFO)<<"file not found.";
    }
    else{
      try{
        boost::archive::text_iarchive ia(ifs);
        ia>>diff_class_binary_models;
        LOG(INFO)<<"Diff class models loaded";

        vector<vector<BinaryModel> >& models = diff_class_binary_models;
        for(unsigned int i=0; i<models.size(); i++){
          for(unsigned int j=0; j<models[i].size(); j++){
            if(i == j){
              continue;
            }
            string path = models[i][j].path;
            CHECK(path.c_str());
            models[i][j].model->read_from_text(path.c_str());
          }
        }
        return true;
      }
      catch(boost::archive::archive_exception& ex){
        LOG(INFO)<<"Reload Tree: Archive exception during deserializiation: "
              <<ex.what();
          LOG(INFO)<<"not able to load poses from: "<<path;
      }
    }
    return false;
}

// destructor
SPLP::~SPLP()
{
  delete model;
  delete env;
}

}

}
