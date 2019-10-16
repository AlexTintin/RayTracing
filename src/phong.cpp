#include "material.h"

Phong::Phong(const PropertyList &propList)
    : Diffuse(propList)
{
    m_specularColor = propList.getColor("specular",Color3f(0.9));
    m_exponent = propList.getFloat("exponent",0.2);
    m_diffuseColor = propList.getColor("diffuse",Color3f(0.2));
}

Color3f Phong::brdf(const Vector3f& viewDir, const Vector3f& dir, const Normal3f& normal, const Vector2f& uv) const
{
    Vector3f i = dir.normalized();
    Vector3f o = viewDir.normalized();
    Vector3f n = normal.normalized();

    Color3f color;

    if (o.dot(n)>Epsilon && i.dot(n)>Epsilon){
        // Diffuse
        color = Diffuse::brdf(o,i,n,uv);
        // Specular
        Vector3f reflected = -o + 2.f*(o.dot(n))*n;
        float alpha = i.dot(reflected);
        if (alpha > Epsilon)
            color +=  m_specularColor * ((m_exponent + 2.f) * std::pow(alpha, m_exponent) / (2.f* M_PI));
    }

    return color;
}

Vector3f Phong::sample_IS(const Vector3f inDir, const Vector3f normal, float *pdf = 0) const
{
      float u((float)rand()/RAND_MAX);
      float v((float)rand()/RAND_MAX);

      Vector3f X = normal.unitOrthogonal();
      Vector3f Y = normal.cross(X);

      if ((m_specularColor.mean() + m_diffuseColor.mean())<u)
      {
        Vector3f dir = Vector3f::Zero();
        *pdf = 10000;
        return dir;
      }
      else if (((m_specularColor.mean() + m_diffuseColor.mean())>u) & (u>m_diffuseColor.mean()))
      {
        float phi = 2.f*M_PI*v;
        float z = pow(u,1.f/(m_exponent+1.f));
        float sin_alpha = sqrt(1.f-z*z);
        float x = cos(phi)*sin_alpha;
        float y = sin(phi)*sin_alpha;
        Vector3f dir = x*X+y*Y+z*normal;
        dir.normalize();
        *pdf = (m_exponent+1.f)*INV_TWOPI*pow(z,m_exponent);
        if (*pdf<Epsilon)
            *pdf = 10000;
        return dir;
      }

      float phi = 2.f*M_PI*v;
      float z = sqrt(u);
      float sin_theta = sqrt(1.f-z*z);
      float x = cos(phi)*sin_theta;
      float y = sin(phi)*sin_theta;
      Vector3f dir = x*X+y*Y+z*normal;
      dir.normalize();
      *pdf = INV_PI*z;
      if (*pdf<Epsilon)
          *pdf = 10000;
      return dir;

}

std::string Phong::toString() const
{
    return tfm::format(
        "Phong[\n"
        "  diffuse color = %s\n"
        "  specular color = %s\n"
        "  exponent = %f\n"
        "]", m_diffuseColor.toString(),
             m_specularColor.toString(),
             m_exponent);
}

REGISTER_CLASS(Phong, "phong")
