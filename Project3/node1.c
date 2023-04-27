#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "project3.h"

#define NODE 1

extern int TraceLevel;
extern int clocktime;

struct distance_table {
    int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt1;
struct NeighborCosts *neighbor1;

void printdt1(int, struct NeighborCosts*, struct distance_table*);


/* students to write the following two routines, and maybe some others */

void rtinit1() {
    printf("At time %f rtinit1 was called.\n", clocktime);
    //init to infinite and then set our connected nodes values
    neighbor1 = getNeighborCosts(NODE);
    for (int i = 0; i < MAX_NODES; ++i) {
        for (int j = 0; j < MAX_NODES; ++j) {
            if(i == NODE){
                if(TraceLevel == 4){
                    printf("Setting costs[%d][%d] to %d\n", NODE,j,neighbor1->NodeCosts[j]);
                }
                dt1.costs[NODE][j] = neighbor1->NodeCosts[j];
            } else {
                dt1.costs[i][j] = INFINITY;
            }
        }
    }

    // let our connected nodes our values by sending to layer 2
    for (int i = 0; i < MAX_NODES; ++i) {
        if(dt1.costs[NODE][i] != INFINITY && i != NODE){
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
    printf("At time %f rtupdate1 was called.\n", clocktime);
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

