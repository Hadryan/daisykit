#include <daisykitsdk/models/pose_detector.h>

using namespace daisykit::common;
using namespace daisykit::models;

PoseDetector::PoseDetector(const std::string& param_file,
                           const std::string& weight_file) {
  LoadModel(param_file, weight_file);
}

void PoseDetector::LoadModel(const std::string& param_file,
                              const std::string& weight_file) {
  if (model_) {
    delete model_;
  }
  model_ = new ncnn::Net;
  model_->load_param(param_file.c_str());
  model_->load_model(weight_file.c_str());
}

#ifdef __ANDROID__
PoseDetector::PoseDetector(AAssetManager* mgr, const std::string& param_file,
                           const std::string& weight_file) {
  LoadModel(mgr, param_file, weight_file);
}

void PoseDetector::LoadModel(AAssetManager* mgr, const std::string& param_file,
                              const std::string& weight_file) {
  if (model_) {
    delete model_;
  }
  model_ = new ncnn::Net;
  model_->load_param(mgr, param_file.c_str());
  model_->load_model(mgr, weight_file.c_str());
}
#endif

// Detect keypoints for single object
std::vector<Keypoint> PoseDetector::Detect(cv::Mat& image, float offset_x,
                                           float offset_y) {
  std::vector<Keypoint> keypoints;
  int w = image.cols;
  int h = image.rows;
  ncnn::Mat in = ncnn::Mat::from_pixels_resize(image.data, ncnn::Mat::PIXEL_RGB,
                                               image.cols, image.rows,
                                               input_width_, input_height_);
  const float mean_vals[3] = {0.485f * 255.f, 0.456f * 255.f, 0.406f * 255.f};
  const float norm_vals[3] = {1 / 0.229f / 255.f, 1 / 0.224f / 255.f,
                              1 / 0.225f / 255.f};
  in.substract_mean_normalize(mean_vals, norm_vals);
  ncnn::MutexLockGuard g(lock_);
  ncnn::Extractor ex = model_->create_extractor();
  ex.set_num_threads(4);
  ex.input("data", in);
  ncnn::Mat out;
  ex.extract("hybridsequential0_conv7_fwd", out);
  keypoints.clear();
  for (int p = 0; p < out.c; p++) {
    const ncnn::Mat m = out.channel(p);
    float max_prob = 0.f;
    int max_x = 0;
    int max_y = 0;
    for (int y = 0; y < out.h; y++) {
      const float* ptr = m.row(y);
      for (int x = 0; x < out.w; x++) {
        float prob = ptr[x];
        if (prob > max_prob) {
          max_prob = prob;
          max_x = x;
          max_y = y;
        }
      }
    }

    Keypoint keypoint;
    keypoint.x = max_x * w / (float)out.w + offset_x;
    keypoint.y = max_y * h / (float)out.h + offset_y;
    keypoint.prob = max_prob;
    keypoints.push_back(keypoint);
  }
  return keypoints;
}

// Detect keypoints for multiple objects
std::vector<std::vector<Keypoint>> PoseDetector::DetectMulti(
    cv::Mat& image, const std::vector<Object>& objects) {
  int img_w = image.cols;
  int img_h = image.rows;
  int x1, y1, x2, y2;

  std::vector<std::vector<Keypoint>> keypoints;
  for (size_t i = 0; i < objects.size(); ++i) {
    x1 = objects[i].x;
    y1 = objects[i].y;
    x2 = objects[i].x + objects[i].w;
    y2 = objects[i].y + objects[i].h;
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 < 0) x2 = 0;
    if (y2 < 0) y2 = 0;
    if (x1 > img_w) x1 = img_w;
    if (y1 > img_h) y1 = img_h;
    if (x2 > img_w) x2 = img_w;
    if (y2 > img_h) y2 = img_h;

    cv::Mat roi = image(cv::Rect(x1, y1, x2 - x1, y2 - y1)).clone();
    std::vector<Keypoint> keypoints_single = Detect(roi, x1, y1);
    keypoints.push_back(keypoints_single);
  }

  return keypoints;
}

// Draw pose
void PoseDetector::DrawKeypoints(const cv::Mat& image,
                             const std::vector<Keypoint>& keypoints) {
  float threshold = 0.2;
  // draw bone
  static const int joint_pairs[16][2] = {
      {0, 1},   {1, 3},   {0, 2},   {2, 4},  {5, 6},  {5, 7},
      {7, 9},   {6, 8},   {8, 10},  {5, 11}, {6, 12}, {11, 12},
      {11, 13}, {12, 14}, {13, 15}, {14, 16}};
  for (int i = 0; i < 16; i++) {
    const Keypoint& p1 = keypoints[joint_pairs[i][0]];
    const Keypoint& p2 = keypoints[joint_pairs[i][1]];
    if (p1.prob < threshold || p2.prob < threshold) continue;
    cv::line(image, cv::Point(p1.x, p1.y), cv::Point(p2.x, p2.y),
             cv::Scalar(255, 0, 0), 2);
  }
  // draw joint
  for (size_t i = 0; i < keypoints.size(); i++) {
    const Keypoint& keypoint = keypoints[i];
    // fprintf(stderr, "%.2f %.2f = %.5f\n", keypoint.p.x, keypoint.p.y,
    // keypoint.prob);
    if (keypoint.prob < threshold) continue;
    cv::circle(image, cv::Point(keypoint.x, keypoint.y), 3,
               cv::Scalar(0, 255, 0), -1);
  }
}