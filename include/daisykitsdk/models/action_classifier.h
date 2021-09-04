#ifndef DAISYKIT_MODELS_ACTION_CLASSIFIER_H_
#define DAISYKIT_MODELS_ACTION_CLASSIFIER_H_

#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <benchmark.h>
#include <cpu.h>
#include <datareader.h>
#include <gpu.h>
#include <net.h>
#include <platform.h>

#if __ARM_NEON
#include <arm_neon.h>
#endif  // __ARM_NEON

#include <daisykitsdk/common/types.h>
#include <daisykitsdk/utils/img_proc/img_utils.h>

namespace daisykit {
namespace models {
class ActionClassifier {
 public:
  ActionClassifier(const std::string& param_file,
                   const std::string& weight_file, bool smooth = true);
  void LoadModel(const std::string& param_file, const std::string& weight_file);
#ifdef __ANDROID__
  ActionClassifier(AAssetManager* mgr, const std::string& param_file,
                   const std::string& weight_file, bool smooth = true);
  void LoadModel(AAssetManager* mgr, const std::string& param_file,
                 const std::string& weight_file);
#endif
  daisykit::common::Action Classify(cv::Mat& image);
  daisykit::common::Action Classify(cv::Mat& image, float& confidence);

 private:
  const int input_width_ = 224;
  const int input_height_ = 224;
  ncnn::Mutex lock_;
  ncnn::Net* model_ = 0;
  bool _smooth = true;  // Smooth the result or not
  long long int last_pushup_time_ = 0;
};

}  // namespace models
}  // namespace daisykit

#endif