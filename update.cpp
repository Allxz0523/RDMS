#include "head.h"

// Check Simulation
// simulation = 1 -> records based on simulation operation
// simulation = 0 -> records are cleared without simulation operation
void Graph::check_simulation(int simulation)
{
	if (simulation == 0){
		// Clear Simulated Records
		timeFlowChange.clear();
		// Initialize Time Records When Traffic Flow Change
		timeFlowChange.resize(graphLength.size());
		for (int i=0;i<timeFlowChange.size();i++){
			timeFlowChange[i].resize(graphLength[i].size()); // num of nei
			for (int j=0;j<graphLength[i].size();j++){
				timeFlowChange[i][j].first = graphLength[i][j].first;
			}
		}
	}
	// Print
	/*
	if (simulation == 0)
		cout << "Simulation status is " << simulation << " -> new routes are based on empty structure" << endl;
	else
		cout << "Simulation status is " << simulation << " -> new routes are adding based on simulation operation." << endl;
	*/
}


// Change NodeID to RoadID
void Graph::nodeID_2_roadID_in_records(vector<vector<pair<int,map<int, vector<vector<int>>>>>> &timeFlowChange){

	// Variable Initialization
	route_timeFlowChange.clear();
	// route_timeFlowChange.resize(roadID2NodeID.size());
    map<int, vector<vector<int>>> placeholder;
    route_timeFlowChange.assign(roadID2NodeID.size(), placeholder);
	int node01 = 0;
    int node02 = 0;
	int routeID = 0;
	for (int i=0;i<timeFlowChange.size();i++){
		node01 = i;
		for (int j=0;j<timeFlowChange[i].size();j++){
			node02 = timeFlowChange[i][j].first;
			routeID = nodeID2RoadID[make_pair(node01,node02)];
			// if (routeID < 0 || routeID > Pi.size())
			// cout << "45 " << routeID << endl;
			route_timeFlowChange[routeID] = timeFlowChange[i][j].second;
		}
	}
	
	/*
	// Print
	for (int i=0;i<route_timeFlowChange.size();i++)
	{
	cout << "RoadSegmentID: " << i << " size " << route_timeFlowChange[i].size() << endl;
	map<int, vector<vector<int>>>::iterator itr;
	for (itr = route_timeFlowChange[i].begin(); itr != route_timeFlowChange[i].end(); ++itr)
	{
	cout << " time " << itr->first;
	for (int j=0;j<itr->second.size();j++)
	{
	cout << " routeID " << itr->second[j][0];
	cout << " status " << itr->second[j][1] << " flow " << itr->second[j][2] << "||";
	}
	}
	cout << "\n" << endl;
	}
	*/
	cout << "Node 2 Route Done." << endl;
}

// Convert int Time into UTC Hour
int Graph::time_2_hour(int intTime)
{
	time_t time = intTime;
	tm *ltm = localtime(&time);
	int hour = ltm->tm_hour - 8;
	return hour;
}

// Find Min Departure Time from Queries
void Graph::min_depar_time(vector<vector<int>> &Q)
{
	vector<int> times;
	// Query Data Size Depend on Different Manipulation
	if (Q.size() != 0){
		for (int i = 0; i < Q.size(); i++){
			times.push_back(Q[i][2]);
		}
		// Find Min Time
		minDeparture = *min_element(times.begin(), times.end());
		minHour = time_2_hour(minDeparture);
		// Print
		/* cout << "Min time is: " << minDeparture << " in " << minHour << " hour." << endl; */
	}
	else{
		// Print
		cout << "Data size is 0. Cannot find min time." << endl;
	}

}

// Convert Hour to Its Belonged Index (24 Hours)
int Graph::hour_2_index(int hour)
{
	int index = hour - minHour;
	// Assign 24 Time Slices for Each Road
	if (index >= 0){
		return index;
		//		return 0;
	}
	else{
		index += 24;
		return index;
		//		return 0;
	}
}


// Split Time Flow Change into Time Slices
void Graph::split_2_time_slices(vector<map<int, vector<vector<int>>>> &route_timeFlowChange)
{	

	// Define First Time Record
	timeSlice.clear();
    map<int, vector<vector<int>>> placeholder1;
    vector<map<int, vector<vector<int>>>> placeholder2;
    placeholder2.assign(24, placeholder1);
	// timeSlice.resize(route_timeFlowChange.size());
    timeSlice.assign(route_timeFlowChange.size(), placeholder2);

	for (int i = 0; i < route_timeFlowChange.size();i++){
		int RoadSegmentID = i;
		map<int, vector<vector<int>>>::iterator itr;
		for (itr = route_timeFlowChange[i].begin(); itr != route_timeFlowChange[i].end(); ++itr){
            int time = itr->first;
            int hour = time_2_hour(time);
            int index = hour_2_index(hour);
            for (int j=0;j<itr->second.size();j++){
                if (timeSlice[RoadSegmentID][index].find(time) == timeSlice[RoadSegmentID][index].end()){
                    timeSlice[RoadSegmentID][index].insert(pair<int, vector<vector<int>>>(itr->first,{itr->second[j]}));
                }
                else{
                    timeSlice[RoadSegmentID][index][time].push_back(itr->second[j]);
                }
            }
		}
	}
	
    for (int i=0;i<timeSlice.size();i++){   // Road
        for (int j=1;j<timeSlice[i].size();j++){    // Time Slice
            // Find The Last Flow Number of Previous Time Slice
            int flowRest = 0;
            if (timeSlice[i][j-1].size() != 0) {
                map<int, vector<vector<int>>> temp = timeSlice[i][j-1];
                map<int, vector<vector<int>>>::iterator itr;
                itr = timeSlice[i][j-1].end();
                itr = --itr;
                vector<vector<int>> temp1 = itr->second;
                vector<int> temp2 = temp1.back();
                int temp3 = temp2[2];
                flowRest = temp3;
                // flowRest = timeSlice[i][j-1].end()->second.back()[2];
            }else{
                flowRest = 0;
			}
            int flowInsert = 0;
            // Add Virtual Records
            while (flowRest > 0){
                flowInsert += 1;
                if (timeSlice[i][j].find(0) == timeSlice[i][j].end()){
                    timeSlice[i][j].insert(pair<int, vector<vector<int>>>((0), {{-1, 1, flowInsert}}));
				}
                else{
                    timeSlice[i][j][0].push_back({-1, 1, flowInsert});
				}
                flowRest -= 1;
            }
        }
    }
	// variable to store the last flow of current time slice
    int lastFlow = 0;
    // iterate all time slice to add placeholder with "out" status
    for (int i=0;i<timeSlice.size();i++){
        for (int j=0;j<timeSlice[i].size();j++){
            if (timeSlice[i][j].size() != 0) {
                map<int, vector<vector<int>>>::iterator itr;
                // locate to the last time record
                itr = timeSlice[i][j].end();
                itr = --itr;
                // locate to the last flow of the last time slice
                lastFlow = itr->second.back()[2];
                while (lastFlow > 0){
                    lastFlow -= 1;
                    if (timeSlice[i][j].find(0) == timeSlice[i][j].end())
                        timeSlice[i][j].insert(pair<int, vector<vector<int>>>((0), {{-1, 0, lastFlow}}));
                    else
                        timeSlice[i][j][0].push_back({-1, 0, lastFlow});
                }
            }
        }
    }
	cout << "Time Slice Split Done." << endl;
}

// Check Correctness of Time Records
void Graph::time_record_correct_check()
{
	// Clear Incorrect Time Records
	for (int i=0; i<route_timeFlowChange.size(); i++) {
		if (route_timeFlowChange[i].size() != 0){
			// If 1st Flow is not 1 -> Delete
			map<int, vector<vector<int>>>::iterator itrBegin;
			itrBegin = route_timeFlowChange[i].begin();
			if (itrBegin->second[0][2] != 1){
				route_timeFlowChange[i].clear();
				continue;
			}
			// If last Flow Is not Zero -> Delete
			map<int, vector<vector<int>>>::iterator itrEnd;
			itrEnd = --route_timeFlowChange[i].end();
			if (itrEnd->second.back()[2] != 0){
				route_timeFlowChange[i].clear();
				continue;
			}

			// If Flows Are not Increase or Decrease by One -> Delete
			map<int, vector<vector<int>>>::iterator itrCurrent;
			for (itrCurrent = route_timeFlowChange[i].begin(); itrCurrent != --route_timeFlowChange[i].end(); ++itrCurrent){
				if (itrCurrent->second.size() == 1){
					int CFlow = itrCurrent->second[0][2];
					++itrCurrent;
					int NFlow = itrCurrent->second[0][2];
					int FlowChange = abs(CFlow - NFlow);
					--itrCurrent;
					if (FlowChange != 1){
						route_timeFlowChange[i].clear();
						break;
					}
				}
				else{
					for (int i=0;i<itrCurrent->second.size()-1;i++){
						int CFlow = itrCurrent->second[i][2];
						int NFlow = itrCurrent->second[i+1][2];
						int FlowChange = abs(CFlow - NFlow);
						if (FlowChange != 1){
							route_timeFlowChange[i].clear();
							break;
						}
					}

					int CFlow = itrCurrent->second.back()[2];
					++itrCurrent;
					int NFlow = itrCurrent->second[0][2];
					int FlowChange = abs(CFlow - NFlow);
					--itrCurrent;
					if (FlowChange != 1){
						route_timeFlowChange[i].clear();
						break;
					}
				}
			}
		}
	}
}

// Convert Node Constructed Route to Road Constructed Route
vector<int> Graph::route_node_2_route_road(vector<int> &routeNode)
{
	// Variable Initialization
	vector<int> routeRoad;
	// routeRoad.resize(routeNode.size()-1);
	routeRoad.reserve(routeNode.size()-1);
	int node01, node02, roadID; 
	int routeID = 0;
	// Convert
	for (int i=0;i<routeNode.size()-1;i++){
		node01 = routeNode[i];
        node02 = routeNode[i+1];
		roadID = nodeID2RoadID[make_pair(node01,node02)];
		routeRoad.push_back(roadID);
	}
	/*
	// Print
	cout << "route with roadSegment ID is: ";
	for (int i=0;i<routeRoad.size();i++){
	cout << routeRoad[i] << " ";
	}
	cout << endl;
	*/
	return routeRoad;
}

// Generate New Data for Insertion
// Main Idea: Apply Existing Route with New Departure Time as New Insertion Data
void Graph::data_generation(string route_file, string depar_file, string Pi_file, int newNum, int avg_length, bool cut)
{
	// Read Data
	vector<vector<int>> query_dataRaw = read_query(queryPath, newNum);
	// Read Route Data
	vector<vector<int>> route_dataRaw = read_route(route_path, newNum);
	vector<vector<int>> query_data;
	vector<vector<int>> route_data;
	if (cut == true){
		pair<vector<vector<int>>, vector<vector<int>>> dataCombine = data_length_modify(query_dataRaw, route_dataRaw, avg_length);
		query_data = dataCombine.first;
		route_data = dataCombine.second;
	}
	else{
		query_data = query_dataRaw;
		route_data = route_dataRaw;
	}

	// Randomly Generate Unordered Integer
	vector<int> randomInt = randperm(route_data.size());
	// Correctness Check
	// Make Sure New Route Data Size Is Smaller Than Size of Existing Routes
	if (newNum > route_data.size()){
		cout << "Error! Required new route data size " << newNum;
		cout << " is bigger than existing " << route_data.size() << endl;
	}
	// Capture Part of Existing Routes
	vector<vector<int>> routeTemp; routeTemp.clear();
	for (int i = 0; i < newNum; i++){
		int routeIndex = randomInt[i];
		routeTemp.push_back(route_data[routeIndex]);
	}
	// Write Captured Route Data Out
	ofstream outRoute;
	outRoute.open(Pi_file);
	for (int i = 0; i < routeTemp.size(); i++){
		outRoute << routeTemp[i].size() << " ";
		for (int j = 0; j < routeTemp[i].size(); j++){
			outRoute << routeTemp[i][j] << " ";
		}
		outRoute << endl;
	}
	outRoute.close();

	// Convert Node Constructed Route to Road Constructed Route
	vector<vector<int>> routeRoad; routeRoad.clear();
	for (int i = 0; i < routeTemp.size(); i++){
		routeRoad.push_back(route_node_2_route_road(routeTemp[i]));
	}
	// Write Captured Route Data Out
	ofstream outRouteRoad;
	outRouteRoad.open(route_file);
	for (int i = 0; i < routeRoad.size(); i++){
		outRouteRoad << routeRoad[i].size() << " ";
		for (int j = 0; j < routeRoad[i].size(); j++){
			outRouteRoad << routeRoad[i][j] << " ";
		}
		outRouteRoad << endl;
	}
	outRouteRoad.close();

	// Write Randomly Generated Departure Time Out
	ofstream outDepar;
	outDepar.open(depar_file);
	vector<int> deparTime;
	deparTime.resize(newNum);
	int time, timeGenerated, timeRangeRight;
	for (int i = 0; i < deparTime.size(); i++){
		time = query_data[i][2];
		// New Data's Departure Time Ranged in Two Minutes
		timeRangeRight = time + 2*60;
		timeGenerated = (rand() % (timeRangeRight - time + 1)) + time;
		outDepar << timeGenerated << " " << endl;
	}
	outDepar.close();
}

