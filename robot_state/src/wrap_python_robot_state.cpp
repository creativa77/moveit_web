/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2013, Silver South LLC
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of Silver South nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Julian Cerruti */

#include <boost/python.hpp>
#include <ros/ros.h>
#include <moveit/py_bindings_tools/roscpp_initializer.h>
#include <moveit/robot_state/robot_state.h>

namespace bp = boost::python;

namespace moveit_web
{

class WebRobotStateWrapper : protected moveit_py_bindings_tools::ROScppInitializer
{
protected:
  robot_state::RobotStatePtr _robot_state;
public:

  // ROSInitializer is constructed first, and ensures ros::init() was called, if needed
  WebRobotStateWrapper() : moveit_py_bindings_tools::ROScppInitializer()
  {
  }

  // Returns stamped poses for each of the links in the Robot for the current
  // state
  bp::dict getLinkPoses()
  {
    // TODO: Make this more standardized, perhaps more similar to a TF message
    /* Returns the pose of all links in global_link frame using a custom format:
     * { 'global_link': [
     *    ('<link_name>', tr.x, tr.y, tr.z, ro.x, ro.y, ro.z, ro.w),
     *    ...
     * ]}
     */
    bp::dict current_state_dict;
    bp::list link_state_list;
    std::vector<robot_state::LinkState*> linkStateVector = _robot_state->getLinkStateVector();
    for(int i=0; i < linkStateVector.size(); i++) {
      Eigen::Affine3d link_pose = linkStateVector[i]->getGlobalLinkTransform();
      bp::list link_state;
      // link_name:
      link_state.append(linkStateVector[i]->getName());
      // translation coordinates:
      link_state.append(link_pose.translation().x());
      link_state.append(link_pose.translation().y());
      link_state.append(link_pose.translation().z());
      // rotation coordinates:
      Eigen::Quaterniond link_rotation(link_pose.rotation());
      link_state.append(link_rotation.x());
      link_state.append(link_rotation.y());
      link_state.append(link_rotation.z());
      link_state.append(link_rotation.w());
      // add to list of states
      link_state_list.append(link_state);
    }
    current_state_dict["global_link"] = link_state_list;
    return current_state_dict;
  }

  bp::dict test()
  {
    bp::dict return_value;
    return_value["hello"] = "world";
    return return_value;
  }
};

void wrap_robot_state()
{
  void (*init_fn)(const std::string&, bp::list&) = &moveit_py_bindings_tools::roscpp_init;
  bp::def("roscpp_init", init_fn);
  bp::def("roscpp_shutdown", &moveit_py_bindings_tools::roscpp_shutdown);

  // bp::class_<WebRobotStateWrapper> WebRobotStateClass("WebRobotStateWrapper", bp::init<void>());
  bp::class_<WebRobotStateWrapper> WebRobotStateClass("WebRobotStateWrapper");

  WebRobotStateClass.def("test", &WebRobotStateWrapper::test);
  WebRobotStateClass.def("getLinkPoses", &WebRobotStateWrapper::getLinkPoses);
}

}

BOOST_PYTHON_MODULE(moveit_web_robot_state)
{
  using namespace moveit_web;
  wrap_robot_state();
}
