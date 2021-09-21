// Copyright 2021 The DaisyKit Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef DAISYKIT_MODELS_FACE_EXTRACTOR_H_
#define DAISYKIT_MODELS_FACE_EXTRACTOR_H_

#include <opencv2/core/core.hpp>
#include <net.h>
#include <platform.h>
#include <benchmark.h>
#include <datareader.h>

namespace daisykit {
namespace models {

<<<<<<< HEAD
class FaceExtractor 
    : public BaseModel<cv::Mat, std::vector<daisykit::types::Feature>> {
 public:
  FaceExtractor(const std::string& param_file,const std::string& weight_file);
  void LoadModel(const std::string& param_file, const std::string& weight_file);
  void preprocess(cv::Mat& image, ncnn::Mat& in);
  virtual std::vector<daisykit::types::Feature> extract(cv::Mat image);
 
 private:
  ncnn::Net* model_ = 0;
  ncnn::Mutex lock_;
=======
class FaceFeature{
    public:
        FaceExtractor(const std::string& param_file,const std::string& weight_file);
        void LoadModel(const std::string& param_file, const std::string& weight_file);
        static bool hasGPU;
        void preprocess(cv::Mat& image, ncnn::Mat& in);
        std::vector<daisykit::types::Features> extract(cv::Mat image);


    private:

        ncnn::Net* model_ = 0;
        ncnn::Mutex lock_;
>>>>>>> 8435931ae78fb88bbd2686605ab5de617f371c55

};


}  // namespace models
}  // namespace daisykit

#endif