//
// Created by Bahare Salmani on 2020-03-18.
//

#include "NETFileCreator.h"
#include <algorithm>


NETFileCreator::NETFileCreator(const std::vector<BNNode>& nodes_, const std::vector<ProbabilityTable>& tables_):
    nodes(nodes_),
    tables(tables_)
{
}

std::string NETFileCreator::create(){
    std::string retValue = "";
    retValue += "net" + NEW_LINE + OPENING_BRACE + NEW_LINE + CLOSING_BRACE + NEW_LINE;
    retValue += buildNodes();
    retValue += buildTables();
    return retValue;
}

std::string NETFileCreator::buildNodes(){
    std::string retValue = "";
    for(BNNode node : nodes){
        retValue += NODE + SPACE + node.getNodeName() + NEW_LINE;
        retValue += OPENING_BRACE + NEW_LINE;
        retValue += TAB + STATES + SPACE + NETEQ + SPACE + OPENING_PARAN;
        for(std::string possibleValue : node.getVariableValueList()){
            retValue += SPACE + DOUBLE_QUOTE + possibleValue + DOUBLE_QUOTE;
        }
        retValue += SPACE + CLOSING_PARAN + SEMICOLON + NEW_LINE;
        retValue += CLOSING_BRACE + NEW_LINE;
    }
    return retValue;
}


std::string NETFileCreator::buildTables(){
    std::string retValue = "";
    int i = 0;
    for(ProbabilityTable table : tables){
        /* create table's header */
        retValue += POTENTIAL + SPACE;
        retValue += OPENING_PARAN + SPACE + table.getNodeName() + SPACE;
        if(table.getParentsNames().size() > 0){
            retValue += "|" + SPACE;
            for(std::string parentName : table.getParentsNames()){
                retValue += parentName + SPACE;
            }
        }
        retValue += SPACE + CLOSING_PARAN + NEW_LINE;

        /* create table rows */
       retValue += OPENING_BRACE + NEW_LINE;
       retValue += TAB + DATA + SPACE + NETEQ + SPACE;

        const std::vector<ProbabilityRow>& sortedRows = table.getRows(); //sortTheRows(table);

        std::string paranthesises[sortedRows.size() + 1];
        handleParanthesises(table, paranthesises);
        int index = 0;
        for(ProbabilityRow row : sortedRows){
            retValue += paranthesises[index];
            retValue += OPENING_PARAN;
            const std::vector<std::string>& rowProbabilities = row.getProbabilities();
            for(std::string probability : rowProbabilities){
                retValue += probability + SPACE;
            }
            if(rowProbabilities.size() > 0){
                retValue.erase(retValue.end() - 1, retValue.end());
            }
            retValue += CLOSING_PARAN;
            index++;
        }
        retValue += paranthesises[index] + SPACE + SEMICOLON;
        retValue += NEW_LINE + CLOSING_BRACE + NEW_LINE;
        std::cout << "table " << i << " " << table.getNodeName() << " netized" << std::endl;
        i ++;
    }
    cout << "number of binarized nodes: " << i << std::endl;

    return retValue;
}




/* paranthesises[] should be of the size of number of rows + 1  */
void NETFileCreator::handleParanthesises(const ProbabilityTable& table, std::string paranthesises[]){
    int size = table.getRows().size();

    const std::vector<BNNode>& parentNodes = table.getParents();
    int numberOfParents = parentNodes.size();

    for(int i = 0; i < numberOfParents; i++){
        paranthesises[0] += OPENING_PARAN;
        paranthesises[size] += CLOSING_PARAN;
    }

    std::vector<int> whereParan;
    int num = 1;
    for(int i = numberOfParents - 1; i >= 0; i --){
        num = num * parentNodes.at(i).getNumberOfValues();
        whereParan.push_back(num);
    }

    for(int i = 1; i < size; i++){
        paranthesises[i] = "";
        for(int j = 0; j < numberOfParents; j ++){
            if(i % whereParan[j] == 0){
                paranthesises[i] += CLOSING_PARAN;
            }
        }
        for(int j = 0; j < numberOfParents; j ++){
            if(i % whereParan[j] == 0){
                paranthesises[i] += OPENING_PARAN;
            }
        }
    }
}


