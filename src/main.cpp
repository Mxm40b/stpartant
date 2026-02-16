#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <print>
#include <raylib.h>
#include <vector>
// #include <time.h>

const int screenWidth = 1600;
const int screenHeight = 900;

const int cellNumberVert = 100;
const int cellNumberHor = 100;

const float scale = 1.f;

const Color bedColor{0, 0, 0, 255};
const Color surfaceColor{100, 150, 200, 255};
const Color sideColor{0, 100, 190, 100};

const float squareWidth = 500.f;

const float density = 10.f;

const float forcesScale = 1.f;

void drawWaterBlok(int i, int j, float heightSurface, float heightBed) {
  float triWidth = float(screenWidth) / cellNumberHor / 2;
  float triHeight = float(screenHeight) / cellNumberVert / 2;
  // drawing bed
  DrawTriangle(Vector2{triWidth * i, triHeight * (j + 1) - heightBed},
               Vector2{triWidth * (i + 1), triHeight * (j + 1) - heightBed},
               Vector2{triWidth * (i + 1), triHeight * j - heightBed},
               bedColor);
  DrawTriangle(Vector2{triWidth * (i + 1), triHeight * (j)-heightBed},
               Vector2{triWidth * (i + 1), triHeight * (j + 1) - heightBed},
               Vector2{triWidth * (i + 2), triHeight * j - heightBed},
               bedColor);
  // drawing sides
  DrawRectangle(triWidth * (i + 1), triHeight * j - heightSurface, triWidth,
                heightSurface - heightBed, sideColor);
  DrawRectangle(triWidth * i, triHeight * (j + 1) - heightSurface, triWidth,
                heightSurface - heightBed, sideColor);
  DrawTriangle(Vector2{triWidth * (i + 1), triHeight * (j)-heightBed},
               Vector2{triWidth * (i + 1), triHeight * (j + 1) - heightBed},
               Vector2{triWidth * (i + 2), triHeight * j - heightBed},
               sideColor);
  // DrawRectangle(triWidth * i, triHeight * j, 399, 299, Color{0,0,0,255});
  // DrawRectangle(triWidth * i, triHeight * j, triWidth,
  //               heightSurface+heightBed, Color{100, 150, 200, 255});
  // drawing surface
  DrawTriangle(Vector2{triWidth * i, triHeight * (j + 1) - heightSurface},
               Vector2{triWidth * (i + 1), triHeight * (j + 1) - heightSurface},
               Vector2{triWidth * (i + 1), triHeight * j - heightSurface},
               surfaceColor);
  DrawTriangle(Vector2{triWidth * (i + 1), triHeight * (j)-heightSurface},
               Vector2{triWidth * (i + 1), triHeight * (j + 1) - heightSurface},
               Vector2{triWidth * (i + 2), triHeight * j - heightSurface},
               surfaceColor);
};

