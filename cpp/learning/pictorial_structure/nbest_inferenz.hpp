/*!
*
* Class implements pictorial structure inference to obtain
* n best poses from an image following the article:
*
* D. Park, D. Ramanan. "N-Best Maximal Decoders for Part Models"
* International Conference on Computer Vision (ICCV) Barcelona, Spain,
* November 2011.
*
* @Author: uiqbal
* @Date: 06.06.2014
*
*/

#ifndef NBESTINFERENZ_H
#define NBESTINFERENZ_H

#include "cpp/learning/pictorial_structure/inferenz.hpp"
#include "cpp/learning/pictorial_structure/model.hpp"
#include "cpp/body_pose/pose.hpp"

#include "cpp/body_pose/body_pose_types.hpp"
#include "cpp/utils/serialization/opencv_serialization.hpp"
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>

using namespace body_pose;

namespace learning
{
namespace ps
{

struct NBestParam
{
  NBestParam():
    num_nbest(100),
    nms_radius(0),
    nms_radius_for_peaks(0),
    thresh(0),
    max_poses(100),
    save_partwise_costs(false),
    nbest_part_ids(),
    part_ids_for_peaks(){}

  unsigned int num_nbest;
  float nms_radius;
  float nms_radius_for_peaks;
  float thresh;
  bool save_partwise_costs;
  int max_poses;
  std::vector<int> nbest_part_ids;
  std::vector<int> part_ids_for_peaks;
};


class NBestInferenz : public Inferenz
{
  public:

    struct by_inferenz_score {
      bool operator()( body_pose::Pose const &a, body_pose::Pose const &b) {
        return a.inferenz_score < b.inferenz_score;
      }
    };

    NBestInferenz(Model* m): Inferenz(m),
                             min_marginals_computed(false),
                             poses_computed(false)
    {
      int n_parts = Inferenz::model->get_num_parts();
    // min marginals for each part
    min_marginals.resize(n_parts);
    score_src_x_d.resize(n_parts);
    score_src_y_d.resize(n_parts);

    };

    virtual ~NBestInferenz();
    bool compute_min_marginals();
    bool compute_diverse_poses(learning::ps::NBestParam param,
                                body_pose::BodyPoseTypes pose_type);

    bool compute_diverse_poses_for_peaks(learning::ps::NBestParam param,
                                body_pose::BodyPoseTypes pose_type);

    bool nms_loop(const cv::Mat_<float>& cost_img,
                             std::vector<cv::Point_<int> >& min_locs,
                             float threshold, int patch_size = 30,
                             int max_count = 50);

    bool normalize_min_marginals();

    bool compute_arg_mins_mm(int part_id,
                              std::vector<cv::Point_<int> >& min_locs,
                              std::vector<int>& old_parents,
                              std::vector<int>& new_parents);

    bool reorder_parts(int new_root, std::vector<int>& new_parents,  body_pose::BodyPoseTypes pose_type);

    bool get_poses(std::vector<std::vector<cv::Point_<int> > >& out_poses);
    bool get_poses(std::vector<Pose>& out_poses);
  protected:

  private:
    bool min_marginals_computed;
    bool poses_computed;
    std::vector<cv::Mat_<float> > min_marginals;
    std::vector<cv::Mat_<int> > score_src_x_d;
    std::vector<cv::Mat_<int> > score_src_y_d;
    std::vector<Pose> poses;
};

bool eliminate_overlapping_poses(std::vector<Pose>& poses
                                 ,std::vector<Pose>& nms_poses,
                                 double threshold = 0.02,
                                 int upper_body_size = 50);


float inferenz_nbest_max_decoder(std::vector<Model> models,
    const std::vector<cv::Mat_<float> >& apperance_scores,
    std::vector<Pose>& poses,
    body_pose::BodyPoseTypes pose_type,
    NBestParam param = NBestParam(),
    bool debug = false,
    const cv::Mat& img = cv::Mat());

float inferenz_nbest_max_decoder(std::vector<Model> models,
    const std::vector<cv::Mat_<float> >& apperance_scores,
    std::vector<std::vector<cv::Point_<int> > >& min_locations,
    body_pose::BodyPoseTypes pose_type,
    NBestParam param =  NBestParam(),
    bool debug = false,
    const cv::Mat& img = cv::Mat());

bool load_nbest_poses(std::string path, std::vector<Pose>& poses);
bool save_nbest_poses(std::vector<Pose>& poses, std::string path);


} /* namespace ps */
} /* namespace learning */

#endif // NBESTINFERENZ_H
