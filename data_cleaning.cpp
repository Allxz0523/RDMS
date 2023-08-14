
#include "head.h"

// Check Why "BJ" had different edge number with "Minimal Travel Time"
void Graph::edge_num_check()
{
    // Variable Initialization
    int ID1, ID2;
    vector<int> IDs;
    // Each Node ID
    for (int i=0;i<graphLength.size();i++){
        ID1 = i;
        // Node ID's Neighbors
        for (int j=0;j<graphLength[i].size();j++){
            ID2 = graphLength[i][j].first;
            IDs.clear();
            // Store Neighbors ID from Minimal Travel Time Data in a List
            for (int k=0;k<graphTime[i].size();k++){
                IDs.push_back(graphTime[i][k].first);
            }
            // Check if Node Pairs Appear in Minimal Travel Time Data
            if (find(IDs.begin(), IDs.end(), ID2) == IDs.end()){
                cout << "ID1 " << ID1 << " ID2 " << ID2 << endl;
			}
        }
    }
}
