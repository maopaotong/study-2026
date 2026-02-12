#pragma once

#include "Common.h"
#include "delaunator/delaunator.h"
#include "DualMesh.h"
#include "PoissonDisk.h"
namespace mg
{

    struct MapGen
    {

        static std::vector<double> toFlatPoints(std::vector<PoissonDisk::Point> points)
        {
            std::vector<double> ps2;
            for (PoissonDisk::Point p : points)
            {
                ps2.push_back(p.x);
                ps2.push_back(p.y);
            }
            return ps2;
        }

        static std::vector<PoissonDisk::Point> generateInteriorBoundaryPoints(PoissonDisk::Bounds bounds, double boundarySpacing)
        {
            // https://www.redblobgames.com/x/2314-poisson-with-boundary/
            const double epsilon = 1e-4;
            const double curvature = 1.0;
            int W = std::ceil((bounds.width - 2 * curvature) / boundarySpacing);
            int32_t H = std::ceil((bounds.height - 2 * curvature) / boundarySpacing);
            std::vector<PoissonDisk::Point> points;
            // Top and bottom
            for (int q = 0; q < W; q++)
            {
                double t = static_cast<double>(q) / static_cast<double>(W);
                double dx = (bounds.width - 2 * curvature) * t;
                double dy = epsilon + curvature * 4 * std::pow((t - 0.5), 2);
                points.push_back({bounds.x + curvature + dx, bounds.y + dy});
                points.push_back({bounds.x + bounds.width - curvature - dx, bounds.y + bounds.height - dy});
            }
            // Left and right
            for (int r = 0; r < H; r++)
            {
                double t = static_cast<double>(r) / static_cast<double>(H);
                double dy = (bounds.height - 2 * curvature) * t;
                double dx = epsilon + curvature * 4 * std::pow((t - 0.5), 2);
                points.push_back({bounds.x + dx, bounds.y + bounds.height - curvature - dy});
                points.push_back({bounds.x + bounds.width - dx, bounds.y + curvature + dy});
            }
            return points;
        }

        static std::vector<PoissonDisk::Point> generateExteriorBoundaryPoints(PoissonDisk::Bounds bounds, double boundarySpacing)
        {
            // https://www.redblobgames.com/x/2314-poisson-with-boundary/
            const double curvature = 1.0;
            const double diagonal = boundarySpacing / std::sqrt(2);
            std::vector<PoissonDisk::Point> points;

            int W = std::ceil((bounds.width - 2 * curvature) / boundarySpacing);
            int H = std::ceil((bounds.height - 2 * curvature) / boundarySpacing);
            // Top and bottom
            for (int q = 0; q < W; q++)
            {
                double t = static_cast<double>(q) / static_cast<double>(W);
                double dx = (bounds.width - 2 * curvature) * t + boundarySpacing / 2;
                points.push_back({bounds.x + dx, bounds.y - diagonal});
                points.push_back({bounds.x + bounds.width - dx, bounds.y + bounds.height + diagonal});
            }
            // Left and right
            for (int r = 0; r < H; r++)
            {
                int t = static_cast<double>(r) / static_cast<double>(H);
                int dy = (bounds.height - 2 * curvature) * t + boundarySpacing / 2;
                points.push_back({bounds.x - diagonal, bounds.y + bounds.height - dy});
                points.push_back({bounds.x + bounds.width + diagonal, bounds.y + dy});
            }
            // Corners
            points.push_back({bounds.x - diagonal, bounds.y - diagonal});
            points.push_back({bounds.x + bounds.width + diagonal, bounds.y - diagonal});
            points.push_back({bounds.x - diagonal, bounds.y + bounds.height + diagonal});
            points.push_back({bounds.x + bounds.width + diagonal, bounds.y + bounds.height + diagonal});
            return points;
        }

        static DualMesh::Data generateDualData(Args args)
        {

            PoissonDisk::Bounds bounds(0, 0, args.meshWidth, args.meshWidth);
            std::vector<PoissonDisk::Point> interiorBoundaryPoints = generateInteriorBoundaryPoints(bounds, args.boundarySpacing);
            std::vector<PoissonDisk::Point> exteriorBoundaryPoints = generateExteriorBoundaryPoints(bounds, args.boundarySpacing);

            PoissonDisk::Generator<std::mt19937> mountainPointsGen(bounds, args.mountainSpacing, args.mountainRetries, std::mt19937{args.seed});
            for (auto p : interiorBoundaryPoints)
            {
                if (!mountainPointsGen.add_point(p, args.spacing * args.spacing))
                {
                    throw std::runtime_error("failed to add point.");
                }
            }

            std::vector<PoissonDisk::Point> interiorPoints = mountainPointsGen.fill();
            int numMoutainPoints = interiorPoints.size() - interiorBoundaryPoints.size();

            PoissonDisk::Generator<std::mt19937> pointsGen(bounds, args.spacing, args.retries, std::mt19937{args.seed});
            for (auto p : interiorPoints)
            {
                if (!pointsGen.add_point(p))
                {
                    throw std::runtime_error("failed to add point.");
                }
            }

            interiorPoints = pointsGen.fill();

            std::vector<PoissonDisk::Point> points;
            points.insert(points.end(), std::make_move_iterator(exteriorBoundaryPoints.begin()), std::make_move_iterator(exteriorBoundaryPoints.end()));
            points.insert(points.end(), std::make_move_iterator(interiorPoints.begin()), std::make_move_iterator(interiorPoints.end()));

            return DualMesh::Data{args.meshWidth, toFlatPoints(points), static_cast<int>(exteriorBoundaryPoints.size())};
        }
    };
};