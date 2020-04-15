#include "utility.h"

template <typename scalar_t>
at::Tensor nms_kernel(const at::Tensor& boxes, const at::Tensor& scores, const float threshold) {
	AT_ASSERTM(boxes.type() == scores.type(), "boxes should have the same type as scores");

	if (boxes.numel() == 0) {
		return at::empty({ 0 }, boxes.options().dtype(at::kLong));
	}

	auto x1_t = boxes.select(1, 0).contiguous();
	auto y1_t = boxes.select(1, 1).contiguous();
	auto x2_t = boxes.select(1, 2).contiguous();
	auto y2_t = boxes.select(1, 3).contiguous();

	at::Tensor areas_t = (x2_t - x1_t) * (y2_t - y1_t);

	auto order_t = std::get<1>(scores.sort(0, true));

	auto nboxes = boxes.size(0);
	at::Tensor suppressed_t = at::zeros({ nboxes }, boxes.options().dtype(at::kByte));
	at::Tensor keep_t = at::zeros({ nboxes }, boxes.options().dtype(at::kLong));

	auto suppressed = suppressed_t.data_ptr<uint8_t>();
	auto keep = keep_t.data_ptr<int64_t>();
	auto order = order_t.data_ptr<int64_t>();

	auto x1 = x1_t.data_ptr<scalar_t>();
	auto y1 = y1_t.data_ptr<scalar_t>();
	auto x2 = x2_t.data_ptr<scalar_t>();
	auto y2 = y2_t.data_ptr<scalar_t>();

	auto areas = areas_t.data_ptr<scalar_t>();

	int64_t num_to_keep = 0;

	for (int64_t _i = 0; _i < nboxes; _i++) {
		auto i = order[_i];

		if (suppressed[i] == 1)
			continue;

		keep[num_to_keep++] = i;
		auto ix1 = x1[i];
		auto iy1 = y1[i];
		auto ix2 = x2[i];
		auto iy2 = y2[i];
		auto iarea = areas[i];

		for (int64_t j = _i + 1; j < nboxes; j++) {

			if (suppressed[j] == 1)
				continue;

			auto xx1 = std::max(ix1, x1[order[j]]);
			auto yy1 = std::max(iy1, y1[order[j]]);
			auto xx2 = std::min(ix2, x2[order[j]]);
			auto yy2 = std::min(iy2, y2[order[j]]);

			auto w = std::max(static_cast<scalar_t>(0), xx2 - xx1);
			auto h = std::max(static_cast<scalar_t>(0), yy2 - yy1);
			auto inter = w * h;
			auto ovr = inter / (iarea + areas[order[j]] - inter);

			if (ovr >= threshold)
				suppressed[order[j]] = 1;
		}
	}

	return keep_t.narrow(0, 0, num_to_keep);
}

at::Tensor nms(const at::Tensor& boxes, const at::Tensor& scores, const float threshold) {
	auto result = at::empty({ 0 }, boxes.options());

	AT_DISPATCH_FLOATING_TYPES(boxes.scalar_type(), "nms", [&] {
		result = nms_kernel<scalar_t>(boxes, scores, threshold);
		});

	return result;
}