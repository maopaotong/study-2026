#pragma once
#include <algorithm>
#include "DualMesh.h"
#include "simplexnoise/SimplexNoise.h"

namespace mg
{
    using namespace simplexnoise;
    struct ElevationGen
    {

        static float noise2D(float x, float y)
        {
            return SimplexNoise::noise(x, y);
        }
        struct Args
        {

            float noisy_coastlines;
            float mountain_sharpness;
            float hill_height;
            float ocean_depth;
            Args() : noisy_coastlines(0.01f),
                     mountain_sharpness(9.8f),
                     hill_height(0.02),
                     ocean_depth(1.40)
            {
            }
        };
        struct Constraints
        {

            static float fbm_noise(std::vector<float> &amplitudes, float nx, float ny)
            {
                float sum = 0, sumOfAmplitudes = 0;
                for (int octave = 0; octave < amplitudes.size(); octave++)
                {
                    int frequency = 1 << octave;
                    sum += amplitudes[octave] * noise2D(nx * frequency, ny * frequency);
                    sumOfAmplitudes += amplitudes[octave];
                }
                return sum / sumOfAmplitudes;
            }

            std::vector<float> elevation;
            int size;
            Constraints(int CANVAS_SIZE, float island) : elevation(CANVAS_SIZE * CANVAS_SIZE, 0.0f), size(CANVAS_SIZE)
            {
                float persistence = 0.5f;
                std::vector<float> amplitudes;
                for (int i = 0; i < 5; i++)
                {
                    amplitudes.push_back(std::pow(persistence, i));
                }

                for (int y = 0; y < CANVAS_SIZE; y++)
                {
                    for (int x = 0; x < CANVAS_SIZE; x++)
                    {
                        int p = y * CANVAS_SIZE + x;
                        float nx = 2 * x / CANVAS_SIZE - 1,
                              ny = 2 * y / CANVAS_SIZE - 1;
                        float distance = std::max(std::abs(nx), std::abs(ny));
                        float e = 0.5 * (fbm_noise(amplitudes, nx, ny) + island * (0.75 - 2 * distance * distance));
                        if (e < -1.0)
                        {
                            e = -1.0;
                        }
                        if (e > +1.0)
                        {
                            e = +1.0;
                        }
                        elevation[p] = e;
                        if (e > 0.0)
                        {
                            float m = (0.5 * noise2D(nx + 30, ny + 50) + 0.5 * noise2D(2 * nx + 33, 2 * ny + 55));
                            // TODO: make some of these into parameters
                            float mountain = std::min(1.0, e * 5.0) * (1 - std::abs(m) / 0.5);
                            if (mountain > 0.0)
                            {
                                elevation[p] = std::max(e, std::min(e * 3, mountain));
                            }
                        }
                    }
                }
            }

            float constraintAt(float x, float y)
            {
                // https://en.wikipedia.org/wiki/Bilinear_interpolation
                // NOTE: there's a tricky "off by one" problem here. Since
                // x can be from 0.000 to 0.999, and I want xInt+1 < size
                // to leave one extra tile for bilinear filtering, that
                // means I want xInt < size-1. So I need to multiply x and
                // y by size-1, not by size.

                x = std::clamp(x * (size - 1), 0.0f, size - 2.0f);
                y = std::clamp(y * (size - 1), 0.0f, size - 2.0f);
                int xInt = std::floor(x),
                    yInt = std::floor(y),
                    xFrac = x - xInt,
                    yFrac = y - yInt;
                int p = size * yInt + xInt;
                float e00 = elevation[p],
                      e01 = elevation[p + 1],
                      e10 = elevation[p + size],
                      e11 = elevation[p + size + 1];
                return ((e00 * (1 - xFrac) + e01 * xFrac) * (1 - yFrac) + (e10 * (1 - xFrac) + e11 * xFrac) * yFrac);
            }

        }; // end of constraints

        struct PrecalculatedNoise
        {
            std::vector<float> noise0_t;
            std::vector<float> noise1_t;
            std::vector<float> noise2_t;
            std::vector<float> noise4_t;
            std::vector<float> noise5_t;
            std::vector<float> noise6_t;

            PrecalculatedNoise(DualMesh &mesh) : noise0_t(mesh.numTriangles),
                                                 noise1_t(mesh.numTriangles),
                                                 noise2_t(mesh.numTriangles),
                                                 noise4_t(mesh.numTriangles),
                                                 noise5_t(mesh.numTriangles),
                                                 noise6_t(mesh.numTriangles)
            {

                for (int t = 0; t < mesh.numTriangles; t++)
                {
                    float nx = (mesh.x_of_t(t) - 500) / 500,
                          ny = (mesh.y_of_t(t) - 500) / 500;
                    noise0_t[t] = noise2D(nx, ny);
                    noise1_t[t] = noise2D(2 * nx + 5, 2 * ny + 5);
                    noise2_t[t] = noise2D(4 * nx + 7, 4 * ny + 7);
                    noise4_t[t] = noise2D(16 * nx + 15, 16 * ny + 15);
                    noise5_t[t] = noise2D(32 * nx + 31, 32 * ny + 31);
                    noise6_t[t] = noise2D(64 * nx + 67, 64 * ny + 67);
                }
            }
        };

        std::vector<float> &elevation_t;
        std::vector<float> &elevation_r;

        DualMesh &mesh;
        ElevationGen(DualMesh &mesh, std::vector<float> &elevation_t,
                     std::vector<float> &elevation_r) : mesh(mesh), elevation_r(elevation_r), elevation_t(elevation_t)
        {
        }

        void assignTriangleElevation(Constraints &constraints, PrecalculatedNoise &precomputed, Args args)
        {
            // Assign elevations to triangles TODO: separate message,
            // store the interpolated values in an array, or maybe for
            // each painted cell store which triangle elevations have to
            // be updated, so that we don't have to recalculate the entire
            // map's interpolated values each time (involves copying 50k
            // floats instead of 16k floats), or maybe send a message with
            // the bounding box of the painted area, or maybe send the
            // drawing positions and parameters and let the painting happen
            // in this thread.

            for (int t = 0; t < mesh.numSolidTriangles; t++)
            {
                float e = constraints.constraintAt(mesh.x_of_t(t) / mesh.width, mesh.y_of_t(t) / mesh.width);
                // TODO: e*e*e*e seems too steep for this, as I want this
                // to apply mostly at the original coastlines and not
                // elsewhere
                elevation_t[t] = e + args.noisy_coastlines * (1 - e * e * e * e) * (precomputed.noise4_t[t] + precomputed.noise5_t[t] / 2 + precomputed.noise6_t[t] / 4);
            }
        }
        void assignRegionElevation()
        {
            for (int r = 0; r < mesh.numRegions; r++)
            {
                int count = 0;
                float e = 0.0f;
                bool water = false;
                int s0 = mesh._s_of_r[r];
                int s_incoming = s0;
                do
                {
                    int t = (s_incoming / 3);
                    e += elevation_t[t];
                    water = water || elevation_t[t] < 0.0;
                    int s_outgoing = mesh.s_next_s(s_incoming);
                    s_incoming = mesh._halfedges[s_outgoing];
                    count++;
                    if (count > 10) // TODO fix and remove.
                    {
                        break;
                    }
                } while (s_incoming != s0);
                e /= count;
                if (water && e >= 0)
                {
                    e = -0.001;
                }
                elevation_r[r] = e;
            }
        }
        void assignElevation(Constraints &constraints, Args args)
        {
            PrecalculatedNoise pNoise(mesh);
            assignTriangleElevation(constraints, pNoise, args);
            assignRegionElevation();
        }
    };
};