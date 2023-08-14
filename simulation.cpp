
#include "head.h"

// flow -> travel time range -> travel time
int Graph::flow2time_by_range(int &ID1index, int &ID2index, int &flow)
{
    // Find Range for Specific Time Range
    vector<pair<int,int>> range = timeRange[ID1index][ID2index].second;
    // Correctness Check
    if (range.size() == 0)
        cout << "Do not find travel time range or its size is zero." << endl;
    // Variable Initialization
    int bound, travelTime; 
	if(graphLength[ID1index][ID2index].second <= 20)
		return range[0].second;
    // Comparison
    for (int j=1;j<range.size();j++){
        bound = range[j].first;
        travelTime = range[j-1].second;
        if (flow < bound){
			/* 
            // Print
            cout << flow << " Travel time is: " << travelTime << endl;
            */
            if (travelTime < 0)
				cout << "!!!!!Travel Time < 0" << endl;
			return travelTime;
        }
    }
    // Travel Time Equals to The Biggest One
    travelTime = range[range.size()-1].second;
    /*
    // Print
    cout << " Travel time is: " << travelTime << endl;
    */
	if (travelTime < 0)
		cout << "!!!!!!Tavel Time < 0" << endl;
    return travelTime;
}

// Simulation
vector<vector<pair<int,int>>> Graph::alg1Records(vector<vector<int>> &Q, vector<vector<int>> &Pi, bool range){
    // Step 1: Variable Initialization
    // -------------------------------------------------------
    benchmark::heap<2, int, int> H(Q.size());
    // Nodes Label Initialization
    // record the time each path passes through each point
    nodes_label.clear();
    nodes_label.resize(Q.size());
    for (int i=0;i<nodes_label.size();i++){
        nodes_label[i].resize(Pi[i].size());
        for (int j=0;j<nodes_label[i].size();j++){
            nodes_label[i][j].resize(3);
        }
    }
    // estimated travel time initialization
    ETA_result.resize(Pi.size());
    for (int i=0;i<Pi.size();i++){
        ETA_result[i].resize(Pi[i].size());
    }
    for (int i=0;i<Pi.size();i++){
        for (int j=0;j<1;j++){
            ETA_result[i][j].first = Pi[i][j];
            ETA_result[i][j].second = Q[i][2];
        }
        for (int k=1;k<Pi[i].size();k++){
            ETA_result[i][k].first = Pi[i][k];
            ETA_result[i][k].second = INF;
        }
    }
    // Initialize Road Segment's Traffic Flow
    vector<vector<pair<int,int>>> F = graphLength;
    for (int i=0;i<F.size();i++){
        for (int j=0;j<F[i].size();j++){
            F[i][j].second = 0;
        }
    }
    // Initialize Time Records When Traffic Flow Change
    timeFlowChange.clear();
    timeFlowChange.resize(F.size());
    for (int i=0;i<timeFlowChange.size();i++){
        timeFlowChange[i].resize(F[i].size()); // num of nei
        for (int j=0;j<F[i].size();j++){
            timeFlowChange[i][j].first = F[i][j].first;
        }
    }
    // Initialize Route Label
    // add into priority queue
    vector<vector<int>> label(Q.size());
    int NodeIndex = 0;
    for (int i=0;i<Q.size();i++){
        label[i] = {i,NodeIndex,Q[i][2]};
        nodes_label[i][0] = label[i];
        H.update(i,label[i][2]); // query index, departure time
    }
	// Initialize Min Travel Time Total
    // Initialize Real Travel Time Total
    minTravel = 0;
    realTravel = 0;

    // Step 2: Simulation
    // -------------------------------------------------------
	std::chrono::high_resolution_clock::time_point t0_1, t0_2;
	std::chrono::duration<double> time_span;
	t0_1=std::chrono::high_resolution_clock::now();

    int current_label_index, current_time, current_node_index;
    map<int,vector<int>> timeRecord;
    while (!H.empty()){
        H.extract_min(current_label_index, current_time);
        timeRecord.clear();

        current_node_index = label[current_label_index][1];
        int current_node = Pi[current_label_index][current_node_index];

        // Step 3: If Current Node is The Last One of The Path
        // -------------------------------------------------------
        if (current_node_index == (Pi[current_label_index].size()-1)){
            int previous_node = Pi[current_label_index][current_node_index-1];
            for (int i=0;i<F[previous_node].size();i++){
                if(F[previous_node][i].first == current_node){
                    F[previous_node][i].second = F[previous_node][i].second - 1;
                    int record_time = label[current_label_index][2];
                    if (timeFlowChange[previous_node][i].second.find(record_time) == timeFlowChange[previous_node][i].second.end()){
                        timeFlowChange[previous_node][i].second.insert(pair<int, vector<vector<int>>>(
                                label[current_label_index][2],{{current_label_index, 0, F[previous_node][i].second}}));
                    }
                    else{
                        timeFlowChange[previous_node][i].second[record_time].push_back({current_label_index, 0, F[previous_node][i].second});
                    }
                }
            }
            continue;
        }
        // Step 4: If Current Node is The First One of The Path
        // -------------------------------------------------------
        else{
            int next_node = Pi[current_label_index][current_node_index+1];
            if (current_node_index == 0){
                for (int i=0;i<F[current_node].size();i++){
                    if(F[current_node][i].first == next_node){
                        F[current_node][i].second = F[current_node][i].second + 1;
                        int record_time = label[current_label_index][2];
                        if (timeFlowChange[current_node][i].second.find(record_time) == timeFlowChange[current_node][i].second.end()){
                            timeFlowChange[current_node][i].second.insert(pair<int, vector<vector<int>>>(
                                    label[current_label_index][2],{{current_label_index, 1, F[current_node][i].second}}));
                        }
                        else{
                            timeFlowChange[current_node][i].second[record_time].push_back({current_label_index, 1, F[current_node][i].second});
                        }
                        // Generate Flow Base for Travel Time Estimation
                        pair<int, int> flowBaseIndex = time_to_base_index(record_time, minRange);   // Hour Index, Minutes Index
                        int hourIndex = flowBaseIndex.first;
                        int minuIndex = flowBaseIndex.second;
                        int flowBase = flowBaseList[current_node][i].second[hourIndex][minuIndex];
                        /*
                        // Print
                        cout << "flowBase is: " << flowBase << endl;
                        */
                        //Estimate Travel Time Based on Latency Function
                        int te;
                        int tm = nodeID2minTime[make_pair(current_node, next_node)];
                        int Cflow = F[current_node][i].second + flowBase;
                        if (range == false){
                            te = tm * (1 + sigma * pow(Cflow/varphi, beta));
							minTravel += tm;
							realTravel += te;
						}
						else{
                            te = flow2time_by_range(current_node, i, Cflow);
							minTravel += tm;
							realTravel += te;
						}
                        //Update Label, Travel Time, Nodes Label, and Priority Queue
                        label[current_label_index][2] = label[current_label_index][2] + te;
                        label[current_label_index][1] = label[current_label_index][1] + 1;
                        ETA_result[current_label_index][current_node_index+1].second = label[current_label_index][2];
                        H.update(label[current_label_index][0], label[current_label_index][2]);
                        nodes_label[current_label_index][label[current_label_index][1]] = label[current_label_index];
                    }
                }
            }
            // Step 5: If Current Node is Not The First One of The Path
            // -------------------------------------------------------
            else{
                int previous_node = Pi[current_label_index][current_node_index-1];
                for (int i=0;i<F[previous_node].size();i++){
                    if(F[previous_node][i].first == current_node){
                        F[previous_node][i].second = F[previous_node][i].second - 1;
                        int record_time = label[current_label_index][2];
                        if (timeFlowChange[previous_node][i].second.find(record_time) == timeFlowChange[previous_node][i].second.end()){
                            timeFlowChange[previous_node][i].second.insert(pair<int, vector<vector<int>>>(
                                    label[current_label_index][2],{{current_label_index, 0, F[previous_node][i].second}}));
                        }
                        else{
                            timeFlowChange[previous_node][i].second[record_time].push_back({current_label_index, 0, F[previous_node][i].second});
                        }
                    }
                }
                for (int i=0;i<F[current_node].size();i++){
                    if(F[current_node][i].first == next_node){
                        F[current_node][i].second = F[current_node][i].second + 1;
                        int record_time = label[current_label_index][2];
                        if (timeFlowChange[current_node][i].second.find(record_time) == timeFlowChange[current_node][i].second.end()){
                            timeFlowChange[current_node][i].second.insert(pair<int, vector<vector<int>>>(
                                    label[current_label_index][2],{{current_label_index, 1, F[current_node][i].second}}));
                        }
                        else{
                            timeFlowChange[current_node][i].second[record_time].push_back({current_label_index, 1, F[current_node][i].second});
                        }
                        // Generate Flow Base for Travel Time Estimation
                        pair<int, int> flowBaseIndex = time_to_base_index(record_time, minRange);   // Hour Index, Minutes Index
                        int hourIndex = flowBaseIndex.first;
                        int minuIndex = flowBaseIndex.second;
                        int flowBase = flowBaseList[current_node][i].second[hourIndex][minuIndex];
                        /*
                        // Print
                        cout << "flowBase is: " << flowBase << endl;
                        */
                        // Estimate Travel Time Based on Latency Function
                        int te;
                        int tm = nodeID2minTime[make_pair(current_node, next_node)];
                        int Cflow = F[current_node][i].second;
                        if (range == false){
                            te = tm * (1 + sigma * pow(Cflow/varphi, beta));
							minTravel += tm;
							realTravel += te;
						}
                        else{
                            te = flow2time_by_range(current_node, i, Cflow);
							minTravel += tm;
							realTravel += te;
						}
                        // Update Label, Travel Time, Nodes Label, and Priority Queue
                        label[current_label_index][2] = label[current_label_index][2] + te;
                        label[current_label_index][1] = label[current_label_index][1] + 1;
                        ETA_result[current_label_index][current_node_index+1].second = label[current_label_index][2];
                        H.update(label[current_label_index][0], label[current_label_index][2]);
                        nodes_label[current_label_index][label[current_label_index][1]] = label[current_label_index];
                    }
                }
            }
        }
    }

    /*
    // Step 6: Print nodes_label
    // -------------------------------------------------------
    for (int i=0;i<nodes_label.size();i++){
        cout << "path " << i << ": ";
        for (int j=0;j<nodes_label[i].size();j++){
            cout << nodes_label[i][j][1] << " " << nodes_label[i][j][2] << " ";
        }
        cout << endl;
    }
    */

    /*
    // Step 7: Print timeFlowChange
    // -------------------------------------------------------
    for (int i=0;i<timeFlowChange.size();i++){
        cout << "node1: " << i << endl;
        for (int j=0;j<F[i].size();j++){
            cout << " node2: " << timeFlowChange[i][j].first;
            cout << " with size: " << timeFlowChange[i][j].second.size() << " ";
            map<int, vector<vector<int>>>::iterator itr;
            for (itr = timeFlowChange[i][j].second.begin(); itr != timeFlowChange[i][j].second.end(); ++itr) {
                for (int i=0;i<itr->second.size();i++){
                    cout << " time " << itr->first << " routeID " << itr->second[i][0];
                    cout << " status " << itr->second[i][1] << " flow " << itr->second[i][2] << "||";
                }
            }
            cout << "\n" << endl;
        }
        cout << "\n" << endl;
    }
    */

    /*
    // Step 8: Print ETA_result
    // -------------------------------------------------------
    for (int i=0;i<ETA_result.size();i++){
        cout << "path " << i << ": ";
        for (int j=0;j<ETA_result[i].size();j++){
            cout << "node " << ETA_result[i][j].first << " with ";
            cout << "time " << ETA_result[i][j].second << " ";
        }
        cout << endl;
    }
    */

    cout <<  "Algorithm I Simulation Done." << endl;

    t0_2=std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t0_2-t0_1);

	if (range == false){
        cout << "Algorithm 1 simulation time consumption without range is: " << time_span.count() <<endl;
	}
    else {
        cout << "Algorithm 1 simulation time consumption is: " << time_span.count() << "s."<< endl;
        cout << "Min travel time is: "<< minTravel << "s & real travel time is: " << realTravel << "s."<< endl;
        cout << "Min travel time avg is: "<< minTravel/Q.size() << "s & real travel time avg is: " << realTravel/Q.size() << "s."<< endl;
    }

    // Step 9: return ETA_result
    // -------------------------------------------------------

	/*
	int timeAll = 0;
    int timeTemp = 0;
    for (int i = 0; i < ETA_result.size(); i++){
        timeTemp = ETA_result[i][ETA_result[i].size() - 1].second - ETA_result[i][0].second;
		if (timeTemp < 0){
            cout << "Error. Simulation Error." << endl;
        }
        timeAll += timeTemp;
    }
    cout << "timeAll is: " << timeAll << endl;
    cout << "average timeAll is: " << timeAll / Q.size() << endl;
	*/
    return ETA_result;
}

