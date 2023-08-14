#include "head.h"

// "ETA_result" Update

void Graph::ETA_update()
{
    // Variable Initialization
    int roadID = 0;
    int timeSlice;
    int nodeID1 = 0;
    int nodeID1Index = INF;
    int nodeID2 = 0;
    int nodeID2Index = INF;
    int time;
    int flow;
    int routeID;
    int status;
    int routeSize;
    // Go Through Each Time Record in "timeSliceInsert" ->
    // Update Values in "ETA_result"
    for (int i=0;i<timeSliceInsert.size();i++){ // Road Segment
        // Assign Values
        roadID = i;
        nodeID1 = roadID2NodeID[roadID].first;
        nodeID2 = roadID2NodeID[roadID].second;
		// Correctness Check
		if (nodeID1 == 0 and nodeID2 == 0){
			cout << "Error. ETA update do not find mapped nodeID1 and nodeID2" << endl;
		}
        for (int j=0;j<timeSliceInsert[i].size();j++){
            if (timeSliceInsert[i][j].size() != 0){
                timeSlice = j;
                // Iterate Each Time Record
                map<int, vector<vector<int>>>::iterator itr;
                for (itr = timeSliceInsert[i][j].begin(); itr != timeSliceInsert[i][j].end(); ++itr){
                    time = itr->first;
                    for (int k=0;k<itr->second.size();k++){
                        routeID = itr->second[k][0];
                        if (routeID < 0){
                            continue;
                        }
						flow = itr->second[k][2];
						status = itr->second[k][1];

                        if (status == 0){
                            int nodeCurrent;
                            int nodePrevious;
							nodeID2Index = INF;
                            // Find NodeID2 Index
                            for (int l=1;l<ETA_result[routeID].size();l++){
								nodeCurrent = ETA_result[routeID][l].first;
								if (nodeCurrent == nodeID2){
									nodePrevious = ETA_result[routeID][l-1].first;
									if (nodePrevious == nodeID1){
										nodeID2Index = l;
										break;
									}
								}
							}
							if (nodeID2Index == INF){
								continue;
							}
							ETA_result[routeID][nodeID2Index].second = time;
						}
                        else{
                            int nodeCurrent;
                            int nodeNext;
							nodeID1Index = INF;
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
							if (nodeID1Index == INF){
								continue;
							}
                            ETA_result[routeID][nodeID1Index].second = time;
                        }
                    }
                }
            }
        }
    }
    // Print
    // -------------------------------------------------------
	/*
    for (int i=0;i<ETA_result.size();i++){
        cout << "path " << i << ": ";
        for (int j=0;j<ETA_result[i].size();j++){
            cout << "node " << ETA_result[i][j].first << " with ";
            cout << "time " << ETA_result[i][j].second << " ";
        }
        cout << endl;
    }
	*/
	int timeAll = 0;
    int timeTemp = 0;
    for (int i = 0; i < ETA_result.size(); i++){
        if (ETA_result[i][ETA_result[i].size() - 1].second == INF or ETA_result[i][0].second == INF){
            timeTemp = 0;
        }
        else{
            timeTemp = ETA_result[i][ETA_result[i].size() - 1].second - ETA_result[i][0].second;
        }
        if (timeTemp <= 0){
            // cout << "Error. Simulation Error." << endl;
        }
        timeAll += timeTemp;
    }
    cout << "Travel Time Estimation (ETA) Update Done." << endl;
}
