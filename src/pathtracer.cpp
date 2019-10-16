#include "integrator.h"
#include "scene.h"
#include "material.h"
#include "areaLight.h"

class PathTracer : public Integrator
{
public:
    PathTracer(const PropertyList &props) {
      m_sampleCount = props.getInteger("maxRecursion",4);
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

            const LightList &lights = scene->lightList();
            for(LightList::const_iterator it=lights.begin(); it!=lights.end(); ++it)
            {
                float dist;
                Vector3f lightDir = (*it)->direction(pos, &dist);
                Color3f light_intensity;
                if(dynamic_cast<const AreaLight*>(*it)){
                    // source étendue
                    const AreaLight* light =  dynamic_cast<const AreaLight*>(*it);

                    Vector3f dir = light->direction(pos,&dist);
                    Point3f y = pos+dir*dist;
                    light_intensity = light->intensity(pos);
                }
                else{
                    // lampe ponctuelle ou directionnelle
                    light_intensity = (*it)->intensity(pos);
                }
                Ray shadowRay(pos + normal*1e-4, lightDir,true);
                Hit shadowHit;
                scene->intersect(shadowRay,shadowHit);
                Color3f attenuation = Color3f(1.f);
                if(shadowHit.t()<dist){
                    attenuation = 0.5f * shadowHit.shape()->material()->transmissivness();
                    if((attenuation <= 1e-6).all())
                        continue;
                }

                float cos_term = std::max(0.f,lightDir.dot(normal));
                Color3f brdf = material->brdf(-ray.direction, lightDir, normal, hit.texcoord());
                radiance += light_intensity * cos_term * brdf * attenuation;
            }

            // reflexions
            if((material->reflectivity() > 1e-6).any())
            {
                Vector3f r = (ray.direction - 2.*ray.direction.dot(hit.normal())*hit.normal()).normalized();
                Ray reflexion_ray(pos + hit.normal()*1e-4, r);
                reflexion_ray.recursionLevel = ray.recursionLevel + 1;
                float cos_term = std::max(0.f,r.dot(normal));
                radiance += material->reflectivity() * Li(scene, reflexion_ray) * cos_term;
            }

            // Indirect illumination
            float albedo = std::min(material->ambientColor().matrix().sum()/3.f,1.f);
            float russian = ((float) rand()/RAND_MAX);
            if(russian<albedo)
            {
                float pdf;
                Vector3f r(material->sample_IS(-ray.direction,normal,&pdf));
                if(normal.dot(r)<0)
                   r=-r;
                float cos_term(std::max(0.f,r.dot(normal)));
                Color3f BRDF(material->brdf(-ray.direction, r, normal, hit.texcoord()));

                Ray reflected_ray(pos+normal*Epsilon, r);
                reflected_ray.recursionLevel = ray.recursionLevel+1;

                radiance += (Li(scene, reflected_ray) * BRDF *cos_term/pdf);
            }
            if(albedo > Epsilon)
                radiance /= (albedo);

        }else
            return scene->backgroundColor();

        return radiance;
  }

    std::string toString() const {
        return tfm::format("PathTracer[\n"
                           " maxRecursion = %f\n"
                           " IS = %b ]\n",
                           m_sampleCount,
                           m_IS);
    }
private:
    int m_sampleCount;
    bool m_IS;

};

REGISTER_CLASS(PathTracer, "path_mats")
