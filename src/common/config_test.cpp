// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "config.hpp"

using std::string;

class config_trivial : public testing::Test {
 protected:
  void SetUp() {
    zk_ = pfi::lang::shared_ptr<jubatus::common::lock_service>(
        jubatus::common::create_lock_service("zk", "localhost:2181", 10,
                                             "/dev/null"));

    engine_ = "engine-name";
    name_ = "test-name";
    jubatus::common::build_config_path(path_, engine_, name_);
  }

  void TearDown() {
    if (!zk_)
      return;

    if (zk_->exists(path_))
      zk_->remove(path_);
  }

  string path_;
  string engine_;
  string name_;
  pfi::lang::shared_ptr<jubatus::common::lock_service> zk_;
};

TEST_F(config_trivial, config_tozk) {
  istringstream config_str("{\"test\":\"config\"}");
  std::string config = config_str.str();

  jubatus::common::prepare_jubatus(*zk_, engine_, name_);
  jubatus::common::config_tozk(*zk_, engine_, name_, config);

  ASSERT_EQ(true, zk_->exists(path_));

  std::string dat;
  zk_->read(path_, dat);
  ASSERT_EQ("{\"test\":\"config\"}", dat);
}

TEST_F(config_trivial, config_fromzk) {
  istringstream config_str("{\"test\":\"config\"}");
  std::string config = config_str.str();

  jubatus::common::prepare_jubatus(*zk_, engine_, name_);
  zk_->create(path_, config);

  std::string dat;
  jubatus::common::config_fromzk(*zk_, engine_, name_, dat);

  ASSERT_EQ("{\"test\":\"config\"}", dat);
}

TEST_F(config_trivial, remove_config_fromzk) {
  istringstream config_str("{\"test\":\"config\"}");
  std::string config = config_str.str();

  jubatus::common::prepare_jubatus(*zk_, engine_, name_);
  zk_->create(path_, config);

  std::string dat;
  jubatus::common::remove_config_fromzk(*zk_, engine_, name_);

  ASSERT_EQ(true, !zk_->exists(path_));
}
