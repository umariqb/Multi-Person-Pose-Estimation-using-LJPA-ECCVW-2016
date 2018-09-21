  // CNN Params
  #include <boost/assign/std/vector.hpp>

  vector<string> feature_names;
  feature_names += "conv1", "conv2", "conv3"; // names of the layers that you want to extract
  vision::features::CNNFeatures cnn_feat_extractor(config_file);
vector<Mat_<float> > features;
cnn_feat_extractor.extract(image, feature_names, features, true, true);
