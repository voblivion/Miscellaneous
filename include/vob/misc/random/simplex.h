#pragma once

#include <array>
#include <cinttypes>
#include <cmath>


namespace vob::misrn::simplex
{
	namespace detail
	{
		std::array<std::array<std::int32_t, 3>, 12> gradients_3d = {
			std::array<std::int32_t, 3>{1, 1, 0},
			{-1, 1, 0}, {1, -1, 0}, {-1, -1, 0}, {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
			{0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1} };


		std::array<std::array<std::int32_t, 4>, 32> gradients_4d = {
			std::array<std::int32_t, 4>{0, 1, 1, 1},
			{0, 1, 1, -1}, {0, 1, -1, 1}, {0, 1, -1, -1}, {0, -1, 1, 1}, {0, -1, 1, -1},
			{0, -1, -1, 1}, {0, -1, -1, -1}, {1, 0, 1, 1}, {1, 0, 1, -1}, {1, 0, -1, 1},
			{1, 0, -1, -1}, {-1, 0, 1, 1}, {-1, 0, 1, -1}, {-1, 0, -1, 1}, {-1, 0, -1, -1},
			{1, 1, 0, 1}, {1, 1, 0, -1}, {1, -1, 0, 1}, {1, -1, 0, -1}, {-1, 1, 0, 1},
			{-1, 1, 0, -1}, {-1, -1, 0, 1}, {-1, -1, 0, -1}, {1, 1, 1, 0}, {1, 1, -1, 0},
			{1, -1, 1, 0}, {1, -1, -1, 0}, {-1, 1, 1, 0}, {-1, 1, -1, 0}, {-1, -1, 1, 0},
			{-1, -1, -1, 0} };

		std::array<std::int32_t, 512> perms = {
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,
			240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,
			33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,
			158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,
			63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,
			109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,
			59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,
			101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,
			246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
			49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,
			93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,
			240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,
			33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,
			158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,
			63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,
			109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,
			59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,
			101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,
			246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
			49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,
			93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

		std::array<std::array<std::int32_t, 6>, 8> orders_3d = {
			std::array<std::int32_t, 6>{1, 0, 0, 1, 1, 0},
			{1, 0, 0, 1, 1, 0}, {1, 0, 0, 1, 0, 1}, {0, 0, 1, 1, 0, 1},
			{0, 1, 0, 1, 1, 0}, {0, 1, 0, 0, 1, 1}, {0, 0, 1, 0, 1, 1}, {0, 0, 1, 0, 1, 1} };

		std::array<std::array<std::int32_t, 4>, 64> simplex_4d = {
			std::array<std::int32_t, 4>{0, 1, 2, 3}, {0, 1, 3, 2}, {0, 0, 0, 0}, {0, 2, 3, 1},
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 2, 3, 0}, {0, 2, 1, 3}, {0, 0, 0, 0},
			{0, 3, 1, 2}, {0, 3, 2, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 3, 2, 0},
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
			{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 2, 0, 3}, {0, 0, 0, 0}, {1, 3, 0, 2}, {0, 0, 0, 0},
			{0, 0, 0, 0}, {0, 0, 0, 0}, {2, 3, 0, 1}, {2, 3, 1, 0}, {1, 0, 2, 3}, {1, 0, 3, 2}, 
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {2, 0, 3, 1}, {0, 0, 0, 0}, {2, 1, 3, 0},
			{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
			{0, 0, 0, 0}, {0, 0, 0, 0}, {2, 0, 1, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
			{3, 0, 1, 2}, {3, 0, 2, 1}, {0, 0, 0, 0}, {3, 1, 2, 0}, {2, 1, 0, 3}, {0, 0, 0, 0},
			{0, 0, 0, 0}, {0, 0, 0, 0}, {3, 1, 0, 2}, {0, 0, 0, 0}, {3, 2, 0, 1}, {3, 2, 1, 0} };
	}

	inline double noise(double x, double y)
	{
		constexpr auto sqrt3 = 1.732050807568877293527446341505872366942805253810380628055806;

		constexpr auto f = 0.5 * (sqrt3 - 1.0);
		auto const s = (x + y) * f;

		auto const i = std::floor(x + s);
		auto const j = std::floor(y + s);

		constexpr auto g = (3.0 - sqrt3) / 6.0;
		auto const t = (i + j) * g;
		auto const ux = i - t;
		auto const uy = j - t;
		auto const x0 = x - ux;
		auto const y0 = y - uy;
		
		auto const i1 = x0 > y0 ? 1 : 0;
		auto const j1 = 1 - i1;

		auto const x1 = x0 - i1 + g;
		auto const y1 = y0 - j1 + g;
		auto const x2 = x0 - 1.0 + 2.0 * g;
		auto const y2 = y0 - 1.0 + 2.0 * g;

		auto const ii = static_cast<std::size_t>(i) & 255;
		auto const jj = static_cast<std::size_t>(j) & 255;
		auto const gi0 = detail::perms[ii + detail::perms[jj]] % 12;
		auto const gi1 = detail::perms[ii + i1 + detail::perms[jj + j1]] % 12;
		auto const gi2 = detail::perms[ii + 1 + detail::perms[jj + 1]] % 12;

		auto const n = [](auto const gs, auto const x, auto const y)
		{
			auto const t = 0.5 - x * x - y * y;
			return t < 0.0 ? 0.0 : t * t * t * t * (gs[0] * x + gs[1] * y);
		};

		auto const n0 = n(detail::gradients_3d[gi0], x0, y0);
		auto const n1 = n(detail::gradients_3d[gi1], x1, y1);
		auto const n2 = n(detail::gradients_3d[gi2], x2, y2);

		return 70.0 * (n0 + n1 + n2);
	}

	inline double noise(double x, double y, double z)
	{
		constexpr auto f = 1.0 / 3.0;
		auto const s = (x + y + z) * f;

		auto const i = std::floor(x + s);
		auto const j = std::floor(y + s);
		auto const k = std::floor(z + s);

		auto const g = 1.0 / 6.0;
		auto const t = (i + j + k) * g;
		
		auto const ux = i - t;
		auto const uy = j - t;
		auto const uz = k - t;
		auto const x0 = x - ux;
		auto const y0 = y - uy;
		auto const z0 = z - uz;

		auto const o = (x0 >= y0) << 2 | (y0 >= z0) << 1 | (x0 >= z0);
		auto const& orders = detail::orders_3d[o];
		auto const i1 = orders[0];
		auto const j1 = orders[1];
		auto const k1 = orders[2];
		auto const i2 = orders[3];
		auto const j2 = orders[4];
		auto const k2 = orders[5];

		auto const x1 = x0 + i1 + g;
		auto const y1 = y0 - j1 + g;
		auto const z1 = z0 - k1 + g;

		auto const x2 = x0 - i2 + 2.0 * g;
		auto const y2 = y0 - j2 + 2.0 * g;
		auto const z2 = z0 - k2 + 2.0 * g;

		auto const x3 = x0 - 1.0 + 3.0 * g;
		auto const y3 = y0 - 1.0 + 3.0 * g;
		auto const z3 = z0 - 1.0 + 3.0 * g;

		auto const ii = static_cast<std::size_t>(i) & 255;
		auto const jj = static_cast<std::size_t>(j) & 255;
		auto const kk = static_cast<std::size_t>(k) & 255;
		auto const gi0 = detail::perms[ii + detail::perms[jj + detail::perms[kk]]] % 12;
		auto const gi1 = detail::perms[ii + i1 + detail::perms[jj + j1 + detail::perms[kk + k1]]] % 12;
		auto const gi2 = detail::perms[ii + i2 + detail::perms[jj + j2 + detail::perms[kk + k2]]] % 12;
		auto const gi3 = detail::perms[ii + 1 + detail::perms[jj + 1 + detail::perms[kk + 1]]] % 12;

		auto const n = [](auto const gs, auto const x, auto const y, auto const z)
		{
			auto const t = 0.5 - x * x - y * y - z * z;
			return t < 0.0 ? 0.0 : t * t * t * t * (gs[0] * x + gs[1] * y + gs[2] * z);
		};

		auto const n0 = n(detail::gradients_3d[gi0], x0, y0, z0);
		auto const n1 = n(detail::gradients_3d[gi1], x1, y1, z1);
		auto const n2 = n(detail::gradients_3d[gi2], x2, y2, z2);
		auto const n3 = n(detail::gradients_3d[gi3], x3, y3, z3);

		return 32.0 * (n0 + n1 + n2 + n3);
	}

	inline double noise(double x, double y, double z, double w)
	{
		constexpr auto sqrt5 = 2.23606797749978969640917366873127623544061835961152572427089;
		auto const f = (sqrt5 - 1.0) / 4.0;
		auto const g = (5.0 - sqrt5) / 20.0;
		
		auto const s = (x + y + z + 1) * f;

		auto const i = std::floor(x + s);
		auto const j = std::floor(y + s);
		auto const k = std::floor(z + s);
		auto const l = std::floor(w + s);

		auto const t = (i + j + k + l) * g;
		auto const ux = i - t;
		auto const uy = j - t;
		auto const uz = k - t;
		auto const uw = l - t;

		auto const x0 = x - ux;
		auto const y0 = x - uy;
		auto const z0 = x - uz;
		auto const w0 = x - uw;

		auto const c1 = (x0 > y0) ? 32 : 1;
		auto const c2 = (x0 > z0) ? 16 : 1;
		auto const c3 = (y0 > z0) ? 8 : 1;
		auto const c4 = (x0 > w0) ? 4 : 1;
		auto const c5 = (y0 > w0) ? 2 : 1;
		auto const c = c1 + c2 + c3 + c4 + c5 + 1;

		auto const i1 = detail::simplex_4d[c][0] >= 3 ? 1 : 0;
		auto const j1 = detail::simplex_4d[c][0] >= 3 ? 1 : 0;
		auto const k1 = detail::simplex_4d[c][0] >= 3 ? 1 : 0;
		auto const l1 = detail::simplex_4d[c][0] >= 3 ? 1 : 0;
		auto const i2 = detail::simplex_4d[c][0] >= 2 ? 1 : 0;
		auto const j2 = detail::simplex_4d[c][0] >= 2 ? 1 : 0;
		auto const k2 = detail::simplex_4d[c][0] >= 2 ? 1 : 0;
		auto const l2 = detail::simplex_4d[c][0] >= 2 ? 1 : 0;
		auto const i3 = detail::simplex_4d[c][0] >= 1 ? 1 : 0;
		auto const j3 = detail::simplex_4d[c][0] >= 1 ? 1 : 0;
		auto const k3 = detail::simplex_4d[c][0] >= 1 ? 1 : 0;
		auto const l3 = detail::simplex_4d[c][0] >= 1 ? 1 : 0;

		auto const x1 = x0 - i1 + g;
		auto const y1 = y0 - j1 + g;
		auto const z1 = z0 - k1 + g;
		auto const w1 = w0 - l1 + g;
		auto const x2 = x0 - i2 + 2.0 * g;
		auto const y2 = y0 - j2 + 2.0 * g;
		auto const z2 = z0 - k2 + 2.0 * g;
		auto const w2 = w0 - l2 + 2.0 * g;
		auto const x3 = x0 - i3 + 3.0 * g;
		auto const y3 = y0 - j3 + 3.0 * g;
		auto const z3 = z0 - k3 + 3.0 * g;
		auto const w3 = w0 - l3 + 3.0 * g;
		auto const x4 = x0 - 1.0 + 4.0 * g;
		auto const y4 = y0 - 1.0 + 4.0 * g;
		auto const z4 = z0 - 1.0 + 4.0 * g;
		auto const w4 = w0 - 1.0 + 4.0 * g;

		auto const ii = static_cast<std::size_t>(i) & 255;
		auto const jj = static_cast<std::size_t>(j) & 255;
		auto const kk = static_cast<std::size_t>(k) & 255;
		auto const ll = static_cast<std::size_t>(l) & 255;
		auto const gi0 = detail::perms[
			ii + detail::perms[jj + detail::perms[kk + detail::perms[ll]]]] % 32;
		auto const gi1 = detail::perms[
			ii + i1 + detail::perms[jj + j1 + detail::perms[kk + k1 + detail::perms[ll + l1]]]] % 32;
		auto const gi2 = detail::perms[
			ii + i2 + detail::perms[jj + j2 + detail::perms[kk + k2 + detail::perms[ll + l2]]]] % 32;
		auto const gi3 = detail::perms[
			ii + i3 + detail::perms[jj + j3 + detail::perms[kk + k3 + detail::perms[ll + l3]]]] % 32;
		auto const gi4 = detail::perms[
			ii + 1 + detail::perms[jj + 1 + detail::perms[kk + 1 + detail::perms[ll + 1]]]] % 32;

		auto const n = [](auto const& gs, auto const x, auto const y, auto const z, auto const w)
		{
			auto const t = 0.5 - x * x - y * y - z * z - w * w;
			return t < 0.0 ? 0.0 : t * t * t * t * (gs[0] * x + gs[1] * y + gs[2] * z + gs[3] * w);
		};

		auto const n0 = n(detail::gradients_4d[gi0], x0, y0, z0, w0);
		auto const n1 = n(detail::gradients_4d[gi1], x1, y1, z1, w1);
		auto const n2 = n(detail::gradients_4d[gi2], x2, y2, z2, w2);
		auto const n3 = n(detail::gradients_4d[gi3], x3, y3, z3, w3);
		auto const n4 = n(detail::gradients_4d[gi4], x4, y4, z4, w4);

		return 27.0 * (n0 + n1 + n2 + n3 + n4);
	}
}