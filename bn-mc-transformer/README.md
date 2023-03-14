# bn-mc-transformer
The ( p)BN2( p)MC transformer on top of Storm that takes (parametric) Bayesian networks in [BIF format] and translates them into the [Jani] specification. The package supports two types of translations:
- The evidence-agnostic translation: it takes the (p)BN and translates it to a (p)MC.
- The evidence-tailored translation: it takes the (p)BN + evidence E (+ hypothesis H), and creates a succincter (p)MC that also takes evidence E into account. 

### Run the transformer
1) Make the target 
```
mkdir build && cd build
cmake ..
make
```
2) Run the target
```
cd bin
./storm-bn-robin
```

   [BIF format]: <http://www.cs.cmu.edu/afs/cs/user/fgcozman/www/Research/InterchangeFormat/>
   [Jani]: <https://jani-spec.org/>

