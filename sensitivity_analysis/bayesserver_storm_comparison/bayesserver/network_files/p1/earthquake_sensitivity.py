
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
network = Network('earthquake')

#  add the nodes/variables

BurglaryTrue = State('True')
BurglaryFalse = State('False')
Burglary = Node('Burglary', [BurglaryTrue, BurglaryFalse])

EarthquakeTrue = State('True')
EarthquakeFalse = State('False')
Earthquake = Node('Earthquake', [EarthquakeTrue, EarthquakeFalse])

AlarmTrue = State('True')
AlarmFalse = State('False')
Alarm = Node('Alarm', [AlarmTrue, AlarmFalse])

JohnCallsTrue = State('True')
JohnCallsFalse = State('False')
JohnCalls = Node('JohnCalls', [JohnCallsTrue, JohnCallsFalse])

MaryCallsTrue = State('True')
MaryCallsFalse = State('False')
MaryCalls = Node('MaryCalls', [MaryCallsTrue, MaryCallsFalse])

nodes = network.getNodes()
nodes.add(Burglary)
nodes.add(Earthquake)
nodes.add(Alarm)
nodes.add(JohnCalls)
nodes.add(MaryCalls)
links = network.getLinks()
links.add(Link(Burglary, Alarm));
links.add(Link(Earthquake, Alarm));
links.add(Link(Alarm, JohnCalls));
links.add(Link(Alarm, MaryCalls));
tableBurglary = Burglary.newDistribution().getTable()

tableBurglary.set(0.01, [BurglaryTrue])
tableBurglary.set(0.99, [BurglaryFalse])
Burglary.setDistribution(tableBurglary)
tableEarthquake = Earthquake.newDistribution().getTable()

tableEarthquake.set(0.02, [EarthquakeTrue])
tableEarthquake.set(0.98, [EarthquakeFalse])
Earthquake.setDistribution(tableEarthquake)
tableAlarm = Alarm.newDistribution().getTable()

tableAlarm.set(0.95, [BurglaryTrue, EarthquakeTrue, AlarmTrue])
tableAlarm.set(0.05, [BurglaryTrue, EarthquakeTrue, AlarmFalse])
tableAlarm.set(0.29, [BurglaryFalse, EarthquakeTrue, AlarmTrue])
tableAlarm.set(0.71, [BurglaryFalse, EarthquakeTrue, AlarmFalse])
tableAlarm.set(0.94, [BurglaryTrue, EarthquakeFalse, AlarmTrue])
tableAlarm.set(0.06, [BurglaryTrue, EarthquakeFalse, AlarmFalse])
tableAlarm.set(0.001, [BurglaryFalse, EarthquakeFalse, AlarmTrue])
tableAlarm.set(0.999, [BurglaryFalse, EarthquakeFalse, AlarmFalse])
Alarm.setDistribution(tableAlarm)
tableJohnCalls = JohnCalls.newDistribution().getTable()

tableJohnCalls.set(0.9, [AlarmTrue, JohnCallsTrue])
tableJohnCalls.set(0.1, [AlarmTrue, JohnCallsFalse])
tableJohnCalls.set(0.05, [AlarmFalse, JohnCallsTrue])
tableJohnCalls.set(0.95, [AlarmFalse, JohnCallsFalse])
JohnCalls.setDistribution(tableJohnCalls)
tableMaryCalls = MaryCalls.newDistribution().getTable()

tableMaryCalls.set(0.7, [AlarmTrue, MaryCallsTrue])
tableMaryCalls.set(0.3, [AlarmTrue, MaryCallsFalse])
tableMaryCalls.set(0.01, [AlarmFalse, MaryCallsTrue])
tableMaryCalls.set(0.99, [AlarmFalse, MaryCallsFalse])
MaryCalls.setDistribution(tableMaryCalls)

evidence = DefaultEvidence(network)
evidence_str = 'default'
# TODO set any evidence here if you need to...

sensitivity = SensitivityToParameters(network, RelevanceTreeInferenceFactory())

parameter = ParameterReference(Alarm, [BurglaryTrue,EarthquakeTrue,AlarmTrue])
parameter_node = 'Alarm'
parameter_entry = '[BurglaryTrue,EarthquakeTrue,AlarmTrue]'
hypothesis = 'MaryCallsTrue'
network_name = 'earthquake'
start = time.time()
oneWay = sensitivity.oneWay(
    evidence,
    MaryCallsTrue,
    parameter
)
end = time.time()

print(f'Network name = {network_name}')
print('\n')

print('Time for Sensitivity Analysis: ' + str(end-start) + 's')
print('\n')

print(f'Parameter node = {parameter_node}')
print(f'Parameter entry = {parameter_entry}')
print(f'Hypothesis = {hypothesis}')
print(f'Evidence = {evidence_str}')

print('\n')

print('Parameter value = {}'.format(oneWay.getParameterValue()))
print('Sensitivity value = {}'.format(oneWay.getSensitivityValue()))
print('P(Abnormal | e) = {}'.format(oneWay.getProbabilityHypothesisGivenEvidence()))
print('Alpha = {}'.format(oneWay.getAlpha()))
print('Beta = {}'.format(oneWay.getBeta()))
print('Delta = {}'.format(oneWay.getDelta()))
print('Gamma = {}'.format(oneWay.getGamma()))
print('\n')

print('Eval(0.2) = {}'.format(oneWay.evaluate(0.2)))
print("Eval'(0.2) = {}".format(oneWay.evaluateDeriv(0.2)))
