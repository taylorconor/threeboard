// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <gtest/gtest.h>

#include "absl/status/status.h"

// Macros for backend tests to be used when we expect the status to be OK.
#define EXPECT_OK(status)                              \
  do {                                                 \
    const absl::Status& status_ = (status);            \
    EXPECT_TRUE(status_.ok()) << "Error: " << status_; \
  } while (0)

#define ASSERT_OK(status)                              \
  do {                                                 \
    const absl::Status& status_ = (status);            \
    ASSERT_TRUE(status_.ok()) << "Error: " << status_; \
  } while (0)

// Error code matches.
#define EXPECT_ERROR(status, err)                           \
  do {                                                      \
    const absl::Status& status_ = (status);                 \
    EXPECT_EQ(status_.code(), err) << "Error: " << status_; \
  } while (0)

// Text of error message matches.
#define ASSERT_ERROR_MSG(status, err)                                        \
  do {                                                                       \
    const absl::Status& status_ = (status);                                  \
    ASSERT_FALSE(status_.ok());                                              \
    ASSERT_EQ(status_.msg().msg(), err) << "Error: " << status_.msg().msg(); \
  } while (0)
