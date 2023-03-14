#include <iostream>
#include <fstream> //For file operation
#include <regex>  // For regular expression

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <array>

using namespace std;

//#include "matplotlib-cpp/matplotlibcpp.h"
//namespace plt = matplotlibcpp;

string benchmarks_directory = "pso-qcqp-gd-benchmarks/";
string csvfiles_directory = "csv-files/";
string prophesy_directory = "/opt/prophesy/scripts/";
string storm_gd_directory = "storm/build/bin/";
double timeout = 15; /* minutes*/
enum Experiment { pso, qcqp, gd,gdex };



string readFile(string filePath, string defaultContent){
    ifstream ifs(filePath);
     string content( (istreambuf_iterator<char>(ifs) ),
                          (istreambuf_iterator<char>()    ) );
    return content;
}

void writeFile(string filePath, string fileContent){
    ofstream outputFile;
    outputFile.open(filePath);
    outputFile << fileContent;
}

string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
       throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

string getPSOTime(string textresult, string infoKey){
    istringstream f(textresult);
    string line;
    bool isTimeOut = true;
    while (getline(f, line)) {
        if(line.find(infoKey) != string::npos){
            isTimeOut = false;
            break;
        }
    }
    if(isTimeOut)
        return "10000"; /*TIMEOUT*/
    vector<string> infolinevector;
    istringstream iss(line);
    for(string s; iss >> line; )
        infolinevector.push_back(line);
    string info = infolinevector.at(3);
    info.erase(prev(info.end()));
    return info;
}

string getQCQPTime(string result, string infoKey){
    istringstream f(result);
    string line;
    bool isTimeOut = true;
    while (getline(f, line)) {
        if(line.find(infoKey) != string::npos){
            isTimeOut = false;
            string delimiter = "=";
            line.erase(0, line.find(delimiter) + delimiter.length());
            return line;
        }
    }
    return "5000"; /*TIMEOUT - two times will be summed up, so we took the half as the extra tick number*/
}


string getGDTime(string textresult, string infoKey){
    istringstream f(textresult);
    string line;
    bool isTimeOut = true;
    while (getline(f, line)) {
        if(line.find(infoKey) != string::npos){
            isTimeOut = false;
            break;
        }
    }
    if(isTimeOut)
        return "10000"; /*TIMEOUT*/
    vector<string> infolinevector;
    istringstream iss(line);
    for(string s; iss >> line; )
        infolinevector.push_back(line);
    string info = infolinevector.at(2);
    info.erase(prev(info.end()));
    return info;
}

string getNumberOfRealParams(string filePath, string addedParamsNum){
    string command = "python3 scripts/number_of_params.py " + filePath + " " + addedParamsNum;
    const char *cmnd = command.c_str();
    string str = exec(cmnd);
    str.erase(remove(str.begin(), str.end(), '\n'), str.end());
    return str;
}

string makePSOCommand(string networkName, string instance, string belowOrAbove, double threshhold){
    string pso_command = "";
    string prophesy_feasibility_command = "";
    prophesy_feasibility_command += "timeout " + to_string(timeout) + "m python3 " + prophesy_directory + "parameter_synthesis.py load-problem " + benchmarks_directory    + networkName + "/drn_files/" + networkName + "_" + instance + ".drn " + benchmarks_directory + networkName + "/" + networkName + ".pctl set-threshold " + to_string(threshhold) + " find-feasible-instantiation " + belowOrAbove + " ";
    pso_command += prophesy_feasibility_command + "pso";
    return pso_command;
}

string makeQCQPCommand(string networkName, string instance, string belowOrAbove, double threshhold){
    string qcqp_command = "";
    string prophesy_feasibility_command = "";
    prophesy_feasibility_command += "timeout " + to_string(timeout) + "m python3 " + prophesy_directory + "parameter_synthesis.py load-problem " + benchmarks_directory   + networkName +  "/drn_files/" + networkName + "_" + instance + ".drn " + benchmarks_directory + networkName + "/" + networkName + ".pctl set-threshold " + to_string(threshhold) + " find-feasible-instantiation " + belowOrAbove + " ";
    qcqp_command += prophesy_feasibility_command + "qcqp";
    cout << qcqp_command << "\n";
    return qcqp_command;
}

