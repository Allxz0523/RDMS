
#include "head.h"

// Generate New Data for Deletion
// Main Idea: Capturing Existing Route as New Deletion Data
// Since Pi Contains Simulated + Inserted Routes ->
// Convert Pi (Node-based Route) into Road-based Route
void Graph::dele_data_generation(string route_file, string Pi_file, string route_index_file, int newNum)
{
    // Randomly Generate Unordered Integer as Deletion Route Data's Index
    // "Pi" Contains Estimated and Inserted Route Data Constructed by Nodes
    vector<int> randomInt = randperm(Pi.size());
    // vector<int> randomInt = randperm(2000000);
    // Correctness Check
    // Make Sure New Route Data Size Is Smaller Than Size of Existing + Inserted Routes
    if (newNum > Pi.size()){
        cout << "Error! Required deleted route data size " << newNum;
        cout << " is greater than existing " << Pi.size() << endl;
    }
    // Variable Initialization to Store Deletion Route Data Constructed by Nodes
    vector<vector<int>> routeTemp; 
	routeTemp.reserve(newNum);
    // Variable Initialization to Store Route Index
    vector<int> delRouteIndex; 
	delRouteIndex.reserve(newNum);
    // Capture Deletion Routes & Route Index in Original List
    for (int i = 0; i < newNum; i++){
        int routeIndex = randomInt[i];
        routeTemp.push_back(Pi[routeIndex]);
        delRouteIndex.push_back(routeIndex);
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
    // Write Captured Route Index Out
    ofstream outIndex;
    outIndex.open(route_index_file);
    for (int i=0;i<delRouteIndex.size();i++){
        outIndex << delRouteIndex[i] << " " << endl;
    }

    // Convert Node Constructed Route to Road Constructed Route
    vector<vector<int>> routeRoad;
    routeRoad.clear();
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

    cout << "Deletion route data generation & write done." << endl;
}

// Read Selected Deleted Route Data
void Graph::read_deletion_data(string routeRoadPathD, string routeRoadIndex)
{
    // Open Files
    ifstream IFRoute(routeRoadPathD);
    if(!IFRoute)
        cout << "Cannot open route (road) file: "<< routeRoadPathD << endl;
    // Variable Initialization
    vector<vector<int>> routeRoad; 
	routeRoad.clear();
    int nodeNum;
    // Read Route Constructed by Nodes
    vector<int> routeRoadTemp; 
    while(IFRoute >> nodeNum){
        // vector<int> routeRoadTemp; 
		routeRoadTemp.clear();
        routeRoadTemp.assign(nodeNum, 0);
        for (int j = 0; j < nodeNum; j++){
            IFRoute >> routeRoadTemp[j];
        }
        routeRoad.push_back(routeRoadTemp);
    }
    // Open Files
    ifstream IFIndex(routeRoadIndex);
    if(!IFIndex)
        cout << "Cannot open route (road) index file: "<< routeRoadIndex << endl;
    // Variable Initialization
    vector<int> routeIndexList;
	routeIndexList.clear();
    int routeIndex;
    while (IFIndex >> routeIndex){
        routeIndexList.push_back(routeIndex);
    }
    // Add Index (ID) for Generated New Routes as Input for Deletion Operation
    routeRoadDelInput.clear();
    for (int i = 0; i < routeRoad.size(); i++){
        routeRoadDelInput.push_back(make_pair(routeIndexList[i], routeRoad[i]));
    }
	/* 
    // Print
    for (int i=0;i<routeRoadDelInput.size();i++)
    {
        cout << "Route Index " << routeRoadDelInput[i].first << " with road ID: " << endl;
		cout << "D: ";
        for (int j=0;j<routeRoadDelInput[i].second.size();j++)
        {
            cout << roadID2NodeID[routeRoadDelInput[i].second[j]].first << " ";
        }
        cout << endl;
		cout << "P: ";
		for (int j=0;j<Pi[routeRoadDelInput[i].first].size();j++)
        {
            cout << Pi[routeRoadDelInput[i].first][j] << " ";
        }
		cout << endl;
		cout << "-------------------------" << endl;
    }
	*/
    
}

// Deletion Operation
void Graph::update_operation_deletion(bool parallel, bool terminal, bool range, bool print)
{
    // Update Operation by Deleting Route Data One by One
    vector<int> affectedRoadID; affectedRoadID.clear();
    // Initialize Time Slices
    timeSliceDel.clear();
    timeSliceDel = timeSliceInsert;

    std::chrono::high_resolution_clock::time_point t0, t1;
    std::chrono::duration<double> time_span1;
    t0=std::chrono::high_resolution_clock::now();

    for (int i = 0; i < routeRoadDelInput.size(); i++){
        // Deletion Operation
        affectedRoadID = one_route_update_deletion(routeRoadDelInput[i], parallel, terminal, range, print);
        /*
        // Print
        for (int j=0;j<affectedRoadID.size();j++)
        {
            map<int, vector<vector<int>>>* timeRecords; int inTime = 5296701;
            int RoadSegmentID = affectedRoadID[j];

            int hour = time_2_hour(inTime); int index = hour_2_index(hour);
            timeRecords = &timeSliceDel[RoadSegmentID][index];

            cout << endl;
            cout << "count " << j << endl;
            cout << "-----------------------------------------" << endl;
            map<int, vector<vector<int>>>::iterator itrPrint;

            cout << "TimeRecord Size is: " << timeRecords->size() << endl;
            cout << "RoadSegmentID: " << RoadSegmentID << " with timeSliceDel: " << index << endl;
            for (itrPrint = timeRecords->begin(); itrPrint != timeRecords->end(); ++itrPrint) {
                for (int k=0;k<itrPrint->second.size();k++)
                {
                    cout << " time " << itrPrint->first << " routeID " << itrPrint->second[k][0];
                    cout << " status " << itrPrint->second[k][1] << " flow " << itrPrint->second[k][2] << "||";
                }
            }
            cout << endl;
        }*/
    }
    t1=std::chrono::high_resolution_clock::now();
    time_span1 = std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0);
    // Print
    if (terminal == true)
        cout << "Deletion operation's time consumption w termination is: " << time_span1.count() << "s." << endl;
    else
        cout << "Deletion operation's time consumption w/o termination is: " << time_span1.count() << "s." << endl;
}

