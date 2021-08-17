/// Copyright 2021 Omar Sherif Fathy
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#include "PrototypePreloadedAssets.h"

// clang-format off
PROTOTYPE_EXTERN nlohmann::json colored_triangle_2d = {
    { "type", "Constant_Colored_Triangles_2D" },
    { "name", "colored_triangle_2d" },
    { "vertices", { -0.5, -0.5, 1.0, 0.0, 0.0, 0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 0.5, 0.0, 0.0, 1.0 } },
    { "indices", { 0, 1, 2 } }
};

PROTOTYPE_EXTERN nlohmann::json colored_plane_2d = {
    { "type", "Constant_Colored_Triangles_2D" },
    { "name", "colored_plane_2d" },
    { "vertices", { -0.5, -0.5, 1.0, 0.0, 0.0, 0.5, -0.5, 0.0, 1.0, 0.0, 0.5, 0.5, 0.0, 0.0, 1.0, -0.5, 0.5, 0.0, 1.0, 1.0 } },
    { "indices", { 0, 1, 2, 2, 3, 0 } }
};

PROTOTYPE_EXTERN nlohmann::json colored_textured_plane_2d = {
    { "type", "Constant_Colored_Textured_Triangles_2D" },
    { "name", "colored_textured_plane_2d" },
    { "vertices", { -0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.5,  -0.5, 1.0, 0.0, 0.0, 1.0, 0.0,
                    0.5,  0.5,  1.0, 1.0, 0.0, 0.0, 1.0, -0.5, 0.5,  0.0, 1.0, 0.0, 1.0, 1.0 } },
    { "indices", { 0, 1, 2, 2, 3, 0 } }
};

PROTOTYPE_EXTERN nlohmann::json plane = { { "type", "Constant_Triangles" },
                                                 { "name", "PLANE" },
                                                 { "vertices",
                                                   {
                                                     1.0,  1.0,  0.0, 1.0, 0.0, 0.0, -1.0, 1.0, 0.6, 0.6, 0.6, 1.0,
                                                     -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.6, 0.6, 0.6, 1.0,
                                                     -1.0, 1.0,  0.0, 0.0, 0.0, 0.0, -1.0, 1.0, 0.6, 0.6, 0.6, 1.0,
                                                     1.0,  -1.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.6, 0.6, 0.6, 1.0,
                                                   } },
                                                 { "indices",
                                                   {
                                                     0,
                                                     1,
                                                     2,
                                                     0,
                                                     3,
                                                     1,
                                                   } } };

PROTOTYPE_EXTERN nlohmann::json cube = {
    { "type", "Constant_Triangles" },
    { "name", "CUBE" },
    { "vertices", 
      {
        //    positionU[4],                 normalV[4],                     color[4]
        -1.0,  1.0, -1.0,  0.0,       -1.0, -0.0,  0.0,  0.333333,    0.6,  0.6,  0.6,  1.0,      
        -1.0, -1.0,  1.0,  0.333333,  -1.0, -0.0,  0.0,  0.666667,    0.6,  0.6,  0.6,  1.0,
        -1.0,  1.0,  1.0,  0.333333,  -1.0, -0.0,  0.0,  0.333333,    0.6,  0.6,  0.6,  1.0,
        -1.0, -1.0, -1.0,  0.333333,   0.0, -1.0,  0.0,  1.0,         0.6,  0.6,  0.6,  1.0,
         1.0, -1.0,  1.0,  0.666667,   0.0, -1.0,  0.0,  0.666667,    0.6,  0.6,  0.6,  1.0,
        -1.0, -1.0,  1.0,  0.333333,   0.0, -1.0,  0.0,  0.666667,    0.6,  0.6,  0.6,  1.0,
         1.0, -1.0, -1.0,  0.666667,   1.0,  0.0,  0.0,  0.333333,    0.6,  0.6,  0.6,  1.0,
         1.0,  1.0,  1.0,  0.333333,   1.0,  0.0,  0.0,  0.666667,    0.6,  0.6,  0.6,  1.0,
         1.0, -1.0,  1.0,  0.666667,   1.0,  0.0,  0.0,  0.666667,    0.6,  0.6,  0.6,  1.0,
         1.0,  1.0, -1.0,  0.0,        0.0,  1.0,  0.0,  1.0,         0.6,  0.6,  0.6,  1.0,
        -1.0,  1.0,  1.0,  0.333333,   0.0,  1.0,  0.0,  0.666667,    0.6,  0.6,  0.6,  1.0,
         1.0,  1.0,  1.0,  0.0,        0.0,  1.0,  0.0,  0.666667,    0.6,  0.6,  0.6,  1.0,
         1.0, -1.0,  1.0,  0.666667,   0.0,  0.0,  1.0,  1.0,         0.6,  0.6,  0.6,  1.0,
        -1.0,  1.0,  1.0,  1.0,        0.0,  0.0,  1.0,  0.666667,    0.6,  0.6,  0.6,  1.0,
        -1.0, -1.0,  1.0,  0.666667,   0.0,  0.0,  1.0,  0.666667,    0.6,  0.6,  0.6,  1.0,
        -1.0, -1.0, -1.0,  0.0,        0.0, -0.0, -1.0,  0.0,         0.6,  0.6,  0.6,  1.0,
         1.0,  1.0, -1.0,  0.333333,   0.0, -0.0, -1.0,  0.333333,    0.6,  0.6,  0.6,  1.0,
         1.0, -1.0, -1.0,  0.333333,   0.0, -0.0, -1.0,  0.0,         0.6,  0.6,  0.6,  1.0,
        -1.0, -1.0, -1.0,  0.0,       -1.0, -0.0,  0.0,  0.666667,    0.6,  0.6,  0.6,  1.0,
         1.0, -1.0, -1.0,  0.666667,   0.0, -1.0,  0.0,  1.0,         0.6,  0.6,  0.6,  1.0,
         1.0,  1.0, -1.0,  0.333333,   1.0,  0.0,  0.0,  0.333333,    0.6,  0.6,  0.6,  1.0,
        -1.0,  1.0, -1.0,  0.333333,   0.0,  1.0,  0.0,  1.0,         0.6,  0.6,  0.6,  1.0,
         1.0,  1.0,  1.0,  1.0,        0.0,  0.0,  1.0,  1.0,         0.6,  0.6,  0.6,  1.0,
        -1.0,  1.0, -1.0,  0.0,        0.0, -0.0, -1.0,  0.333333,    0.6,  0.6,  0.6,  1.0,
      } 
    },
    { 
      "indices",
      {
        0, 1,  2, 3, 4,  5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
        0, 18, 1, 3, 19, 4, 6, 20, 7, 9, 21, 10, 12, 22, 13, 15, 23, 16,
      }
    }
};

// clang-format on