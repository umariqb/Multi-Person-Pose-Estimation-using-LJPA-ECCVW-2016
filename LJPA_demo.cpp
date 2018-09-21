/*
 * LJPA_demo.cpp
 *
 *  Created on: July 22, 2016
 *      Author: Umar Iqbal
 */

#include <istream>
#include <cassert>
#include <opencv2/opencv.hpp>
#include <boost/progress.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/format.hpp>
#include <boost/random.hpp>

#include "cpp/learning/SPLPI/splpi.hpp"
#include "cpp/learning/SPLPI/learn_model_parameter.hpp"

#include "cpp/body_pose/utils.hpp"
#include "cpp/body_pose/body_pose_types.hpp"

#include "cpp/vision/features/cnn/cnn_features.hpp"
#include "cpp/learning/SPLPI/utils.hpp"
#include "cpp/vision/geometry_utils.hpp"

using namespace boost::assign;
using namespace std;
using namespace cv;

static const int NUM_THREADS = 12;

bool nms_loop(const cv::Mat_<float>& cost_img,
              std::vector<cv::Point_<int> >& max_locs,
              float threshold,
              int radius,
              int n_part_proposals)
{

    cv::Mat_<float> cost = cost_img.clone();

    double max_val = numeric_limits<double>::max();
    int count = 0;
    while(count<n_part_proposals)
    {
        count++;
        Point_<int> max_loc;
        minMaxLoc(cost, 0, &max_val, 0, &max_loc);

        if(max_val <= threshold)
        {
            break;
        }

        Rect box(max_loc.x - radius, max_loc.y - radius, 2*radius, 2*radius);
        Rect inter = vision::geometry_utils::intersect(box, Rect(0,0,cost.cols,cost.rows));
        Mat roi = cost(inter);
        Mat mask = Mat::zeros(roi.rows, roi.cols, CV_8U);
        circle(mask, Point(max_loc.x-inter.x, max_loc.y-inter.y), radius, Scalar(255,255,255), -1);
        cv::Scalar m = mean( roi, mask);

        if(m[0] > threshold)
        {
            max_locs.push_back(max_loc);
        }
        circle(cost, max_loc, radius, Scalar(0,0,0), -1);
    }
    return true;
}

