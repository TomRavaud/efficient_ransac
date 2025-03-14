#include "plane.h"

namespace efficient_ransac {

Plane::Plane(std::vector<pcl::PointNormal> candidate_points,
             Thresholds thresholds, CellSize cell_size)
    : Shape(candidate_points, thresholds, cell_size) {
  // compute the plane parameters
  point_ = candidate_points[0].getVector3fMap();
  Eigen::Vector3f p0p1 = candidate_points[1].getVector3fMap() - point_;
  Eigen::Vector3f p0p2 = candidate_points[2].getVector3fMap() - point_;
  normal_ = p0p1.cross(p0p2);
  normal_.normalize();
}

bool Plane::isValid(std::vector<pcl::PointNormal> candidate_points) {
  for (auto point : candidate_points) {
    if (angle(point)>= thresholds_.normal) return false;
  }
  return true;
}

void Plane::extractLargestConnectedComponent(
    const std::shared_ptr<pcl::PointCloud<pcl::PointNormal>> &cloud) {
  // local coordinate system in the plane
  Eigen::Vector3f u, v;
  u = normal_.unitOrthogonal();
  v = normal_.cross(u);

  // fill the bitmap (only active cells are stored)
  std::unordered_map<CellCoord, std::vector<int>, CellCoordHasher> bitmap;
  for (int idx : inliers_indices_) {
    Eigen::Vector3f local_coords = cloud->at(idx).getVector3fMap() - point_;
    int cx = static_cast<int>(std::floor(local_coords.dot(u) / cell_size_.x));
    int cy = static_cast<int>(std::floor(local_coords.dot(v) / cell_size_.y));
    CellCoord key = {cx, cy};
    bitmap[key].push_back(idx);
  }

  // find the largest connected component
  inliers_indices_ = extractLargestConnectedComponentFromBitmap(bitmap);
}

}  // namespace efficient_ransac