std::vector<ProbabilityRow> NETFileCreator::sortTheRows(const ProbabilityTable& table) {
    std::vector<ProbabilityRow> sortedRows;

    const std::vector<ProbabilityRow>& rows = table.getRows();
    int numberOfRows = rows.size();
    int rowIndexes[numberOfRows];
    for(int i = 0; i < numberOfRows; i ++){
        rowIndexes[i] = i;
    }
    mergeSort(table, rowIndexes, 0, numberOfRows - 1);
    for(int i = 0; i < numberOfRows; i ++){
        sortedRows.push_back(rows.at(rowIndexes[i]));
    }
    return sortedRows;
}

/* plus plus the current order of parents, according to NET file format ordering on parents
 * for example consider the case when the node has three parents and the first and the last parents have two possible values,
 * and the second parent has three possible values
 * this is the order in the table:
     * parent1: first value, parent2: first value, parent3: first Value ( ( ( ( p1, p2 )
     * parent1: first value, parent 2: first value, parent3: second value ( p3 , p4) )
     * parent1: first value, parent2: second value, parent3: first value ( (p5, p6)
     * parent1: first value, parent2: second value, parent 3: second value (p7, p8) )
     * parent1: first value, parent2: third value, parent3: first value ( (p9, p10)
     * parent1: first value, parent2: third value, parent 3: second value (p11, p12) ) )
     * parent1: second value, parent2: first value, parent3: first Value ( ( (p13, p14)
     * parent1: second value, parent 2: first value, parent3: second value (p15, p16) )
     * parent1: second value, parent2: second value, parent3: first value ( (p17, p18)
     * parent1: second value, parent2: second value, parent 3: second value (p19, p20) )
     * parent1: second value, parent2: third value, parent3: first value ( (p21, p22)
     * parent1: second value, parent2: third value, parent 3: second value (p23, p24) ) ) )*/

void NETFileCreator::plusPlus(int parentsCurrentOrders[], int size, const std::vector<BNNode>& parentNodes){

    for(int i = size - 1; i >= 0 ; i --){
        if(parentsCurrentOrders[i] < parentNodes.at(i).getNumberOfValues() - 1){
            parentsCurrentOrders[i] ++;
            break;
        }
        else{
            if(i == 0){
                break;
            }else{
                parentsCurrentOrders[i - 1] ++;
                for(int j = i; j < size; j ++)
                    parentsCurrentOrders[j] = 0;
            }
            break;
        }
    }
}

/* the rows should come from the same table */
bool NETFileCreator::isRow1lowerEqRow2(const ProbabilityRow& row1, const ProbabilityRow &row2, const std::vector<BNNode>& parentsNodes){
    const std::vector<std::string>& row1ParentsValues = row1.getParentValues();
    const std::vector<std::string>& row2ParentsValues = row2.getParentValues();

    int size = parentsNodes.size();
    for(int i = 0; i < size ; i ++) {
        if( parentsNodes.at(i).getIndexOfAParticularValue(row1ParentsValues.at(i)) >
            parentsNodes.at(i).getIndexOfAParticularValue(row2ParentsValues.at(i)) )
            return false;
        if( parentsNodes.at(i).getIndexOfAParticularValue(row1ParentsValues.at(i)) <
            parentsNodes.at(i).getIndexOfAParticularValue(row2ParentsValues.at(i)) )
            return true;
    }

    return true;
}



// Merges two subarrays of arr[]. Taken from https://www.geeksforgeeks.org/merge-sort/
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void NETFileCreator::merge(const ProbabilityTable & table, int arr[], int l, int m, int r)
{
    const std::vector<ProbabilityRow>& rows = table.getRows();
    const std::vector<BNNode>& parentNodes = table.getParents();
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    /* create temp arrays */
    int L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (isRow1lowerEqRow2(rows.at(L[i]), rows.at(R[j]), parentNodes)) /*instead of L[i] <= R[j]) */
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void NETFileCreator::mergeSort(const ProbabilityTable& table, int arr[], int l, int r)
{
    if (l < r)
    {
        int m = l+(r-l)/2;
        mergeSort(table, arr, l, m);
        mergeSort(table, arr, m+1, r);

        merge(table, arr, l, m, r);
    }
}