string makeGDCommands(string networkName, string instance, string belowOrAbove, double threshhold, string PCTLFormulae, double leaningRate){
    string gd_command = "";
    string PmaxPmin = "";
    if(belowOrAbove == "below"){
        belowOrAbove = "<=";
        PmaxPmin = "Pmin";
    }
    if(belowOrAbove == "above"){
        belowOrAbove = ">=";
        PmaxPmin = "Pmax";
    }
    gd_command += "timeout " + to_string(timeout) + "m " + storm_gd_directory + "storm-pars --explicit-drn " + benchmarks_directory + networkName + "/drn_files/" + networkName + "_" + instance + ".drn --prop \"" + PmaxPmin + belowOrAbove + to_string(threshhold) + " " + PCTLFormulae + "\" --find-feasible";
    cout << gd_command << "\n";
    return gd_command;
}

string makeGDCommandsExtremum(string networkName, string instance, string belowOrAbove, string PCTLFormulae, double leaningRate){
    string gd_command = "";
    string PmaxPmin = "";
    if(belowOrAbove == "below"){
        belowOrAbove = "<=";
        PmaxPmin = "Pmin";
    }
    if(belowOrAbove == "above"){
        belowOrAbove = ">=";
        PmaxPmin = "Pmax";
    }
    gd_command += "timeout " + to_string(timeout) + "m " + storm_gd_directory + "storm-pars --explicit-drn " + benchmarks_directory + networkName + "/drn_files/" + networkName + "_" + instance + ".drn --prop \"" + PmaxPmin + "=? " + PCTLFormulae + "\" --find-extremum";
    cout << gd_command << "\n";
    return gd_command;
}

void runASingleExperiment(string networkName, string instance, string belowOrAbove, double threshhold, string PCTLFormulae, double leaningRate, Experiment e){
    string command = "";
    string result;
    switch(e)
    {
        case pso  : command = makePSOCommand(networkName, instance, belowOrAbove, threshhold);  break;
        case qcqp: command = makeQCQPCommand(networkName, instance, belowOrAbove, threshhold); break;
        case gd : command = makeGDCommands(networkName, instance, belowOrAbove, threshhold, PCTLFormulae, leaningRate);  break;
        case gdex : command = makeGDCommandsExtremum(networkName, instance, belowOrAbove, PCTLFormulae, leaningRate);  break;
    }
    const char *cmnd = command.c_str();
    try{
        result = exec(cmnd);
    }
    catch(const exception& e){
        result = -1;
    }
    
    string filePathPrefix = benchmarks_directory + networkName + "/" + networkName + "_" + instance + "_" + to_string(threshhold) + "_" + belowOrAbove;
    switch(e)
    {
        case pso  : writeFile(filePathPrefix + "_pso.txt", result);  break;
        case qcqp  : writeFile(filePathPrefix + "_qcqp.txt", result);  break;
        case gd  : writeFile(filePathPrefix + "_gd.txt", result);  break;
        case gdex : writeFile(filePathPrefix + "_gd_extremum.txt", result);  break;
    }
}


void runFeasibilityExperimentsForOneNetwork(string network, double thresholds[], string instances[], string belowOrAbove[], int numberOfInstances){
        for(int i = 0; i < numberOfInstances; i++){
        string PCTLFormulae = readFile(benchmarks_directory + network + "/" + network + ".pctl", "P=? [F(true)]");
        string gd_format_PCTLFormulae = readFile(benchmarks_directory + network + "/" + network + "_gd_format.pctl", "P=? [F(true)]");
        runASingleExperiment(network, instances[i], belowOrAbove[i], thresholds[i], gd_format_PCTLFormulae, 0.1, gd);
        runASingleExperiment(network, instances[i], belowOrAbove[i], thresholds[i], gd_format_PCTLFormulae, 0.1, gdex);
        runASingleExperiment(network, instances[i], belowOrAbove[i], thresholds[i], PCTLFormulae, 0.1, pso);
        runASingleExperiment(network, instances[i], belowOrAbove[i], thresholds[i], PCTLFormulae, 0.1, qcqp);
    }
}


