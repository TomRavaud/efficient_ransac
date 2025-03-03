#pragma once

#include "shape.h"

namespace efficient_ransac {

class Plane : public Shape {
 public:
  Plane(std::vector<pcl::PointNormal> candidate_points);

  bool isValid(std::vector<pcl::PointNormal> candidate_points,
               thresholds thresholds) override;

  void computeInliersIndices(
      const std::shared_ptr<pcl::PointCloud<pcl::PointNormal>> &cloud,
      const thresholds thresholds,
      const std::vector<bool> &remaining_points) override;

 private:
  inline bool distanceCheck(Eigen::Vector3f point, double threshold) {
    return std::abs(normal_.dot(point - point_)) < threshold;
  }
  inline bool normalCheck(Eigen::Vector3f normal, double threshold) {
    return acos(std::abs(normal_.dot(normal))) < threshold;
  }

  Eigen::Vector3f point_;
  Eigen::Vector3f normal_;
};

}  // namespace efficient_ransac
