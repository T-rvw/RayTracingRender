#include "Camera.h"
#include "ExampleBase.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Sphere.h"

int main()
{
    // World
    auto groudMaterial  = new Lambertian(Color(0.8, 0.8, 0.0));
    auto centerMaterial = new Lambertian(Color(0.7, 0.3, 0.3));
    auto leftMaterial   = new Metal(Color(0.8, 0.8, 0.8), 0.3);
    auto rightMaterial  = new Metal(Color(0.8, 0.6, 0.2), 1.0);
    
    HittableList hittableList;
    hittableList.reserve(4);
    hittableList.add(new Sphere(XYZ(+0.0, -100.5, -1.0), 100.0)).setMaterial(groudMaterial);
    hittableList.add(new Sphere(XYZ(+0.0,    0.0, -1.0),   0.5)).setMaterial(centerMaterial);
    hittableList.add(new Sphere(XYZ(-1.2,    0.0, -1.0),   0.5)).setMaterial(leftMaterial);
    hittableList.add(new Sphere(XYZ(+1.2,    0.0, -1.0),   0.5)).setMaterial(rightMaterial);

	// Camera
    constexpr int imageWidth = 400;
    constexpr int imageHeight = 225;
    constexpr double aspectRatio = static_cast<double>(imageWidth) / static_cast<double>(imageHeight);
    Camera camera(90.0, aspectRatio);

    // Init example and run
    ExampleBase example(imageWidth, imageHeight);
    example.setSampleTimes(100);
    example.setMaxRecursiveDepth(50);
    example.process(camera, hittableList);
    example.generate("test.png");

    return 0;
}