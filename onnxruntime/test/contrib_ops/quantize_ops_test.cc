// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "gtest/gtest.h"
#include "test/common/tensor_op_test_utils.h"
#include "test/providers/provider_test_utils.h"

namespace onnxruntime {
namespace test {

// scalar zero & scale with uint8
TEST(DequantizeLinearOpTest, DequantizeLinear_per_tensor_float_uint8) {
  OpTester test("DequantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{4};
  test.AddInput<uint8_t>("x", dims, {0, 3, 128, 255});
  test.AddInput<float>("x_scale", {}, {2.0f});
  test.AddInput<uint8_t>("x_zero_point", {}, {128});
  test.AddOutput<float>("y", dims, {-256.0f, -250.0f, 0.0f, 254.0f});
  test.Run();
}

// scalar zero & scale with int8
TEST(DequantizeLinearOpTest, DequantizeLinear_per_tensor_float_int8) {
  OpTester test("DequantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{4};
  test.AddInput<int8_t>("x", dims, {-30, -3, 100, 127});
  test.AddInput<float>("x_scale", {}, {2.0f});
  test.AddInput<int8_t>("x_zero_point", {}, {-10});
  test.AddOutput<float>("y", dims, {-40.0f, 14.0f, 220.0f, 274.0f});
  test.Run();
}

#ifdef USE_CUDA
TEST(DequantizeLinearOpTest, DequantizeLinear_per_tensor_half_uint8) {
  OpTester test("DequantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{4};
  test.AddInput<uint8_t>("x", dims, {0, 3, 128, 255});
  test.AddInput<MLFloat16>("x_scale", {}, ToFloat16({2.0f}));
  test.AddInput<uint8_t>("x_zero_point", {}, {128});
  test.AddOutput<MLFloat16>("y", dims, ToFloat16({-256.0f, -250.0f, 0.0f, 254.0f}));
  test.Run();
}

// scalar zero & scale with int8
TEST(DequantizeLinearOpTest, DequantizeLinear_per_tensor_half_int8) {
  OpTester test("DequantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{4};
  test.AddInput<int8_t>("x", dims, {-30, -3, 100, 127});
  test.AddInput<MLFloat16>("x_scale", {}, ToFloat16({2.0f}));
  test.AddInput<int8_t>("x_zero_point", {}, {-10});
  test.AddOutput<MLFloat16>("y", dims, ToFloat16({-40.0f, 14.0f, 220.0f, 274.0f}));
  test.Run();
}
#endif

// 1d zero & scale with uint8 broadcast axis 0
TEST(DequantizeLinearContribOpTest, DequantizeLinear_0) {
  OpTester test("DequantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{3, 4};
  test.AddInput<uint8_t>("X", dims,
                         {0, 1, 2, 3,
                          0, 1, 2, 3,
                          0, 10, 20, 30});
  test.AddAttribute<int64_t>("axis", 0);
  test.AddInput<float>("scale", {3},
                       {1.0f,
                        2.0f,
                        4.0f});
  test.AddInput<uint8_t>("zero_point", {3},
                         {0,
                          0,
                          0});
  test.AddOutput<float>("Y", dims,
                        {0, 1, 2, 3,
                         0, 2, 4, 6,
                         0, 40, 80, 120});
  test.Run();
}

// 1d zero & scale with int8 broadcast axis 1
TEST(DequantizeLinearContribOpTest, DequantizeLinear_1) {
  OpTester test("DequantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{3, 4};
  test.AddInput<int8_t>("X", dims,
                        {0, 1, 2, 3,
                         0, 2, 4, 6,
                         0, 10, 20, 30});
  test.AddAttribute<int64_t>("axis", 1);
  test.AddInput<float>("scale", {4}, {1, 2, 4, 8});
  test.AddInput<int8_t>("zero_point", {4}, {0, -10, -20, -30});
  test.AddOutput<float>("Y", dims,
                        {0, 22, 88, 264,
                         0, 24, 96, 288,
                         0, 40, 160, 480});
  test.Run();
}

// 1d zero & scale with int8 broadcast axis 0 with 4d tensor input/output
TEST(DequantizeLinearContribOpTest, DequantizeLinear_2) {
  OpTester test("DequantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{2, 3, 2, 4};
  test.AddInput<int8_t>("X", dims,
                        {7, 9, 10, 10,
                         5, 8, 9, 1,

                         8, 6, 7, 9,
                         10, 0, 7, 10,

                         8, 2, 6, 0,
                         5, 9, 8, 1,

                         2, 7, 5, 3,
                         2, 4, 1, 3,

                         8, 7, 4, 8,
                         10, 1, 5, 5,

                         7, 7, 0, 2,
                         4, 4, 0, 5});
  test.AddAttribute<int64_t>("axis", 1);
  test.AddInput<float>("scale", {3}, {1, 10, 7});
  test.AddInput<int8_t>("zero_point", {3}, {10, 2, 1});
  test.AddOutput<float>("Y", dims,
                        {-3, -1, 0, 0,
                         -5, -2, -1, -9,

                         60, 40, 50, 70,
                         80, -20, 50, 80,

                         49, 7, 35, -7,
                         28, 56, 49, 0,

                         -8, -3, -5, -7,
                         -8, -6, -9, -7,

                         60, 50, 20, 60,
                         80, -10, 30, 30,

                         42, 42, -7, 7,
                         21, 21, -7, 28});
  test.Run();
}

// 1d zero & scale with uint8 broadcast axis -2 (-2 resolves to axis 0)
TEST(DequantizeLinearContribOpTest, DequantizeLinear_3) {
  OpTester test("DequantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{3, 4};
  test.AddInput<uint8_t>("X", dims,
                         {0, 1, 2, 3,
                          0, 1, 2, 3,
                          0, 10, 20, 30});
  test.AddAttribute<int64_t>("axis", -2);
  test.AddInput<float>("scale", {3},
                       {1.0f,
                        2.0f,
                        4.0f});
  test.AddInput<uint8_t>("zero_point", {3},
                         {0,
                          0,
                          0});
  test.AddOutput<float>("Y", dims,
                        {0, 1, 2, 3,
                         0, 2, 4, 6,
                         0, 40, 80, 120});
  test.Run();
}

// quantize with scalar zero point and scale
TEST(QuantizeLinearContribOpTest, QuantizeLinear_per_tensor_float_uint8) {
  OpTester test("QuantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{6};
  test.AddInput<float>("x", dims, {0, 2, 3, 1000, -254, -1000});
  test.AddInput<float>("y_scale", {}, {2.0f});
  test.AddInput<uint8_t>("y_zero_point", {}, {128});
  test.AddOutput<uint8_t>("y", dims, {128, 129, 130, 255, 1, 0});
  test.Run();
}

TEST(QuantizeLinearContribOpTest, QuantizeLinear_per_tensor_float_int8) {
  OpTester test("QuantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{6};
  test.AddInput<float>("x", dims, {0, 2, 3, 1000, -254, -1000});
  test.AddInput<float>("y_scale", {}, {2.0f});
  test.AddInput<int8_t>("y_zero_point", {}, {1});
  test.AddOutput<int8_t>("y", dims, {1, 2, 3, 127, -126, -127});
  test.Run();
}

#ifdef USE_CUDA
// quantize with scalar zero point and scale
TEST(QuantizeLinearContribOpTest, QuantizeLinear_per_tensor_half_uint8) {
  OpTester test("QuantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{6};
  test.AddInput<MLFloat16>("x", dims, ToFloat16({0, 2, 3, 1000, -254, -1000}));
  test.AddInput<MLFloat16>("y_scale", {}, ToFloat16({2.0f}));
  test.AddInput<uint8_t>("y_zero_point", {}, {128});
  test.AddOutput<uint8_t>("y", dims, {128, 129, 130, 255, 1, 0});
  test.Run(OpTester::ExpectResult::kExpectSuccess, "", {kTensorrtExecutionProvider});
}

TEST(QuantizeLinearContribOpTest, QuantizeLinear_per_tensor_half_int8) {
  OpTester test("QuantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{6};
  test.AddInput<MLFloat16>("x", dims, ToFloat16({0, 2, 3, 1000, -254, -1000}));
  test.AddInput<MLFloat16>("y_scale", {}, ToFloat16({2.0f}));
  test.AddInput<int8_t>("y_zero_point", {}, {1});
  test.AddOutput<int8_t>("y", dims, {1, 2, 3, 127, -126, -128});
  test.Run(OpTester::ExpectResult::kExpectSuccess, "", {kTensorrtExecutionProvider});
}
#endif

// quantize with broadcasting
TEST(QuantizeLinearContribOpTest, QuantizeLinear_per_channel) {
  OpTester test("QuantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{3, 4};
  test.AddInput<float>("X", dims,
                       {0, 2, 3, 1000,
                        0, 2, 3, 1000,
                        0, 2, 3, 1000});
  test.AddAttribute<int64_t>("axis", 0);
  test.AddInput<float>("scale", {3}, {1, 2, 4});
  test.AddInput<uint8_t>("zero_point", {3}, {0, 0, 0});
  test.AddOutput<uint8_t>("Y", dims,
                          {0, 2, 3, 255,
                           0, 1, 2, 255,
                           0, 1, 1, 250});
  test.Run();
}

// quantize with broadcasting and negative axis (-2 resolves to axis 0)
TEST(QuantizeLinearContribOpTest, QuantizeLinear_per_channel_negative_axis) {
  OpTester test("QuantizeLinear", 1, onnxruntime::kMSDomain);
  std::vector<int64_t> dims{3, 4};
  test.AddInput<float>("X", dims,
                       {0, 2, 3, 1000,
                        0, 2, 3, 1000,
                        0, 2, 3, 1000});
  test.AddAttribute<int64_t>("axis", -2);
  test.AddInput<float>("scale", {3}, {1, 2, 4});
  test.AddInput<uint8_t>("zero_point", {3}, {0, 0, 0});
  test.AddOutput<uint8_t>("Y", dims,
                          {0, 2, 3, 255,
                           0, 1, 2, 255,
                           0, 1, 1, 250});
  test.Run();
}
}  // namespace test
}  // namespace onnxruntime