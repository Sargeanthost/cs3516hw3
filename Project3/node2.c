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

//1 if connected0, 0 if not connected0. determined on init
int connected2[MAX_NODES];

struct distance_table dt2;
struct NeighborCosts *neighbor2;

void printdt2(int, struct NeighborCosts *, struct distance_table *);

//returns a iff a is less than b and vice versa. equality returns b
int min(int, int);

/* students to write the following two routines, and maybe some others */

void rtinit2() {
    printf("At time %f rtinit2 was called.\n", clocktime);
    //init to infinite and then set our connected0 nodes values
    neighbor2 = getNeighborCosts(NODE);
    memcpy(connected2, neighbor2->NodeCosts, sizeof(connected2));
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            if (i == NODE) {
                if (TraceLevel == 4) {
                    printf("Setting costs[%d][%d] to %d\n", NODE, j, neighbor2->NodeCosts[j]);
                }
                dt2.costs[NODE][j] = neighbor2->NodeCosts[j];
            } else {
                dt2.costs[i][j] = INFINITY;
            }
        }
    }

    // let our connected0 nodes our values by sending to layer 2
    for (int i = 0; i < MAX_NODES; i++) {
        if (dt2.costs[NODE][i] != INFINITY && i != NODE) {
            //send packet
            struct RoutePacket *pkt = malloc(sizeof(struct RoutePacket));
            pkt->sourceid = NODE;
            pkt->destid = i;
            memcpy(pkt->mincost, dt2.costs[NODE], sizeof(dt2.costs[NODE]));
            printf("At time %f, node %d sends packet to node %d with: %d %d %d %d\n", clocktime, NODE, i,
                   pkt->mincost[0], pkt->mincost[1], pkt->mincost[2], pkt->mincost[3]);
            toLayer2(*pkt);
        }
    }
}

void rtupdate2(struct RoutePacket *rcvdpkt) {
    printf("At time %f rtupdate2 was called by a packet from node %d.\n", clocktime, rcvdpkt->sourceid);

    int to_notify = 0;
    //the node the has updated its distances
    int const WORKING_NODE = rcvdpkt->sourceid;

    //First step is to set our tracker array of the working node's array
    for (int i = 0; i < MAX_NODES; i++) {
        dt2.costs[WORKING_NODE][i] = rcvdpkt->mincost[i];
    }
    //second and third step is to check if there is a difference in what our weights while using the BF alg.
    //for N=0 & WN=2: cost(NODE,WORKING_NODE) = min(cost(NODE,WORKING_NODE)=3,
    //                                          cost(NODE,1)=1 + cost(1,WORKING_NODE)=1,
    //                                          cost(NODE,3)=7 + cost(3,WORKING_NODE)=2)
    //the left hand side has working_node change base on i.
    for (int i = 0; i < MAX_NODES; i++) {
        //cost to get to itself will never change so just skip
        if (i == NODE) {
            continue;
        }
        //really not sure how to do this better, so just hardcoding
        int first;
        int second;
        if (i == 0) {
            first = 1;
            second = 3;
        } else if (i == 1) {
            first = 0;
            second = 3;
        } else {
            //3
            first = 0;
            second = 1;
        }
        int old_value = dt2.costs[NODE][i];
        int new_value = min(dt2.costs[NODE][i],
                            min(dt2.costs[NODE][first] + dt2.costs[first][i], dt2.costs[NODE][second] +
                                                                              dt2.costs[second][i]));
        if (old_value != new_value) {
            to_notify = 1;
            dt2.costs[NODE][i] = new_value;
        }
    }

    //if a vector from us changes value notify through layer2
    if (to_notify) {
        printf("At time %f, node %d current distance vector: %d %d %d %d.\n", clocktime, NODE, dt2.costs[NODE][0],
               dt2.costs[NODE][1], dt2.costs[NODE][2], dt2.costs[NODE][3]);
        for (int i = 0; i < MAX_NODES; i++) {
            //dont notify ourselves or non neighbors
            if (i == NODE || connected2[i] == INFINITY) {
                continue;
            }

            struct RoutePacket *pkt = malloc(sizeof(struct RoutePacket));
            pkt->sourceid = NODE;
            pkt->destid = i;
            memcpy(pkt->mincost, dt2.costs[NODE], sizeof(dt2.costs[NODE]));
            printf("At time %f, node %d sends packet to node %d with: %d %d %d %d\n", clocktime, NODE, i,
                   pkt->mincost[0], pkt->mincost[1], pkt->mincost[2], pkt->mincost[3]);
            toLayer2(*pkt);

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

