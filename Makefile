CC=g++

SRC=LJPA_demo.cpp \
 ../cpp/body_pose/clustering/body_clustering.cpp \
 ../cpp/body_pose/body_part_sample.cpp \
 ../cpp/body_pose/clustering/clustering_features.cpp\
 ../cpp/body_pose/utils.cpp \
 ../cpp/learning/pictorial_structure/inferenz.cpp \
 ../cpp/learning/pictorial_structure/learn_model_parameter.cpp \
 ../cpp/learning/pictorial_structure/part.cpp \
 ../cpp/learning/pictorial_structure/utils.cpp \
 ../cpp/utils/file_utils.cpp \
 ../cpp/utils/image_file_utils.cpp \
 ../cpp/utils/serialization/serialization.cpp \
 ../cpp/utils/string_utils.cpp \
 ../cpp/utils/system_utils.cpp \
 ../cpp/utils/threading.cpp \
 ../cpp/vision/geometry_utils.cpp \
 ../cpp/vision/image_utils.cpp \
 ../cpp/third_party/kmeans/kmeans.cpp \
 ../cpp/vision/features/cnn/cnn_features.cpp \
 ../cpp/vision/features/cnn/caffe_utils.cpp \
 ../cpp/utils/pyramid_stitcher/pyramid_stitcher.cpp \
 ../cpp/utils/pyramid_stitcher/image_pyramid.cpp \
 ../cpp/utils/pyramid_stitcher/patchwork.cpp \
 ../cpp/utils/pyramid_stitcher/Rectangle.cpp \
 ../cpp/learning/pictorial_structure/nbest_inferenz.cpp \
 ../cpp/body_pose/nbest_body_pose_utils.cpp \
 ../cpp/learning/SPLPI/splpi.cpp \
  ../cpp/learning/SPLPI/utils.cpp \
 ../cpp/learning/SPLPI/learn_model_parameter.cpp \
 ../cpp/learning/logistic_regression/LogisticRegression.cpp \
 ../cpp/vision/features/bow_extractor.cpp \
 ../cpp/vision/features/feature_channels/feature_channel_factory.cpp \
 ../cpp/vision/features/feature_channels/hog_extractor.cpp \
 ../cpp/vision/features/simple_feature.cpp \
 ../cpp/vision/features/global/gist.cpp \
 ../cpp/vision/features/local_word_container.cpp \
 ../cpp/vision/features/low_level/color.cpp \
 ../cpp/vision/features/low_level/dense_surf.cpp \
 ../cpp/vision/features/low_level/hog.cpp \
 ../cpp/vision/features/low_level/holbp.cpp \
 ../cpp/vision/features/low_level/lbp.cpp \
 ../cpp/vision/features/low_level/low_level_feature_extractor.cpp \
 ../cpp/vision/features/low_level/self_similarity.cpp \
 ../cpp/vision/features/low_level/surf.cpp \
 ../cpp/vision/features/low_level_features.cpp \
 ../cpp/vision/features/spmbow_extractor.cpp \
 ../cpp/third_party/hog/hog.cpp \
 ../cpp/vision/mean_shift.cpp \
 ../cpp/vision/min_max_filter.cpp \
 ../cpp/utils/libsvm/libsvm.cpp \
 ../cpp/third_party/libsvm/svm.cpp
 



INCLUDES = -I../ \
	   -I../cpp \
	   -I/home/ibal_109/opencv-2.4.8/include/opencv \
	   -I/home/ibal_109/opencv-2.4.8/include \
	   -I/home/ibal_109/glog-0.3.3 \
	   -I/home/ibal_109/gflags \
	   -I/usr/local/include \
       -I../cpp/third_party/caffe/include \
	   -I../cpp/third_party/caffe/build/src/ \
	   -I../cpp/third_party/caffe/src/ \
	   -I../cpp/third_party/graph \
	   -I/usr/local/cuda-6.5/include \
	   -I/home/ibal_109/gurobi605/linux64/include 

LIBRARY := -L/home/ibal_109/lib \
	   -L../cpp/third_party/caffe/build/lib/ \
	   -L/usr/local/cuda-6.5/lib64/ \
	   -L/usr/local/lib \
	   -L/usr/lib \
	   -L/home/ibal_109/gurobi605/linux64/lib/

OCV =  -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_features2d -lopencv_flann -lopencv_nonfree -lopencv_video -lopencv_imgproc -lopencv_ml
BOOST = -lboost_date_time -lboost_iostreams -lboost_program_options -lboost_regex -lboost_serialization -lboost_system -lboost_thread -lboost_filesystem
GLOG = -lglog
GFLAGS = -lgflags
CAFFE = -lcaffe
PROTOBUF = -lprotobuf
PTHREAD = -lpthread
LIBC = -lc
GROUBI = -lgurobi_c++ -lgurobi60
LIBCONFIG = -lconfig++

OBJ=$(SRC:.cpp=.o)

release:
	$(CC) -std=c++0x -Wall -s -w $(INCLUDES) $(LIBRARY) $(SRC) $(OCV) $(BOOST) $(GLOG) $(GFLAGS) $(CAFFE) $(PROTOBUF) $(PTHREAD) $(LIBC) $(CUDA) $(GROUBI) $(LIBCONFIG) -o eval_pose_bb_cpm_ilp.bin

debug :
	$(CC) -std=c++0x -Wall -s -g -w $(INCLUDES) $(LIBRARY) $(SRC) $(OCV) $(BOOST) $(GLOG) $(GFLAGS) $(CAFFE) $(PROTOBUF) $(PTHREAD) $(LIBC) $(CUDA) $(GROUBI) $(LIBCONFIG) -o eval_pose_bb_cpm_ilp.bin
clean:
	rm -f $(OBJ)
