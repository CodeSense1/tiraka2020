Short explanation of test files:

Files named "...-in.txt" are input files containing commands for different tests.

Files named "...-out.txt" are output files containing expected output of the corresponding input.

You can either just run the input file with command read "...-in.txt" or compare your own output with the expected output with command testread "...-in.txt" "...-out.txt" (the graphical UI's file selector allows you to select both files at once by control-clicking with mouse).

Files named "perftest-....txt" are performance tests of various types of operations. They can be run with command read "perftest-....txt".

Correctness tests:
- simpletest-compulsory-in/out.txt: Really simple test to find most obvious bugs
- example-*-in/out.txt: The example in the assignment description, showing basic operation of all commands
- Tretest-*-in/out.txt: Somewhat bigger tests with real Tampere centre information

Performance tests:
- perftest-compulsory.txt: Tests adding routes, routes_from and route_any
- perftest-journey_least_stops.txt: Tests journey_least_stops
- perftest-journey_with_cycle.txt: Tests journey_with_cycle
- perftest-journey_shortest_distance.txt: Tests journey_shortest_distance
- perftest-journey_earliest_arrival.txt: Tests journey_earliest_arrival

More info on testing on the course web pages!
