#include "integrator.h"
#include "scene.h"
#include "material.h"
#include "areaLight.h"

class DirectMats : public Integrator
{
public:
    DirectMats(const PropertyList &props) {
      m_sampleCount = props.getInteger("samples",4);
      m_IS = props.getBoolean("IS", false);
    }

    Color3f Li(const Scene *scene, const Ray &ray) const {

        Color3f radiance = Color3f::Zero();

        Hit hit;
        scene->intersect(ray, hit);
        if (hit.foundIntersection())
        {
            Point3f pos = ray.at(hit.t());
            Normal3f normal = hit.normal();

            const Material* material = hit.shape()->material();
            float pdf;
            for(int i=0; i<m_sampleCount; i++)
            {
                Vector3f r(material->sample_IS(-ray.direction,normal,&pdf));
                if(normal.dot(r)<0)
                     r=-r;
                float cos_term(std::max(0.f,r.dot(normal)));
                Color3f BRDF(material->brdf(-ray.direction, r, normal, hit.texcoord()));
                radiance += (scene->backgroundColor(r) * BRDF *cos_term/pdf);
          }
        }
        else
            return scene->backgroundColor(ray.direction);

        return radiance/m_sampleCount;
    }

    std::string toString() const {
        return tfm::format("DirectMats[\n"
                           " samples = %f\n"
                           " IS = %b ]\n",
                           m_sampleCount,
                           m_IS);
    }
private:
    int m_sampleCount;
    bool m_IS;

};

REGISTER_CLASS(DirectMats, "direct_mats")
