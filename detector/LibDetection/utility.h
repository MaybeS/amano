#pragma once

#include <torch/script.h>

torch::Tensor nms(const at::Tensor& boxes, const at::Tensor& scores, const float threshold);