void runFeasibilityExperiments(){
    string network = "alarm";
    string instances_a[9] = {"1", "2", "4", "8","16","32","64","128","256"};
    double thresholds_a[9] = {0.9920,0.9917,0.9915,0.9900,0.9892,0.9210,0.1000,0.0500,0.0200};
    string belowOrAbove_a[9] = {"below","below","below","below","below","below","below","below","below"};
    runFeasibilityExperimentsForOneNetwork(network, thresholds_a, instances_a, belowOrAbove_a, 9);

    network = "hepar2";
    double thresholds_hepar[10] = {0.9999,0.9995,0.9992,0.9990,0.9989,0.9988,0.9987,0.9986,0.1000,0.0500}; 
    string instances_hepar[10] = {"1", "2", "4", "8","16","32","64","128","256","512"};
    string belowOrAbove_hepar[10] = {"below","below","below","below","below","below","below","below","below","below"};
    runFeasibilityExperimentsForOneNetwork(network, thresholds_hepar, instances_hepar, belowOrAbove_hepar, 10);
    
    
    network = "win95pts";
    string instances_w[9] = {"1", "2", "4", "8","32","64","128","256","512"};
    double thresholds_w[9] = {0.9998,0.9996,0.9980,0.9975,0.9721,0.9000,0.1000,0.0500,0.0200};
    string belowOrAbove_w[9] = {"below","below","below","below","below","below","below","below","below"};
    runFeasibilityExperimentsForOneNetwork(network, thresholds_w, instances_w, belowOrAbove_w, 9);
    
    network = "sachs";
    string instances_s[8] = {"1", "2", "4", "8","16","32","64","128"};
    double thresholds_s[8] = {0.66,0.65,0.64,0.55,0.25,0.1,0.05,0.02};
    string belowOrAbove_s[8] = {"below","below","below","below","below","below","below","below"};
    runFeasibilityExperimentsForOneNetwork(network, thresholds_s, instances_s, belowOrAbove_s, 8);

    network = "hailfinder";
    string instances_h[7] = {"32", "64", "128", "256","512","1024","2048"};
    double thresholds_h[7] = {0.600000,0.600000,0.600000,0.600000,0.600000,0.600000,0.600000};
    string belowOrAbove_h[7] = {"below","below","below","below","below","below","below"};
    runFeasibilityExperimentsForOneNetwork(network, thresholds_h, instances_h, belowOrAbove_h, 7);
}


string parseFeasibilityExperimentsForOneNetwork(string networkName, double thresholds[], string instances[], string belowOrAbove[], int numberOfInstances){
        string csvcontent;
        string filePathPrefix = "";
        for(int i = 0; i < numberOfInstances; i++){
            filePathPrefix = benchmarks_directory + networkName + "/"  + networkName + "_" + instances[i] + "_" + to_string(thresholds[i]) + "_" + belowOrAbove[i];
            string result = readFile(filePathPrefix + "_pso.txt","");
            string psotime = getPSOTime(result, "This procedure took");
            
            result = readFile(filePathPrefix + "_qcqp.txt","");
            string qcqptime = to_string(stod((getQCQPTime(result, "solver time"))) + stod(getQCQPTime(result, "encoding time")));

            result = readFile(filePathPrefix + "_gd.txt","");
            string gdtime = getGDTime(result, "Finished in");
            
            string realparamnum = getNumberOfRealParams(filePathPrefix + "_gd.txt ", instances[i]);

            /* the new line on csvfile*/
        csvcontent += networkName + "," + instances[i] + "," + realparamnum + "," + to_string(thresholds[i])  + "," + gdtime + "," + psotime + "," + qcqptime + "," + networkName + "\n";
    }
    return csvcontent;
}