// Generate Data Same as Simulation Operation for Comparison
void Graph::data_generation_same_simulation(
		string route_file, string depar_file, string Pi_file, vector<vector<int>> &query_data, vector<vector<int>> &route_data)
{
	int newNum = query_data.size();
	// Capture Part of Existing Routes
	vector<vector<int>> routeTemp; routeTemp.clear();
	routeTemp = route_data;
	// Write Captured Route Data Out
	ofstream outRoute;
	outRoute.open(Pi_file);
	for (int i = 0; i < routeTemp.size(); i++){
		outRoute << routeTemp[i].size() << " ";
		for (int j = 0; j < routeTemp[i].size(); j++){
			outRoute << routeTemp[i][j] << " ";
		}
		outRoute << endl;
	}
	outRoute.close();

	// Convert Node Constructed Route to Road Constructed Route
	vector<vector<int>> routeRoad; routeRoad.clear();
	for (int i = 0; i < routeTemp.size(); i++){
		routeRoad.push_back(route_node_2_route_road(routeTemp[i]));
	}
	// Write Captured Route Data Out
	ofstream outRouteRoad;
	outRouteRoad.open(route_file);
	for (int i = 0; i < routeRoad.size(); i++){
		outRouteRoad << routeRoad[i].size() << " ";
		for (int j = 0; j < routeRoad[i].size(); j++){
			outRouteRoad << routeRoad[i][j] << " ";
		}
		outRouteRoad << endl;
	}
	outRouteRoad.close();

	// Write Randomly Generated Departure Time Out
	ofstream outDepar;
	outDepar.open(depar_file);
	vector<int> deparTime;
	deparTime.resize(newNum);
	int time;
	for (int i = 0; i < deparTime.size(); i++){
		time = query_data[i][2];
		outDepar << time << " " << endl;
	}
	outDepar.close();
	cout << "Test route and departure time generation & write done." << endl;
}


// Read Generated Route Data
void Graph::read_new_data(string routeRoadPath, string deparTimePath, string routeNodePath)
{
	// Open Files
	ifstream IFRoute(routeRoadPath);
	if(!IFRoute)
		cout << "Cannot open route (road) file: "<< routeRoadPath << endl;
	// Variable Initialization
	vector<vector<int>> routeRoad; int nodeNum;
	// Read Route Constructed by Nodes
	while(IFRoute >> nodeNum){
		vector<int> routeRoadTemp; routeRoadTemp.clear();
		routeRoadTemp.resize(nodeNum);
		for (int j = 0; j < nodeNum; j++){
			IFRoute >> routeRoadTemp[j];
		}
		routeRoad.push_back(routeRoadTemp);
	}
	// Add Index (ID) for Generated New Routes as Input for Update Operation
	for (int i = 0; i < routeRoad.size(); i++){
		routeRoadInput.push_back(make_pair(i+routeDataSize, routeRoad[i]));
	}

	// Open Files
	ifstream IFDepar(deparTimePath);
	if(!IFDepar)
		cout << "Cannot open departure time file: "<< deparTimePath << endl;
	// Variable Initialization
	int deparNum = CountLines(deparTimePath);
	int departTime;
	for (int i = 0; i < deparNum; i++){
		IFDepar >> departTime;
		departTimeList.push_back(departTime);
	}

	// Open Files
	ifstream IFRouteNode(routeNodePath);
	if(!IFRouteNode)
		cout << "Cannot open route constructed by nodes file: "<< routeNodePath << endl;
	// Variable Initialization
	int routeNodeNum;
	while(IFRouteNode >> routeNodeNum){
		vector<int> routeNode; routeNode.clear();
		routeNode.resize(routeNodeNum);
		for (int j = 0; j < routeNodeNum; j++){
			IFRouteNode >> routeNode[j];
		}
		Pi.push_back(routeNode);
	}
}

// "ETA_result" Initialization
void Graph::ETA_initialization(bool simulation, bool print)
{
	if (simulation == 0){
		ETA_result.clear(); // RouteID -> NodeID + Arrived Time
		// estimated travel time initialization
		ETA_result.resize(Pi.size());
		for (int i=0;i<Pi.size();i++){
			ETA_result[i].resize(Pi[i].size());
		}
		for (int i=0;i<Pi.size();i++){
			for (int j=0;j<1;j++){
				ETA_result[i][j].first = Pi[i][j];
				ETA_result[i][j].second = departTimeList[i];
			}
			for (int k=1;k<Pi[i].size();k++){
				ETA_result[i][k].first = Pi[i][k];
				ETA_result[i][k].second = INF;
			}
		}
	}
	else{
		// Put "ETA_results" to A Temp One
		vector<vector<pair<int,int>>> ETA_resultTemp;
		ETA_resultTemp = ETA_result;
		//
		ETA_result.clear();
		// Re-Initialize "ETA_results"
		// Resize Un-Simulated Results
		ETA_result.resize(Pi.size());

		for (int i=0;i<Pi.size();i++){
			ETA_result[i].resize(Pi[i].size());
		}
		// Assign Original Simulated Results
		for (int i=0;i<ETA_resultTemp.size();i++){
			for (int j=0;j<ETA_resultTemp[i].size();j++){
				ETA_result[i][j].first = ETA_resultTemp[i][j].first;
				ETA_result[i][j].second = ETA_resultTemp[i][j].second;
			}
		}
		for (int i=ETA_resultTemp.size();i<ETA_result.size();i++){
			for (int j=0;j<1;j++){
				ETA_result[i][j].first = Pi[i][j];
				ETA_result[i][j].second = departTimeList[i - ETA_resultTemp.size()];
			}
			for (int k=1;k<ETA_result[i].size();k++){
				ETA_result[i][k].first = Pi[i][k];
				ETA_result[i][k].second = INF;
			}
		}
	}

	// Print
	// Step 8: Print ETA_result
	// -------------------------------------------------------
	if (print ==true){
		for (int i=0;i<ETA_result.size();i++){
			cout << "path " << i << ": ";
			for (int j=0;j<ETA_result[i].size();j++){
				cout << "node " << ETA_result[i][j].first << " with ";
				cout << "time " << ETA_result[i][j].second << " ";
			}
			cout << endl;
		}
	}
}

// Insertion Operation (Main)
void Graph::update_operation_insertion(bool parallel, bool terminal, bool range, bool print)
{
	// Update Operation by Inserting New Route Data One by One
	vector<int> affectedRoadID; affectedRoadID.clear();
	// Initialize Time Slices
	timeSliceInsert.clear();
	timeSliceInsert = timeSlice;

	std::chrono::high_resolution_clock::time_point t0, t1;
	std::chrono::duration<double> time_span1;
	t0=std::chrono::high_resolution_clock::now();

	for (int i = 0; i < departTimeList.size(); i++){

		// Update Operation
		affectedRoadID = one_route_update_insertion(routeRoadInput[i], departTimeList[i], parallel, terminal, range, print);
		/*
		// Print
		for (int j=0;j<affectedRoadID.size();j++)
		{
		map<int, vector<vector<int>>>* timeRecords; int inTime = 5296701;
		int RoadSegmentID = affectedRoadID[j];

		int hour = time_2_hour(inTime); int index = hour_2_index(hour);
		timeRecords = &timeSliceInsert[RoadSegmentID][index];

		cout << endl;
		cout << "count " << j << endl;
		cout << "-----------------------------------------" << endl;
		map<int, vector<vector<int>>>::iterator itrPrint;

		cout << "TimeRecord Size is: " << timeRecords->size() << endl;
		cout << "RoadSegmentID: " << RoadSegmentID << " with timeSliceInsert: " << index << endl;
		for (itrPrint = timeRecords->begin(); itrPrint != timeRecords->end(); ++itrPrint) {
		for (int k=0;k<itrPrint->second.size();k++)
		{
		cout << " time " << itrPrint->first << " routeID " << itrPrint->second[k][0];
		cout << " status " << itrPrint->second[k][1] << " flow " << itrPrint->second[k][2] << "||";
		}
		}
		cout << endl;
		}
		*/
	}
	t1=std::chrono::high_resolution_clock::now();
	time_span1 = std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0);
	// Print
	if (terminal == true)
		cout << "Insertion operation's time consumption w termination is: " << time_span1.count() << "s." << endl;
	else
		cout << "Insertion operation's time consumption w/o termination is: " << time_span1.count() << "s." << endl;
}

// For a Road ID Constructed Route, Find Next Road ID
int Graph::find_next_roadID(vector<int> route, int roadID)
{
	int nextRoadID;
	std::vector<int>::iterator itFind;
	itFind = std::find (route.begin(), route.end(), roadID);
	if (itFind != route.end() and itFind != --route.end()){
		++itFind;
        nextRoadID = *itFind;
	}
	else{
        if (itFind == --route.end()){
            nextRoadID = -1;
        }
        else{
            nextRoadID = -1;
            // cout << "Cannot Find The Next RoadSegmentID or It's The Last One." << endl;
            // cout << "roadID: " << roadID << endl;
        }
	}
	return nextRoadID;
}

// Check If Two Traffic Flows Are In Same Range
bool Graph::flow_same_range_check(int &ID1index, int &ID2index, int &flow1, int &flow2)
{
	// Variable Initialization
	bool checkSame;
    int bound;
	int index1 = -1;
	int index2 = -1;
	// Find Range for Specific Time Range
	vector<pair<int,int>> range = timeRange[ID1index][ID2index].second;
	// Correctness Check
	if (range.size() == 0)
		cout << "Do not find travel time range or its size is zero." << endl;
    if (range[0].first <= 5){
        return true;
    }
	// Estimate Range Index for The First Traffic Flow
	for (int j=0;j<range.size();j++){
		bound = range[j].first;
		if (flow1 < bound)
			index1 = bound;
	}
	// Estimate Range Index for The Second Traffic Flow
	for (int j=0;j<range.size();j++){
		bound = range[j].first;
		if (flow2 < bound)
			index2 = bound;
	}
	// Check Equation	
	if (index1 == index2)
		return true;
	else
		return false;
	// return true;
}

