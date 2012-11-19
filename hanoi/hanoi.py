#!/usr/bin/python

import sys, collections

# Enumerate all legal moves to previously-unseen states
def combinations(thisConfig, history):
    resultList = []
    for fromIndex, fromRings in enumerate(thisConfig):
        for destIndex, destRings in enumerate(thisConfig):
            if ( fromIndex == destIndex or not fromRings ):
                # Either the move is null or the from tower has no rings
                continue
            if ( not destRings ):
                # Move is viable because the destination tower has no rings
                workList = list(thisConfig)
                workList[fromIndex] = fromRings[1:]
                workList[destIndex] = (fromRings[0],)
                newConfig = tuple(workList)
                if ( newConfig not in history ):
                    resultList.append(newConfig)
            elif ( destRings[0] > fromRings[0] ):
                # Move is viable because the destination tower's top ring is
                # larger than the from tower's top ring.
                workList = list(thisConfig)
                workList[fromIndex] = fromRings[1:]
                workList[destIndex] = (fromRings[0],) + destRings
                newConfig = tuple(workList)
                if ( newConfig not in history ):
                    resultList.append(newConfig)
    return resultList

# Get the start state and a set of viable solutions for a given number of towers and rings
def getStartAndSolutions(numTowers, numRings):
    startPile = tuple(range(1, numRings+1))
    startList = [startPile]
    for i in range(1, numTowers):
        startList.append(())
    startConfig = tuple(startList)
    solutionSet = set()
    shiftPile = startConfig
    for i in range(1, numTowers):
        shiftPile = shiftPile[1:] + (shiftPile[0],)
        solutionSet.add(shiftPile)
    return (startConfig, solutionSet)

# Main entry point
if __name__ == "__main__":
    # Get the number of towers and rings from the command-line
    if ( len(sys.argv) != 3 ):
        print "Synopsis: {0} <numTowers> <numRings>".format(sys.argv[0])
        exit(1)
    numTowers = int(sys.argv[1])
    numRings = int(sys.argv[2])

    # Set up the initial states and the breadth-first search queue
    (startConfig, solutionSet) = getStartAndSolutions(numTowers, numRings)
    historySet = set()
    historySet.add(startConfig)
    moveList = (startConfig,)
    queue = collections.deque()
    queue.appendleft((moveList, historySet))
    optimalMoves = sys.maxsize

    # Do breadth-first search for sequences of moves which end with a valid solution
    while ( queue ):
        (moveList, historySet) = queue.pop()
        numMoves = len(moveList)
        if ( numMoves > optimalMoves ):
            break;  # only include optimal solutions
        thisConfig = moveList[-1]
        if ( thisConfig in solutionSet ):
            print "Found solution:"
            for index, step in enumerate(moveList):
                print "  {0}: {1}".format(index, step)
            optimalMoves = numMoves # don't seach deeper than this
        else:
            for moveConfig in combinations(thisConfig, historySet):
                newHistorySet = historySet.copy()
                newHistorySet.add(moveConfig)
                newMoveList = moveList[:] + (moveConfig,)
                queue.appendleft((newMoveList, newHistorySet))
