#!/usr/bin/perl -w

use myret;

$x = myret::createFoo1();
print "x = $x\n";
myret::destroyFoo($x);

$y = myret::createFoo2();
print "y = $y\n";
myret::destroyFoo($y);

myret::possibleError(10);
