#include <ldof.h>
#include <COpticFlowPart.h>
int main (int argc, char* argv[])
{
  CTensor<float> img1, img2;
  img1.readFromPPM("tennis492.ppm");
  img2.readFromPPM("tennis493.ppm");
  CTensor<float> flow;
  runFlow( img1, img2, flow );

  CTensor<float> flow_img;
  COpticFlow::flowToImage(flow,flow_img);
  flow_img.writeToPPM("flow.ppm");


  
  return 0;
}
