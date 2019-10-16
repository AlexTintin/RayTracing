#include "material.h"

Ward::Ward(const PropertyList &propList)
    : Diffuse(propList)
{
    m_specularColor = propList.getColor("specular",Color3f(0.9));
    m_diffuseColor = propList.getColor("diffuse",Color3f(0.2));
    m_alphaX = propList.getFloat("alphaX",0.2);
    m_alphaY = propList.getFloat("alphaY",0.2);
}

Color3f Ward::brdf(const Vector3f& viewDir, const Vector3f& dir, const Normal3f& normal, const Vector2f& uv) const
{
    Vector3f i = dir.normalized();
    Vector3f o = viewDir.normalized();
    Vector3f n = normal.normalized();

    Color3f color;

    if (o.dot(n)>Epsilon && i.dot(n)>Epsilon){
      Vector3f x(Vector3f(0,1,0) - Vector3f(0,1,0).dot(n)*n);
      x.normalize();
      Vector3f y(n.cross(x));
      Vector3f h((o+dir).normalized());
      float hx(h.dot(x)/m_alphaX);
      float hy(h.dot(y)/m_alphaY);
      float hn(h.dot(n));
      float den(1/(m_alphaX*m_alphaY*sqrt(i.dot(n)*o.dot(n))));
      color = m_specularColor*INV_FOURPI*den*exp(-(hx*hx+hy*hy)/(hn*hn));
      color += INV_PI*m_diffuseColor;
    }

    return color;
}

Vector3f Ward::sample_IS(const Vector3f inDir, const Vector3f normal, float *pdf = 0) const
{
      float u((float)rand()/RAND_MAX);
      float v((float)rand()/RAND_MAX);

      if (m_specularColor.mean() / (m_specularColor.mean() + m_diffuseColor.mean())<u)
      {
        Vector3f dir = Diffuse::sample_IS(inDir,normal,pdf);
        return dir;
      }

      Vector3f X = normal.unitOrthogonal();
      Vector3f Y = normal.cross(X);

      float phiH = atan2(m_alphaY * tan(2.f * M_PI * v),m_alphaX);
      if(v>0.5)
          phiH += M_PI;
      float dencos =  cos(phiH) / m_alphaX ;
      float densin = sin(phiH)/ m_alphaY;
      float thetaH = atan2( sqrt(-log(u)), sqrt((dencos*dencos) + (densin*densin)));

      float cos_thetaH = cos(thetaH);
      float sin_thetaH = sin(thetaH);
      float cos_phiH = cos(phiH);
      float sin_phiH = sin(phiH);
      float tan_thetaH = tan(thetaH);
      Vector3f h = sin_thetaH*cos_phiH*X + sin_thetaH*sin_phiH*Y + cos_thetaH*normal;
      h.normalize();

      Vector3f dir = h * (2.f * inDir.dot(h)) - inDir;
      float ex = cos_phiH/m_alphaX;
      float ey = sin_phiH/m_alphaY;
      float exposant = -(tan_thetaH*tan_thetaH)*(ex*ex+ey*ey);
      float den = 1/(m_alphaX*m_alphaY*(h.dot(inDir))*cos_thetaH*cos_thetaH*cos_thetaH);
      *pdf = INV_FOURPI*exp(exposant)*den;
      if (*pdf<Epsilon)
          *pdf = 10000;
      return dir;
}



std::string Ward::toString() const
{
    return tfm::format(
        "Ward[\n"
        "  diffuse color = %s\n"
        "  specular color = %s\n"
        "  alphaX = %f\n"
        "  alphaY = %f\n"
        "]", m_diffuseColor.toString(),
             m_specularColor.toString(),
             m_alphaX,
             m_alphaY);
}

REGISTER_CLASS(Ward, "ward")
