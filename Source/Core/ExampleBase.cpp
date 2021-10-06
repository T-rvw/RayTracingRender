#include "ExampleBase.h"
#include "GeometryBase.h"

#include <atomic>
#include <iostream>

ExampleBase::ExampleBase(int imageWidth, int imageHeight)
    : m_imageWidth(imageWidth),
    m_imageHeight(imageHeight),
    m_pixelNumber(imageWidth * imageHeight),
    m_imageExporter(imageWidth, imageHeight)
{
}

void ExampleBase::generate(const char* fileName)
{
    if (m_imageExporter.generate(fileName))
    {
        std::cout << "Succeed to generate image." << std::endl;
    }
    else
    {
        std::cout << "Failed to generate image." << std::endl;
    }
}

void ExampleBase::process(const Camera& camera, const HittableList& world)
{
    uint64_t beginTimeStamp = ::time(nullptr);
    std::atomic<int> curPixelCount = 0;
#pragma omp parallel for
    for (int jj = m_imageHeight - 1; jj >= 0; --jj)
    {
        for (int ii = 0; ii < m_imageWidth; ++ii)
        {
            Color pixelColor(0.0, 0.0, 0.0);

            for (int sampleTimes = 0; sampleTimes < m_sampleTimes; ++sampleTimes)
            {
                // Generate random rays in a cluster
                double u = static_cast<double>(ii + MathUtils::randomDouble() / (m_imageWidth - 1));
                double v = static_cast<double>(jj + MathUtils::randomDouble() / (m_imageHeight - 1));
                Ray ray = camera.getRay(u, v);
                pixelColor += getRayColor(ray, world, m_maxRecursiveDepth);
            }

            // sample && gamma-correct(1/2)
            double sampleScale = 1.0 / m_sampleTimes;
            pixelColor[0] = std::clamp(pow(pixelColor.x() * sampleScale, 0.5), 0.0, 1.0);
            pixelColor[1] = std::clamp(pow(pixelColor.y() * sampleScale, 0.5), 0.0, 1.0);
            pixelColor[2] = std::clamp(pow(pixelColor.z() * sampleScale, 0.5), 0.0, 1.0);

            // size_t pixelIndex = (imageHeight - 1 - jj) * imageWidth + ii;
            size_t pixelIndex = m_pixelNumber - (jj + 1) * m_imageWidth + ii;
            m_imageExporter.fillColor(pixelIndex, pixelColor);

            printf("Fill color pixel placed at %d, progress = %d/%d.\n", static_cast<int>(pixelIndex), ++curPixelCount, m_pixelNumber);
        }
    }

    uint64_t endTimeStamp = ::time(nullptr);
    printf("Finish processing, costs %d seconds.\n", static_cast<int>(endTimeStamp - beginTimeStamp));
}

Color ExampleBase::getRayColor(const Ray& ray, const HittableList& world, int curDepth)
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (curDepth <= 0)
    {
        return Color(0.0, 0.0, 0.0);
    }

    std::optional<HitRecord> optHitRecord = world.hit(ray, DOUBLE_EPS, DOUBLE_INFINITY);
    if (optHitRecord.has_value())
    {
        const HitRecord& hitRecord = optHitRecord.value();
        if (const GeometryBase* pHitObject = hitRecord.hitObject())
        {
            Ray scattered;
            Color attenuation;
            if (Material* pMaterial = pHitObject->material())
            {
                if (pMaterial->scatter(ray, hitRecord, attenuation, scattered))
                {
                    return attenuation * getRayColor(scattered, world, curDepth - 1);
                }

                return Color(0.0, 0.0, 0.0);
            }
        }
    }

    // background
    XYZ unitDir = unit(ray.direction());
    double factor = 0.5 * (unitDir.y() + 1.0);
    return (1.0 - factor) * Color(1.0, 1.0, 1.0) + factor * Color(0.5, 0.7, 1.0);
}