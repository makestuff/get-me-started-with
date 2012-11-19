#!/opt/scala-2.9.2/bin/scala
!#

// Types to make things easier to follow
type Pile = List[Int]
type Config = IndexedSeq[Pile]
type History = Set[Config]

// Enumerate all legal moves to previously-unseen states
def combinations(thisConfig: Config, history: History): List[Config] = {
  val pileRange = 0 to thisConfig.length-1
  val unfiltered = for {
    fromIndex <- pileRange
    destIndex <- pileRange
    if fromIndex != destIndex
    if !thisConfig(fromIndex).isEmpty
    if thisConfig(destIndex).isEmpty ||
       thisConfig(fromIndex).head < thisConfig(destIndex).head
  } yield {
    val fromPile = thisConfig(fromIndex)
    val destPile = thisConfig(destIndex)
    thisConfig
      .updated(fromIndex, fromPile.tail)
      .updated(destIndex, fromPile.head :: destPile)
  }
  unfiltered.toList.filterNot(history.contains(_))
}

// Get the start state and a set of viable solutions for a given number of towers and rings
def getStartAndSolutions(numTowers: Int, numRings: Int): (Config, Set[Config]) = {
  val startPile: Pile = (1 to numRings).toList
  val otherPiles: Config = for ( i <- 1 until numTowers ) yield List[Int]()
  val startConfig: Config = startPile +: otherPiles
  def shift(conf: Config): Config = conf.tail :+ conf.head
  def genSolutions(conf: Config, acc: Set[Config]): Set[Config] = {
    if ( conf == startConfig ) acc
    else genSolutions(shift(conf), acc + conf)
  }
  val solutions: Set[Config] = genSolutions(shift(startConfig), Set[Config]())
  (startConfig, solutions)
}

// Generate a stream of legal move sequences, breadth-first
def generate(initial: Stream[List[Config]], explored: Set[Config]): Stream[List[Config]] = {
  val moveList = initial.head
  val thisConfig = moveList.head
  val nextMoves = combinations(thisConfig, explored) map (_ :: moveList)
  moveList #:: generate(initial.tail #::: nextMoves.toStream, explored + thisConfig)
}

// Get the number of towers and rings from the command-line
if ( args.length != 2 ) {
  println("Synopsis: hanoi.scala <numTowers> <numRings>")
  System.exit(1)
}
val numTowers = args(0).toInt
val numRings = args(1).toInt
val (startConfig, solutions) = getStartAndSolutions(numTowers, numRings)
val stream = generate(Stream(List(startConfig)), Set(startConfig))
val filtered = stream.filter(x => solutions.contains(x.head))
for ( move <- filtered.head.reverse ) {
  println(move)
}
