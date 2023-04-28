#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "project3.h"

#define NODE 1

extern int TraceLevel;
extern float clocktime;

struct distance_table {
    int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt1;
struct NeighborCosts *neighbor1;

void printdt1(int, struct NeighborCosts *, struct distance_table *);

//returns a iff a is less than b and vice versa. equality returns b
int min(int, int);

/* students to write the following two routines, and maybe some others */

void rtinit1() {
    printf("At time %f rtinit1 was called.\n", clocktime);
    //init to infinite and then set our connected nodes values
    neighbor1 = getNeighborCosts(NODE);
    for (int i = 0; i < MAX_NODES; ++i) {
        for (int j = 0; j < MAX_NODES; ++j) {
            if (i == NODE) {
                if (TraceLevel == 4) {
                    printf("Setting costs[%d][%d] to %d\n", NODE, j, neighbor1->NodeCosts[j]);
                }
                dt1.costs[NODE][j] = neighbor1->NodeCosts[j];
            } else {
                dt1.costs[i][j] = INFINITY;
            }
        }
    }

    // let our connected nodes our values by sending to layer 2
    for (int i = 0; i < MAX_NODES; ++i) {
        if (dt1.costs[NODE][i] != INFINITY && i != NODE) {
            //send packet
            struct RoutePacket *pkt = malloc(sizeof(struct RoutePacket));
            pkt->sourceid = NODE;
            pkt->destid = i;
            memcpy(pkt->mincost, dt1.costs[NODE], sizeof(dt1.costs[NODE]));
            toLayer2(*pkt);
        }
    }
}


void rtupdate1(struct RoutePacket *rcvdpkt) {
    printf("At time %f rtupdate0 was called.\n", clocktime);
    printf("At time %f node %d received a packet from %d\n", clocktime, NODE, rcvdpkt->sourceid);

    int old_value;
    int new_value;
    //the node the has updated its distances
    int const WORKING_NODE = rcvdpkt->sourceid;

    //First step is to set our tracker array of the working node's array
    for (int i = 0; i < MAX_NODES; ++i) {
        dt1.costs[WORKING_NODE][i] = rcvdpkt->mincost[i];
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
        if (i == 0) {
            first = 2;
            second = 3;
        } else if (i == 2) {
            first = 0;
            second = 3;
        } else {
            //3
            first = 0;
            second = 2;
        }
        old_value = dt1.costs[NODE][i];
        new_value = min(dt1.costs[NODE][i],
                        min(dt1.costs[NODE][first] + dt1.costs[first][i], dt1.costs[NODE][second] +
                                                                          dt1.costs[second][i]));
        dt1.costs[NODE][i] = new_value;
    }

    //if a vector from us changes value notify through layer2
    if (old_value != new_value) {
        for (int i = 0; i < MAX_NODES; ++i) {
            if (dt1.costs[NODE][i] != INFINITY && i != NODE) {
                //send packet
                struct RoutePacket *pkt = malloc(sizeof(struct RoutePacket));
                pkt->sourceid = NODE;
                pkt->destid = i;
                memcpy(pkt->mincost, dt1.costs[NODE], sizeof(dt1.costs[NODE]));
                printf("At time %f node %d is sending an updated cost array to node %d\n", clocktime, NODE, i);
                if (TraceLevel == 4) {
                    printf("Contents of node %d's min cost array are: [%d, %d, %d, %d]\n", NODE, dt1.costs[NODE][0],
                         dt1.costs[NODE][1], dt1.costs[NODE][2], dt1.costs[NODE][3]);
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
void printdt1(int MyNodeNumber, struct NeighborCosts *neighbor,
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
}    // End of printdt1

