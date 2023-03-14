#include <iostream>
#include "storm-config.h"
#include "../storm-bn/parser/BNNetwork.h"
#include "../storm-bn/transformer/Binarizator.h"
#include "../storm-bn/transformer/NaiveBinarizator.h"
#include "../storm-bn/transformer/MatureBinarizator.h"
#include "../storm-bn/transformer/ChildBinarizator.h"



#include <string>
#include "storm-bn/creator/JaniFileCreator.h"
#include "storm-bn/creator/NETFileCreator.h"

#include "../storm-bn/transformer/MarkovianChainLevel.h"
#include "../storm-bn/transformer/EvidenceInjector.h"
#include <time.h>
#include <algorithm>

#include "storm/utility/initialize.h"
#include "storm-cli-utilities/cli.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/exceptions/FileIoException.h"


#include "storm/storage/jani/visitor/JSONExporter.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm-parsers/parser/ExpressionParser.h"
#include <sstream>

using namespace std::chrono;


using namespace std;

int main(const int argc, const char **argv) {

    try {



        storm::utility::setUp();
        storm::cli::printHeader("Storm-bn", argc, argv);
        Utils util;


        std::string fileName = "A";
        auto start0 = high_resolution_clock::now();

        BNNetwork bnnetwork(STORM_TEST_RESOURCES_DIR "/bn/", fileName, ".bif");


        auto start = high_resolution_clock::now();

      /*  ChildBinarizator binarizator(bnnetwork);

        const BNNetwork& binarizedNetwork = binarizator.getBinarizedNetwork();
        NETFileCreator netFileCreator(binarizator.getBinarizedNodes(), binarizator.getBinarizedTables());

        std::string netContent = netFileCreator.create();
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        cout << "Time taken by binarization: "
             << duration.count() << " microseconds" << endl;

        util.writeToFile(netContent, STORM_SOURCE_DIR "/src/storm-bn-cli/netfiles/" + fileName + "mb" + ".net"); */


        JaniFileCreator janicreator(bnnetwork);
        std::string janiContent = janicreator.create();
        auto stop2 = high_resolution_clock::now();
        auto duration2 = duration_cast<microseconds>(stop2 - start0);

       util.writeToFile(janiContent, STORM_SOURCE_DIR "/src/storm-bn-cli/janifiles/" + fileName + ".jani");

        cout << "Time taken for jani file creation: "
             << duration2.count() << " microseconds" << endl;


        return 0;
    }

    catch (storm::exceptions::BaseException const&exception) {
        STORM_LOG_ERROR(
                "An exception caused Storm to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const&exception) {
        STORM_LOG_ERROR(
                "An unexpected exception occurred and caused Storm to terminate. The message of this exception is: "
                << exception.what());
        return 2;
    }
}


