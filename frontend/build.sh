CMD="g++ -o jsimfrontend main.cpp -std=c++11 -O3 -march=native"
CMD="${CMD} -I./../lib/xml/ -I./../lib/xml/wrapper/"
CMD="${CMD} -I./../lib/gates/"

# actually call the command
`${CMD}`