int main(int argc, char** argv)
{

    string config_file = "./config_file.txt";

    if(argc > 1)
    {
        config_file = argv[1];
    }

    Config config;
    try
    {
        config.readFile(config_file.c_str());
    }
    catch(const FileIOException &fioex)
    {
        std::cerr << "I/O error while reading file." << std::endl;
        return(EXIT_FAILURE);
    }
    catch(const ParseException &pex)
    {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return(EXIT_FAILURE);
    }

    string folder   = config.lookup("dataset_name");
    string cache    = config.lookup("cache");
    int p_type      = config.lookup("pose_type");
    body_pose::BodyPoseTypes pose_type = static_cast<body_pose::BodyPoseTypes>(p_type);
    bool save = config.lookup("save");

    std::string experiment_name = config.lookup("experiment_name");
    string test_file = config.lookup("test_file_multi");
    LOG(INFO)<<"Test file: "<<test_file;
    LOG(INFO)<<"Loading annotation...";
    vector<MultiAnnotation> annotations;
    CHECK(load_multi_annotations(annotations, test_file));
    LOG(INFO) << annotations.size() << " found.";


    bool use_flipped_anns   =  config.lookup("use_flipped_anns");
    int ref_len             =  config.lookup("ref_len");
    float scale_factor      =  200.0f/ref_len;
    float patch_ratio       =  config.lookup("patch_ratio");
    int patch_size          =  patch_ratio*ref_len;
    int offset              =  150/scale_factor;
    int n_part_proposals		   = config.lookup("n_part_proposals");
    float part_proposals_thresh  = config.lookup("part_proposals_thresh");

    string train_file  = config.lookup("train_file_multi");
    string results_dir = config.lookup("results_dir");
    string bbox_file   = config.lookup("bbox_file");

    // Create SPLP model
    const Setting &root       = config.getRoot();
    Setting &splp_params      = root["splp_params"];
    bool detect_single_person =  true; // (bool)splp_params["detect_single_person"];


    learning::splpi::SPLPI splp_model(static_cast<int>(pose_type), patch_size,
                                      pose_type, detect_single_person);

    learning::splpi::learn_model_parameters_mp(splp_model, train_file,
            patch_size, scale_factor, pose_type, cache);

    // CNN Params
    vector<string> feature_names;
    feature_names += "Mconv5_stage6";
    vision::features::CNNFeatures cnn_feat_extractor(config_file);
    // output file
    ofstream outfile;
    string file_name (boost::str(boost::format("%s/%s/%s.txt") %results_dir %folder %experiment_name));

    vector<vector<Rect> > person_bboxes;
    vector<vector<float> > bbox_scores;
    load_bounding_boxes(bbox_file, person_bboxes, bbox_scores);

    CHECK_EQ(person_bboxes.size(), annotations.size());

    vector<Scalar> line_colors;
    line_colors.push_back(Scalar(255,0,0));
    line_colors.push_back(Scalar(0,255,0));
    line_colors.push_back(Scalar(0,0,255));
    line_colors.push_back(Scalar(255,255,0));
    line_colors.push_back(Scalar(255,0,255));
    line_colors.push_back(Scalar(0,255,255));

    boost::progress_display show_progress(annotations.size());
    for (size_t i = 0; i < annotations.size(); ++i, ++show_progress)
    {
        // load image
        MultiAnnotation m_ann = annotations[i];
        LOG(INFO)<<m_ann.url;
        Mat org_image         = imread(m_ann.url);
        CHECK(org_image.data) << "Cannot open image "<<m_ann.url;

        Mat image;
        float scale = rescale_img_with_avg_group_scale(org_image, image, m_ann, scale_factor);
        Rect extracted_region;
        extract_group_roi_mpii(image, m_ann, image, offset, offset, &extracted_region);

        vector<Rect> bboxes = person_bboxes[i];
        vector<float> scores = bbox_scores[i];
        CHECK_EQ(scores.size(), bboxes.size());

        vector<Rect> correct_bboxes;
        for(size_t b=0; b<bboxes.size(); b++)
        {

            Rect bb = bboxes[b];

            bb.x = round(bb.x*scale);
            bb.y = round(bb.y*scale);
            bb.width = round(bb.width*scale);
            bb.height = round(bb.height*scale);

            bb.x -= extracted_region.x;
            bb.y -= extracted_region.y;

            int area = bb.height * bb.width;

            if(area >= 80*80)
            {
                correct_bboxes.push_back(bb);
            }
            //	if(scores[b] >= 0.8){
            //		correct_bboxes.push_back(bb);
            //	}
        }

        int num_persons = correct_bboxes.size();
        if(false)
        {
            for(unsigned int n=0; n < num_persons; n++)
            {
                Rect bb = correct_bboxes[n];
                rectangle(image, bb, line_colors[n%line_colors.size()], 2);
            }
            imshow("image", image);
            waitKey(0);
        }
        vector<Mat_<float> > maps;
        std::vector<std::vector<cv::Point> > maximas, ref_maximas;
        for(size_t n=0; n < num_persons; n++)
        {

            Rect bb = correct_bboxes[n];

            Point_<int> center = Point(round(bb.x + bb.width/2), round(bb.y + bb.height/2));

            Rect bbox = Rect(center.x - offset, center.y - offset, 2*offset, 2*offset);
            bbox = vision::geometry_utils::intersect(bbox, Rect(0,0, image.cols, image.rows));
            center = center - Point(bbox.x, bbox.y);

            Mat test_image = image(bbox);

            cnn_feat_extractor.extract_cpm(test_image, feature_names, maps, center, true, false);

            vector<vector<learning::splpi::Detection> > part_candidates((int)pose_type);
            for(unsigned int pIdx=0; pIdx<maps.size()-1; pIdx++)
            {

                std::vector<cv::Point> max_locs;
                nms_loop(maps[pIdx], max_locs, part_proposals_thresh, 5, n_part_proposals);

                for(unsigned int nIdx=0; nIdx < max_locs.size(); nIdx++)
                {

                    learning::splpi::Detection detection;
                    detection.loc = max_locs[nIdx];

                    Mat probs = Mat::zeros(1, maps.size(), CV_32F);
                    for(unsigned int p=0; p < maps.size(); p++)
                    {
                        probs.at<float>(0,p) = maps[p].at<float>(max_locs[nIdx]);
                    }

                    detection.conf_values = probs;
                    detection.label = pIdx;
                    detection.possible_labels.push_back(pIdx);
                    detection.score =  probs.at<float>(0, pIdx);
                    part_candidates[pIdx].push_back(detection);
                }
            }

            // create detections
            std::vector<learning::splpi::Detection> detections;
            for(unsigned int pIdx=0; pIdx < part_candidates.size(); pIdx++)
            {
                detections.insert(detections.end(),
                                  part_candidates[pIdx].begin(), part_candidates[pIdx].end());
            }

            // ILP optimizations
            std::vector<std::vector<cv::Point> > ref_locs;

            vector<vector<int> > mst_parents;
            try
            {
                if(detections.size())
                {
                    if(detections.size() > 1)
                    {
                        splp_model.optimize(detections, ref_locs, mst_parents);
                    }
                    else
                    {
                        if(detections.size() == 1)
                        {
                            vector<Point_<int> > locs((int)pose_type, Point_<int>(-1, -1));
                            locs[detections[0].label] = detections[0].loc;
                            ref_locs.push_back(locs);
                        }
                    }
                    CHECK_LE(ref_locs.size(), 1);
                }
            }
            catch(GRBException ex)
            {
                LOG(INFO)<<ex.getMessage();
            }

            if(ref_locs.size())
            {
                for(size_t pIdx=0; pIdx<ref_locs[0].size(); pIdx++)
                {
                    if(is_valid(ref_locs[0][pIdx]))
                    {
                        ref_locs[0][pIdx] += Point(bbox.x, bbox.y);
                    }
                }
            }

            if(ref_locs.size())
            {
                ref_maximas.push_back(ref_locs[0]);
            }
        }

        maximas = ref_maximas;
        remove_duplicates(maximas, ref_maximas, 5);

        if(false)
        {

            int num_persons = ref_maximas.size();
            outfile << annotations[i].url<<" "<<num_persons<<" "<<(int)pose_type<<" ";

            for(int i=0; i<ref_maximas.size(); i++)
            {
                for( int j=0; j < ref_maximas[i].size(); j++)
                {
                    Point pt = ref_maximas[i][j];
                    if(is_valid(pt))
                    {
                        pt += Point(extracted_region.x, extracted_region.y);
                        pt.x = round(pt.x/scale);
                        pt.y = round(pt.y/scale);
                    }
                    outfile << pt.x  << " " << pt.y << " ";
                }
            }
            outfile<<endl;
            outfile.flush();

            if(false)
            {
                for(unsigned int pIdx=0; pIdx<ref_maximas.size(); pIdx++)
                {
                    _mplot_14(image, ref_maximas[pIdx], line_colors[pIdx%line_colors.size()], "", 1, true, "Pose");
                }
                string name = boost::str(boost::format("eccvw_2016/%04d.png") %i);
                imwrite(name, image);
            }
        }
        else
        {
            for(unsigned int pIdx=0; pIdx<ref_maximas.size(); pIdx++)
            {
                _mplot_14(image, ref_maximas[pIdx], line_colors[pIdx%line_colors.size()], "", 1, true, "Pose");
            }
            waitKey(0);
        }
    }
    LOG(INFO) << "DONE";
    return 0;
}