// Insertion Operation for Different Road Has Different Operation
// This Is Insertion Operation for The First Road of New Route
vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> Graph::update_operation_1st(
		int &RoadSegmentID, int &inTime, pair<int, vector<int>> &newRoutePair, bool &parallel, bool &terminal, bool &range, bool print)
{

	// Step 1: Variable Initialization
	// -------------------------------------------
	// Traffic Flow without Insertion
	int trafficFlow;
	// Traffic Flow with Insertion
	int trafficFlowInsert;
	// Check If Fit Terminal Condition: 0 -> Not, 1 -> Yes
	int terminalCondition = 0;
	// If (flow == flow'), Record Time
	// If Flow with And without Insertion of Time Records Before Record Time Are All Same
	// Achieve Terminal Condition
	int recordTime = 0;
	// Initialize Insert List and Deletion List to Store Time Records Which Need Insertion & Deletion
	map<int, vector<vector<int>>> insertList; vector<int> deletionList;
	// insertList.insert(pair<int, vector<vector<int>>>(0, {{0, 0, 0}}));
	// Capture New Route ID and New Route from Argument
	int newRouteID = newRoutePair.first; 
	vector<int> newRoute = newRoutePair.second;
	// Initialize Returned Values
	// "Null" If Current Road Is The Last One of This Route
	// Otherwise, Return Generated Information on The Current Road to The Next One
	vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> Null;
	vector<int> roadAffectedID; int NewTimeLeave;
	vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> roadSegmentAffected;

	// Step 2: Insertion Operation
	// -------------------------------------------

	// Step 2.1: Target Time Records
	// -------------------------------------------
	int hour = time_2_hour(inTime); 
	int index = hour_2_index(hour);

    // Generate Flow Base for Travel Time Estimation
	int current_node = roadID2NodeID[RoadSegmentID].first;
	int next_node = roadID2NodeID[RoadSegmentID].second;
	int tm = nodeID2minTime[make_pair(current_node, next_node)];
	int nextNodeIndex;
	for (int i=0;i<timeRange[current_node].size();i++){
		if (timeRange[current_node][i].first == next_node){
			nextNodeIndex = i;
		}
	}
	pair<int, int> flowBaseIndex = time_to_base_index(inTime, minRange);   // Hour Index, Minutes Index
	int hourIndex = flowBaseIndex.first;
	int minuIndex = flowBaseIndex.second;

	int flowBase = flowBaseList[current_node][nextNodeIndex].second[hourIndex][minuIndex];
	// Update Traffic Flow Number
	trafficFlow = flowBase;
	trafficFlowInsert = flowBase;

	// Step 2.2: Find Time Value or Insert
	// -------------------------------------------
	// Variable Initialization
	int preFlow; 
	vector<int> features = {newRouteID, 1, 0};
	map<int, vector<vector<int>>>::iterator itFind; itFind = timeSliceInsert[RoadSegmentID][index].find(inTime); 
	// If Time Not Find, Insert Time
	if (itFind == timeSliceInsert[RoadSegmentID][index].end()){
		timeSliceInsert[RoadSegmentID][index].insert(pair<int, vector<vector<int>>>(inTime, {features}));
		// Step 2.3: Find Previous Value for Traffic Flow
		// -------------------------------------------
		map<int, vector<vector<int>>>::iterator itPre;
		itPre = timeSliceInsert[RoadSegmentID][index].find(inTime);
		if (itPre != timeSliceInsert[RoadSegmentID][index].end()){
			// If Time Record is The First One
			if (itPre == timeSliceInsert[RoadSegmentID][index].begin()){
				preFlow = 0;
				/* cout << "time is the first one with flow : " << preFlow << endl; */
			}
			else{
				itPre = --itPre;
				preFlow = itPre->second.back()[2];
				/* cout << "previous time: " << itPre->first << " flow: " << preFlow << endl; */
			}
		}
		// If Time Record Not Find, It Must Be Error Since We Just Insert It
		else{
			cout << "No value found. Please check Insertion." << endl;
			return Null;
		}
	}
	// If Time Record Find, Push Back Feature in It
	else{
		preFlow = itFind->second.back()[2];
		features = {newRouteID, 1, preFlow};
		timeSliceInsert[RoadSegmentID][index][inTime].push_back(features);
	}

	// Step 2.4: Estimate New Time Record's Travel Time
	// -------------------------------------------

	tuple<int, map<int, vector<vector<int>>>, vector<int>> tempReturn;
	int nextRoadSegmentID = 0;
	int routeID = 0;
	nextRoadSegmentID = find_next_roadID(newRoutePair.second, RoadSegmentID);
	
	// Update Traffic Flow of Driving In Time Record
	if (preFlow == 0){
		trafficFlow += preFlow; 
		trafficFlowInsert += preFlow + 1;
	}
	else{
		trafficFlow = preFlow;
		trafficFlowInsert = preFlow + 1;
	}

	map<int, vector<vector<int>>>::iterator itFind1;
	itFind1 = timeSliceInsert[RoadSegmentID][index].find(inTime);
	itFind1->second.back()[2] = trafficFlowInsert;
	// Estimate New Time Record's Leaving Time
	// int tm = nodeID2minTime[make_pair(current_node, next_node)];
	int te = 0;
	for (int i=0;i<timeRange[current_node].size();i++){
		if(timeRange[current_node][i].first == next_node){
			if (range == false)
				te = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
			else
				te = flow2time_by_range(current_node, nextNodeIndex, trafficFlowInsert);
		}
	}
	// Estimate Leaving Time
	NewTimeLeave = inTime + te;
	if (insertList.find(NewTimeLeave) == insertList.end())
		insertList.insert(pair<int, vector<vector<int>>>((NewTimeLeave), {{newRouteID, 0, 0}}));
	else
		insertList[NewTimeLeave].push_back({newRouteID, 0, 0});

	// If Min Travel Time Is Smaller Than 1
    // No Records Are Affected -> Directly Process The Next One
    // However, It Needs to Add The Not Inserted Time Record,
    // Which May Not Decrease The Time Consumption A Lot
    if (tm < 1){
        int iCurrentRoad = RoadSegmentID;
        int routeID = newRoutePair.first;
        vector<int> affected_route = Pi[routeID];
        vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
        int iNextRoadID = find_next_roadID(affected_route_w_roadSegmentID, RoadSegmentID);

        if (iNextRoadID == -1){
            if (timeSliceInsert[RoadSegmentID][index].find(NewTimeLeave) == timeSliceInsert[RoadSegmentID][index].end()){
                timeSliceInsert[RoadSegmentID][index].insert(pair<int, vector<vector<int>>>(NewTimeLeave, {{newRouteID, 0, trafficFlowInsert-1}}));
            }
            else{
                timeSliceInsert[RoadSegmentID][index][NewTimeLeave].push_back({newRouteID, 0, trafficFlowInsert-1});
            }
            return Null;
        }
        else{
			if (timeSliceInsert[RoadSegmentID][index].find(NewTimeLeave) == timeSliceInsert[RoadSegmentID][index].end()){
                timeSliceInsert[RoadSegmentID][index].insert(pair<int, vector<vector<int>>>(NewTimeLeave, {{newRouteID, 0, trafficFlowInsert-1}}));
            }
            else{
                timeSliceInsert[RoadSegmentID][index][NewTimeLeave].push_back({newRouteID, 0, trafficFlowInsert-1});
            }
            tuple<int, map<int, vector<vector<int>>>, vector<int>> tempReturn;
            get<0>(tempReturn) = inTime;
            roadSegmentAffected.push_back(make_pair(make_pair(iNextRoadID,routeID),tempReturn));
            return roadSegmentAffected;
        }
    }

	// Step 3: Update Operation
	// -------------------------------------------

	// Step 3.1: Check New Time Record is Last One
	// -------------------------------------------
	map<int, vector<vector<int>>>::iterator itrInser;
	itrInser = timeSliceInsert[RoadSegmentID][index].find(inTime);
	// Correctness Check
	if (itrInser == timeSliceInsert[RoadSegmentID][index].end()){
		cout << "Error. Time Records Do Not Contain Others Except the New Driving In One" << endl;
	}

	// Step 3.2: For Each Time Record Start From New's Next (Core Design).
	// -------------------------------------------
	int timeCurrent = 0;
	int RoadIDCurrent = 0;
	map<int, vector<vector<int>>>::iterator itr;
	// Check If Traffic Flow Change from New's Next to The Last One
	for (itr = ++itrInser; itr != timeSliceInsert[RoadSegmentID][index].end();){
		bool bBreak = false;
		bool bBreak2 = false;
		int dynamicSize = itr->second.size();
		for (int i = 0; i < dynamicSize; i++){

			timeCurrent = itr->first;
			RoadIDCurrent = itr->second[i][0];

			// Step 3.3: Compare Time Records in Insert Set
			// -------------------------------------------
			int RouteIDI;
			map<int, vector<vector<int>>>::iterator itrI;
			for (itrI = ++insertList.begin(); itrI != insertList.end();){
				int timeI = itrI->first;
				if (timeI < timeCurrent){
					// trafficFlowInsert -= 1;
					RouteIDI = itrI->second[i][0];
					// If Inserted Time Record Had Driving In Status
					// Inserted Time Records Should Also Be Driving In Status
					if (itrI->second[i][1] == 1){   // Status: In
						if (timeSliceInsert[RoadSegmentID][index].find(timeI) == timeSliceInsert[RoadSegmentID][index].end()){
							timeSliceInsert[RoadSegmentID][index].insert(pair<int, vector<vector<int>>>(timeI, {{RouteIDI, 1, trafficFlowInsert}}));
						}
						else{
							timeSliceInsert[RoadSegmentID][index][timeI].push_back({RouteIDI, 1, trafficFlowInsert});
						}
					}
					// Otherwise, Inserted Time Records Should Also Be Leaving Status
					else{                           // Status Out
						if (timeSliceInsert[RoadSegmentID][index].find(timeI) == timeSliceInsert[RoadSegmentID][index].end()){
							timeSliceInsert[RoadSegmentID][index].insert(pair<int, vector<vector<int>>>(timeI, {{RouteIDI, 0, trafficFlowInsert}}));
						}
						else{
							timeSliceInsert[RoadSegmentID][index][timeI].push_back({RouteIDI, 0, trafficFlowInsert});
						}
					}
					// Erase Inserted One from Insert List
					itrI = insertList.erase(itrI);
					continue;
				}
				++itrI;
			}

			// Step 3.4: If Time Record is Driving In Status.
			// -------------------------------------------
			map<int, vector<vector<int>>>::iterator itrIPre;
			int RouteIDIPre;
			int teCurrent = 0;
			if (itr->second[i][1] == 1){
				// If Terminal Condition Is Active
				if (terminalCondition == 1){
					// If Current Time Record Is The Last One of Current Route
					if (itr == timeSliceInsert[RoadSegmentID][index].end()){
						break;
					}
					// If Terminal Condition Is Active but Still Not Reach The Record Time
					if (timeCurrent <= recordTime){
						if (trafficFlow != trafficFlowInsert){
							// Check If Traffic Flows Are in Same Range
							bool sameRange = flow_same_range_check(current_node, nextNodeIndex, trafficFlow, trafficFlowInsert);
							if (sameRange == false or range == false){
								// If Traffic Flows Are Different, Inactive Terminal Condition
								terminalCondition = 0;
								// Update Changed Information
								trafficFlow += 1;
								trafficFlowInsert += 1;
								// Update Time Record's Traffic Flow
								itr->second[i][2] = trafficFlowInsert;

								routeID = itr->second[i][0];
								// Estimate New Travel Time
								if (range == false){
									teCurrent = tm * (1 + sigma * pow(trafficFlowInsert / varphi, beta));
								}
								else{
									teCurrent = flow2time_by_range(current_node, nextNodeIndex, trafficFlowInsert);
								}

								// Add Estimated Travel Time to Insert List for Further Insertion Operation
								if (insertList.find(timeCurrent + teCurrent) == insertList.end()){
									insertList.insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent), {{RoadIDCurrent, 0, 0}}));
								} 
								else{
									insertList[timeCurrent + teCurrent].push_back({RoadIDCurrent, 0, 0});
								}

								// Add Odd Vehicle's (route) ID to Deletion List for Further Deletion Operation
								deletionList.push_back(routeID);

								// Find Next Road ID
								vector<int> affectedRoute = Pi[routeID];
								vector<int> affectedRouteRoadID = route_node_2_route_road(affectedRoute);
								int unaffectedNextRoadID = find_next_roadID(affectedRouteRoadID,RoadSegmentID);
								
								// Decide If Next Road Belongs to New Route for Different Operation
								std::vector<int>::iterator itrFindNew;
								itrFindNew = std::find(newRoute.begin(), newRoute.end(), unaffectedNextRoadID);

								// Find If Next Road Belongs to Affected Road
								roadAffectedID.clear();
								for (int i = 0; i < roadSegmentAffected.size(); i++){
									int tempRoadSegmentID = roadSegmentAffected[i].first.first;
									roadAffectedID.push_back(tempRoadSegmentID);
								}
								std::vector<int>::iterator itFindExit;
								itFindExit = std::find(roadAffectedID.begin(), roadAffectedID.end(), unaffectedNextRoadID);

								// If Already Added
								// Because Time Records Are Delayed, Driving in Time in The Next Road Should Change
								// Code Related to "get" Mainly Return Information Related to The Next Road
								if (itFindExit != roadAffectedID.end()){
									int position = itFindExit - roadAffectedID.begin();
									tempReturn = roadSegmentAffected[position].second;

									if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
										get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent), {{RoadIDCurrent, 1, 0}}));
									} 
									else{
										get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
									}
									get<2>(tempReturn).push_back(RoadIDCurrent);
									// If Road Belongs to New Route
									if (itrFindNew != newRoute.end()){
										get<0>(tempReturn) = NewTimeLeave;
										roadSegmentAffected[position].second = tempReturn;
									} 
									else{
										get<0>(tempReturn) = 0;
										roadSegmentAffected[position].second = tempReturn;
									}
								}
								else{
									if (itrFindNew != newRoute.end()){
										get<0>(tempReturn) = NewTimeLeave;
										get<1>(tempReturn).clear();
										if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
											get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent), {{RoadIDCurrent, 1, 0}}));
										}
										else{
											get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
										}
										get<2>(tempReturn).clear();
										// get<2>(tempReturn).push_back(routeID);
										get<2>(tempReturn).push_back(RoadIDCurrent);
										roadSegmentAffected.push_back(make_pair(make_pair(nextRoadSegmentID,routeID), tempReturn));
									}
									else{
										get<0>(tempReturn) = 0;
										get<1>(tempReturn).clear();
										if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
											get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent), {{RoadIDCurrent, 1, 0}}));
										}
										else{
											get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
										}
										get<2>(tempReturn).clear();
										get<2>(tempReturn).push_back(RoadIDCurrent);
										roadSegmentAffected.push_back(make_pair(make_pair(unaffectedNextRoadID,routeID), tempReturn));
									}
								}
							}
							else{
								// Update Changed Information
								trafficFlow += 1;
								trafficFlowInsert += 1;
								// Although Range Are Same, Since Flows Do Change, Flow Number Still Needs to Update
								itr->second[i][2] = trafficFlowInsert;
							}
						}
						else{
							trafficFlow += 1;
							trafficFlowInsert += 1;
						}
					}
					else{
						// Active Terminal Condition, No Further Time Records Are Affected
						if (terminal == true and insertList.size() == 0 and trafficFlowInsert == 0){
							bBreak2 = true;
							break;
						}
					}
				}
				// If Terminal Condition Is Not Active
				else{
					trafficFlow += 1;
					trafficFlowInsert += 1;
					// Check If Flows Belongs to The Same Range
					// Check IF Traffic Flows Are in Same Range
					bool sameRange = flow_same_range_check(current_node, nextNodeIndex, trafficFlow, trafficFlowInsert);
					if (sameRange == true){
						// Active Terminal Condition
						itr->second[i][2] = trafficFlowInsert;
						terminalCondition = 1;
						// Estimate Record Time (Decide If No Further Affect)
						if (range == false){
							teCurrent = tm * (1 + sigma * pow(trafficFlowInsert / varphi, beta));
						}
						else{
							teCurrent = flow2time_by_range(current_node, nextNodeIndex, trafficFlowInsert);
						}
						recordTime = timeCurrent + teCurrent;
					}
					else{
						itr->second[i][2] = trafficFlowInsert;
						routeID = itr->second[i][0];

						if (range == false){
							teCurrent = tm * (1 + sigma * pow(trafficFlowInsert / varphi, beta));
						}
						else{
							teCurrent = flow2time_by_range(current_node, nextNodeIndex, trafficFlowInsert);	
						}
						if (insertList.find(timeCurrent + teCurrent) == insertList.end()){
							insertList.insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent), {{RoadIDCurrent, 0, 0}}));
						}
						else{
							insertList[timeCurrent + teCurrent].push_back({RoadIDCurrent, 0, 0});
						}
						deletionList.push_back(routeID);

						vector<int> affected_route = Pi[routeID];
						vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
						int unaffected_next_roadSegmentID = find_next_roadID(affected_route_w_roadSegmentID, RoadSegmentID);
						
						// Find IF Next Road Belongs to New Route
						std::vector<int>::iterator itrFindNew;
						itrFindNew = std::find(newRoute.begin(), newRoute.end(), unaffected_next_roadSegmentID);
						// Find IF Next Road Belongs to Affected Road
						roadAffectedID.clear();
						for (int i = 0; i < roadSegmentAffected.size(); i++){
							int tempRoadSegmentID = roadSegmentAffected[i].first.first;
							roadAffectedID.push_back(tempRoadSegmentID);
						}

						std::vector<int>::iterator itFindExit;
						itFindExit = std::find(roadAffectedID.begin(), roadAffectedID.end(), unaffected_next_roadSegmentID);

						if (itFindExit != roadAffectedID.end()){
							int position = itFindExit - roadAffectedID.begin();
							tempReturn = roadSegmentAffected[position].second;

							if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()) {
								get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent), {{RoadIDCurrent, 1, 0}}));
							}
							else{
								get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
							}

							get<2>(tempReturn).push_back(RoadIDCurrent);

							if (itrFindNew != newRoute.end()){
								get<0>(tempReturn) = NewTimeLeave;
								roadSegmentAffected[position].second = tempReturn;
							}
							else{
								get<0>(tempReturn) = 0;
								roadSegmentAffected[position].second = tempReturn;
							}
						}
						else{
							if (itrFindNew != newRoute.end()){
								get<0>(tempReturn) = NewTimeLeave;

								get<1>(tempReturn).clear();

								if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
									get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent), {{RoadIDCurrent, 1, 0}}));
								}
								else{
									get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
								}

								get<2>(tempReturn).clear();
								//get<2>(tempReturn).push_back(routeID);
								get<2>(tempReturn).push_back(RoadIDCurrent);

								roadSegmentAffected.push_back(make_pair(make_pair(nextRoadSegmentID,routeID), tempReturn));
							}
							else{
								get<0>(tempReturn) = 0;
								get<1>(tempReturn).clear();
								if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
									get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),{{RoadIDCurrent, 1, 0}}));
								}
								else{
									get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
								}

								get<2>(tempReturn).clear();
								get<2>(tempReturn).push_back(RoadIDCurrent);
								
								roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,routeID), tempReturn));
							}
						}
					}
				}
			}
			// Step 3.5: If Time Record is Driving Out Status.
			// -------------------------------------------
			else{
				// Step 3.6: Compare Time Records with Element in the Deletion Set
				// -------------------------------------------
				if (deletionList.size() != 0){
					int deleID;
					for (int l=0;l<deletionList.size();l++){
						deleID = deletionList[l];
						if (RoadIDCurrent == deleID){
							trafficFlow -= 1;
							itr->second.erase(itr->second.begin() + i);
							dynamicSize = itr->second.size();
							i--;
							vector<int>::iterator k = deletionList.begin() + l;
							deletionList.erase(k);

							if (itr->second.size() == 0){
								itr = timeSliceInsert[RoadSegmentID][index].erase(itr);
								if (timeSliceInsert[RoadSegmentID][index].size() == 0){
									bBreak2 = true;
									break;
								}
								bBreak = true;
								break;
							}
							break;
						}
					}
				}
				else{
					trafficFlowInsert -= 1;
					trafficFlow -= 1;
					itr->second[i][2] = trafficFlowInsert;
				}
			}
		}
		if(bBreak)
			continue;
		if(bBreak2)
			break;
		dynamicSize = itr->second.size();
		++itr;
	}

	// Step 3.7 Insert Rest Insert Elements
	// -------------------------------------------
	map<int, vector<vector<int>>>::iterator itrEnd; itrEnd = --timeSliceInsert[RoadSegmentID][index].end();
	int FlowEnd = itrEnd->second.back()[2];
	int timeRest, RouteIDRest, FlowRest;
	map<int, vector<vector<int>>>::iterator itrRestI;

	for (itrRestI = insertList.begin(); itrRestI != insertList.end(); ++itrRestI){
		for (int i=0;i<itrRestI->second.size();i++){
			timeRest = itrRestI->first; 
			RouteIDRest = itrRestI->second[i][0];
			FlowRest = FlowEnd - 1;
			if (timeSliceInsert[RoadSegmentID][index].find(timeRest) == timeSliceInsert[RoadSegmentID][index].end()){
				timeSliceInsert[RoadSegmentID][index].insert(pair<int, vector<vector<int>>>(timeRest, {{RouteIDRest, 0, FlowRest}}));
			}
			else{
				timeSliceInsert[RoadSegmentID][index][timeRest].push_back({RouteIDRest, 0, FlowRest});
			}
		}
	}
	// Step 4: Print Time Records
	// -------------------------------------------
	/*
	   map<int, vector<vector<int>>>::iterator itrPrint;
	   cout << "TimeRecord Size is: " << timeRecords->size() << endl;
	   cout << "RoadSegmentID: " << RoadSegmentID << " with TimeSlice: " << index << endl;
	   for (itrPrint = timeRecords->begin(); itrPrint != timeRecords->end(); ++itrPrint) {
	   for (int i=0;i<itrPrint->second.size();i++)
	   {
	   cout << " time " << itrPrint->first << " routeID " << itrPrint->second[i][0];
	   cout << " status " << itrPrint->second[i][1] << " flow " << itrPrint->second[i][2] << "||";
	   }
	   }
	   cout << endl;
	   */

	// Step 5: Update Result
	// -------------------------------------------
	/* timeSliceInsert[RoadSegmentID][index] = timeRecords;*/

	// Step 6: Define Returned Value Even No Affected Propagated Route
	// -------------------------------------------
	/*
	   cout << "NewTimeLeave is: " << NewTimeLeave << " nextRoadSegmentID is: " << nextRoadSegmentID << endl;
	   */

	std::vector<int>::iterator itr_findNextExist;
	roadAffectedID.clear();
	for (int i=0;i<roadSegmentAffected.size();i++){
		roadAffectedID.push_back(roadSegmentAffected[i].first.first);
	}
	itr_findNextExist = std::find(roadAffectedID.begin(), roadAffectedID.end(), nextRoadSegmentID);
	tuple<int, map<int, vector<vector<int>>>, vector<int>> fillReturn;
	if (nextRoadSegmentID != -1 and itr_findNextExist == roadAffectedID.end()){
		get<0>(fillReturn) = NewTimeLeave; get<1>(fillReturn).clear(); get<2>(fillReturn).clear();
		roadSegmentAffected.push_back(make_pair(make_pair(nextRoadSegmentID,newRoutePair.first),fillReturn));
	}

	// Step 7: Print Further Affected Road Segment
	// -------------------------------------------
	/*
	   cout << endl;
	   cout << "Step 4: Returned Results" << endl;
	   cout << "-----------------------------------------" << endl;
	   for (int i=0;i<roadSegmentAffected.size();i++){
	   cout << " route ID is: " << roadSegmentAffected[i].first.second << " with entrance time: " << get<0>(roadSegmentAffected[i].second) << endl;
	   map<int, vector<vector<int>>>::iterator itrInsertPrint;
	   for (itrInsertPrint = get<1>(roadSegmentAffected[i].second).begin(); itrInsertPrint != get<1>(roadSegmentAffected[i].second).end(); ++itrInsertPrint) {
	   for (int i=0;i<itrInsertPrint->second.size();i++)
	   {
	   cout << " time " << itr->first << " routeID " << itr->second[i][0];
	   }
	   }
	   cout << endl;
	   }
	   cout << endl;
	   */

	return roadSegmentAffected;
}

