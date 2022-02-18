// Copyright 2022 Chen Jun

#ifndef ARMOR_PROCESSOR__ARMOR_PROCESSOR_NODE_HPP_
#define ARMOR_PROCESSOR__ARMOR_PROCESSOR_NODE_HPP_

// ROS
#include <message_filters/subscriber.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/create_timer_ros.h>
#include <tf2_ros/message_filter.h>
#include <tf2_ros/transform_listener.h>

#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/detail/marker_array__struct.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

// STD
#include <memory>
#include <string>
#include <vector>

#include "armor_processor/kalman_filter.hpp"
#include "armor_processor/tracker.hpp"
#include "auto_aim_interfaces/msg/armors.hpp"
#include "auto_aim_interfaces/msg/target.hpp"

namespace rm_auto_aim
{
using tf2_filter = tf2_ros::MessageFilter<auto_aim_interfaces::msg::Armors>;
class ArmorProcessorNode : public rclcpp::Node
{
public:
  explicit ArmorProcessorNode(const rclcpp::NodeOptions & options);

private:
  void armorsCallback(const auto_aim_interfaces::msg::Armors::SharedPtr armors_ptr);

  void deleteMarkers();
  void publishMarkers(const rclcpp::Time & time, const Eigen::VectorXd & state);
  void publishTarget(const Eigen::VectorXd & kf_state);

  // Last time received msg
  rclcpp::Time last_time_;

  // KalmanFilter
  std::unique_ptr<KalmanFilter> kf_;
  double dt_;
  // Initial KF matrices
  Eigen::Matrix<double, 6, 6> A_;       // state transition matrix
  Eigen::Matrix<double, 3, 6> H_;       // measurement matrix
  Eigen::DiagonalMatrix<double, 6> Q_;  // process noise covariance matrix
  Eigen::DiagonalMatrix<double, 3> R_;  // measurement noise covariance matrix
  Eigen::DiagonalMatrix<double, 6> P_;  // error estimate covariance matrix
  // KF output
  Eigen::VectorXd kf_prediction_;
  Eigen::VectorXd kf_corretion_;

  std::unique_ptr<Tracker> tracker_;

  // Subscriber with tf2 message_filter
  std::string target_frame_;
  std::shared_ptr<tf2_ros::Buffer> tf2_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf2_listener_;
  message_filters::Subscriber<auto_aim_interfaces::msg::Armors> armors_sub_;
  std::shared_ptr<tf2_filter> tf2_filter_;

  // Publisher
  auto_aim_interfaces::msg::Target target_msg_;
  rclcpp::Publisher<auto_aim_interfaces::msg::Target>::SharedPtr target_pub_;

  // Visualization marker publisher
  visualization_msgs::msg::Marker position_marker_;
  visualization_msgs::msg::Marker velocity_marker_;
  visualization_msgs::msg::MarkerArray marker_array_;
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;

  // Debug information publishers
  bool debug_;
};

}  // namespace rm_auto_aim

#endif  // ARMOR_PROCESSOR__ARMOR_PROCESSOR_NODE_HPP_
