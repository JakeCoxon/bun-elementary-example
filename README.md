# bun-elementary-example

* The CMakeLists.txt only works for Macos but it should be easy to modify for other systems

* Clone this repository with `--recurse-submodules`

* Requires Bun

* Run this
```
mkdir build; cd build
cmake ..
make
cd ../
yarn
bun app.ts
```

* You should hear a tone for 1 second and then exits