//  This Is Insertion Operation for The Further Road of New Route or Propagated Road
vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> Graph::updateOperationFurther(
		pair<int,int> &RoadSegmentID, int &inTime, pair<int, vector<int>> &newRoutePair, pair<int, vector<int>> &routeProp,
		map<int, vector<vector<int>>> &InsertPre, vector<int> &DeletionPre, bool &parallel, bool &terminal, bool &range, bool print){

	map<int, vector<vector<int>>> InsertPreCopy = InsertPre;
	vector<int> DeletionPreCopy = DeletionPre;

    bool STAll;
    STAll = true;
	// Step 1: Variable Initialization
	// -------------------------------------------
	// Traffic Flow without Insertion
	int trafficFlow;
	// Traffic Flow with Insertion
	int trafficFlowInsert;
	// Check If Fit Terminal Condition: 0 -> Not, 1 -> Yes
	int terminalCondition = 0;
	// If (flow == flow'), Record Time
	// If Flow with And without Insertion of Time Records Before Record Time Are All Same
	// Achieve Terminal Condition
	int recordTime = 0;
	// Initialize Insert List and Deletion List to Store Time Records Which Need Insertion & Deletion
	map<int, vector<vector<int>>> insertList; 
	vector<int> deletionList;
	deletionList = DeletionPre;
	// Capture New Route ID and New Route from Argument
	int newRouteID = newRoutePair.first; 
	vector<int> newRoute = newRoutePair.second;
	// Initialize Returned Values
	// "Null" If Current Road Is The Last One of This Route
	// Otherwise, Return Generated Information on The Current Road to The Next One
	vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> Null;
	vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> roadSegmentAffected;
	vector<int> roadSegmentAffectedID;
	// If Current Road Is The Last One without Any Information Input
	if (inTime == 0 and InsertPre.size() == 0 and DeletionPre.size() == 0){
		return Null;
	}

	// Step 2: Insertion Operation
	// -------------------------------------------
	// Variable Initialization
	int NewTimeLeave; 
	//map<int, vector<vector<int>>>* timeRecords;
	int tm = 0;
	int currentNode;
	int nextNode;
	int te = 0;
	int index;
	// Define Nodes of Current Road
	currentNode = roadID2NodeID[RoadSegmentID.first].first;
	int routeIDTemp = newRoutePair.first;
	nextNode = roadID2NodeID[RoadSegmentID.first].second;
	int nextNodeIndex;
	for (int i=0;i<timeRange[currentNode].size();i++){
		if(timeRange[currentNode][i].first == nextNode){
			nextNodeIndex = i;
		}
	}
	// Define Road's Minimal Travel Time
	tm = nodeID2minTime[make_pair(currentNode, nextNode)];
	// If Current Road Not Belongs to New Route
	if (inTime != 0){
		// Step 2.1: Find Time Records
		// -------------------------------------------
		int hour = time_2_hour(inTime); 
		index = hour_2_index(hour);

		// timeRecords = &timeSliceInsert[RoadSegmentID.first][index];
		// Generate Flow Base for Travel Time Estimation
		pair<int, int> flowBaseIndex = time_to_base_index(inTime, minRange);   // Hour Index, Minutes Index
		int hourIndex = flowBaseIndex.first;						        
		int minuIndex = flowBaseIndex.second;
        int flowBase = flowBaseList[currentNode][nextNodeIndex].second[hourIndex][minuIndex];
		// Update Traffic Flow Number
		trafficFlow = flowBase;
		trafficFlowInsert = flowBase;
		// Print Time Records Before Manipulation
		if (print == true){
			map<int, vector<vector<int>>>::iterator itrPrintV1;
			cout << endl;
			cout << "Step 2: time records before manipulation" << endl;
			cout << "-----------------------------------------" << endl;
			cout << "TimeRecord Size is: " << timeSliceInsert[RoadSegmentID.first][index].size() << endl;
			cout << "RoadSegmentID: " << RoadSegmentID.first << " with TimeSlice: " << index << endl;
			for (itrPrintV1 = timeSliceInsert[RoadSegmentID.first][index].begin(); itrPrintV1 != timeSliceInsert[RoadSegmentID.first][index].end(); ++itrPrintV1){
				for (int i=0;i<itrPrintV1->second.size();i++){
					cout << " time " << itrPrintV1->first << " routeID " << itrPrintV1->second[i][0];
					cout << " status " << itrPrintV1->second[i][1] << " flow " << itrPrintV1->second[i][2] << "||";
				}
			}
			cout << endl;
		}

		// Step 2.2: Find Time Value or Insert
		// -------------------------------------------
		// Variable Initialization
		int preFlow; 
		vector<int> features = {newRouteID, 1, 0};
		map<int, vector<vector<int>>>::iterator itFind; 
		itFind = timeSliceInsert[RoadSegmentID.first][index].find(inTime);

		// If Time Not Find, Insert Time
		if (itFind == timeSliceInsert[RoadSegmentID.first][index].end()){
			timeSliceInsert[RoadSegmentID.first][index].insert(pair<int, vector<vector<int>>>(inTime, {features}));

			// Step 2.3: Find Previous Value for Traffic Flow
			// -------------------------------------------
			map<int, vector<vector<int>>>::iterator itPre;
			itPre = timeSliceInsert[RoadSegmentID.first][index].find(inTime);
			if (itPre != timeSliceInsert[RoadSegmentID.first][index].end()){
				// If Time Record is The First One
				if (itPre == timeSliceInsert[RoadSegmentID.first][index].begin()){
					preFlow = 0;
					/* cout << "time is the first one with flow : " << preFlow << endl; */
				}
				else{
					itPre = --itPre; 
					preFlow = itPre->second.back()[2];
					/* cout << "previous time: " << itPre->first << " flow: " << preFlow << endl; */
				}
			}
			// If Time Record Not Find, It Must Be Error Since We Just Insert It
			else{
				cout << "Error. no value found, please check Insertion." << endl;
				return Null;
			}
		}
		// If Time Record Find, Push Back Feature in It
		else{
			preFlow = itFind->second.back()[2];
			features = {newRouteID, 1, preFlow};
			timeSliceInsert[RoadSegmentID.first][index][inTime].push_back(features);
		}

		// Step 2.4: Estimate New Time Record's Travel Time
		// -------------------------------------------
		// Update Traffic Flow
		// Update Traffic Flow of Driving In Time Record
		if (preFlow == 0){
			trafficFlow += preFlow; 
			trafficFlowInsert += preFlow + 1;
		}
		else{
			trafficFlow = preFlow; 
			trafficFlowInsert = preFlow + 1;
		}
		// Update Traffic Flow of Driving In Time Record
		map<int, vector<vector<int>>>::iterator itFind1;
		itFind1 = timeSliceInsert[RoadSegmentID.first][index].find(inTime);
		itFind1->second.back()[2] = trafficFlowInsert;
		// Estimate New Time Record's Leaving Time
		if (range == false)
			te = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
		else
			te = flow2time_by_range(currentNode, nextNodeIndex, trafficFlowInsert);

		// Estimate Leaving Time
		NewTimeLeave = inTime + te;

		// Put New Time Record's Leaving Time Record in Insert Set
		if (insertList.find(NewTimeLeave) == insertList.end())
			insertList.insert(pair<int, vector<vector<int>>>((NewTimeLeave), {{newRouteID, 0, 0}}));
		else
			insertList[NewTimeLeave].push_back({newRouteID, 0, 0});
	}
	// If Current Time Record Does Not Belong to New Route
	else{
		std::map<int, vector<vector<int>>>::iterator itr_find1st;
		if (InsertPre.size() != 0){
			if((*InsertPre.begin()).first < 0){
				cout << "Insert Pre < 0" << endl;
			}
			itr_find1st = InsertPre.begin();
			int hour = time_2_hour(itr_find1st->first);
			index = hour_2_index(hour);

			//timeRecords = &timeSliceInsert[RoadSegmentID.first][index];
			NewTimeLeave = 0;
			// Generate Flow Base for Travel Time Estimation
			pair<int, int> flowBaseIndex = time_to_base_index(itr_find1st->first, minRange);   // Hour Index, Minutes Index
			int hourIndex = flowBaseIndex.first;
			int minuIndex = flowBaseIndex.second;

			// int flowBase = 0;
			int flowBase = flowBaseList[currentNode][nextNodeIndex].second[hourIndex][minuIndex];
			// Update Traffic Flow Number
			trafficFlow = flowBase;
			trafficFlowInsert = flowBase;

			// Print Time Records Before Manipulation
			if (print == true){
				map<int, vector<vector<int>>>::iterator itrPrintV1;
				cout << endl;
				cout << "Step 2: time records before manipulation" << endl;
				cout << "-----------------------------------------" << endl;
				cout << "TimeRecord Size is: " << timeSliceInsert[RoadSegmentID.first][index].size() << endl;
				cout << "RoadSegmentID: " << RoadSegmentID.first << " with TimeSlice: " << index << endl;
				for (itrPrintV1 = timeSliceInsert[RoadSegmentID.first][index].begin(); itrPrintV1 != timeSliceInsert[RoadSegmentID.first][index].end(); ++itrPrintV1){
					for (int i=0;i<itrPrintV1->second.size();i++){
						cout << " time " << itrPrintV1->first << " routeID " << itrPrintV1->second[i][0];
						cout << " status " << itrPrintV1->second[i][1] << " flow " << itrPrintV1->second[i][2] << "||";
					}
				}
				cout << endl;
			}
		}
		else{
			cout << "I' size equals to 0." << endl;

			return Null;
		}
	}

	// If Min Travel Time Is Smaller Than 1
	// No Records Are Affected -> Directly Process The Next One
	// However, It Needs to Add The Not Inserted Time Record,
	// Which May Not Decrease The Time Consumption A Lot
	if (tm < 1){
	    int iCurrentRoad = RoadSegmentID.first;
	    // int routeID = newRoutePair.first;     // 这里应该是routeID
	    // vector<int> affected_route = Pi[routeID];
	    // vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
        int iNextRoadID = 0;
        if (inTime == 0){
            iNextRoadID = find_next_roadID(routeProp.second, RoadSegmentID.first);
        }
        else{
            iNextRoadID = find_next_roadID(newRoutePair.second, RoadSegmentID.first);
        }

	    if (iNextRoadID == -1){
			if (timeSliceInsert[RoadSegmentID.first][index].find(NewTimeLeave) == timeSliceInsert[RoadSegmentID.first][index].end()){
                timeSliceInsert[RoadSegmentID.first][index].insert(pair<int, vector<vector<int>>>(NewTimeLeave, {{newRouteID, 0, trafficFlowInsert-1}}));
            }
            else{
                timeSliceInsert[RoadSegmentID.first][index][NewTimeLeave].push_back({newRouteID, 0, trafficFlowInsert-1});
            }

	        return Null;
	    }
	    else{
			if (timeSliceInsert[RoadSegmentID.first][index].find(NewTimeLeave) == timeSliceInsert[RoadSegmentID.first][index].end()){
                timeSliceInsert[RoadSegmentID.first][index].insert(pair<int, vector<vector<int>>>(NewTimeLeave, {{newRouteID, 0, trafficFlowInsert-1}}));
            }
            else{
                timeSliceInsert[RoadSegmentID.first][index][NewTimeLeave].push_back({newRouteID, 0, trafficFlowInsert-1});
            }

	        tuple<int, map<int, vector<vector<int>>>, vector<int>> tempReturn;
	        get<0>(tempReturn) = inTime;
	        get<1>(tempReturn) = InsertPre;
	        get<2>(tempReturn) = DeletionPre;

			if (inTime == 0){
                roadSegmentAffected.push_back(make_pair(make_pair(iNextRoadID,routeProp.first),tempReturn));
            }
            else{
                roadSegmentAffected.push_back(make_pair(make_pair(iNextRoadID,newRoutePair.first),tempReturn));
            }
	        return roadSegmentAffected;
	    }
	}

	// Step 3: Update Operation
	// -------------------------------------------

	// timeRecords = &timeSliceInsert[RoadSegmentID.first][index];
	// Check New Time Record is Last One
	map<int, vector<vector<int>>>::iterator itrInser;
	// For Each Time Record Start From New Next.
	// Variable Initialization
	int timeCurrent = 0;
	int RoadIDCurrent = 0;
	int teCurrent = 0;
	int RouteIDIPre;
    int nextRoadSegmentID;
	tuple<int, map<int, vector<vector<int>>>, vector<int>> tempReturn;
	// Find Next Road ID
    if (inTime == 0){
        nextRoadSegmentID = find_next_roadID(routeProp.second, RoadSegmentID.first);
    }
    else{
        nextRoadSegmentID = find_next_roadID(newRoutePair.second, RoadSegmentID.first);
    }

	// If Current Road Belongs to New Route, Iteration Starts from The Next Position of Insert Time
	if (inTime != 0){
		itrInser = ++timeSliceInsert[RoadSegmentID.first][index].find(inTime);
	}
	// If Current Road Not belongs to New Route, Iteration Starts from The First Time Record
	else{
		itrInser = timeSliceInsert[RoadSegmentID.first][index].begin();
	}
	// Check If Traffic Flow Change from New's Next to The Last One
	map<int, vector<vector<int>>>::iterator itr;
	for (itr = itrInser; itr != timeSliceInsert[RoadSegmentID.first][index].end();){
		// Define Break Variable to Stop Iteration Further
		bool bBreak = false;        // Continues to Next Iteration
		bool bBreak2 = false;       // Break Iteration
		// Each Time Contain One or More Time Records
		// For Each Time Record (Value) Under A Time (Key)
		int dynamicSize = itr->second.size();
		//for (int i = 0; i < itr->second.size(); i++)
		for (int i = 0; i < dynamicSize; i++){
			// Variable Initialization
			timeCurrent = itr->first; 
			int secondSize = itr->second.size(); 
			RoadIDCurrent = itr->second[i][0];
			int temp = trafficFlowInsert;
			// Number of Time Records Under Current Time
			int temp_size = itr->second.size();
			int insetionPre_check = 0;
			// Insert Time Records In "I'" Set
			map<int, vector<vector<int>>>::iterator itrInPre;
            int RouteIDInPre;
			for (itrInPre = InsertPre.begin(); itrInPre != InsertPre.end();){
				// Variable Initialization
				// Current Time Record in "I'" set
				int timeIPre = itrInPre->first;
				if (timeIPre < timeCurrent){
					// Insert Time Records In "I" set
					map<int, vector<vector<int>>>::iterator itrI0;
					int RouteIDI0;
					for (itrI0 = insertList.begin(); itrI0 != insertList.end();){
						// Variable Initialization
						// Current Time Record in "I" set
						int timeI0 = itrI0->first;
						// Compare Each Time Record in "I" Set
						if (timeI0 < timeIPre){
							for (int j = 0; j < itrI0->second.size(); j++)
							{
								// "I" Set Only Contains Time Records with Leaving Status
								trafficFlowInsert -= 1;
								RouteIDI0 = itrI0->second[j][0];
								// If Time Record Is Driving in Status
								if (itrI0->second[j][1] == 1){
									cout << "Error. element in I is not leaving status." << endl;
								}
								else{
									// Time (Key) Not Exist -> Insert One with Time Record as Value
									if (timeSliceInsert[RoadSegmentID.first][index].find(timeI0) == timeSliceInsert[RoadSegmentID.first][index].end()) 
										timeSliceInsert[RoadSegmentID.first][index].insert(pair<int, vector<vector<int>>>(timeI0,
													{{RouteIDI0, 0, trafficFlowInsert}}));
									// Push Back Time Record (Value) after Time (Key)
									else
										timeSliceInsert[RoadSegmentID.first][index][timeI0].push_back({RouteIDI0, 0, trafficFlowInsert});
								}
							}
							// Remove Inserted Time Record from "I" Set
							itrI0 = insertList.erase(itrI0);
						}
						else{
							// Iterate to Next Time Record
							++itrI0;
						}
					}
					// Insertion Operation of "I'" Srt
					// Variable Initialization
					int inside_time_current;
					int inside_roadID_current = 0;
					for (int j = 0; j < itrInPre->second.size(); j++){
						// "I'" Set Only Contains Time Records with Entrance Status
						trafficFlowInsert += 1;

						RouteIDInPre = itrInPre->second[j][0];
						// If Time Record Is Driving in Status
						if (itrInPre->second[j][1] == 1){
							if (timeSliceInsert[RoadSegmentID.first][index].find(timeIPre) == timeSliceInsert[RoadSegmentID.first][index].end())
								timeSliceInsert[RoadSegmentID.first][index].insert(pair<int, vector<vector<int>>>(timeIPre,
											{{RouteIDInPre, 1, trafficFlowInsert}}));
							else
								timeSliceInsert[RoadSegmentID.first][index][timeIPre].push_back({RouteIDInPre, 1, trafficFlowInsert});
						}
						else{
							cout << "Error. element in I' is not driving in status." << endl;
						}

						inside_time_current = itrInPre->first; 
						inside_roadID_current = itrInPre->second[j][0];
						insetionPre_check = 1;
						// Estimate Inserted Driving In Time Record's Travel Time
						// Which Is Its Leaving Time
						if (range == false){
							teCurrent = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
						}
						else{
							teCurrent = flow2time_by_range(currentNode, nextNodeIndex, trafficFlowInsert);
						}
						// Add Estimated Time Record with Leaving Status to "I" List
						if (insertList.find(NewTimeLeave) == insertList.end())
							insertList.insert(pair<int, vector<vector<int>>>((inside_time_current+teCurrent),
										{{inside_roadID_current, 0, 0}}));
						else
							insertList[inside_time_current+teCurrent].push_back({inside_roadID_current, 0, 0});
						// Add Road ID into "D" (Deletion) List Because Its Leaving Time Is Changed

						// Find Next Road ID
						// int routeID =  itrInPre->second[j][0];
						int unaffected_next_roadSegmentID = nextRoadSegmentID;
						// Decide If Next Road Belongs to New Route for Different Operation
						std::vector<int>::iterator itrFindNew;
						itrFindNew = std::find(newRoute.begin(), newRoute.end(), unaffected_next_roadSegmentID);
						// Find IF Next Road Belongs to Affected Road
						roadSegmentAffectedID.clear();
						for (int k=0;k<roadSegmentAffected.size();k++){
							int tempRoadSegmentID = roadSegmentAffected[k].first.first;
							roadSegmentAffectedID.push_back(tempRoadSegmentID);
						}
						std::vector<int>::iterator itFindExit;
						itFindExit = std::find(roadSegmentAffectedID.begin(), roadSegmentAffectedID.end(), unaffected_next_roadSegmentID);
						// If Already Added
						// Because Time Records Are Delayed, Driving in Time in The Next Road Should Change
						// Code Related to "get" Mainly Return Information Related to The Next Road
						if (itFindExit != roadSegmentAffectedID.end()){
							int position = itFindExit - roadSegmentAffectedID.begin();
							tempReturn = roadSegmentAffected[position].second;
							if (get<1>(tempReturn).find(inside_time_current + teCurrent) == get<1>(tempReturn).end()){
								get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((inside_time_current + teCurrent), {{inside_roadID_current, 1, 0}}));
							}
							else{
								get<1>(tempReturn)[inside_time_current + teCurrent].push_back({inside_roadID_current, 1, 0});
							}
							get<2>(tempReturn).push_back(inside_roadID_current);
							// If Road Belongs to New Route
							if (itrFindNew != newRoute.end()){
								get<0>(tempReturn) = NewTimeLeave;
								roadSegmentAffected[position].second = tempReturn;
							}
							else{
								get<0>(tempReturn) = 0;
								roadSegmentAffected[position].second = tempReturn;
							}
						}
						else{
							if (itrFindNew != newRoute.end()){
								get<0>(tempReturn) = NewTimeLeave;
								get<1>(tempReturn).clear();
								if (get<1>(tempReturn).find(inside_time_current + teCurrent) == get<1>(tempReturn).end()){
									get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((inside_time_current + teCurrent), {{inside_roadID_current, 1, 0}}));
								}
								else{
									get<1>(tempReturn)[inside_time_current + teCurrent].push_back({inside_roadID_current, 1, 0});
								}
								get<2>(tempReturn).clear();
								get<2>(tempReturn).push_back(inside_roadID_current);
								roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,itrInPre->second[j][0]), tempReturn));
							}
							else{
								get<0>(tempReturn) = 0;
								get<1>(tempReturn).clear();
								if (get<1>(tempReturn).find(inside_time_current + teCurrent) == get<1>(tempReturn).end()){
									get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((inside_time_current + teCurrent), {{inside_roadID_current, 1, 0}}));
								}
								else{
									get<1>(tempReturn)[inside_time_current + teCurrent].push_back({inside_roadID_current});
								}
								get<2>(tempReturn).clear();
								get<2>(tempReturn).push_back(inside_roadID_current);
								roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,itrInPre->second[j][0]), tempReturn));
							}
						}
					}
					// Remove Inserted Value in "I'"
					itrInPre = InsertPre.erase(itrInPre);
				}
				else{
					// Iterate to Next Time Record in "I'"
					++itrInPre;
				}
			}

			// Step 4.1: Compare Time Records in Insert Set
			// -------------------------------------------
			map<int, vector<vector<int>>>::iterator itrI; int RouteIDI;
			for (itrI = insertList.begin(); itrI != insertList.end();){
				int timeI = itrI->first;
				if (timeI < timeCurrent){
					for (int j = 0; j < itrI->second.size(); j++){
						// Compare Each Time Record in Insert Set.
						trafficFlowInsert -= 1;
						RouteIDI = itrI->second[j][0];
						// If Inserted Time Record Had Driving In Status
						// Inserted Time Records Should Also Be Driving In Status
						if (itrI->second[j][1] == 1){
							cout << "Error. element in I is not leaving status." << endl;
						}
						else{
							if (timeSliceInsert[RoadSegmentID.first][index].find(timeI) == timeSliceInsert[RoadSegmentID.first][index].end()){
								timeSliceInsert[RoadSegmentID.first][index].insert(pair<int, vector<vector<int>>>(timeI,{{RouteIDI, 0, trafficFlowInsert}}));
							}
							else{
								timeSliceInsert[RoadSegmentID.first][index][timeI].push_back({RouteIDI, 0, trafficFlowInsert});
							}
						}
					}
					// Remove Inserted Value in "I" List
					itrI = insertList.erase(itrI);
				}
				else{
					// Iterate to The Next Time Record
					++itrI;
				}
			}
			// If No Values Need to Insert or Delete and Current Road Not Belong to New Route
			// Stop Iteration
            if (STAll == true){
                if (inTime == 0 and InsertPre.size() == 0 and DeletionPre.size() == 0 and insertList.size() == 0 and deletionList.size() == 0){
                    bBreak2 = true;
                    break;
                }
            }

			// Step 4.2: Iteration of Update Operation
			// -------------------------------------------
			// If Time Record is Driving In Status.
			map<int, vector<vector<int>>>::iterator itrIPre;

			if (itr->second[i][1] == 1){
				// If No Values Need to Insert or Delete and Terminal Condition Is Active
				// Stop Iteration
				if (terminalCondition == 1 and InsertPre.size() == 0 and DeletionPre.size() == 0 and insertList.size() == 0 and deletionList.size() == 0){
					if (terminal == true and insertList.size() == 0 and trafficFlowInsert == 0){  // Decide If Terminal Condition Is Involved
						bBreak2 = true;
						break;
					}
				}
				// Check Deletion in "D'" List
				if (DeletionPre.size() != 0){
					if (RoadIDCurrent == DeletionPre[0]){
						// Update Traffic Flow
						trafficFlow += 1;
						// Remove Current Time Record from Time Records
						itr->second.erase(itr->second.begin() + i);
						dynamicSize = itr->second.size();
						i--;
						// Remove Current Value in "D'" List
						vector<int>::iterator k = DeletionPre.begin();
						DeletionPre.erase(k);
						// If Time (Key) Is Empty after Deletion Operation
						// Remove Time (Key) from Time Records abd Stop Iteration in Current Time (Key)
						if(itr->second.size() == 0){
							itr = timeSliceInsert[RoadSegmentID.first][index].erase(itr);
							bBreak = true;
							break;
						}
						// Continue to The Next Iteration
						continue;
					}
				}
                // If Terminal Condition Is Active
                if (terminalCondition == 1){
                    // If Current Time Record Is The Last One of Current Route
                    // And Current Time Record (Value) Is The Last One In Time (Key)
                    if (itr == timeSliceInsert[RoadSegmentID.first][index].end() and i == itr->second.size()){
                        if (terminal == true  and insertList.size() == 0 and trafficFlowInsert == 0){ // Decide If Terminal Condition Is Involved
                            bBreak2 = true;
                            break;
                        }
                    }
                    // If Terminal Condition Is Active but Still Not Reach The Record Time
                    if (timeCurrent <= recordTime){
                        if (trafficFlow != trafficFlowInsert){
                            // Check IF Traffic Flows Are in Same Range
                            bool sameRange = flow_same_range_check(currentNode,
                                                                   nextNodeIndex, trafficFlow, trafficFlowInsert);
                            if (sameRange == false or range == false){
                                // If Traffic Flows Are Different, Inactive Terminal Condition
                                terminalCondition = 0;
                                // Update Changed Information
                                trafficFlow += 1;
                                trafficFlowInsert += 1;
                                // Update Time Record's Traffic Flow
                                itr->second[i][2] = trafficFlowInsert;
                                // Estimate New Travel Time
                                if (range == false){
                                    teCurrent = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
                                }
                                else{
                                    teCurrent = flow2time_by_range(currentNode, nextNodeIndex, trafficFlowInsert);
                                }
                                // Add Estimated Travel Time to Insert List for Further Insertion Operation
                                if (insertList.find(timeCurrent + teCurrent) == insertList.end())
                                    insertList.insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                                     {{RoadIDCurrent, 0, 0}}));
                                else
                                    insertList[timeCurrent + teCurrent].push_back({RoadIDCurrent, 0, 0});
                                // Add Odd Vehicle's (route) ID to Deletion List for Further Deletion Operation
                                deletionList.push_back(RoadIDCurrent);
                                // Find Next Road ID
                                // int routeID =  itr->second[i][0];
                                int unaffected_next_roadSegmentID;
                                if (itr->second[i][0] == newRouteID){
                                    unaffected_next_roadSegmentID = nextRoadSegmentID;
                                }
                                else{
                                    vector<int> affected_route = Pi[itr->second[i][0]];
                                    vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
                                    unaffected_next_roadSegmentID = find_next_roadID(affected_route_w_roadSegmentID, RoadSegmentID.first);
                                }
                                // Decide If Next Road Belongs to New Route for Different Operation
                                std::vector<int>::iterator itrFindNew;
                                itrFindNew = std::find (newRoute.begin(), newRoute.end(), unaffected_next_roadSegmentID);
                                // Find IF Next Road Belongs to Affected Road
                                roadSegmentAffectedID.clear();
                                for (int k=0;k<roadSegmentAffected.size();k++){
                                    int tempRoadSegmentID = roadSegmentAffected[k].first.first;
                                    roadSegmentAffectedID.push_back(tempRoadSegmentID);
                                }
                                std::vector<int>::iterator itFindExit;
                                itFindExit = std::find(roadSegmentAffectedID.begin(), roadSegmentAffectedID.end(), unaffected_next_roadSegmentID);
                                // If Already Added
                                // Because Time Records Are Delayed, Driving in Time in The Next Road Should Change
                                // Code Related to "get" Mainly Return Information Related to The Next Road
                                if (itFindExit != roadSegmentAffectedID.end()){
                                    int position = itFindExit - roadSegmentAffectedID.begin();
                                    tempReturn = roadSegmentAffected[position].second;
                                    if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
                                        get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                                                 {{RoadIDCurrent, 1, 0}}));
                                    }
                                    else{
                                        get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
                                    }
                                    get<2>(tempReturn).push_back(RoadIDCurrent);
                                    // If Road Belongs to New Route
                                    if (itrFindNew != newRoute.end()){
                                        get<0>(tempReturn) = NewTimeLeave;
                                        roadSegmentAffected[position].second = tempReturn;
                                    }
                                    else{
                                        get<0>(tempReturn) = 0;
                                        roadSegmentAffected[position].second = tempReturn;
                                    }
                                }
                                else{
                                    if (itrFindNew != newRoute.end()){
                                        get<0>(tempReturn) = NewTimeLeave;
                                        get<1>(tempReturn).clear();
                                        if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
                                            get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                                                     {{RoadIDCurrent, 1, 0}}));
                                        }
                                        else{
                                            get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
                                        }
                                        get<2>(tempReturn).clear();
                                        get<2>(tempReturn).push_back(RoadIDCurrent);
                                        roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,itr->second[i][0]), tempReturn));
                                    }
                                    else{
                                        get<0>(tempReturn) = 0;
                                        get<1>(tempReturn).clear();
                                        if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
                                            get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                                                     {{RoadIDCurrent, 1, 0}}));
                                        }
                                        else{
                                            get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
                                        }
                                        get<2>(tempReturn).clear();
                                        get<2>(tempReturn).push_back(RoadIDCurrent);
                                        roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,itr->second[i][0]), tempReturn));
                                    }
                                }
                            }
                            // Flow & Flow' in The Same Range, but Different Values
                            // Update Flow & Flow' and Related Time Record
                            else{
                                // Update Changed Information
                                trafficFlow += 1;
                                trafficFlowInsert += 1;
                                // Although Range Are Same, Since Flows Do Change, Flow Number Still Needs to Update
                                itr->second[i][2] = trafficFlowInsert;
                            }
                        }
                        // Flow & Flow' Have Same Different Values
                        else{
                            // Update Changed Information
                            trafficFlow += 1;
                            trafficFlowInsert += 1;
                        }
                    }
                    else{
                        if (InsertPre.size() == 0 and DeletionPre.size() == 0){
                            if (terminal == true  and insertList.size() == 0 and trafficFlowInsert == 0){
                                bBreak2 = true;
                                break;
                            }
                        }
                        // If "I'" & "D'" Still Have Value, Keep Iteration Until They Are Empty
                        trafficFlow += 1;
                        trafficFlowInsert += 1;
                        itr->second[i][2] = trafficFlowInsert;
                    }
                }
                // If Terminal Condition Is Not Active
                else{
                    trafficFlow += 1;
                    trafficFlowInsert += 1;
                    // Check If Flows Belongs to The Same Range
                    // Check IF Traffic Flows Are in Same Range
                    bool sameRange = flow_same_range_check(currentNode,
                                                           nextNodeIndex, trafficFlow, trafficFlowInsert);
                    if (sameRange == true){
                        // Active Terminal Condition
                        itr->second[i][2] = trafficFlowInsert;
                        terminalCondition = 1;
                        // Estimate Record Time (Decide If No Further Affect)
                        if (range == false){
                            teCurrent = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
                        }
                        else{
                            teCurrent = flow2time_by_range(currentNode, nextNodeIndex, trafficFlowInsert);
                        }
                        recordTime = timeCurrent+teCurrent;
                    }
                    else{
                        itr->second[i][2] = trafficFlowInsert;

                        RoadIDCurrent =  itr->second[i][0];
                        if (range == false){
                            teCurrent = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
                        }
                        else{
                            teCurrent = flow2time_by_range(currentNode, nextNodeIndex, trafficFlowInsert);
                        }
                        if (insertList.find(timeCurrent + teCurrent) == insertList.end())
                            insertList.insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                             {{RoadIDCurrent, 0, 0}}));
                        else
                            insertList[timeCurrent + teCurrent].push_back({RoadIDCurrent, 0, 0});
                        deletionList.push_back(RoadIDCurrent);
                        // int routeID =  itr->second[i][0];
                        int unaffected_next_roadSegmentID;
                        if (itr->second[i][0] == newRouteID){
                            unaffected_next_roadSegmentID = nextRoadSegmentID;
                        }
                        else{
                            vector<int> affected_route = Pi[itr->second[i][0]];
                            vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
                            unaffected_next_roadSegmentID = find_next_roadID(affected_route_w_roadSegmentID, RoadSegmentID.first);
                        }
                        // Find IF Next Road Belongs to New Route
                        std::vector<int>::iterator itrFindNew;
                        itrFindNew = std::find (newRoute.begin(), newRoute.end(), unaffected_next_roadSegmentID);
                        // Find IF Next Road Belongs to Affected Road
                        roadSegmentAffectedID.clear();
                        for (int k=0;k<roadSegmentAffected.size();k++){
                            int tempRoadSegmentID = roadSegmentAffected[k].first.first;
                            roadSegmentAffectedID.push_back(tempRoadSegmentID);
                        }
                        std::vector<int>::iterator itFindExit;
                        itFindExit = std::find(roadSegmentAffectedID.begin(), roadSegmentAffectedID.end(), unaffected_next_roadSegmentID);
                        if (itFindExit != roadSegmentAffectedID.end()){
                            int position = itFindExit - roadSegmentAffectedID.begin();
                            tempReturn = roadSegmentAffected[position].second;
                            if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
                                get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent), {{RoadIDCurrent, 1, 0}}));
                            }
                            else{
                                get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
                            }
                            get<2>(tempReturn).push_back(RoadIDCurrent);
                            // If Road Belongs to New Route
                            if (itrFindNew != newRoute.end()){
                                get<0>(tempReturn) = NewTimeLeave;
                                roadSegmentAffected[position].second = tempReturn;
                            }
                            else{
                                get<0>(tempReturn) = 0;
                                roadSegmentAffected[position].second = tempReturn;
                            }
                        }
                        else{
                            if (itrFindNew != newRoute.end()){
                                get<0>(tempReturn) = NewTimeLeave;
                                get<1>(tempReturn).clear();
                                if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
                                    get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),{{RoadIDCurrent, 1, 0}}));
                                }
                                else{
                                    get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
                                }
                                get<2>(tempReturn).clear();
                                get<2>(tempReturn).push_back(RoadIDCurrent);
                                roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,itr->second[i][0]), tempReturn));
                            }
                            else{
                                get<0>(tempReturn) = 0;
                                get<1>(tempReturn).clear();
                                if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end())
                                    get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                                             {{RoadIDCurrent, 1, 0}}));
                                else
                                    get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
                                get<2>(tempReturn).clear();
                                get<2>(tempReturn).push_back(RoadIDCurrent);
                                roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,itr->second[i][0]), tempReturn));
                            }
                        }
                    }
                }
			}
			else{
				if (DeletionPre.size() != 0 or inTime != 0 or deletionList.size() != 0){
					// if (deletionList.size() != 0 and RoadIDCurrent == deletionList[0])
					if (deletionList.size() != 0){
						int deleID;
						for (int l=0;l<deletionList.size();l++){
							deleID = deletionList[l];
							if (RoadIDCurrent == deleID){
								trafficFlow -= 1;
								itr->second.erase(itr->second.begin() + i);
								dynamicSize = itr->second.size();
								i--;
								vector<int>::iterator k = deletionList.begin() + l; 
								deletionList.erase(k);
								if(itr->second.size() == 0){
									itr = timeSliceInsert[RoadSegmentID.first][index].erase(itr);
									if (timeSliceInsert[RoadSegmentID.first][index].size() == 0){
										// cout << "timeRecords size is: " << timeSliceInsert[RoadSegmentID.first][index].size() << endl;
										bBreak2 = true;
										break;
									}
									bBreak = true;
									break;
								}
								break;
							}
						}
					}
					else{
						if (itr->second[i][1] == 0){
							trafficFlowInsert -= 1;
							trafficFlow -= 1;
							int routeIDDele = itr->second[i][0];
							itr->second[i][2] = trafficFlowInsert;
						}
					}
				}
			}
		}
		if(bBreak)
			continue;
		if(bBreak2)
			break;
		dynamicSize = itr->second.size();
		++itr;
	}

	// Insert Rest Values Contained in "I'"
	map<int, vector<vector<int>>>::iterator itrInPreRest; int RouteIDInPre;
	if (InsertPre.size() != 0){
		for (itrInPreRest = InsertPre.begin(); itrInPreRest != InsertPre.end(); ++itrInPreRest){
			for (int i=0;i<itrInPreRest->second.size();i++){
				// Compare Each Time Record in Insert Set.
				int timeIPre = itrInPreRest->first;
				// Compare Each Time Record in I' with Time Records in I
				map<int, vector<vector<int>>>::iterator itrI2; int RouteIDI;
                for (itrI2 = insertList.begin(); itrI2 != insertList.end();){
					int timeI2 = itrI2->first;
					if (timeI2 < timeIPre){
						for (int j = 0; j < itrI2->second.size(); j++){
							// Compare Each Time Record in Insert Set.
							trafficFlowInsert += 1;
							RouteIDI = itrI2->second[j][0];
							// Status In
							if (itrI2->second[j][1] == 1){
								cout << "Error. element in I is not leaving status." << endl;
							}
							// Status Out
							else{
								if (timeSliceInsert[RoadSegmentID.first][index].find(timeI2) == timeSliceInsert[RoadSegmentID.first][index].end()){
									timeSliceInsert[RoadSegmentID.first][index].insert(
											pair<int, vector<vector<int>>>(timeI2, {{RouteIDI, 0, trafficFlowInsert}}));
								}
								else{
									timeSliceInsert[RoadSegmentID.first][index][timeI2].push_back({RouteIDI, 0, trafficFlowInsert});
								}
							}
						}
						itrI2 = insertList.erase(itrI2);
					}
					else{
						++itrI2;
					}
				}
				trafficFlowInsert += 1;
                RouteIDInPre = itrInPreRest->second[i][0];
				if (timeSliceInsert[RoadSegmentID.first][index].find(timeIPre) == timeSliceInsert[RoadSegmentID.first][index].end())
					timeSliceInsert[RoadSegmentID.first][index].insert(pair<int, vector<vector<int>>>(timeIPre, {{RouteIDInPre,1,trafficFlowInsert}}));
				else
					timeSliceInsert[RoadSegmentID.first][index][timeIPre].push_back({RouteIDInPre,1,trafficFlowInsert});
				int timeCurrent = itrInPreRest->first;
				int RoadIDCurrent = itrInPreRest->second[i][0];
				// Estimate New Time Record's Leaving Time
				if (range == false)
					teCurrent = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
				else
					te = flow2time_by_range(currentNode, nextNodeIndex, trafficFlowInsert);
				if (insertList.find(timeCurrent + teCurrent) == insertList.end())
					insertList.insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
								{{RoadIDCurrent, 0, 0}}));
				else
					insertList[timeCurrent + teCurrent].push_back({RoadIDCurrent, 0, 0});

				deletionList.push_back(RoadIDCurrent);
				// Find Next Road ID
				// int routeID =  itrInPreRest->second[i][0];
				int unaffected_next_roadSegmentID = nextRoadSegmentID;
				std::vector<int>::iterator itrFindNew;
				itrFindNew = std::find(newRoute.begin(), newRoute.end(), unaffected_next_roadSegmentID);
				// Find IF Next Road Belongs to Affected Road
				roadSegmentAffectedID.clear();
				for (int i=0;i<roadSegmentAffected.size();i++){
					int tempRoadSegmentID = roadSegmentAffected[i].first.first;
					roadSegmentAffectedID.push_back(tempRoadSegmentID);
				}
				std::vector<int>::iterator itFindExit;
				itFindExit = std::find(roadSegmentAffectedID.begin(), roadSegmentAffectedID.end(), unaffected_next_roadSegmentID);
				// If Already Added
				if (itFindExit != roadSegmentAffectedID.end()){
					int position = itFindExit - roadSegmentAffectedID.begin();
					tempReturn = roadSegmentAffected[position].second;
					if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end())
						get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
									{{RoadIDCurrent, 1, 0}}));
					else
						get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
					get<2>(tempReturn).push_back(RoadIDCurrent);
					// If Road Belongs to New Route
					if (itrFindNew != newRoute.end()){
						get<0>(tempReturn) = NewTimeLeave;
						roadSegmentAffected[position].second = tempReturn;
					}
					else{
						get<0>(tempReturn) = 0;
						roadSegmentAffected[position].second = tempReturn;
					}
				}
				else{
					if (itrFindNew != newRoute.end()){
						get<0>(tempReturn) = NewTimeLeave;
						get<1>(tempReturn).clear();
						if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end())
							get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
										{{RoadIDCurrent, 1, 0}}));
						else
							get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
						get<2>(tempReturn).clear();
						get<2>(tempReturn).push_back(RoadIDCurrent);
						roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,itrInPreRest->second[i][0]), tempReturn));
					}
					else{
						get<0>(tempReturn) = 0;
						get<1>(tempReturn).clear();
						if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end())
							get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
										{{RoadIDCurrent, 1, 0}}));
						else
							get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
						get<2>(tempReturn).clear();
						get<2>(tempReturn).push_back(RoadIDCurrent);
						roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,itrInPreRest->second[i][0]), tempReturn));
					}
				}
			}
		}
	}
	// Rest Deletion Element
	if (deletionList.size() != 0){
		// Update Traffic Flow
		trafficFlow -= 1;
		map<int, vector<vector<int>>>::iterator itrend;
		itrend = --timeSliceInsert[RoadSegmentID.first][index].end();
		for (int i = 0; i < itrend->second.size();){
			if (deletionList.size() == 0)
				break;
			// Define Route ID
			RoadIDCurrent = itrend->second[i][0];
			// Check If Is Target Route and Leaving Status
			if (itrend->second[i][1] == 0 and RoadIDCurrent == deletionList[0]){
				itrend->second.erase(itrend->second.begin() + i);
				if (deletionList.size() == 0){
					cout << "deletionList size is 0 " << endl;
				}
				vector<int>::iterator k = deletionList.begin();
				deletionList.erase(k);
				if(itrend->second.size() == 0){
					timeSliceInsert[RoadSegmentID.first][index].erase(itrend);
					break;
				}
			}
			else{
				i++;
				continue;
			}
		}
	}

	// Rest Insert Elements
	map<int, vector<vector<int>>>::iterator itrEnd; int FlowEnd;
	itrEnd = --timeSliceInsert[RoadSegmentID.first][index].end();
	FlowEnd = itrEnd->second.back()[2];
	map<int, vector<vector<int>>>::iterator itrRestI;
	int timeRest, RouteIDRest, FlowRest;
	if (insertList.size() != 0){
		for (itrRestI = insertList.begin();itrRestI != insertList.end();++itrRestI){
			for (int i=0;i<itrRestI->second.size();i++){
				timeRest = itrRestI->first; RouteIDRest = itrRestI->second[i][0];
				FlowRest = FlowEnd - 1;;
				FlowEnd -= 1;
				if (timeSliceInsert[RoadSegmentID.first][index].find(timeRest) == timeSliceInsert[RoadSegmentID.first][index].end())
					timeSliceInsert[RoadSegmentID.first][index].insert(pair<int, vector<vector<int>>>(timeRest, {{RouteIDRest, 0, FlowRest}}));
				else
					timeSliceInsert[RoadSegmentID.first][index][timeRest].push_back({RouteIDRest, 0, FlowRest});
			}
		}
	}

	// Step 5: Print Result
	// -------------------------------------------

	if (print == true){
		cout << endl;
		cout << "Step 3: time records after manipulation" << endl;
		cout << "-----------------------------------------" << endl;
		map<int, vector<vector<int>>>::iterator itrPrint;
		cout << "TimeRecord Size is: " << timeSliceInsert[RoadSegmentID.first][index].size() << endl;
		cout << "RoadSegmentID: " << RoadSegmentID.first << " with TimeSlice: " << index << endl;
		for (itrPrint = timeSliceInsert[RoadSegmentID.first][index].begin(); itrPrint != timeSliceInsert[RoadSegmentID.first][index].end(); ++itrPrint) {
			for (int i=0;i<itrPrint->second.size();i++)
			{
				cout << " time " << itrPrint->first << " routeID " << itrPrint->second[i][0];
				cout << " status " << itrPrint->second[i][1] << " flow " << itrPrint->second[i][2] << "||";
			}
		}
		cout << endl;
	}

	// Step 5: Update Result
	// -------------------------------------------

	std::vector<int>::iterator itr_findNextExist;
	roadSegmentAffectedID.clear();
	for (int i=0;i<roadSegmentAffected.size();i++){
		roadSegmentAffectedID.push_back(roadSegmentAffected[i].first.first);
	}
	itr_findNextExist = std::find(roadSegmentAffectedID.begin(), roadSegmentAffectedID.end(), nextRoadSegmentID);
	tuple<int, map<int, vector<vector<int>>>, vector<int>> fillReturn;
	if (nextRoadSegmentID != -1 and itr_findNextExist == roadSegmentAffectedID.end()){
		get<0>(fillReturn) = NewTimeLeave; get<1>(fillReturn).clear(); get<2>(fillReturn).clear();
		// roadSegmentAffected.push_back(make_pair(make_pair(nextRoadSegmentID,0),fillReturn));
		if (inTime == 0){
			roadSegmentAffected.push_back(make_pair(make_pair(nextRoadSegmentID,routeProp.first),fillReturn));
		}
		else{
			roadSegmentAffected.push_back(make_pair(make_pair(nextRoadSegmentID,newRoutePair.first),fillReturn));
		}
	}
	return roadSegmentAffected;
}

