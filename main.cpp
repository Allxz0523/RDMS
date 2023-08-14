#include "head.h"

int main() {

	// Class "Graph"
    Graph g;

    // Step 1: Data Cleaning
    // -----------------------------------------------------------------------------
    cout << "\n" << "Step 1: Data Cleaning" << endl;
    cout << "-------------------------------------" << endl;

    cout << "Dataa Cleaning Done." << endl;

	// Step 2: Data Preparation
    // -----------------------------------------------------------------------------
    cout << "\n" << "Step 2: DATA PREPARE" << endl;
    cout << "-------------------------------------" << endl;

    // Read Road Network
    g.read_graph();        // Can Be "one-way" (edge:651748) or "two-way" (edge:774660)
    // Read Road Information
    g.read_road_info();
    // Read Query Data
    int readNum = 10000;    // Number of Routes for Simulation Operation
    vector<vector<int>> queryDataRaw = g.read_query(g.queryPath, readNum);
    // Read Route Data
    vector<vector<int>> routeDataRaw = g.read_route(g.route_path, readNum);
    // Split Route and Query Data to Average Length
    bool cut = true;		// If cut data or not
    vector<vector<int>> queryData;
    vector<vector<int>> routeData;
    int avg_length = 30;    // Parameter (Average Route Length)
    if (cut == true){
        pair<vector<vector<int>>, vector<vector<int>>> dataCombine;
		dataCombine = g.data_length_modify(queryDataRaw, routeDataRaw, avg_length);
        queryData = dataCombine.first;
        routeData = dataCombine.second;
    }
    else
    {
        queryData = queryDataRaw;
        routeData = routeDataRaw;
    }
    // Find Min Departure Time from Queries
    g.min_depar_time(queryData);
    // Convert Route from "Node ID Pair" to "Road ID"
    g.route_nodeID_2_roadID(routeData);
    // Classify Each Road with A Unique Latency Function
    g.classify_latency_function();
    // Define Flow Base
    g.minRange = 20;    // minRange -> Min Time Range
    g.flowIni = 0;      // flowIni -> Initialized Flow Base Value
    g.flow_base_ini(g.minRange, g.flowIni);

    // Step 3: ALGORITHM I SIMULATION
    // -----------------------------------------------------------------------------
    cout << "\n" << "Step 3: ALGORITHM I SIMULATION" << endl;
    cout << "-------------------------------------" << endl;
	
	bool range = true;
    // Algorithm I: Simulation
    vector<vector<pair<int,int>>> Test = g.alg1Records(queryData, routeData, range);

    // Step 4: ROUTE DATA UPDATE OPERATIONS PREPARATION
    // -----------------------------------------------------------------------------
    cout << "\n" << "Step 4: ROUTE DATA UPDATE OPERATIONS PREPARATION" << endl;
    cout << "-------------------------------------" << endl;

    // Check Simulation
    bool simulation = 1;    // simulation = 1 -> records based on simulation operation
    g.check_simulation(simulation);
    // Convert Time Records from Node ID Pairs to Road ID
    g.nodeID_2_roadID_in_records(g.timeFlowChange);
    // Check Correctness of Time Records
    g.time_record_correct_check();
    // Split Time Flow Change into Time Slices
    g.split_2_time_slices(g.route_timeFlowChange);
	
    // Step 5: ROUTE DATA INSERTION OPERATIONS
    // -----------------------------------------------------------------------------
    cout << "\n" << "Step 5: ROUTE DATA INSERTION OPERATIONS" << endl;
    cout << "-------------------------------------" << endl;

    bool terminal = true;   // Parameter (Decide If Active Terminal Condition)
    bool parallel = false;  // Decide If Parallel Operation

    // Generate New Insertion Data & Write into Files
    // Do Not Have to Generate Every Time. Only Generate When Change Data or Numbers
    if (simulation == 1){    // simulation = 1 -> records based on simulation operation
        int newGenerNum = 1000;     // Number of Routes for Insertion Operation
        cout << "Add " << newGenerNum << " New Route." << endl;
        srand((unsigned)time(NULL));
		g.data_generation(g.routeRoadPath, g.deparTimePath, g.routeNodePath, newGenerNum, avg_length, cut);
        // Variable Initialization
		g.Pi.reserve(readNum + newGenerNum);
        g.Pi = routeData;
        g.routeDataSize = routeData.size();
    }
    else{    // simulation = 0 -> same data with input of simulation operation
        cout << "Inserting " << readNum << " routes for clear road network" << endl;
        // Variable Initialization
        g.Pi.clear();
        g.routeDataSize = 0;
    }
    // Read Generated Route Data
    g.read_new_data(g.routeRoadPath, g.deparTimePath, g.routeNodePath); 
    // "ETA_result" Initialization
    g.ETA_initialization(simulation, false); // bool -> not print ETA
    // Insertion Operation (Main)
    g.update_operation_insertion(parallel, true, range, false);   // w Termination (terminal == true; range == true & print == false)
    g.update_operation_insertion(parallel, false, range, false);  //w/o Termination (terminal == false; range == true & print == false)
    // Insertion Operation w Parallel
    // Split New Route Data into Groups
    parallel = true;
    int threadNum = 7;		// Define Thread Number
    g.multi_new_data_initial();
    g.update_operation_parallel(g.newDataMulti, parallel, true, range, false, threadNum);  // Parallel w Termination (range == true & print == false)
    g.update_operation_parallel(g.newDataMulti, parallel, false, range, false, threadNum);  // Parallel w/o Termination (range == true & print == false)
	
	// Step 6: TRAVEL TIME ESTIMATION (ETA) UPDATE
    // -----------------------------------------------------------------------------
    cout << "\n" << "Step 6: TRAVEL TIME ESTIMATION (ETA) UPDATE" << endl;
    cout << "-------------------------------------" << endl;

    // "ETA_result" Update
    g.ETA_update();

    // Step 7: ROUTE DATA DELETION OPERATIONS
    // -----------------------------------------------------------------------------
    cout << "\n" << "Step 7: ROUTE DATA INSERTION OPERATIONS" << endl;
    cout << "-------------------------------------" << endl;

    // Generate New Deletion Data & Write into Files
    // Do Not Have to Generate Every Time. Only Generate When Change Data or Numbers
    int deleteNum = 1000;   // // Number of Routes for Deletion Operation
    parallel = false;
    cout << "Generate " << deleteNum << " existing route for deletion operation." << endl;
    srand((unsigned)time(NULL));
    g.dele_data_generation(g.routeRoadPathD, g.routeNodePathD, g.routeIndexpath, deleteNum);
    // Read Generated Route Data
    g.read_deletion_data(g.routeRoadPathD, g.routeIndexpath);
    // Deletion Operation
	g.update_operation_deletion(parallel, true, range, false);   // w Termination (range == true & print == false)
    g.update_operation_deletion(parallel, false, range, false);  // w/o Termination (range == true & print == false)
	// Deletion Operation Parallel
    // Split Target Route Data into Groups
    parallel = true;
    g.multi_del_data_initial();
    g.del_operation_parallel(g.delDataMulti, parallel, true, range, false, threadNum);	 // Parallel w Termination (range == true & print == false)
    g.del_operation_parallel(g.delDataMulti, parallel, false, range, false, threadNum);  // Parallel w/o Termination (range == true & print == false)

    // Step 8: ... ...
    // -----------------------------------------------------------------------------
    cout << "\n" << "Step 8: ... ..." << endl;
    cout << "-------------------------------------" << endl;
    std::cout << "All Operations Done." << std::endl;
    return 0;
}
