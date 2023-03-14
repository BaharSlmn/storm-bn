
import jpype.imports
from jpype.types import *
import time
classpath = "bayesserver-9.5/Java/bayesserver-9.5.jar"

# Launch the JVM
jpype.startJVM(classpath=[classpath])

# import the Java modules
from com.bayesserver import *
from com.bayesserver.inference import *
from com.bayesserver.analysis import *
network = Network('cancer')

#  add the nodes/variables

Pollutionlow = State('low')
Pollutionhigh = State('high')
Pollution = Node('Pollution', [Pollutionlow, Pollutionhigh])

SmokerTrue = State('True')
SmokerFalse = State('False')
Smoker = Node('Smoker', [SmokerTrue, SmokerFalse])

CancerTrue = State('True')
CancerFalse = State('False')
Cancer = Node('Cancer', [CancerTrue, CancerFalse])

Xraypositive = State('positive')
Xraynegative = State('negative')
Xray = Node('Xray', [Xraypositive, Xraynegative])

DyspnoeaTrue = State('True')
DyspnoeaFalse = State('False')
Dyspnoea = Node('Dyspnoea', [DyspnoeaTrue, DyspnoeaFalse])

nodes = network.getNodes()
nodes.add(Pollution)
nodes.add(Smoker)
nodes.add(Cancer)
nodes.add(Xray)
nodes.add(Dyspnoea)
links = network.getLinks()
links.add(Link(Pollution, Cancer));
links.add(Link(Smoker, Cancer));
links.add(Link(Cancer, Xray));
links.add(Link(Cancer, Dyspnoea));
tablePollution = Pollution.newDistribution().getTable()

tablePollution.set(0.9, [Pollutionlow])
tablePollution.set(0.1, [Pollutionhigh])
Pollution.setDistribution(tablePollution)
tableSmoker = Smoker.newDistribution().getTable()

tableSmoker.set(0.3, [SmokerTrue])
tableSmoker.set(0.7, [SmokerFalse])
Smoker.setDistribution(tableSmoker)
tableCancer = Cancer.newDistribution().getTable()

tableCancer.set(0.03, [Pollutionlow, SmokerTrue, CancerTrue])
tableCancer.set(0.97, [Pollutionlow, SmokerTrue, CancerFalse])
tableCancer.set(0.05, [Pollutionhigh, SmokerTrue, CancerTrue])
tableCancer.set(0.95, [Pollutionhigh, SmokerTrue, CancerFalse])
tableCancer.set(0.001, [Pollutionlow, SmokerFalse, CancerTrue])
tableCancer.set(0.999, [Pollutionlow, SmokerFalse, CancerFalse])
tableCancer.set(0.02, [Pollutionhigh, SmokerFalse, CancerTrue])
tableCancer.set(0.98, [Pollutionhigh, SmokerFalse, CancerFalse])
Cancer.setDistribution(tableCancer)
tableXray = Xray.newDistribution().getTable()

tableXray.set(0.9, [CancerTrue, Xraypositive])
tableXray.set(0.1, [CancerTrue, Xraynegative])
tableXray.set(0.2, [CancerFalse, Xraypositive])
tableXray.set(0.8, [CancerFalse, Xraynegative])
Xray.setDistribution(tableXray)
tableDyspnoea = Dyspnoea.newDistribution().getTable()

tableDyspnoea.set(0.65, [CancerTrue, DyspnoeaTrue])
tableDyspnoea.set(0.35, [CancerTrue, DyspnoeaFalse])
tableDyspnoea.set(0.3, [CancerFalse, DyspnoeaTrue])
tableDyspnoea.set(0.7, [CancerFalse, DyspnoeaFalse])
Dyspnoea.setDistribution(tableDyspnoea)

evidence = DefaultEvidence(network)
evidence_str = 'default'
# TODO set any evidence here if you need to...

sensitivity = SensitivityToParameters(network, RelevanceTreeInferenceFactory())

parameter1 = ParameterReference(Cancer, [Pollutionlow,SmokerTrue,CancerTrue])
parameter_node1 = 'Cancer'
parameter_entry1 = '[Pollutionlow,SmokerTrue,CancerTrue]'
parameter2 = ParameterReference(Smoker, [SmokerTrue])
parameter_node2 = 'Smoker'
parameter_entry2 = '[SmokerTrue]'
hypothesis = 'DyspnoeaTrue'
network_name = 'cancer'
start = time.time()
twoWay = sensitivity.twoWay(
    evidence,
    DyspnoeaTrue,
    parameter1,
    parameter2
)
end = time.time()

print(f'Network name = {network_name}')
print('\n')

print('Time for Sensitivity Analysis: ' + str(end-start) + 's')
print('\n')
print(f'Parameter node 1 = {parameter_node1}')
print(f'Parameter entry 1 = {parameter_entry1}')
print(f'Parameter node 2 = {parameter_node2}')
print(f'Parameter entry 2 = {parameter_entry2}')
print(f'Hypothesis = {hypothesis}')
print(f'Evidence = {evidence_str}')
print('\n')

print('Parameter value 1 = {}'.format(twoWay.getParameterValue1()))

print(f'P({hypothesis} | e) = {twoWay.getProbabilityHypothesisGivenEvidence()}')
print('Alpha1 = {}'.format(twoWay.getAlpha1()))
print('Beta1 = {}'.format(twoWay.getBeta1()))
print('Delta1 = {}'.format(twoWay.getDelta1()))
print('Gamma1 = {}'.format(twoWay.getGamma1()))
print('\n')

print('Parameter value 2 = {}'.format(twoWay.getParameterValue2()))
print('Alpha2 = {}'.format(twoWay.getAlpha2()))
print('Beta2 = {}'.format(twoWay.getBeta2()))
print('Delta2 = {}'.format(twoWay.getDelta2()))
print('Gamma2 = {}'.format(twoWay.getGamma2()))
print('\n')

print('Eval(0.2,0.2) = {}'.format(twoWay.evaluate(0.2,0.2)))
