# Storm-bn Refactored

IMPORTANT: Current state of Refactoring is in branch storm-bn-refactoring!

## What is already refactored: 

- Parser
- Settings
- Storage (except addEdgesToDAG, createEvidenceIndexToValue, createHypothesisIndicator,sortTheTables)

## Not yet refactored (only adapted to refactored data structures): 

- cretor
- transformer

## Problems

- Error when excecuting on networks with observed evidence, that has non-integer values in function createEvidenceIndexToValue (stoi)

## Todo (maybe)

The code before refactoring is in storm-bn-bahare and storm-bn-bahare-cli. 
I think I replaced all references in all files pointing to storm-bn with storm-bn-bahare. For building storm-bn-bahare the two subdirectories have to be added in src/CmakeLists.txt and cmake .. has to be executed again in the build folder. 
