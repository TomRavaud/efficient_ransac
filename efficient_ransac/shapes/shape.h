#pragma once

#include <pcl/octree/octree_search.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <Eigen/Dense>
#include <unordered_map>
#include <unsupported/Eigen/NonLinearOptimization>

#include "bitmap.h"

namespace efficient_ransac {

struct Thresholds {
  float distance = 0.0;
  float normal = 0.0;
};

class Shape {
 public:
  Shape(std::vector<pcl::PointNormal> candidate_points, Thresholds thresholds,
        CellSize cell_size) {
    thresholds_ = thresholds;
    cell_size_ = cell_size;
  };

  virtual bool isValid(std::vector<pcl::PointNormal> candidate_points) = 0;

  void computeInliersIndices(
      const std::shared_ptr<pcl::PointCloud<pcl::PointNormal>> &cloud,
      const std::shared_ptr<
          pcl::octree::OctreePointCloudSearch<pcl::PointNormal>> &octree,
      const std::vector<bool> &remaining_points,
      const bool compute_connected_components = true);

  void computeExpectationScore(
      const std::shared_ptr<pcl::PointCloud<pcl::PointNormal>> &cloud,
      const std::shared_ptr<
          pcl::octree::OctreePointCloudSearch<pcl::PointNormal>> &small_octree,
      const std::vector<bool> &remaining_points,
      const std::vector<int> &subsampled_indices);

  virtual void refit(
      const std::shared_ptr<pcl::PointCloud<pcl::PointNormal>> &cloud,
      const std::shared_ptr<
          pcl::octree::OctreePointCloudSearch<pcl::PointNormal>> &octree,
      const std::vector<bool> &remaining_points) = 0;

  // Getters
  std::vector<int> inliers_indices() { return inliers_indices_; }
  int expected_score() { return expected_score_; }

 protected:
  virtual inline float distance(pcl::PointNormal point) = 0;
  virtual inline float angle(pcl::PointNormal normal) = 0;

  virtual void extractLargestConnectedComponent(
      const std::shared_ptr<pcl::PointCloud<pcl::PointNormal>> &cloud) = 0;

  void getIndicesCloseToShape(
      const std::shared_ptr<
          pcl::octree::OctreePointCloudSearch<pcl::PointNormal>> &octree,
      const std::vector<bool> &remaining_points,
      std::vector<int> &indices_close_to_shape);

  void getIndicesCloseToShapeSmall(
      const std::shared_ptr<
          pcl::octree::OctreePointCloudSearch<pcl::PointNormal>> &small_octree,
      const std::vector<bool> &remaining_points,
      const std::vector<int> &subsampled_indices,
      std::vector<int> &indices_close_to_shape);

  std::vector<int> inliers_indices_;

  int expected_score_;

  Thresholds thresholds_;
  CellSize cell_size_;
};

}  // namespace efficient_ransac
