# Raytracing

This work is an implementaion of a path_tracer with importance sampling.

## Getting Started

### Prerequisite

cmake

### Installing

Clone this git
then from the forlder execute the following command lines:

```bash
mkdir build
cd build
cmake  ..
make -j6
```



### Running

execute the following:

```bash
./sire_raytracer link-to-scene
```

or just

```bash
./sire_raytracer
```

than press :

l to load a new scene
r to renderer the scene
c to change camera position

## Done myself

During this class I have implemented:
	* Mesh intersection function
	* Antialiasing function
	* Ward BRDF function and the importance sampling
	* Phong importance sampling
	* Area light sampling
	* Path tracer with russian roulette
	* (A photon mapper is on the way)

## Exemple of Result

You can obtain images like this:

![Init_state](https://github.com/AlexTintin/RayTracing/blob/master/data/sphereWardEnvMap.png)

This image have been obtained for a Ward BRDF function with importance sampling with environment map lightning.

## Acknowledgment

This work is based on the code fournish by Antoine Lucat for the class "Simulation Radiométrique Avancée"
The based code is available at this adress :
http://manao.inria.fr/perso/~alucat/teaching/SIRE/index.html