// Insertion Operation by Inserting New Route Data One by One
// "newRoute" Contains: Route ID + Route Constructed by Roads
vector<int> Graph::one_route_update_insertion(pair<int, vector<int>> &newRoute, int inTime, bool parallel, bool terminal, bool range, bool print)
{
    bool prop;
    prop = false;

	// Initialize A List to Store All Affected Road ID
	// "tuple" includes: <0/leaveTime, I', D'>
	vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> roadSegmentSet;
	int RoadSegmentID1st = newRoute.second[0]; 
	int count = 0;
	// Add New Inserted Route Data into List
	vector<int> affectedRoadID; affectedRoadID.clear();
	affectedRoadID.push_back(RoadSegmentID1st);

	if (print == true){
		cout << "=======================================================================" << endl;
		cout << "count: " << count << endl;
	}
    roadSegmentSet = update_operation_1st(RoadSegmentID1st, inTime, newRoute, parallel, terminal, range, print);
	count += 1;

	// When List Is Not Empty
	// Store Temple Road Information
	// Value Changes During Each Iteration
	pair<int, tuple<int, map<int, vector<vector<int>>>, vector<int>>> roadSegmentTemp;
	int leaveTime;                              // Driving In Time for The Next Road
	map<int, vector<vector<int>>> insert;       // Updated Time Records to Insertion
	vector<int> deletion;                       // Odd Time Records for Deletion
	pair<int,int> roadSegmentIDTemp;            //
	vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> roadSegmentSetTemp;
	// When List Is Not Empty
	while(roadSegmentSet.size() != 0){
		// Add Operation Number (Number of Affected Road)
		count += 1;
		// Select The First Affected Road
		std::vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>>::iterator itr;
		itr = roadSegmentSet.begin();
		// Define ID and Information
		roadSegmentIDTemp = itr->first;
		int routeID = itr->first.second;
        vector<int> affected_route = Pi[routeID];
        vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
        pair<int, vector<int>> routeProp = make_pair(routeID,affected_route_w_roadSegmentID);
		leaveTime = get<0>(itr->second);
		// Check If Current Affected Time Are Updated
		vector<int>::iterator itr_findDup;
		itr_findDup = std::find(affectedRoadID.begin(), affectedRoadID.end(), roadSegmentIDTemp.first);
		// If Current Road Is The Last One of New Route or Already Updated
        if (prop == true){
            if (roadSegmentIDTemp.first == -1 or itr_findDup != affectedRoadID.end()){
                roadSegmentSet.erase(itr);
                continue;
            }
        }
        else{
            if (roadSegmentIDTemp.first == -1){
                roadSegmentSet.erase(itr);
                continue;
            }
        }

		// Print Road ID and Driving in Time
		if (print == true){
			cout << "=======================================================================" << endl;
			cout << endl;
			cout << "Step 1: round start" << endl;
			cout << "-----------------------------------------" << endl;
			cout << "count: " << count << endl;
			cout << "road segment ID is: " << roadSegmentIDTemp.first << " with leaveTime: " << leaveTime << endl;
		}
		// Define Insertion and Deletion Information & Print
		insert = get<1>(itr->second);
		deletion = get<2>(itr->second);
		if (print == true){
			cout << "Insert set size is: " << insert.size() << " Deletion size is: " << deletion.size() << endl;
			map<int, vector<vector<int>>>::iterator itrInsert;
			for (itrInsert = insert.begin(); itrInsert != insert.end();++itrInsert)
			{
				cout << "inserted time is: " << itrInsert->first << " with size: " << itrInsert->second.size() << endl;
			}
		}
		// Remove Current Road from List
		roadSegmentSet.erase(itr);
		// Update Operation for Further Road
		roadSegmentSetTemp = updateOperationFurther(roadSegmentIDTemp, leaveTime, newRoute, routeProp, insert, deletion, parallel, terminal, range, print);
		// Add Affected Road ID in List
		affectedRoadID.push_back(roadSegmentIDTemp.first);
		// Add Next Affected Road Information for Next Iteration
		for (int i=0;i<roadSegmentSetTemp.size();i++){
			roadSegmentSet.push_back(roadSegmentSetTemp[i]);
		}
	}

	if (print == true){
		cout << "This Route Update Done." << endl;
		cout << "=======================================================================" << endl;
	}
	return affectedRoadID;
}

