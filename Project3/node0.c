#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project3.h"

#define NODE 0

extern int TraceLevel;
extern float clocktime;

//costs[n][k] where n is a 1x4 array showing the values from n to the k nodes.
struct distance_table {
    int costs[MAX_NODES][MAX_NODES];
};

struct distance_table dt0;
struct NeighborCosts *neighbor0;

void printdt0(int, struct NeighborCosts *, struct distance_table *);

//returns a iff a is less than b and vice versa. equality returns b
int min(int a, int b) {
    return a < b ? a : b;
}

/* students to write the following two routines, and maybe some others */
void rtinit0() {
    printf("At time %f rtinit0 was called.\n", clocktime);
    //init to infinite and then set our connected nodes values
    neighbor0 = getNeighborCosts(NODE);
    for (int i = 0; i < MAX_NODES; ++i) {
        for (int j = 0; j < MAX_NODES; ++j) {
            if (i == NODE) {
                if (TraceLevel == 4) {
                    printf("Setting costs[%d][%d] to %d\n", NODE, j, neighbor0->NodeCosts[j]);
                }
                dt0.costs[NODE][j] = neighbor0->NodeCosts[j];
            } else {
                dt0.costs[i][j] = INFINITY;
            }
        }
    }

    // let our connected nodes our values by sending to layer 2
    for (int i = 0; i < MAX_NODES; ++i) {
        if (dt0.costs[NODE][i] != INFINITY && i != NODE) {
            //send packet
            struct RoutePacket *pkt = malloc(sizeof(struct RoutePacket));
            pkt->sourceid = NODE;
            pkt->destid = i;
            memcpy(pkt->mincost, dt0.costs[NODE], sizeof(dt0.costs[NODE]));
            toLayer2(*pkt);
        }
    }
}


void rtupdate0(struct RoutePacket *rcvdpkt) {
    printf("At time %f rtupdate0 was called.\n", clocktime);
    printf("At time %f node %d received a packet from %d\n", clocktime, NODE, rcvdpkt->sourceid);

    int old_value;
    int new_value;
    //the node the has updated its distances
    int const WORKING_NODE = rcvdpkt->sourceid;

    //First step is to set our tracker array of the working node's array
    for (int i = 0; i < MAX_NODES; ++i) {
        dt0.costs[WORKING_NODE][i] = rcvdpkt->mincost[i];
    }
    //second and third step is to check if there is a difference in what our weights while using the BF alg.
    //for N=0 & WN=2: cost(NODE,WORKING_NODE) = min(cost(NODE,WORKING_NODE)=3,
    //                                          cost(NODE,1)=1 + cost(1,WORKING_NODE)=1,
    //                                          cost(NODE,3)=7 + cost(3,WORKING_NODE)=2)
    //the left hand side has working_node change.
    for (int i = 0; i < MAX_NODES; ++i) {
        //cost to get to itself will never change so just skip
        if (i == NODE) {
            continue;
        }
        //really not sure how to do this better, so just hardcoding
        int first;
        int second;
        if (i == 1) {
            first = 2;
            second = 3;
        } else if (i == 2) {
            first = 1;
            second = 3;
        } else {
            //3
            first = 1;
            second = 2;
        }
        old_value = dt0.costs[NODE][i];
        new_value = min(dt0.costs[NODE][i],
                        min(dt0.costs[NODE][first] + dt0.costs[first][i], dt0.costs[NODE][second] +
                                                                          dt0.costs[second][i]));
        dt0.costs[NODE][i] = new_value;
    }

    //if a vector from us changes value notify through layer2
    if (old_value != new_value) {
        for (int i = 0; i < MAX_NODES; ++i) {
            if (dt0.costs[NODE][i] != INFINITY && i != NODE) {
                //send packet to directly connected nodes
                struct RoutePacket *pkt = malloc(sizeof(struct RoutePacket));
                pkt->sourceid = NODE;
                pkt->destid = i;
                memcpy(pkt->mincost, dt0.costs[NODE], sizeof(dt0.costs[NODE]));
                printf("At time %f node %d is sending an updated cost array to node %d\n", clocktime, NODE, i);
                if (TraceLevel == 4) {
                    printf("Contents of node %d's min cost array are: [%d, %d, %d, %d]\n", NODE, dt0.costs[NODE][0],
                           dt0.costs[NODE][1], dt0.costs[NODE][2], dt0.costs[NODE][3]);
                }
                toLayer2(*pkt);
            }
        }
    }
}


/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure 
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is 
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt0(int MyNodeNumber, struct NeighborCosts *neighbor,
              struct distance_table *dtptr) {
    int i, j;
    int TotalNodes = neighbor->NodesInNetwork;     // Total nodes in network
    int NumberOfNeighbors = 0;                     // How many neighbors
    int Neighbors[MAX_NODES];                      // Who are the neighbors

    // Determine our neighbors 
    for (i = 0; i < TotalNodes; i++) {
        if ((neighbor->NodeCosts[i] != INFINITY) && i != MyNodeNumber) {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }
    // Print the header
    printf("                via     \n");
    printf("   D%d |", MyNodeNumber);
    for (i = 0; i < NumberOfNeighbors; i++)
        printf("     %d", Neighbors[i]);
    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for (i = 0; i < TotalNodes; i++) {
        if (i != MyNodeNumber) {
            printf("dest %d|", i);
            for (j = 0; j < NumberOfNeighbors; j++) {
                printf("  %4d", dtptr->costs[i][Neighbors[j]]);
            }
            printf("\n");
        }
    }
    printf("\n");
}    // End of printdt0

