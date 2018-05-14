cd diskimages
./shcopy
cd ..
gcc -w ./src/main.c -o ./bin/a.out
echo "Built as ./bin/a.out"
./bin/a.out
