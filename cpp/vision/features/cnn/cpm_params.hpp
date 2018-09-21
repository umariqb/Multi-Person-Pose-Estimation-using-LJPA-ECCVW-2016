#ifndef CPM_PARAM_HPP_
#define CPM_PARAM_HPP_

namespace vision
{
namespace features
{


struct CPMParam
{
  int stages;
  int boxsize;
  int np;
  int padvalue;
  std::vector<float> multiplier;
  float sigma = 21;
};

} // features

} // vision

#endif
