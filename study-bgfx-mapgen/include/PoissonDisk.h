// poisson_disk.hpp
// A modern C++20 header-only implementation of 2D Poisson Disk Sampling
// with support for interior/exterior boundary constraints.
//
// Algorithm: Robert Bridson (2007) + boundary-aware sampling (Red Blob Games style)
// License: MIT
// Usage:
//   auto points = poisson_disk::sample(
//       {0, 0, 1000, 1000},      // bounds = {x, y, width, height}
//       50.0,                    // min distance
//       30,                      // max attempts per point
//       std::mt19937{seed}       // RNG
//   );
//
// Optional: add fixed points (e.g., mountain peaks, boundary points)
//   poisson_disk::Generator gen(bounds, min_dist, k, rng);
//   gen.add_point({100, 200});
//   auto points = gen.fill();
#pragma once

#include <vector>
#include <random>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <numbers>

namespace mg
{
    struct PoissonDisk
    {

        struct Point
        {
            double x, y;
            Point() = default;
            Point(double x, double y) : x(x), y(y) {}
        };

        struct Bounds
        {
            double x, y, width, height;
            Bounds(double x, double y, double w, double h) : x(x), y(y), width(w), height(h) {}
            bool contains(const Point &p) const
            {
                return p.x >= x && p.x < x + width && p.y >= y && p.y < y + height;
            }
        };

        // Grid-based acceleration structure
        struct Grid
        {
            std::vector<std::vector<int>> cells;
            double cell_size;
            int grid_width, grid_height;

            Grid(double width, double height, double min_dist)
                : cell_size(min_dist / std::sqrt(2.0)),
                  grid_width(static_cast<int>(std::ceil(width / cell_size))),
                  grid_height(static_cast<int>(std::ceil(height / cell_size)))
            {
                cells.assign(grid_width * grid_height, std::vector<int>{});
            }

            int index(int x, int y) const
            {
                if (x < 0 || x >= grid_width || y < 0 || y >= grid_height)
                    return -1;
                return y * grid_width + x;
            }

            void insert(const Point &p, int point_index, double offset_x, double offset_y)
            {
                int gx = static_cast<int>((p.x - offset_x) / cell_size);
                int gy = static_cast<int>((p.y - offset_y) / cell_size);
                int idx = index(gx, gy);
                if (idx != -1)
                    cells[idx].push_back(point_index);
            }

            bool is_valid(const Point &p, const std::vector<Point> &points,
                          double min_dist_sq, double offset_x, double offset_y) const
            {
                int gx = static_cast<int>((p.x - offset_x) / cell_size);
                int gy = static_cast<int>((p.y - offset_y) / cell_size);

                for (int dy = -2; dy <= 2; ++dy)
                {
                    for (int dx = -2; dx <= 2; ++dx)
                    {
                        int idx = index(gx + dx, gy + dy);
                        if (idx == -1)
                            continue;
                        for (int i : cells[idx])
                        {
                            const auto &q = points[i];
                            double dx_ = p.x - q.x;
                            double dy_ = p.y - q.y;
                            if (dx_ * dx_ + dy_ * dy_ < min_dist_sq)
                            {
                                return false;
                            }
                        }
                    }
                }
                return true;
            }
        };

        template <typename RNG>
        class Generator
        {
            Bounds bounds_;
            double min_dist_;
            double min_dist_sq_;
            int k_;
            RNG rng_;
            std::uniform_real_distribution<double> uniform_;
            std::vector<Point> points_;
            Grid grid_;

        public:
            Generator(Bounds bounds, double min_dist, int k, RNG rng)
                : bounds_(bounds),
                  min_dist_(min_dist),
                  min_dist_sq_(min_dist * min_dist),
                  k_(k),
                  rng_(rng),
                  uniform_(0.0, 1.0),
                  grid_(bounds.width, bounds.height, min_dist) {}

            // Add a fixed point (e.g., boundary or mountain)
            // Returns true on success, false if too close to existing point
            bool add_point(Point p)
            {
                return add_point(p, min_dist_sq_);
            }

            bool add_point(Point p, double minDistSqr)
            {
                if (!grid_.is_valid(p, points_, minDistSqr, bounds_.x, bounds_.y))
                {
                    return false;
                }
                points_.push_back(p);
                grid_.insert(p, static_cast<int>(points_.size()) - 1, bounds_.x, bounds_.y);
                return true;
            }

            // Generate all remaining points
            std::vector<Point> fill()
            {
                std::vector<Point> active_list(points_.begin(), points_.end());

                std::uniform_real_distribution<double> angle_dist(0.0, 2.0 * std::numbers::pi);
                std::uniform_real_distribution<double> radius_dist(min_dist_, 2.0 * min_dist_);

                while (!active_list.empty())
                {
                    // Pick random active point
                    size_t idx = static_cast<size_t>(uniform_(rng_) * active_list.size());
                    Point p = active_list[idx];

                    bool found = false;
                    for (int i = 0; i < k_; ++i)
                    {
                        double angle = angle_dist(rng_);
                        double radius = radius_dist(rng_);
                        Point candidate{
                            p.x + radius * std::cos(angle),
                            p.y + radius * std::sin(angle)};

                        if (!bounds_.contains(candidate))
                            continue;
                        if (!grid_.is_valid(candidate, points_, min_dist_sq_, bounds_.x, bounds_.y))
                            continue;

                        // Accept point
                        points_.push_back(candidate);
                        grid_.insert(candidate, static_cast<int>(points_.size()) - 1, bounds_.x, bounds_.y);
                        active_list.push_back(candidate);
                        found = true;
                        break;
                    }

                    if (!found)
                    {
                        // Remove from active list
                        active_list[idx] = active_list.back();
                        active_list.pop_back();
                    }
                }

                return points_;
            }
        };

        // Convenience function
        template <typename RNG = std::mt19937>
        std::vector<Point> sample(Bounds bounds, double min_dist, int k = 30, RNG rng = {})
        {
            Generator<RNG> gen(bounds, min_dist, k, rng);
            return gen.fill();
        }
    };
} // namespace
