#include "material.h"

#include <Eigen/Geometry>
#include <iostream>
#include <math.h>

void Material::loadTextureFromFile(const std::string& fileName)
{
    if (fileName.size()==0)
        std::cerr << "Material error : no texture file name provided" << std::endl;
    else
        m_texture = new Bitmap(fileName);
}

Diffuse::Diffuse(const PropertyList &propList)
{
    m_diffuseColor = propList.getColor("diffuse",Color3f(0.2));
    m_reflectivity = propList.getColor("reflectivity",Color3f(0.0));
    m_transmissivness = propList.getColor("transmissivness",Color3f(0.0));
    m_etaA = propList.getFloat("etaA",1);
    m_etaB = propList.getFloat("etaB",1);

    std::string texturePath = propList.getString("texture","");
    if(texturePath.size()>0){
        filesystem::path filepath = getFileResolver()->resolve(texturePath);
        loadTextureFromFile(filepath.str());
        setTextureScale(propList.getFloat("scale",1));
        setTextureMode(TextureMode(propList.getInteger("mode",0)));
    }
}

Color3f Diffuse::diffuseColor(const Vector2f& uv) const
{
    if(texture() == nullptr)
        return m_diffuseColor;

    float u = uv[0];
    float v = uv[1];

    // Take texture scaling into account
    u /= textureScaleU();
    v /= textureScaleV();

    // Compute pixel coordinates
    const int i = int(fabs(u - floor(u)) * texture()->cols());
    const int j = int(fabs(v - floor(v)) * texture()->rows());

    Color3f fColor = (*texture())(j,i);

    // Compute color
    switch(textureMode())
    {
    case MODULATE:
        return  fColor * m_diffuseColor;
    case REPLACE:
        return fColor;
    }
    return fColor;
}

Color3f Diffuse::brdf(const Vector3f& viewDir, const Vector3f& dir, const Normal3f& normal, const Vector2f& uv) const
{
    Vector3f i = dir.normalized();
    Vector3f o = viewDir.normalized();
    Vector3f n = normal.normalized();

    if (o.dot(n)>Epsilon && i.dot(n)>Epsilon)
        return diffuseColor(uv)/M_PI;
    return Color3f();
}

Vector3f Diffuse::sample_IS(const Vector3f inDir, const Vector3f normal, float *pdf = 0) const
{
      float u1((float)rand()/RAND_MAX);
      float u2((float)rand()/RAND_MAX);

      float z(1.f-2.f*u1);
      float r(std::sqrt(std::max(0.f,1.f-z*z)));
      float phi(2.f*M_PI*u2);
      float x(r*cos(phi));
      float y(r*sin(phi));

      Vector3f dir(x, y, z);
      dir.normalize();
      *pdf = INV_TWOPI;
      return dir;
}

std::string Diffuse::toString() const
{
    return tfm::format(
        "Diffuse[\n"
        "  color = %s\n"
        "]", m_diffuseColor.toString());
}

REGISTER_CLASS(Diffuse, "diffuse")
