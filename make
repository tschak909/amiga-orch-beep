lc1 -i:include/ -i:include/lattice/ orch
if not exists "orch.q"
    echo "Compile failed."
    quit 20
    endif
lc2 -v orch.q
atom orch.o orch.atom.o -pc -cdb
alink FROM LIB:AStartup.obj+orch.atom.o TO orch LIBRARY LIB:amiga.lib+LIB:lc.lib
delete orch.o
delete orch.atom.o
echo "-- Compile complete --"