//  Split New Route Data into Groups
void Graph::multi_new_data_initial(void)
{
	for (int i = 0; i < departTimeList.size(); i++){
		newDataMulti.push_back(make_pair(routeRoadInput[i], departTimeList[i]));
	}
}

void Graph::update_operation_parallel(vector<pair<pair<int, vector<int>>, int>> &newDataMulti, bool &parallel, bool terminal,
		bool &range, bool print, int &threadNum){
	// Initialize Time Slices
	timeSliceInsert.clear();
	timeSliceInsert = timeSlice;
	vLock.reserve(roadInfor.size());

	for(int i = 0; i < roadInfor.size(); i++)
	{
        Semaphore *sm = new Semaphore(1);
        vLock.push_back(sm);
	}
	boost::thread_group threadf;
	int step = newDataMulti.size()/threadNum;
	vector<int> id_temp;

	std::chrono::high_resolution_clock::time_point t0, t1;
	std::chrono::duration<double> time_span1;
	t0=std::chrono::high_resolution_clock::now();

	for(int i = 0; i < threadNum; i++){
		if(i < threadNum - 1)
			threadf.add_thread(new boost::thread(&Graph::one_route_parallel_update, this, boost::ref(newDataMulti),
						i*step, (i+1)*step, boost::ref(vLock), parallel, terminal, range, print));
		else
			threadf.add_thread(new boost::thread(&Graph::one_route_parallel_update, this, boost::ref(newDataMulti),
						i*step, newDataMulti.size(), boost::ref(vLock), parallel, terminal, range, print));
	}
	threadf.join_all();

	t1=std::chrono::high_resolution_clock::now();
	time_span1 = std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0);
	if (terminal == true)
		cout << "Insertion operation's time consumption w threads & w termination is: " << time_span1.count() << "s." << endl;
	else
		cout << "Insertion operation's time consumption w threads & w/o termination is: " << time_span1.count() << "s." << endl;

	// Print Manipulated Time Records.
	/*
	   for (int i=0;i<affectedRoadParallel.size();i++)
	   {
	   map<int, vector<vector<int>>>* timeRecords; int inTime = 5296701;
	   int RoadSegmentID = affectedRoadParallel[i];

	   int hour = time_2_hour(inTime); int index = hour_2_index(hour);
	   timeRecords = &timeSliceInsert[RoadSegmentID][index];

	   cout << endl;
	   cout << "count " << i << endl;
	   cout << "-----------------------------------------" << endl;
	   map<int, vector<vector<int>>>::iterator itrPrint;

	   cout << "TimeRecord Size is: " << timeRecords->size() << endl;
	   cout << "RoadSegmentID: " << RoadSegmentID << " with TimeSlice: " << index << endl;
	   for (itrPrint = timeRecords->begin(); itrPrint != timeRecords->end(); ++itrPrint) {
	   for (int i=0;i<itrPrint->second.size();i++)
	   {
	   cout << " time " << itrPrint->first << " routeID " << itrPrint->second[i][0];
	   cout << " status " << itrPrint->second[i][1] << " flow " << itrPrint->second[i][2] << "||";
	   }
	   }
	   cout << endl;
	   }
	   */
}