// float flowDiffX(unsigned long i, unsigned long j,
//                 std::vector<std::vector<float>> ratesOfChangeX) {
//   if (0 <= i && i < ratesOfChangeX.size() - 1 && 0 <= j &&
//       j < ratesOfChangeX[0].size() - 1) {
//     return (ratesOfChangeX[i][j] - ratesOfChangeX[i][j + 1] +
//             ratesOfChangeX[i + 1][j] - ratesOfChangeX[i + 1][j] + 1) /
//            2;
//   } else {
//     return 0;
//   }
// }

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------

  std::vector<std::vector<float>> heights;
  std::vector<std::vector<float>> bedHeights;

  for (int i = 0; i < cellNumberVert; i++) {
    heights.emplace_back(std::vector<float>());
    bedHeights.emplace_back(std::vector<float>());
    for (int j = 0; j < cellNumberHor; j++) {
      heights[i].emplace_back(200);
      bedHeights[i].emplace_back(-500);
    }
  }

  std::vector<std::vector<float>> ratesOfChangeX;

  for (int i = 0; i < cellNumberHor - 1; i++) {
    ratesOfChangeX.emplace_back(cellNumberVert);
  }

  // set initial speed for test:
  // for (int i = 20; i < 30; i++) {
  //   for (int j = 0; j < 50; j++)
  //     ratesOfChangeX[i][j] = 60.f;
  // }

  std::vector<std::vector<float>> ratesOfChangeY;

  for (int i = 0; i < cellNumberHor; i++) {
    ratesOfChangeY.emplace_back(cellNumberVert - 1);
  }
  // for (int i = 0; i < 30; i++) {
  //   for (int j = 0; j < 50; j++)
  //     ratesOfChangeX[i][j] = 60.f;
  // }

  // for (int i = 20; i < 30; i++) {
  //   for (int j = 0; j < 50; j++)
  //     heights[i][j] = 400;
  // }
  //
  heights[30][50] = 30000;


  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------
    // for (int i = 0; i < cellNumberHor; i++) {
    //   for (int j = 0; j < cellNumberVert; j++) {
    //     heights[i][j] += 0;
    //   }
    // }

    float g = 9.81;
    float Sf = 0;
    float h;       // height deviation from the mean
    float H;       // mean height
    float tau = 0; // ??
    float R =
        0; // hydraulic radius
           // float f_d; // darcy friction factor
           // (https://en.wikipedia.org/wiki/Darcy_friction_factor_formulae)
           // // calculating rate of change X
           // // ratesOfChangeX[40][4] = 1;
           // commented because i will be using Newton's shear stress formula
           // (https://en.wikipedia.org/wiki/Shear_stress#Shear_stress_in_fluids)
    float dynamicViscosity = 0.001; // The dynamic viscosity of water at 20
                                    // degrees Celsius is approximately 1 mPa·s
    float P;
    float topP;
    float bottomP;
    float A;

    float leftVelocity;
    float centerVelocityX;
    float rightVelocity;
    float gradientVelocityX;

    float D;
    float Re; // the Reynolds number

    for (unsigned long i = 0; i < ratesOfChangeX.size(); i++) {
      for (unsigned long j = 0; j < ratesOfChangeX[0].size(); j++) {

        if (i == 0) {
          leftVelocity = 0;
          rightVelocity = ratesOfChangeX[i + 1][j];

        } else if (i == ratesOfChangeX.size() - 1) {
          leftVelocity = ratesOfChangeX[i - 1][j];
          rightVelocity = 0;
        } else {
          leftVelocity = ratesOfChangeX[i - 1][j];
          rightVelocity = ratesOfChangeX[i + 1][j];

        } // these statements are to not access the velocity in a place at the
          // walls. In that case, the speed of the liquid is zero.
        centerVelocityX = ratesOfChangeX[i][j];

        gradientVelocityX =
            ((rightVelocity - centerVelocityX) +
             (centerVelocityX - leftVelocity)) /
            (2 * squareWidth); // we need du/dx so i am averaging the gradient
                               // on the right and on the left.
        float b =
            centerVelocityX *
            gradientVelocityX; // TODO: understand better what this term means

        float e = g * (bedHeights[i + 1][j] - bedHeights[i][j]) /
                  squareWidth; // same as c but for the bed slope
        float c = g * (heights[i + 1][j] - heights[i][j]) / squareWidth;
        // c = 0;

        A = squareWidth *
            (heights[i][j] - bedHeights[i][j] + heights[i + 1][j] -
             bedHeights[i + 1][j]) /
            2;

        if (j == 0) {
          topP = 0;
          bottomP = (heights[i][j] - bedHeights[i][j] + heights[i][j + 1] -
                     bedHeights[i][j + 1]) /
                    2;
        } else if (j == ratesOfChangeX[0].size() - 1) {
          topP = (heights[i][j - 1] - bedHeights[i][j - 1] + heights[i][j] -
                  bedHeights[i][j]) /
                 2;
          bottomP = 0;
        } else {
          topP = (heights[i][j - 1] - bedHeights[i][j - 1] + heights[i][j] -
                  bedHeights[i][j]) /
                 2;
          bottomP = (heights[i][j] - bedHeights[i][j] + heights[i][j + 1] -
                     bedHeights[i][j + 1]) /
                    2;
        };

        P = (topP + bottomP) *
            squareWidth; // this is supposed to be the perimeter of a cross
                         // sectional area, but if the squares are large
                         // enough, it may be more accurate for the
                         // "perimeter" to be multiplied by the squareWidth;

        R = A / P;

        D = 4 * R;

        tau = 8 * dynamicViscosity * density * ratesOfChangeX[i][j] / D;

        Sf = tau / (density * g * R);
        // std::println("Re: {}", Re);
        float d = g * Sf;
        // float d = 0;
        if (R < 0.1) {
          d = 0;
        }

        ratesOfChangeX[i][j] += - forcesScale * (b + c + d + e);
        // std::println("b: {}, pressure gradient c: {}, friction term "
        //              "d: {}, gravity term e: {}, corresponding i: {} and j: {}",
        //              b, c, d, e, i, j);
        // WaitTime(0.001);
        // if (i == 66 && j == 67){
        //   WaitTime(1);
        // }
      }
    }


    float gradientVelocityY;
    float centerVelocityY;
    float topVelocity;
    float bottomVelocity;

    float leftP;
    float rightP;

    for (unsigned long i = 0; i < ratesOfChangeY.size(); i++) {
      for (unsigned long j = 0; j < ratesOfChangeY[0].size(); j++) {
        if (j == 0) {
          topVelocity = 0;
          bottomVelocity = ratesOfChangeY[i][j+1];

        } else if (j == ratesOfChangeY[0].size() - 1) {
          topVelocity = ratesOfChangeY[i][j-1];
          bottomVelocity = 0;
        } else {
          topVelocity = ratesOfChangeY[i][j-1];
          bottomVelocity = ratesOfChangeY[i][j+1];

        } // these statements are to not access the velocity in a place at the
          // walls. In that case, the speed of the liquid is zero.
        centerVelocityY = ratesOfChangeY[i][j];

        gradientVelocityY =
            ((bottomVelocity - centerVelocityY) +
             (centerVelocityY - topVelocity)) /
            (2 * squareWidth); // we need du/dx so i am averaging the gradient
                               // on the right and on the left.
        float b =
            centerVelocityY *
            gradientVelocityY; // TODO: understand better what this term means

        float e = g * (bedHeights[i][j+1] - bedHeights[i][j]) /
                  squareWidth; // same as c but for the bed slope
        float c = g * (heights[i][j+1] - heights[i][j]) / squareWidth;
        // c = 0;

        A = squareWidth *
            (heights[i][j] - bedHeights[i][j] + heights[i][j+1] -
             bedHeights[i][j+1]) /
            2;

        if (i == 0) {
          leftP = 0;
          rightP = (heights[i][j] - bedHeights[i][j] + heights[i+1][j] -
                     bedHeights[i+1][j]) /
                    2;
        } else if (i == ratesOfChangeY.size() - 1) {
          leftP = (heights[i-1][j] - bedHeights[i-1][j] + heights[i][j] -
                  bedHeights[i][j]) /
                 2;
          rightP = 0;
        } else {
          leftP = (heights[i-1][j] - bedHeights[i-1][j] + heights[i][j] -
                  bedHeights[i][j]) /
                 2;
          rightP = (heights[i][j] - bedHeights[i][j] + heights[i+1][j] -
                     bedHeights[i+1][j]) /
                    2;
        };

        P = (leftP + rightP) *
            squareWidth; // this is supposed to be the perimeter of a cross
                         // sectional area, but if the squares are large
                         // enough, it may be more accurate for the
                         // "perimeter" to be multiplied by the squareWidth;

        R = A / P;

        D = 4 * R;

        tau = 8 * dynamicViscosity * density * ratesOfChangeY[i][j] / D;

        Sf = tau / (density * g * R);
        // std::println("Re: {}", Re);
        float d = g * Sf;
        // float d = 0;
        if (R < 0.1) {
          d = 0;
        }

        ratesOfChangeY[i][j] += -forcesScale * (b + c + d + e);
        // std::println("b: {}, pressure gradient c: {}, friction term "
        //              "d: {}, gravity term e: {}, corresponding i: {} and j: {}",
        //              b, c, d, e, i, j);
        // WaitTime(0.001);
        // if (i == 66 && j == 67){
        //   WaitTime(1);
        // }
      }
    }

    // applying rate of change X
    for (unsigned long i = 0; i < ratesOfChangeX.size(); i++) {
      for (unsigned long j = 0; j < ratesOfChangeX[0].size(); j++) {

        heights[i][j] -= ratesOfChangeX[i][j];
        heights[i + 1][j] += ratesOfChangeX[i][j];
      }
    }

    // applying rate of change Y
    for (unsigned long i = 0; i < ratesOfChangeY.size(); i++) {
      for (unsigned long j = 0; j < ratesOfChangeY[0].size(); j++) {
        heights[i][j] -= ratesOfChangeY[i][j];
        heights[i][j + 1] += ratesOfChangeY[i][j];
      }
    }

    // WaitTime(1);

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    // DrawText("Congrats! You created your first window!", 300, 200, 20,
    //          LIGHTGRAY);

    for (int i = 0; i < cellNumberHor; i++) {

      for (int j = 0; j < cellNumberVert; j++) {
        drawWaterBlok(i + cellNumberHor - j - 1, j + cellNumberVert,
                      heights[i][j] * scale, bedHeights[i][j] * scale);
      }
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
