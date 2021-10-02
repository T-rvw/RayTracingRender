#pragma once

#include "GeometryBase.h"

class Rotate : public GeometryBase
{
public:
    Rotate(std::shared_ptr<GeometryBase> pGeometry, double angle);

    virtual std::optional<HitRecord> hit(const Ray& ray, double minT, double maxT) const override;
    virtual UV uv(const XYZ& point) const override;

private:
    double m_sinTheta;
    double m_cosTheta;
    std::shared_ptr<GeometryBase> m_pObject;
};