// Insertion Operation by Inserting New Route Data One by One (Parallel Version)
void Graph::one_route_parallel_update(vector<pair<pair<int,vector<int>>, int>> &newDataMulti,
		int i, int j, vector<Semaphore*>& vLock, bool parallel, bool terminal, bool range, bool print)
{
    bool prop;
    prop = true;

	for (int k = i; k < j; k++){
		// Capture New Routes in Different Groups
		pair<int, vector<int>> newRoute = newDataMulti[k].first;
		int inTime = newDataMulti[k].second;
		// Initialize A List to Store All Affected Road ID
		// "tuple" includes: <0/leaveTime, I', D'>
		vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> roadSegmentSet;
		int RoadSegmentID1st = newRoute.second[0]; 
		int count = 0;
		// Add New Inserted Route Data into List
		vector<int> affectedRoadID; affectedRoadID.clear();
		affectedRoadID.push_back(RoadSegmentID1st);

		if (print == true) {
			cout << "=======================================================================" << endl;
			cout << "count: " << count << endl;
		}

		int hour = time_2_hour(inTime); int index = hour_2_index(hour);
		// Lock TimeSlice
		vLock[RoadSegmentID1st]->Wait();
		roadSegmentSet = update_operation_1st(RoadSegmentID1st, inTime, newRoute, parallel, terminal, range, print);
		count += 1;
		vLock[RoadSegmentID1st]->Signal();

		// When List Is Not Empty
		// Store Temple Road Information
		// Value Changes During Each Iteration
		pair<int, tuple<int, map<int, vector<vector<int>>>, vector<int>>> roadSegmentTemp;
		int leaveTime;                              // Driving In Time for The Next Road
		map<int, vector<vector<int>>> insert;       // Updated Time Records to Insertion
		vector<int> deletion;                       // Odd Time Records for Deletion
		pair<int,int> roadSegmentIDTemp;
		vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> roadSegmentSetTemp;
		// When List Is Not Empty
		while(roadSegmentSet.size() != 0){
            // Add Operation Number (Number of Affected Road)
            count += 1;
			// Select The First Affected Road
			std::vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>>::iterator itr;
			itr = roadSegmentSet.begin();
            int routeID = itr->first.second;
            vector<int> affected_route = Pi[routeID];
            vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
            pair<int, vector<int>> routeProp = make_pair(routeID,affected_route_w_roadSegmentID);
			// Define ID and Information
			roadSegmentIDTemp = itr->first; 
			leaveTime = get<0>(itr->second);
			// Check If Current Affected Time Are Updated
			vector<int>::iterator itr_findDup;
			itr_findDup = std::find (affectedRoadID.begin(), affectedRoadID.end(), roadSegmentIDTemp.first);
			// If Current Road Is The Last One of New Route or Already Updated
            if (prop == true){
                if (roadSegmentIDTemp.first == -1 or itr_findDup != affectedRoadID.end()) {
                    // Remove Current Road from List and Continue
                    roadSegmentSet.erase(itr);
                    continue;
                }
            }
            else{
                if (roadSegmentIDTemp.first == -1) {
                    // Remove Current Road from List and Continue
                    roadSegmentSet.erase(itr);
                    continue;
                }
            }

			if (print == true)
			{
				cout << "=======================================================================" << endl;
				cout << endl;
				cout << "Step 1: round start" << endl;
				cout << "-----------------------------------------" << endl;
				cout << "count: " << count << endl;
				cout << "road segment ID is: " << roadSegmentIDTemp.first << " with leaveTime: " << leaveTime << endl;
			}
			// Define Insertion and Deletion Information & Print
			insert = get<1>(itr->second); 
			deletion = get<2>(itr->second);
			if (print == true){
				cout << "Insert set size is: " << insert.size() << " Deletion size is: " << deletion.size() << endl;
				map<int, vector<vector<int>>>::iterator itrInsert;
				for (itrInsert = insert.begin(); itrInsert != insert.end();++itrInsert) {
					cout << "inserted time is: " << itrInsert->first << " with size: " << itrInsert->second.size() << endl;
				}
			}
			// Remove Current Road from List
			roadSegmentSet.erase(itr);

			// Update Operation for Further Road
			vLock[roadSegmentIDTemp.first]->Wait();
			roadSegmentSetTemp = updateOperationFurther(roadSegmentIDTemp, leaveTime, newRoute, routeProp, insert, deletion, parallel, terminal, range, print);
			vLock[roadSegmentIDTemp.first]->Signal();
			// Add Affected Road ID in List
			affectedRoadID.push_back(roadSegmentIDTemp.first);
			// Add Next Affected Road Information for Next Iteration
			for (int l=0;l<roadSegmentSetTemp.size();l++) {
				roadSegmentSet.push_back(roadSegmentSetTemp[l]);
			}
		}

		if (print == true){
			cout << "This Route Update Done." << endl;
			cout << "=======================================================================" << endl;
		}
	}
}

