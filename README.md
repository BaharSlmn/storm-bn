# storm-bn
storm-bn is a prototypical tool for the analysis of (parametric) Bayesian networks. It presents alternative techniques for (a) **inference** on classical Bayesian networks in which all probabilities are fixed, and for (b) **parameter synthesis** problems when conditional probability tables (CPTs) in such networks contain symbolic variables rather than concrete probabilities. The key idea is to exploit **probabilistic model checking** as well as its recent extension to parameter synthesis techniques thereof for parametric Markov chains.  To enable this, the (parametric) Bayesian networks are transformed into (parametric) Markov chains and their objectives are mapped onto probabilistic temporal logic formulas (PCTL).

(a) Given the Bayesian network B (and the evidence E), our tool-chain enables computing the inference probabilities for the given queries.

(b) Given the parametric Bayesian network (pBN) B (and the evidence E), it enables

- computing the sensitivity functions (and values) for a given query

- synthesizing and tuning the parameters with respect to a given constraint, i.e., finding a single parameter instantiation that satisfies the constraint.

 - partitioning the entire n-dimensional parameter space of the pBN into satisfying, rejecting, and --with some approximation factor- unknown regions for a given query and a given coverage factor, i.e., it provides the thorough information: which parameters instantiations satisfy and which violate the constraint.

storm-bn parameter synthesis features are applicable to **arbitrarily many, possibly dependent, parameters that may occur in multiple CPTs**. This lifts restrictions, e.g., on the number of parametrized CPTs, or on parameter dependencies between several CPTs, that exist in the existing pBN analysis tools.

## Booting the Docker image
To access the tool easier, a Docker container containing all tools and directories is provided. To use the containers, [Docker] needs to be installed. 
Download the Docker [image]:
```sh
docker pull hansvrapi/storm-bn:latest
```
Run the Docker image:
```
docker run -it hansvrapi/storm-bn:latest
```
## Subdirectories Description:

*bn-mc-transformer*: this is the pBN2pMC transformer on top of Storm that takes (parametric) Bayesian networks in [BIF format] and translates them into the [Jani] specification. The package supports two types of translations:
- The evidence-agnostic translation: it takes the (p)BN and translates it to a (p)MC.
- The evidence-tailored translation: it takes the (p)BN + evidence E (+ hypothesis H), and creates a succincter (p)MC that also takes evidence E into account. 

*auxilary_scripts*: this package includes the scripts for parameterizing the networks. 

*ace_storm_comparison*: this subdirectory includes the experiments that compare Storm and [Ace] in probabilistic inference on classical BNs.

*psdd_storm_comparison*: this subdirectory includes the experiments that compare psdd_package and Storm in performing **symbolic** probabilistic inference on classical BNs. 

*storm_evidence*: this directory includes the experiments that compare our evidence-agnostic and evidence-tailored translation in performing probabilistic inference on BNs.

*sensitivity_analysis*: this subdirectory includes the experiments that compare Storm and [Bayesserver] in computing the sensitivity functions for pBNs.

*feasibility_analysis*: this subdirectory includes the experiments that perform parameter tuning on pBNs using the state-of-the-art feasibility checking techniques for parametric Markov chains. The experiments compare the methods of "Particle Swarm Optimization", and "Quadratically-Constrained Quadratic Program (QCQP), and Gradient-Descent (GD) for this task. 

*parameters_space_paritioning*: this subdirectory contains all the experiments for pBN partitioning using the parameter lifting algorithm.


## Benchmarks:
We took benchmarks from Bayesian network repository: [bnlearn]. The (parameteric) benchmarks and the query files for all the experiments are accessible in the corresponding subdirectories.

## Dependencies:
- [Storm]: the backend probabilistic model checker and the parameter synthesis tools for the tasks "sensitivity function computation", "parameter space partitioning", and "feasibility analysis: gradient-descent".
- [Prophesy]: the backend parameter synthesis tool for feasibility analysis: QCQP and PSO.

Principle developer:
- Bahare Salmani (contact point: salmani at cs.rwth-aachen.de)

Developers:
- Robin Drahovskey
- Caroline Jabs
- David Korzeniewski


   [Storm]: <https://www.stormchecker.org/>
   [Prophesy]: <https://moves-rwth.github.io/prophesy/index.html>
   [Docker]: <https://docs.docker.com/get-docker/>
   [DockerHub]: <https://hub.docker.com/r/hansvrapi/storm-bn>
   [BIF format]: <http://www.cs.cmu.edu/afs/cs/user/fgcozman/www/Research/InterchangeFormat/>
   [Jani]: <https://jani-spec.org/>
   [Ace]: <http://reasoning.cs.ucla.edu/ace/>
   [Bayesserver]: https://www.bayesserver.com/
   [bnlearn]: <https://www.bnlearn.com/bnrepository/>
   [image]: <https://hub.docker.com/repository/docker/hansvrapi/storm-bn>
   