// Deletion Operation by deleting Selected Route Data One by One
// "delRoute" Contains: Route ID + Route Constructed by Roads
vector<int> Graph::one_route_update_deletion(pair<int, vector<int>> delRoute, bool parallel, bool terminal, bool range, bool print)
{
    bool prop;
    prop = true;

    // Initialize The Number of Manipulated Road Segments
    int count = 0;
    // Initialize A List to Store All Affected Road ID
    // "tuple" includes: <0/leaveTime, I', D'>
    vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> roadSegmentSet;
    int RoadSegmentID1st = delRoute.second[0];
    int routeID = delRoute.first;
    vector<int> route = delRoute.second;
    // Add Deleted Route Data into List
    vector<int> affectedRoadID;
    affectedRoadID.clear();
    affectedRoadID.push_back(RoadSegmentID1st);

    // Capture Driving in Time for Current Road Segment
    int nodeID1 = roadID2NodeID[RoadSegmentID1st].first;
    int nodeID1Index;
    int nodeID2 = roadID2NodeID[RoadSegmentID1st].second;
    int nodeCurrent;
    int nodeNext;
    int routeSize = ETA_result[routeID].size();
    // Find NodeID1 Index
    nodeID1Index = INF;
    for (int i=0;i<ETA_result[routeID].size() - 1;i++){
		// nodeID1Index = INF;
        nodeCurrent = ETA_result[routeID][i].first;
		if (nodeCurrent == nodeID1){
			nodeNext = ETA_result[routeID][i+1].first;
			if (nodeNext == nodeID2){
				nodeID1Index = i;
				break;
			}
		}
    }
    if (nodeID1Index == INF){
        cout << "nodeID1: " << nodeID1 << endl;
        cout << "nodeID2: " << nodeID2 << endl;
        cout << "routeID: " << routeID << " with nodes ";

        for (int i=0;i<ETA_result[routeID].size();i++){
            cout << ETA_result[routeID][i].first << " ";
        }
        cout << endl;

        for (int i=0;i<Pi[routeID].size();i++){
            cout << Pi[routeID][i] << " ";
        }
        cout << endl;
        cout << "nodeID1Index cannot find" << endl;
    }
	
    // Find Driving in Time
    int inTime = ETA_result[routeID][nodeID1Index].second;

    if (print == true){
        cout << "=======================================================================" << endl;
        cout << "count: " << count << endl;
    }

    roadSegmentSet = deletion_operation_1st(RoadSegmentID1st, inTime, delRoute, parallel, terminal, range, print);

    // When List Is Not Empty
    // Store Temple Road Information
    // Value Changes During Each Iteration
    pair<int, tuple<int, map<int, vector<vector<int>>>, vector<int>>> roadSegmentTemp;
    map<int, vector<vector<int>>> insert;       // Updated Time Records to Insertion
    vector<int> deletion;                       // Only Route ID of Time Records for Deletion
    pair<int,int> roadSegmentIDTemp;
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

        nodeID1 = roadID2NodeID[roadSegmentIDTemp.first].first;
        nodeID2 = roadID2NodeID[roadSegmentIDTemp.first].second;
        // Find NodeID1 Index
        for (int i=0;i<ETA_result[routeID].size();i++){
			nodeID1Index = INF;
            if (i + 1 > routeSize){
                continue;
            }
            else{
                nodeCurrent = ETA_result[routeID][i].first;
                nodeNext = ETA_result[routeID][i+1].first;
            }
            if (nodeCurrent == nodeID1 and nodeNext == nodeID2){
                nodeID1Index = i;
				break;
            }
        }
		if (nodeID1Index == INF){
			nodeID1Index = 0;
		}
	
		// Find Driving in Time
        int inTime = ETA_result[routeID][nodeID1Index].second;

		// Check If Current Affected Time Are Updated
        vector<int>::iterator itr_findDup;
        itr_findDup = std::find (affectedRoadID.begin(), affectedRoadID.end(), roadSegmentIDTemp.first);
        // If Current Road Is The Last One of New Route or Already Updated
        if (prop == true){
            if (roadSegmentIDTemp.first == -1 or itr_findDup != affectedRoadID.end()){
                // Remove Current Road from List and Continue
                roadSegmentSet.erase(itr);
                continue;
            }
        }
        else{
            if (roadSegmentIDTemp.first == -1){
                // Remove Current Road from List and Continue
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
            cout << "road segment ID is: " << roadSegmentIDTemp.first << " with leaveTime: " << inTime << endl;
        }
        // Define Insertion and Deletion Information & Print
        insert = get<1>(itr->second);
        deletion = get<2>(itr->second);
        if (print == true){
            cout << "Insert set size is: " << insert.size() << " Deletion size is: " << deletion.size() << endl;
            map<int, vector<vector<int>>>::iterator itrInsert;
            for (itrInsert = insert.begin(); itrInsert != insert.end();++itrInsert){
                cout << "inserted time is: " << itrInsert->first << " with size: " << itrInsert->second.size() << endl;
            }
        }
        // Remove Current Road from List
        roadSegmentSet.erase(itr);

        // Update Operation for Further Road
        roadSegmentSetTemp = update_operation_deletion_further(roadSegmentIDTemp, inTime, delRoute, routeProp, insert, deletion, parallel, terminal, range, print);

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

// Deletion Operation for Different Road Has Different Operation
// This Is Deletion Operation for The First Road of A Route
vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> Graph::deletion_operation_1st(
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
    insertList.insert(pair<int, vector<vector<int>>>(0, {{0, 0, 0}}));
    // Capture New Route ID and New Route from Argument
    int newRouteID = newRoutePair.first;
    vector<int> newRoute = newRoutePair.second;
    // Initialize Returned Values
    // "Null" If Current Road Is The Last One of This Route
    // Otherwise, Return Generated Information on The Current Road to The Next One
    vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> Null;
    vector<int> roadAffectedID;
    int NewTimeLeave;
    vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> roadSegmentAffected;

    // Step 2: Insertion Operation
    // -------------------------------------------

    // Step 2.1: Target Time Records
    // -------------------------------------------
    int hour = time_2_hour(inTime);
    int index = hour_2_index(hour);
    map<int, vector<vector<int>>>* timeRecords = &timeSliceDel[RoadSegmentID][index];
    // Generate Flow Base for Travel Time Estimation
    int current_node = roadID2NodeID[RoadSegmentID].first;
    int next_node = roadID2NodeID[RoadSegmentID].second;
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

    // Step 2.2: Estimate New Time Record's Travel Time
    // -------------------------------------------
    // Variable Initialization
    int preFlow = 0;
    trafficFlow += preFlow; trafficFlowInsert += preFlow + 1;
    tuple<int, map<int, vector<vector<int>>>, vector<int>> tempReturn;
    int nextRoadSegmentID, routeID;
    nextRoadSegmentID = find_next_roadID(newRoutePair.second, RoadSegmentID);

    // Define Min Travel Time and Estimated Travel Time
    int tm = nodeID2minTime[make_pair(current_node, next_node)];
    int te = 0;

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
            tuple<int, map<int, vector<vector<int>>>, vector<int>> tempReturn;
            get<0>(tempReturn) = inTime;
            roadSegmentAffected.push_back(make_pair(make_pair(iNextRoadID,routeID),tempReturn));
            return roadSegmentAffected;
        }
    }

    // Step 3: Update Operation
    // -------------------------------------------
	map<int, vector<vector<int>>>::iterator itrInser;
    itrInser = timeRecords->find(inTime);
    // Correctness Check
    if (itrInser == timeRecords->end() and inTime != 0){
		return Null;
    }

    // Step 3.1: For Each Time Record Start From New's Next (Core Design).
    // -------------------------------------------
    int timeCurrent = 0;
	int RoadIDCurrent;
    map<int, vector<vector<int>>>::iterator itr;
    // Check If Traffic Flow Change from New's Next to The Last One
    // for (itr = timeRecords->begin(); itr != timeRecords->end();)
	for (itr = itrInser; itr != timeRecords->end();){
        bool bBreak = false;        // Continues to Next Iteration
        bool bBreak2 = false;       // Break Iteration

        int dynamicSize = itr->second.size();
        for (int i = 0; i < dynamicSize; i++){
            timeCurrent = itr->first;
            RoadIDCurrent = itr->second[i][0];

            // Step 3.2: If Current Route ID Is What We Want to Delete
            if (RoadIDCurrent == newRouteID){
                if (itr->second[i][1] == 1){
                    // Update Traffic Flow
                    trafficFlow += 1;
                    // Remove Current Time Record from Time Records
                    itr->second.erase(itr->second.begin() + i);
                    i--;
					dynamicSize = itr->second.size();
                    // If Time (Key) Is Empty after Deletion Operation
                    // Remove Time (Key) from Time Records abd Stop Iteration in Current Time (Key)
                    if (timeRecords->size() == 0){
                        bBreak2 = true;
                        break;
                    }
                    if(itr->second.size() == 0){
                        itr = timeRecords->erase(itr);
                        bBreak = true;
                        break;
                    }
					continue;
                }
                else{
                    // Update Traffic Flow
                    trafficFlow -= 1;
                    // Remove Current Time Record from Time Records
                    itr->second.erase(itr->second.begin() + i);
                    i--;
					dynamicSize = itr->second.size();
                    // If Time (Key) Is Empty after Deletion Operation
                    // Remove Time (Key) from Time Records abd Stop Iteration in Current Time (Key)
                    if (timeRecords->size() == 0){
                        bBreak2 = true;
                        break;
                    }
                    if(itr->second.size() == 0){
                        itr = timeRecords->erase(itr);
                        bBreak = true;
                        break;
                    }
					continue;
                }
            }

            // Step 3.3: Compare Time Records in Insert Set
            // -------------------------------------------
            int RouteIDI;
            map<int, vector<vector<int>>>::iterator itrI;
            for (itrI = ++insertList.begin(); itrI != insertList.end();){
                int timeI = itrI->first;
                if (timeI < timeCurrent){
                    trafficFlowInsert -= 1;
                    RouteIDI = itrI->second[i][0];
                    // If Inserted Time Record Had Driving In Status
                    // Inserted Time Records Should Also Be Driving In Status
                    if (itrI->second[i][1] == 1){   // Status: In
                        if (timeRecords->find(timeI) == timeRecords->end()){
                            timeRecords->insert(pair<int,
                                    vector<vector<int>>>(timeI, {{RouteIDI, 1, trafficFlowInsert}}));
                        }
                        else{
                            (*timeRecords)[timeI].push_back({RouteIDI, 1, trafficFlowInsert});
                        }
                    }
                    // Otherwise, Inserted Time Records Should Also Be Leaving Status
                    else{                         // Status Out
                        if (timeRecords->find(timeI) == timeRecords->end()){
                            timeRecords->insert(pair<int,
                                    vector<vector<int>>>(timeI, {{RouteIDI, 0, trafficFlowInsert}}));
                        }
                        else{
                            (*timeRecords)[timeI].push_back({RouteIDI, 0, trafficFlowInsert});
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
                    if (itr == timeRecords->end()){
                        break;
                    }
                    // If Terminal Condition Is Active but Still Not Reach The Record Time
                    if (timeCurrent <= recordTime){
                        if (trafficFlow != trafficFlowInsert){
                            // Check If Traffic Flows Are in Same Range
                            bool sameRange = flow_same_range_check(current_node,
                                                                   nextNodeIndex, trafficFlow, trafficFlowInsert);
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
                                // teCurrent = tm * (1 + sigma * pow(trafficFlowInsert / varphi, beta));
                                if (range == false){
                                    teCurrent = tm * (1 + sigma * pow(trafficFlowInsert / varphi, beta));
                                }
                                else{
                                    teCurrent = flow2time_by_range(current_node, nextNodeIndex, trafficFlowInsert);
                                }

                                // Add Estimated Travel Time to Insert List for Further Insertion Operation
                                if (insertList.find(timeCurrent + teCurrent) == insertList.end()){
                                    insertList.insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                                     {{RoadIDCurrent, 0, 0}}));
                                } else{
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
                                        get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                                                 {{RoadIDCurrent, 1, 0}}));
                                    } else{
                                        get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
                                    }
                                    get<2>(tempReturn).push_back(routeID);
                                    // If Road Belongs to New Route
                                    if (itrFindNew != newRoute.end()){
                                        get<0>(tempReturn) = NewTimeLeave;
                                        roadSegmentAffected[position].second = tempReturn;
                                    } else{
                                        get<0>(tempReturn) = 0;
                                        roadSegmentAffected[position].second = tempReturn;
                                    }
                                }
                                else{
                                    if (itrFindNew != newRoute.end()){
                                        get<0>(tempReturn) = NewTimeLeave;
                                        get<1>(tempReturn).clear();
                                        if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
                                            get<1>(tempReturn).insert(
                                                    pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                                   {{RoadIDCurrent, 1, 0}}));
                                        }
                                        else{
                                            get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
                                        }
                                        get<2>(tempReturn).clear();
                                        get<2>(tempReturn).push_back(routeID);
                                        roadSegmentAffected.push_back(make_pair(make_pair(nextRoadSegmentID,routeID), tempReturn));
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
                                        get<2>(tempReturn).push_back(routeID);
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
						// if (terminal == true and insertList.size() == 0 and trafficFlowInsert == 0)
                        if (terminal == true and insertList.size() == 0){
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
                    bool sameRange = flow_same_range_check(current_node,
                                                           nextNodeIndex, trafficFlow, trafficFlowInsert);
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
                            insertList.insert(
                                    pair<int, vector<vector<int>>>((timeCurrent + teCurrent), {{RoadIDCurrent, 0, 0}}));
                        }
                        else{
                            insertList[timeCurrent + teCurrent].push_back({RoadIDCurrent, 0, 0});
                        }
                        deletionList.push_back(routeID);

                        vector<int> affected_route = Pi[routeID];
                        vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
                        int unaffected_next_roadSegmentID = find_next_roadID(affected_route_w_roadSegmentID,
                                                                             RoadSegmentID);
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
                        itFindExit = std::find(roadAffectedID.begin(),
                                               roadAffectedID.end(), unaffected_next_roadSegmentID);
                        if (itFindExit != roadAffectedID.end()){
                            int position = itFindExit - roadAffectedID.begin();
                            tempReturn = roadSegmentAffected[position].second;
                            if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end()){
                                get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                                         {{RoadIDCurrent, 1, 0}}));
                            }
                            else{
                                get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
                            }
                            get<2>(tempReturn).push_back(routeID);
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
                                get<2>(tempReturn).push_back(routeID);
                                roadSegmentAffected.push_back(make_pair(make_pair(nextRoadSegmentID,routeID), tempReturn));
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
                                get<2>(tempReturn).push_back(routeID);
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
                                itr = timeRecords->erase(itr);
                                if (timeRecords->size() == 0){
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
    map<int, vector<vector<int>>>::iterator itrEnd;
    if (timeRecords->size() != 0){
        itrEnd = --timeRecords->end();
        int FlowEnd = itrEnd->second.back()[2];
        int timeRest, RouteIDRest, FlowRest;
        map<int, vector<vector<int>>>::iterator itrRestI;

        for (itrRestI = ++insertList.begin(); itrRestI != insertList.end(); ++itrRestI){
            for (int i=0;i<itrRestI->second.size();i++){
                timeRest = itrRestI->first; RouteIDRest = itrRestI->second[i][0];
                FlowRest = FlowEnd - 1;
                if (timeRecords->find(timeRest) == timeRecords->end()){
                    timeRecords->insert(pair<int, vector<vector<int>>>(timeRest, {{RouteIDRest, 0, FlowRest}}));
                }
                else{
                    (*timeRecords)[timeRest].push_back({RouteIDRest, 0, FlowRest});
                }
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
    /* timeSliceDel[RoadSegmentID][index] = timeRecords; */

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
        roadSegmentAffected.push_back(make_pair(make_pair(nextRoadSegmentID, 0), fillReturn));
    }

    // Step 6: Print Further Affected Road Segment
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

//  This Is Deletion Operation for The Further Road of New Route or Propagated Road
vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> Graph::update_operation_deletion_further(
        pair<int,int> &RoadSegmentID, int &inTime, pair<int, vector<int>> &newRoutePair, pair<int, vector<int>> &routeProp,
        map<int, vector<vector<int>>> &InsertPre, vector<int> &DeletionPre, bool &parallel, bool &terminal, bool &range, bool print){

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
    // Capture New Route ID and New Route from Argument
    int newRouteID = newRoutePair.first; vector<int> newRoute = newRoutePair.second;
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
    int NewTimeLeave; map<int, vector<vector<int>>>* timeRecords;
    int tm = 0;
	int currentNode, nextNode;
	int te = 0;
	int index;
    // Define Nodes of Current Road
    currentNode = roadID2NodeID[RoadSegmentID.first].first;

    int routeIDTemp = newRoutePair.first;
    nextNode = roadID2NodeID[RoadSegmentID.first].second;
    int nextNodeIndex;
    for (int i=0;i<timeRange[currentNode].size();i++){
        if(timeRange[currentNode][i].first == nextNode)
        {
            nextNodeIndex = i;
        }
    }
    // Define Road's Minimal Travel Time
    tm = nodeID2minTime[make_pair(currentNode, nextNode)];

    // If Current Road Not Belongs to New Route
    if (inTime != 0){
        // Step 2.1: Find Time Records
        // -------------------------------------------
        int hour = time_2_hour(inTime); index = hour_2_index(hour);
        timeRecords = &timeSliceDel[RoadSegmentID.first][index];
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
            cout << "TimeRecord Size is: " << timeRecords->size() << endl;
            cout << "RoadSegmentID: " << RoadSegmentID.first << " with TimeSlice: " << index << endl;
            for (itrPrintV1 = timeRecords->begin(); itrPrintV1 != timeRecords->end(); ++itrPrintV1){
                for (int i=0;i<itrPrintV1->second.size();i++){
                    cout << " time " << itrPrintV1->first << " routeID " << itrPrintV1->second[i][0];
                    cout << " status " << itrPrintV1->second[i][1] << " flow " << itrPrintV1->second[i][2] << "||";
                }
            }
            cout << endl;
        }

        // Step 2.2: Find Time Value or Insert
        // -------------------------------------------

        // Step 2.3: Estimate New Time Record's Travel Time
        // -------------------------------------------
        // Variable Initialization
        int preFlow = 0;
		map<int, vector<vector<int>>>::iterator itPre;
        itPre = timeRecords->find(inTime);
        if (itPre != timeRecords->end()){
            // If Time Record is The First One
            if (itPre == timeRecords->begin()){
                preFlow = 0;
            }
            else{
                itPre = --itPre;
                preFlow = itPre->second.back()[2];
            }
        }
        // If Time Record Not Find, It Must Be Error Since We Just Insert It
        else{
            preFlow = 0;
        }
		if (preFlow == 0){
            trafficFlow += preFlow;
            trafficFlowInsert += preFlow + 1;
        }
        else{
            trafficFlow = preFlow;
            trafficFlowInsert = preFlow + 1;
        }
    }
    // If Current Time Record Does Not Belong to New Route
    else{
        std::map<int, vector<vector<int>>>::iterator itr_find1st;
        if (InsertPre.size() != 0){
            itr_find1st = InsertPre.begin();
            int hour = time_2_hour(itr_find1st->first); index = hour_2_index(hour);
            timeRecords = &timeSliceDel[RoadSegmentID.first][index];
            NewTimeLeave = 0;
            // Generate Flow Base for Travel Time Estimation
            pair<int, int> flowBaseIndex = time_to_base_index(itr_find1st->first, minRange);   // Hour Index, Minutes Index
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
                cout << "TimeRecord Size is: " << timeRecords->size() << endl;
                cout << "RoadSegmentID: " << RoadSegmentID.first << " with TimeSlice: " << index << endl;
                for (itrPrintV1 = timeRecords->begin(); itrPrintV1 != timeRecords->end(); ++itrPrintV1){
                    for (int i=0;i<itrPrintV1->second.size();i++){
                        cout << " time " << itrPrintV1->first << " routeID " << itrPrintV1->second[i][0];
                        cout << " status " << itrPrintV1->second[i][1] << " flow " << itrPrintV1->second[i][2] << "||";
                    }
                }
                cout << endl;
            }
        }
        else{
            if (print == true){
                cout << "I' size equals to 0." << endl;
            }
			return Null;
        }
    }

    
    // If Min Travel Time Is Smaller Than 1
	// No Records Are Affected -> Directly Process The Next One
	// However, It Needs to Add The Not Inserted Time Record,
	// Which May Not Decrease The Time Consumption A Lot
	if (tm < 1){
	    int iCurrentRoad = RoadSegmentID.first;
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
    // For Each Time Record
    // Variable Initialization
    int timeCurrent = 0; 
	int RoadIDCurrent;
	int teCurrent = 0; 
	int RouteIDIPre, nextRoadSegmentID;
    tuple<int, map<int, vector<vector<int>>>, vector<int>> tempReturn;
    // Find Next Road ID
    // nextRoadSegmentID = find_next_roadID(newRoutePair.second, RoadSegmentID.first);
    if (inTime == 0){
        nextRoadSegmentID = find_next_roadID(routeProp.second, RoadSegmentID.first);
    }
    else{
        nextRoadSegmentID = find_next_roadID(newRoutePair.second, RoadSegmentID.first);
    }

	// Check New Time Record is Last One
    map<int, vector<vector<int>>>::iterator itrInser;
    if (inTime != 0){
        itrInser = timeRecords->find(inTime);
		if (itrInser == timeRecords->end()){			
            itrInser = timeRecords->begin();
		}
	}
    else{
        itrInser = timeRecords->begin();
	}

	// Check If Traffic Flow Change from Each Time Record
    map<int, vector<vector<int>>>::iterator itr;
    // for (itr = timeRecords->begin(); itr != timeRecords->end();)
	for (itr = itrInser; itr != timeRecords->end();){
        // Define Break Variable to Stop Iteration Further
        bool bBreak = false;        // Continues to Next Iteration
        bool bBreak2 = false;       // Break Iteration
        // Each Time Contain One or More Time Records
        // For Each Time Record (Value) Under A Time (Key)
        int dynamicSize = itr->second.size();
        for (int i = 0; i < dynamicSize; i++){
            // Variable Initialization
            timeCurrent = itr->first;
            RoadIDCurrent = itr->second[i][0];
            int temp = trafficFlowInsert;
            // Number of Time Records Under Current Time
            int temp_size = itr->second.size();
            int insetionPre_check = 0;

            // Step 3.1: If Current Route ID Is What We Want to Delete
            if (RoadIDCurrent == newRouteID){
                if (itr->second[i][1] == 1){
                    // Update Traffic Flow
                    trafficFlow += 1;
                    // Remove Current Time Record from Time Records
                    itr->second.erase(itr->second.begin() + i);
                    i--;
					dynamicSize = itr->second.size();
                    // If Time (Key) Is Empty after Deletion Operation
                    // Remove Time (Key) from Time Records abd Stop Iteration in Current Time (Key)
                    if (timeRecords->size() == 0){
                        bBreak2 = true;
                        break;
                    }
                    if(itr->second.size() == 0){
                        itr = timeRecords->erase(itr);
                        bBreak = true;
                        break;
                    }
					continue;
                }
                else{
                    // Update Traffic Flow
                    trafficFlow -= 1;
                    // Remove Current Time Record from Time Records
                    itr->second.erase(itr->second.begin() + i);
                    i--;
					dynamicSize = itr->second.size();
                    // If Time (Key) Is Empty after Deletion Operation
                    // Remove Time (Key) from Time Records abd Stop Iteration in Current Time (Key)
                    if (timeRecords->size() == 0){
                        bBreak2 = true;
                        break;
                    }
                    if(itr->second.size() == 0){
                        itr = timeRecords->erase(itr);
                        bBreak = true;
                        break;
                    }
					continue;
                }
            }

            // Insert Time Records In "I'" Set
            map<int, vector<vector<int>>>::iterator itrInPre; int RouteIDInPre;
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
                            for (int j = 0; j < itrI0->second.size(); j++){
                                // "I" Set Only Contains Time Records with Leaving Status
                                trafficFlowInsert -= 1; RouteIDI0 = itrI0->second[j][0];
                                // If Time Record Is Driving in Status
                                if (itrI0->second[j][1] == 1){
                                    cout << "Error. element in I is not leaving status." << endl;
                                }
                                else{
                                    // Time (Key) Not Exist -> Insert One with Time Record as Value
                                    if (timeRecords->find(timeI0) == timeRecords->end())
                                        timeRecords->insert(pair<int, vector<vector<int>>>(timeI0,
                                                                                          {{RouteIDI0, 0, trafficFlowInsert}}));
                                        // Push Back Time Record (Value) after Time (Key)
                                    else
                                        (*timeRecords)[timeI0].push_back({RouteIDI0, 0, trafficFlowInsert});
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
                    int inside_time_current, inside_roadID_current;
                    for (int j = 0; j < itrInPre->second.size(); j++){
                        // "I'" Set Only Contains Time Records with Leaving Status
                        trafficFlowInsert += 1;

                        RouteIDInPre = itrInPre->second[j][0];
                        // If Time Record Is Driving in Status
                        if (itrInPre->second[j][1] == 1){
                            if (timeRecords->find(timeIPre) == timeRecords->end())
                                timeRecords->insert(pair<int, vector<vector<int>>>(timeIPre,
                                                                                  {{RouteIDInPre, 1, trafficFlowInsert}}));
                            else
                                (*timeRecords)[timeIPre].push_back({RouteIDInPre, 1, trafficFlowInsert});
                        }
                        else{
                            cout << "Error. element in I' is not driving in status." << endl;
                        }
                        inside_time_current = itrInPre->first; inside_roadID_current = itrInPre->second[j][0];
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
                        deletionList.push_back(inside_roadID_current);
                        // Find Next Road ID
                        int routeID =  itrInPre->second[j][0];
                        int unaffected_next_roadSegmentID;
                        if (routeID == newRouteID){
                            unaffected_next_roadSegmentID = nextRoadSegmentID;
                        }
                        else{
                            vector<int> affected_route = Pi[routeID];
                            vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
                            unaffected_next_roadSegmentID = find_next_roadID(affected_route_w_roadSegmentID, RoadSegmentID.first);
                        }
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
                        itFindExit = std::find(roadSegmentAffectedID.begin(),
                                               roadSegmentAffectedID.end(), unaffected_next_roadSegmentID);
                        // If Already Added
                        // Because Time Records Are Delayed, Driving in Time in The Next Road Should Change
                        // Code Related to "get" Mainly Return Information Related to The Next Road
                        if (itFindExit != roadSegmentAffectedID.end()){
                            int position = itFindExit - roadSegmentAffectedID.begin();
                            tempReturn = roadSegmentAffected[position].second;
                            if (get<1>(tempReturn).find(inside_time_current + teCurrent) == get<1>(tempReturn).end())
                                get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((inside_time_current + teCurrent),
                                                                                         {{inside_roadID_current, 1, 0}}));
                            else
                                get<1>(tempReturn)[inside_time_current + teCurrent].push_back({inside_roadID_current, 1, 0});
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
                                if (get<1>(tempReturn).find(inside_time_current + teCurrent) == get<1>(tempReturn).end())
                                    get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((inside_time_current + teCurrent),
                                                                                             {{inside_roadID_current, 1, 0}}));
                                else
                                    get<1>(tempReturn)[inside_time_current + teCurrent].push_back({inside_roadID_current, 1, 0});

                                get<2>(tempReturn).clear();get<2>(tempReturn).push_back(inside_roadID_current);
                                roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,routeID), tempReturn));
                            }
                            else{
                                get<0>(tempReturn) = 0;
                                get<1>(tempReturn).clear();
                                if (get<1>(tempReturn).find(inside_time_current + teCurrent) == get<1>(tempReturn).end())
                                    get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((inside_time_current + teCurrent),
                                                                                             {{inside_roadID_current, 1, 0}}));
                                else
                                    get<1>(tempReturn)[inside_time_current + teCurrent].push_back({inside_roadID_current, 1, 0});
                                get<2>(tempReturn).clear();
                                get<2>(tempReturn).push_back(inside_roadID_current);
                                roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,routeID), tempReturn));
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

            // Step 4: Compare Time Records in Insert Set
            // -------------------------------------------

            // Step 4.1: Compare Time Records in Insert Set
            // -------------------------------------------
            map<int, vector<vector<int>>>::iterator itrI; int RouteIDI;
            for (itrI = insertList.begin(); itrI != insertList.end();){
                int timeI = itrI->first;
                if (timeI < timeCurrent){
                    for (int j = 0; j < itrI->second.size(); j++){
                        // Compare Each Time Record in Insert Set.
                        trafficFlowInsert -= 1; RouteIDI = itrI->second[j][0];
                        // If Inserted Time Record Had Driving In Status
                        // Inserted Time Records Should Also Be Driving In Status
                        if (itrI->second[j][1] == 1){
                            cout << "Error. element in I is not leaving status." << endl;
                        }
                        else{
                            if (timeRecords->find(timeI) == timeRecords->end())
                                timeRecords->insert(pair<int, vector<vector<int>>>(timeI,
                                                                                  {{RouteIDI, 0, trafficFlowInsert}}));
                            else
                                (*timeRecords)[timeI].push_back({RouteIDI, 0, trafficFlowInsert});
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
            if (inTime == 0 and InsertPre.size() == 0 and DeletionPre.size() == 0 and insertList.size() == 0 and deletionList.size() == 0){
                bBreak2 = true;
                break;
            }

            // Step 4.2: Iteration of Update Operation
            // -------------------------------------------
            // If Time Record is Driving In Status.
            map<int, vector<vector<int>>>::iterator itrIPre;

            if (itr->second[i][1] == 1){
                // If No Values Need to Insert or Delete and Terminal Condition Is Active
                // Stop Iteration
                if (terminalCondition == 1 and InsertPre.size() == 0 and DeletionPre.size() == 0 and insertList.size() == 0 and deletionList.size() == 0){
					// if (terminal == true and insertList.size() == 0 and trafficFlowInsert == 0)
                    if (terminal == true and insertList.size() == 0){
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
                            itr = timeRecords->erase(itr);
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
					
                    if (itr == timeRecords->end() and i == itr->second.size()){
                        if (terminal == true and insertList.size() == 0){
                            bBreak2 = true;
                            break;
                        }
                    }

                    // If Terminal Condition Is Active but Still Not Reach The Record Time
                    if (timeCurrent <= recordTime){
                        if (trafficFlow != trafficFlowInsert){
                            // Check If Traffic Flows Are in Same Range
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
                                // teCurrent = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
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
                                int routeID =  itr->second[i][0];
                                int unaffected_next_roadSegmentID;
                                if (routeID == newRouteID){
                                    unaffected_next_roadSegmentID = nextRoadSegmentID;
                                }
                                else{
                                    vector<int> affected_route = Pi[routeID];
                                    vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
                                    unaffected_next_roadSegmentID = find_next_roadID(affected_route_w_roadSegmentID, RoadSegmentID.first);
                                }
                                // Decide If Next Road Belongs to Target Route for Different Operation
                                std::vector<int>::iterator itrFindNew;
                                itrFindNew = std::find (newRoute.begin(), newRoute.end(), unaffected_next_roadSegmentID);
                                // Find If Next Road Belongs to Affected Road
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
                                        get<2>(tempReturn).clear();get<2>(tempReturn).push_back(RoadIDCurrent);
                                        roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,routeID), tempReturn));
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
                                        roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,routeID), tempReturn));
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
							// if (terminal == true and insertList.size() == 0 and trafficFlowInsert == 0)
                            if (terminal == true and insertList.size() == 0){
								bBreak2 = true;
                                break;
                            }
                        }
                        // If "I'" & "D'" Still Have Value, Keep Iteration Until They Are Empty
                        trafficFlow += 1; trafficFlowInsert += 1;
                        itr->second[i][2] = trafficFlowInsert;
                    }
                }
                // If Terminal Condition Is Not Active
                else{
                    trafficFlow += 1;
                    trafficFlowInsert += 1;
                    // Check If Flows Belongs to The Same Range
                    // Check If Traffic Flows Are in Same Range
                    bool sameRange = flow_same_range_check(currentNode,
                                                           nextNodeIndex, trafficFlow, trafficFlowInsert);
                    if (sameRange == true){
                        // Active Terminal Condition
                        itr->second[i][2] = trafficFlowInsert;
						terminalCondition = 1;
                        // Estimate Record Time (Decide If No Further Affect)
                        // teCurrent = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
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
                        // teCurrent = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
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

                        int routeID =  itr->second[i][0];
                        int unaffected_next_roadSegmentID;
                        if (routeID == newRouteID){
                            unaffected_next_roadSegmentID = nextRoadSegmentID;
                        }
                        else{
                            vector<int> affected_route = Pi[routeID];
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
                        itFindExit = std::find(roadSegmentAffectedID.begin(),
                                               roadSegmentAffectedID.end(), unaffected_next_roadSegmentID);

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

                                get<2>(tempReturn).clear();get<2>(tempReturn).push_back(RoadIDCurrent);
                                roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,routeID), tempReturn));
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
                                roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,routeID), tempReturn));
                            }
                        }
                    }
                }
            }
            else{
                if (DeletionPre.size() != 0 or inTime != 0 or deletionList.size() != 0){
                    if (deletionList.size() != 0){
                        int deleID;
                        for (int l=0;l<deletionList.size();l++) {
                            deleID = deletionList[l];
                            if (RoadIDCurrent == deleID) {
                                trafficFlow -= 1;
                                itr->second.erase(itr->second.begin() + i);
                                dynamicSize = itr->second.size();
                                i--;
                                vector<int>::iterator k = deletionList.begin() + l;
                                deletionList.erase(k);

                                if (itr->second.size() == 0) {
                                    itr = timeRecords->erase(itr);
                                    if (timeRecords->size() == 0) {
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
                            trafficFlowInsert -= 1; RouteIDI = itrI2->second[j][0];
                            // Status In
                            if (itrI2->second[j][1] == 1){
                                cout << "Error. element in I is not leaving status." << endl;
                            }
                            // Status Out
                            else{
                                if (timeRecords->find(timeI2) == timeRecords->end()){
                                    timeRecords->insert(
                                            pair<int, vector<vector<int>>>(timeI2, {{RouteIDI, 0, trafficFlowInsert}}));
                                }
                                else{
                                    (*timeRecords)[timeI2].push_back({RouteIDI, 0, trafficFlowInsert});
                                }
                            }
                        }
                        itrI2 = insertList.erase(itrI2);
                    }
                    else{
                        ++itrI2;
                    }
                }
                trafficFlowInsert += 1; RouteIDInPre = itrInPreRest->second[i][0];
                if (timeRecords->find(timeIPre) == timeRecords->end())
                    timeRecords->insert(pair<int, vector<vector<int>>>(timeIPre, {{RouteIDInPre,1,trafficFlowInsert}}));
                else
                    (*timeRecords)[timeIPre].push_back({RouteIDInPre,1,trafficFlowInsert});
                int timeCurrent = itrInPreRest->first;
                int RoadIDCurrent = itrInPreRest->second[i][0];
                // Estimate Target Time Record's Leaving Time
                if (range == false)
                    teCurrent = tm * (1 + sigma * pow(trafficFlowInsert/varphi, beta));
                else
                    teCurrent = flow2time_by_range(currentNode, nextNodeIndex, trafficFlowInsert);

                if (insertList.find(timeCurrent + teCurrent) == insertList.end())
                    insertList.insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                     {{RoadIDCurrent, 0, 0}}));
                else
                    insertList[timeCurrent + teCurrent].push_back({RoadIDCurrent, 0, 0});

                deletionList.push_back(RoadIDCurrent);
                // Find Next Road ID
                int routeID =  itrInPreRest->second[i][0];
                int unaffected_next_roadSegmentID;
                if (routeID == newRouteID){
                    unaffected_next_roadSegmentID = nextRoadSegmentID;
                }
                else{
                    vector<int> affected_route = Pi[routeID];
                    vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
                    unaffected_next_roadSegmentID = find_next_roadID(affected_route_w_roadSegmentID, RoadSegmentID.first);
                }

                std::vector<int>::iterator itrFindNew;
                itrFindNew = std::find(newRoute.begin(), newRoute.end(), unaffected_next_roadSegmentID);

                // Find If Next Road Belongs to Affected Road
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
                        get<2>(tempReturn).clear();get<2>(tempReturn).push_back(RoadIDCurrent);
                        roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,routeID), tempReturn));
                    }
                    else{
                        get<0>(tempReturn) = 0;
                        get<1>(tempReturn).clear();
                        if (get<1>(tempReturn).find(timeCurrent + teCurrent) == get<1>(tempReturn).end())
                            get<1>(tempReturn).insert(pair<int, vector<vector<int>>>((timeCurrent + teCurrent),
                                                                                     {{RoadIDCurrent, 1, 0}}));
                        else
                            get<1>(tempReturn)[timeCurrent + teCurrent].push_back({RoadIDCurrent, 1, 0});
                        get<2>(tempReturn).clear();get<2>(tempReturn).push_back(RoadIDCurrent);
                        roadSegmentAffected.push_back(make_pair(make_pair(unaffected_next_roadSegmentID,routeID), tempReturn));
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
        if (timeRecords->size() != 0){
            itrend = --timeRecords->end();
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
                        timeRecords->erase(itrend);
                        break;
                    }
                }
                else{
                    i++;
                    continue;
                }
            }
        }
    }

    // Rest Insert Elements
    map<int, vector<vector<int>>>::iterator itrEnd; int FlowEnd;
    if (timeRecords->size() != 0){
        itrEnd = --timeRecords->end();
        FlowEnd = itrEnd->second.back()[2];

        map<int, vector<vector<int>>>::iterator itrRestI;
        int timeRest, RouteIDRest, FlowRest;

        if (insertList.size() != 0){
            for (itrRestI = insertList.begin();itrRestI != insertList.end();++itrRestI){
                for (int i=0;i<itrRestI->second.size();i++){
                    timeRest = itrRestI->first; RouteIDRest = itrRestI->second[i][0];
                    FlowRest = FlowEnd - 1;;
                    FlowEnd -= 1;
                    if (timeRecords->find(timeRest) == timeRecords->end())
                        timeRecords->insert(pair<int, vector<vector<int>>>(timeRest, {{RouteIDRest, 0, FlowRest}}));
                    else
                        (*timeRecords)[timeRest].push_back({RouteIDRest, 0, FlowRest});
                }
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
        cout << "TimeRecord Size is: " << timeRecords->size() << endl;
        cout << "RoadSegmentID: " << RoadSegmentID.first << " with TimeSlice: " << index << endl;
        for (itrPrint = timeRecords->begin(); itrPrint != timeRecords->end(); ++itrPrint) {
            for (int i=0;i<itrPrint->second.size();i++)
            {
                cout << " time " << itrPrint->first << " routeID " << itrPrint->second[i][0];
                cout << " status " << itrPrint->second[i][1] << " flow " << itrPrint->second[i][2] << "||";
            }
        }
        cout << endl;
    }

    // Step 6: Update Result
    // -------------------------------------------
    if (RoadSegmentID.first > route_timeFlowChange.size()){
        cout << "RoadSegmentID is " << RoadSegmentID.first << " greater than 774660" << endl;
    }
    if (index > 24 or index < 0){
        cout << "index is " << index << " which is out of range" << endl;
    }

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

//  Split Target Route Data into Groups
void Graph::multi_del_data_initial(void)
{
    for (int i = 0; i < routeRoadDelInput.size(); i++){
        delDataMulti.push_back(routeRoadDelInput[i]);
    }
}

//  Deletion Operation by Deleting Target Route Data One by One (Parallel Version)
void Graph::del_operation_parallel(vector<pair<int, vector<int>>> &delDataMulti,
                                     bool &parallel, bool terminal, bool &range, bool print, int &threadNum)
{
    // Initialize Time Slices
    timeSliceDel.clear();
    timeSliceDel = timeSliceInsert;

    vLockDel.reserve(roadInfor.size());
    for(int i = 0; i < roadInfor.size(); i++){
        Semaphore *sm = new Semaphore(1);
        vLockDel.push_back(sm);
    }

    boost::thread_group threadf;
    int step = delDataMulti.size()/threadNum;
    vector<int> id_temp;

    std::chrono::high_resolution_clock::time_point t0, t1;
    std::chrono::duration<double> time_span1;
    t0=std::chrono::high_resolution_clock::now();

    for(int i = 0; i < threadNum; i++){
        if(i < threadNum - 1)
            threadf.add_thread(new boost::thread(&Graph::one_route_parallel_update_del, this, boost::ref(delDataMulti),
                                                 i*step, (i+1)*step, boost::ref(vLockDel), parallel, terminal, range, print));
        else
            threadf.add_thread(new boost::thread(&Graph::one_route_parallel_update_del, this, boost::ref(delDataMulti),
                                                 i*step, delDataMulti.size(), boost::ref(vLockDel), parallel, terminal, range, print));
    }
    threadf.join_all();

    t1=std::chrono::high_resolution_clock::now();
    time_span1 = std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0);

    if (terminal == true)
        cout << "Deletion operation's time consumption w threads & w termination is: " << time_span1.count() << "s." << endl;
    else
        cout << "Deletion operation's time consumption w threads & w/o termination is: " << time_span1.count() << "s." << endl;

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
void Graph::one_route_parallel_update_del(vector<pair<int,vector<int>>> &delDataMulti,
                                      int i, int j, vector<Semaphore*>& vLockDel, bool parallel, bool terminal, bool range, bool print)
{
    bool prop;
    prop = true;

    for (int k = i; k < j; k++){
        // Initialize The Number of Manipulated Road Segments
        int count = 0;
        // Initialize A List to Store All Affected Road ID
        // "tuple" includes: <0/leaveTime, I', D'>
        vector<pair<pair<int,int>, tuple<int, map<int, vector<vector<int>>>, vector<int>>>> roadSegmentSet;
        // Capture New Routes in Different Groups
        pair<int, vector<int>> newRoute = delDataMulti[k];
        int RoadSegmentID1st = newRoute.second[0];
        int routeID = delDataMulti[k].first;
        // Add Deleted Route Data into List
        vector<int> affectedRoadID; 
		affectedRoadID.clear();
        affectedRoadID.push_back(RoadSegmentID1st);
        // Capture Driving in Time for Current Road Segment
        int nodeID1 = roadID2NodeID[RoadSegmentID1st].first;
        int nodeID1Index;
        int nodeID2 = roadID2NodeID[RoadSegmentID1st].second;
        int nodeCurrent;
        int nodeNext;
        int routeSize = ETA_result[routeID].size();
        // Find NodeID1 Index
        for (int l=0;l<ETA_result[routeID].size()-1;l++){
			// nodeID1Index = INF;
			nodeCurrent = ETA_result[routeID][l].first;
			if (nodeCurrent == nodeID1){
				nodeNext = ETA_result[routeID][l+1].first;
				if (nodeNext == nodeID2){
				    nodeID1Index = l;
					break;
				}
			}
		}

		int inTime = ETA_result[routeID][nodeID1Index].second;

        if (print == true) {
            cout << "=======================================================================" << endl;
            cout << "count: " << count << endl;
        }

        vLockDel[RoadSegmentID1st]->Wait();
        roadSegmentSet = deletion_operation_1st(RoadSegmentID1st, inTime, newRoute, parallel, terminal, range, print);
        vLockDel[RoadSegmentID1st]->Signal();

        // When List Is Not Empty
        // Store Temple Road Information
        // Value Changes During Each Iteration
        pair<int, tuple<int, map<int, vector<vector<int>>>, vector<int>>> roadSegmentTemp;
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
            // Define ID and Information
            roadSegmentIDTemp = itr->first;

            int routeID = itr->first.second;
            vector<int> affected_route = Pi[routeID];
            vector<int> affected_route_w_roadSegmentID = route_node_2_route_road(affected_route);
            pair<int, vector<int>> routeProp = make_pair(routeID,affected_route_w_roadSegmentID);

            nodeID1 = roadID2NodeID[roadSegmentIDTemp.first].first;
            nodeID2 = roadID2NodeID[roadSegmentIDTemp.first].second;
            // Find NodeID1 Index
            for (int l=0;l<ETA_result[routeID].size()-1;l++){
                nodeCurrent = ETA_result[routeID][l].first;
				if (nodeCurrent == nodeID1){
					nodeNext = ETA_result[routeID][l+1].first;
					if (nodeNext == nodeID2){
						nodeID1Index = l;
						break;
					}
				}
            }
            // Find Driving in Time
            int inTime = ETA_result[routeID][nodeID1Index].second;
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

            if (print == true){
                cout << "=======================================================================" << endl;
                cout << endl;
                cout << "Step 1: round start" << endl;
                cout << "-----------------------------------------" << endl;
                cout << "count: " << count << endl;
                cout << "road segment ID is: " << roadSegmentIDTemp.first << " with leaveTime: " << inTime << endl;
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
            vLockDel[roadSegmentIDTemp.first]->Wait();
            roadSegmentSetTemp = update_operation_deletion_further(roadSegmentIDTemp, inTime, newRoute, routeProp, insert, deletion, parallel, terminal, range, print);
            vLockDel[roadSegmentIDTemp.first]->Signal();
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

        // Record Affected Road ID Estimated by Parallel
        affectedRoadParallel.clear();
        affectedRoadParallel = affectedRoadID;
    }
}