string parseFeasibilityExperiments(){
    string first_line = "networkname,networkinstance,realparamnum,threshhold,gdtime,psotime,qcqptime,scatterclass\n";
    string csvcontent = first_line;
    string parsecontent = "";

    string network = "hepar2";
    double thresholds_hepar[10] = {0.9999,0.9995,0.9992,0.9990,0.9989,0.9988,0.9987,0.9986,0.1000,0.0500};
    string instances_hepar[10] = {"1", "2", "4", "8","16","32","64","128","256","512"};
    string belowOrAbove_hepar[10] = {"below","below","below","below","below","below","below","below","below","below"};
    parsecontent = parseFeasibilityExperimentsForOneNetwork(network, thresholds_hepar, instances_hepar, belowOrAbove_hepar, 10);
    writeFile("csv_files/hepar2-feasibility-result.csv", first_line + parsecontent);
    csvcontent += parsecontent;

    
    network = "win95pts";
    string instances_w[9] = {"1", "2", "4", "8","32","64","128","256","512"};
    double thresholds_w[9] = {0.9998,0.9996,0.9980,0.9975,0.9721,0.9000,0.1000,0.0500,0.0200};
    string belowOrAbove_w[9] = {"below","below","below","below","below","below","below","below","below"};
    parsecontent = parseFeasibilityExperimentsForOneNetwork(network, thresholds_w, instances_w, belowOrAbove_w, 9);
    writeFile("csv_files/win95pts-feasibility-result.csv", first_line + parsecontent);
    csvcontent += parsecontent;


    network = "alarm";
    string instances_a[9] = {"1", "2", "4", "8","16","32","64","128","256"};
    double thresholds_a[9] = {0.9920,0.9917,0.9915,0.9900,0.9892,0.9210,0.1000,0.0500,0.0200};
    string belowOrAbove_a[9] = {"below","below","below","below","below","below","below","below","below"};
    parsecontent = parseFeasibilityExperimentsForOneNetwork(network, thresholds_a, instances_a, belowOrAbove_a, 9);
    writeFile("csv_files/alarm-feasibility-result.csv", first_line + parsecontent);
    csvcontent += parsecontent;

    network = "sachs";
    string instances_s[8] = {"1", "2", "4", "8","16","32","64","128"};
    double thresholds_s[8] = {0.66,0.65,0.64,0.55,0.25,0.1,0.05,0.02};
    string belowOrAbove_s[8] = {"below","below","below","below","below","below","below","below"};
    parsecontent = parseFeasibilityExperimentsForOneNetwork(network, thresholds_s, instances_s, belowOrAbove_s, 8); 
    writeFile("csv_files/sachs-feasibility-result.csv", first_line + parsecontent);
    csvcontent += parsecontent;

    network = "hailfinder";
    string instances_h[7] = {"32", "64", "128", "256","512","1024","2048"};
    double thresholds_h[7] = {0.600000,0.600000,0.600000,0.600000,0.600000,0.600000,0.600000};
    string belowOrAbove_h[7] = {"below","below","below","below","below","below","below"};
    parsecontent = parseFeasibilityExperimentsForOneNetwork(network, thresholds_h, instances_h, belowOrAbove_h, 7); 
    writeFile("csv_files/hailfinder-feasibility-result.csv", first_line + parsecontent);
    csvcontent += parsecontent;

    
    return csvcontent;
}

bool parse_boolean(string value){
    vector<string> list{"true", "yes", "y", "1", "t"};

    if (find(begin(list), end(list), value) != end(list)){    
        return true;
    }        
    return false;
}

int main(int argc, char** argv) {
    bool flag; //boolean variable that indicates whether the used wants to rerun the experiments
    if(argc > 1){
        flag = parse_boolean(argv[1]);
    }
    if(flag){
        runFeasibilityExperiments();
    }
    string csvcontent = parseFeasibilityExperiments();
    writeFile("csv_files/feasibility-results.csv", csvcontent);
    return 0;
}

