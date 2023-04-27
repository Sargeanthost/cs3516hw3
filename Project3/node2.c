#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project3.h"
#define NODE 2

extern int TraceLevel;
extern float clocktime;

struct distance_table {
    int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt2;
struct NeighborCosts *neighbor2;

void printdt2(int, struct NeighborCosts*, struct distance_table*);


/* students to write the following two routines, and maybe some others */

void rtinit2() {
    printf("At time %f rtinit2 was called.\n", clocktime);
    //init to infinite and then set our connected nodes values
    neighbor2 = getNeighborCosts(NODE);
    for (int i = 0; i < MAX_NODES; ++i) {
        for (int j = 0; j < MAX_NODES; ++j) {
            if(i == NODE){
                if(TraceLevel == 4){
                    printf("Setting costs[%d][%d] to %d\n", NODE,j,neighbor2->NodeCosts[j]);
                }
                dt2.costs[NODE][j] = neighbor2->NodeCosts[j];
            } else {
                dt2.costs[i][j] = INFINITY;
            }
        }
    }

    // let our connected nodes our values by sending to layer 2
    for (int i = 0; i < MAX_NODES; ++i) {
        if(dt2.costs[NODE][i] != INFINITY && i != NODE){
            //send packet
            struct RoutePacket *pkt = malloc(sizeof(struct RoutePacket));
            pkt->sourceid = NODE;
            pkt->destid = i;
            memcpy(pkt->mincost, dt2.costs[NODE], sizeof(dt2.costs[NODE]));
            toLayer2(*pkt);
        }
    }
}


void rtupdate2(struct RoutePacket *rcvdpkt) {
    printf("At time %f rtupdate2 was called.\n", clocktime);
    printf("Sender is %d\n", rcvdpkt->sourceid);
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
void printdt2(int MyNodeNumber, struct NeighborCosts *neighbor,
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
}    // End of printdt2

